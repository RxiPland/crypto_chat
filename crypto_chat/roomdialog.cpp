/*
Window for creating new room OR joining existing room
*/

#include "roomdialog.h"
#include "ui_roomdialog.h"

#include <QDir>
#include <QCloseEvent>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QNetworkReply>
#include <QCryptographicHash>
#include <QProcess>


RoomDialog::RoomDialog(QWidget *parent, bool createRoom, QString server_url, QString room_id) :
    QDialog(parent),
    ui(new Ui::RoomDialog)
{

    ui->setupUi(this);
    this->setWindowIcon(QIcon("://images/hacker.ico"));

    RoomDialog::room_id = room_id;
    RoomDialog::server_url = server_url;
    RoomDialog::createRoom = createRoom;

    ui->pushButton->setDisabled(true);

    ui->lineEdit->setHidden(true);
    ui->lineEdit_2->setDisabled(true);
    ui->lineEdit_2->setReadOnly(true);
    ui->lineEdit_2->setText(server_url);


    ui->pushButton->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    ui->pushButton_2->setFocusPolicy(Qt::FocusPolicy::NoFocus);

    if(createRoom){

        this->setWindowTitle("Vytvoření chatovací místnosti");

        ui->lineEdit_3->setDisabled(true);
        ui->lineEdit_3->setReadOnly(true);
        ui->lineEdit_3->setText(RoomDialog::generateId());

        ui->pushButton->setText(" Vytvořit místnost ");

    } else{

        this->setWindowTitle("Připojení do chatovací místnosti");

        ui->lineEdit_3->setDisabled(false);
        ui->lineEdit_3->setReadOnly(false);
        ui->lineEdit_3->clear();

        ui->pushButton->setText(" Připojit se do místnosti ");
    }

    this->show();
}

RoomDialog::~RoomDialog()
{
    delete ui;
}

void RoomDialog::closeEvent(QCloseEvent *bar)
{
    // Before application close

    this->close();

    if(bar != nullptr){
        bar->accept();
    }

    QApplication::quit();
}

void RoomDialog::disable_widgets(bool disable){
    // disable widgets during network requests, ...

    ui->pushButton->setDisabled(disable);
    ui->pushButton_2->setDisabled(disable);

    ui->lineEdit->setDisabled(disable);
    ui->lineEdit_4->setDisabled(disable);

    if(!createRoom){
        ui->lineEdit_3->setDisabled(disable);
    }

    ui->checkBox->setDisabled(disable);
}

QString RoomDialog::generateId(){

    //QString command = QString("/C python config/cryptographic_tool.exe generate_id");
    QString command = QString("/C python config/cryptographic_tool.py generate_id");

    // decrypt RSA encrypted data
    QProcess process;
    process.start("cmd", QStringList(command));

    while(process.state() == QProcess::Running){
        qApp->processEvents();
    }

    if (process.readAllStandardError().isEmpty()){

        return process.readAllStandardOutput().trimmed();
    }

    return "";
}


void RoomDialog::createRoomFunc()
{
    // create new room

    RoomDialog::disable_widgets(true);
    RoomDialog::room_id = ui->lineEdit_3->text().trimmed();

    if(RoomDialog::room_id.isEmpty()){
        QMessageBox::critical(this, "Chyba", "Pole pro ID místnosti nemůže být prázdné!");

        RoomDialog::disable_widgets(false);
        ui->lineEdit_3->setFocus();
        return;
    }


    RoomDialog::username = ui->lineEdit_4->text().trimmed();

    if(RoomDialog::username.isEmpty()){
        QMessageBox::critical(this, "Chyba", "Pole pro jméno nemůže být prázdné!");

        RoomDialog::disable_widgets(false);
        ui->lineEdit_4->setFocus();
        return;
    }


    QJsonObject objData;
    objData["rsa_pem"] = RoomDialog::rsaPublicKeyPem;
    QJsonDocument docData(objData);
    QByteArray CreateRoomData = docData.toJson();

    QNetworkRequest request;
    QUrl qurl_address = QUrl(server_url + "/create-room");

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::UserAgentHeader, RoomDialog::user_agent);

    if(RoomDialog::authentication_required){
        // authentication

        qurl_address.setUserName(authentication_username);
        qurl_address.setPassword(authentication_password);
    }

    request.setUrl(qurl_address);

    QNetworkReply *reply_post = manager.post(request, CreateRoomData);

    while (!reply_post->isFinished())
    {
        qApp->processEvents();
    }


    QByteArray response = reply_post->readAll();

    if(reply_post->error() == QNetworkReply::ConnectionRefusedError){
        QMessageBox::critical(this, "Chyba", "Nelze se připojit k internetu nebo server není dostupný!");

        RoomDialog::disable_widgets(false);
        return;

    } else if(reply_post->error() != QNetworkReply::NoError){
        // Any error

        QMessageBox::critical(this, "Odpověd serveru (chyba)", tr("Nastala neznámá chyba!\nOznačení QNetworkReply chyby: %1\n\nOdpověd serveru: %2").arg(reply_post->error()).arg(response));

        RoomDialog::disable_widgets(false);
        return;
    }


    QJsonDocument jsonResponse = QJsonDocument::fromJson(response);
    QJsonObject jsonObject = jsonResponse.object();
    QString dataHex = jsonObject["data"].toString();


    //QString command = QString("/C python config/cryptographic_tool.exe decrypt_rsa %1 %2").arg(rsaPrivateKeyPem, dataHex);
    QString command = QString("/C python config/cryptographic_tool.py decrypt_rsa %1 %2").arg(rsaPrivateKeyPemHex, dataHex);

    // decrypt RSA encrypted data
    QProcess process;
    process.start("cmd", QStringList(command));

    while(process.state() == QProcess::Running){
        qApp->processEvents();
    }

    // get decrypted data
    QByteArray decryptedDataHex = process.readAllStandardOutput().trimmed();


    if (decryptedDataHex.isEmpty()){

        QMessageBox::critical(this, "Chyba", "Nepodařilo se dešifrovat data!");

        RoomDialog::disable_widgets(false);
        return;
    }

    jsonResponse = QJsonDocument::fromJson(decryptedDataHex);
    jsonObject = jsonResponse.object();


    QString statusCode = jsonObject["status_code"].toString();

    if (statusCode != "1"){

        QMessageBox::critical(this, "Chyba", "Serveru se nepodařilo zašifrovat data veřejným klíčem!");

        RoomDialog::disable_widgets(false);
        return;
    }

    RoomDialog::roomAesKeyHex = jsonObject["room_aes_key"].toString();

    if(RoomDialog::roomAesKeyHex.isEmpty()){
        QMessageBox::critical(this, "Chyba", "Server nevytvořil symetrický klíč místnosti!");

        RoomDialog::disable_widgets(false);
        return;
    }

    //RoomDialog::room_id = jsonObject["room_id"].toString();

    if(RoomDialog::room_id.isEmpty()){
        QMessageBox::critical(this, "Chyba", "Server nevytvořil ID místnosti!");

        RoomDialog::disable_widgets(false);
        return;
    }

    // set password for room
    if(ui->checkBox->isChecked()){
        RoomDialog::setPassword(ui->lineEdit->text());
    }

    RoomDialog::successful = true;
    QMessageBox::information(this, "Oznámení", "Místnost byla úspěšně vytvořena");

    this->close();
}


void RoomDialog::joinRoomFunc()
{
    RoomDialog::disable_widgets(true);

    QString nickname = ui->lineEdit_4->text().trimmed();
    QString roomId = ui->lineEdit_3->text().trimmed();


    if (roomId.isEmpty()){
        QMessageBox::critical(this, "Chyba", "Pole pro ID místnosti nemůže být prázdné!");

        RoomDialog::disable_widgets(false);
        ui->lineEdit_3->setFocus();
        return;

    } else if (roomId.length() != 32){
        QMessageBox::critical(this, "Neplatné ID", "ID místnosti musí mít délku přesně 32 znaků!");

        RoomDialog::disable_widgets(false);
        ui->lineEdit_3->setFocus();
        return;

    } else if (nickname.isEmpty()){
        QMessageBox::critical(this, "Chyba", "Pole pro jméno nemůže být prázdné!");

        RoomDialog::disable_widgets(false);
        ui->lineEdit_4->setFocus();
        return;
    }

    QJsonObject objMessage;
    objMessage["room_id"] = roomId;

    if (ui->checkBox->isChecked()){
        RoomDialog::room_password = ui->lineEdit->text();
        objMessage["room_password"] = RoomDialog::room_password;

    } else{
        RoomDialog::room_password = "";
        objMessage["room_password"] = "";
    }

    QJsonDocument docMessage(objMessage);
    QString message_data = docMessage.toJson().toHex();

    //QString command = QString("/C python config/cryptographic_tool.exe encrypt_aes %1 %2 %3").arg(RoomDialog::serverAesKeyHex, "False", message_data);
    QString command = QString("/C python config/cryptographic_tool.py encrypt_aes %1 %2 %3").arg(RoomDialog::serverAesKeyHex, "False", message_data);


    // encrypt
    QProcess process;
    process.start("cmd", QStringList(command));

    while (process.state() == QProcess::Running){
        qApp->processEvents();
    }

    QString encryptedData = process.readAllStandardOutput().trimmed();


    if (encryptedData.isEmpty()){
        QMessageBox::critical(this, "Chyba", QString("Nastala chyba při šifrování!\n\nChyba: %1").arg(process.readAllStandardError()));

        RoomDialog::disable_widgets(false);
        return;
    }

    QJsonObject objData;
    objData["data"] = encryptedData;
    QJsonDocument docData(objData);
    QByteArray JoinRoomData = docData.toJson();

    QNetworkRequest request;
    QUrl qurl_address = QUrl(server_url + "/join-room");

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::UserAgentHeader, RoomDialog::user_agent);

    if (RoomDialog::authentication_required){
        // authentication

        qurl_address.setUserName(authentication_username);
        qurl_address.setPassword(authentication_password);
    }

    request.setUrl(qurl_address);

    QNetworkReply *reply_post = manager.post(request, JoinRoomData);

    while (!reply_post->isFinished())
    {
        qApp->processEvents();
    }

    QByteArray response = reply_post->readAll();

    if(reply_post->error() == QNetworkReply::ConnectionRefusedError){
        QMessageBox::critical(this, "Chyba", "Nelze se připojit k internetu nebo server není dostupný!");

        RoomDialog::disable_widgets(false);
        return;

    } else if (reply_post->error() != QNetworkReply::NoError){
        // Any error

        QMessageBox::critical(this, "Odpověd serveru (chyba)", tr("Nastala neznámá chyba!\nOznačení QNetworkReply chyby: %1\n\nOdpověd serveru: %2").arg(reply_post->error()).arg(response));

        RoomDialog::disable_widgets(false);
        return;
    }


    QStringList names;
    names.append("status_code");
    names.append("room_aes_key");
    names.append("messages_count");

    QStringList responseData = getJson(names, response);

    if(responseData.isEmpty()){
        QMessageBox::critical(this, "Chyba", "Server byl pravděpodobně restartován a kvůli tomu máte starý symetrický klíč. Restartuje program pro získání nového.");

        RoomDialog::disable_widgets(false);
        return;
    }


    if (responseData[0] == "5"){
        QMessageBox::critical(this, "Chyba", "Server byl pravděpodobně restartován a kvůli tomu máte starý symetrický klíč. Restartuje program pro získání nového.");

        RoomDialog::disable_widgets(false);
        return;

    } else if (responseData[0] == "2"){
        QMessageBox::critical(this, "Chyba", "Na serveru neexistuje soubor se symetrickým klíčem místnosti!");

        RoomDialog::disable_widgets(false);
        return;

    } else if (responseData[0] == "3"){

        if (ui->checkBox->isChecked()){
            QMessageBox::critical(this, "Chyba", "Špatné heslo!");

        } else{
            QMessageBox::critical(this, "Chyba", "Místnost je chráněna heslem!");
        }

        ui->checkBox->setChecked(true);
        RoomDialog::on_checkBox_clicked();

        RoomDialog::disable_widgets(false);
        ui->lineEdit->setFocus();

        return;

    } else if (responseData[0] == "4"){
        QMessageBox::critical(this, "Chyba", "Místnost s tímto ID neexistuje!");

        RoomDialog::disable_widgets(false);
        ui->lineEdit_3->setFocus();

        return;

    } else if (responseData[0] != "1"){
        QMessageBox::critical(this, "Chyba", "Nastala neznámá chyba při pokusu o připojení do místnosti!");

        RoomDialog::disable_widgets(false);
        return;
    }

    // get room symetric key in hex
    RoomDialog::roomAesKeyHex = responseData[1];


    QMessageBox::information(this, "Oznámení", "Připojení do místnosti proběhlo úspěšně");

    RoomDialog::successful = true;
    RoomDialog::username = nickname;
    RoomDialog::room_id = roomId;


    QString number = responseData[2];

    if(!number.isEmpty()){
        RoomDialog::serverMessagesCount = number.toInt();
    }


    this->close();
}


void RoomDialog::setPassword(QString password)
{

    //hash password
    QCryptographicHash hash(QCryptographicHash::Sha256);

    if(ui->checkBox->isChecked()){
        hash.addData(password.toStdString());
        RoomDialog::room_password = password;

    } else{
        return;
    }

    QByteArray passwordSha256 = hash.result().toHex();

    //QString command = QString("/C python config/cryptographic_tool.exe encrypt_aes %1 %2 %3").arg(RoomDialog::roomAesKeyHex, "False", passwordSha256.toHex());
    QString command = QString("/C python config/cryptographic_tool.py encrypt_aes %1 %2 %3").arg(RoomDialog::roomAesKeyHex, "False", passwordSha256.toHex());

    // encrypt hashed password with room key
    QProcess process;
    process.start("cmd", QStringList(command));

    while(process.state() == QProcess::Running){
        qApp->processEvents();
    }

    // get decrypted data
    QString encryptedPasswordHex = process.readAllStandardOutput().trimmed();

    QJsonObject objData;
    objData["room_id"] = RoomDialog::room_id;
    objData["encrypted_room_password"] = encryptedPasswordHex;
    QJsonDocument docData(objData);
    QByteArray setPasswordData = docData.toJson();


    //command = QString("/C python config/cryptographic_tool.exe encrypt_aes %1 %2 %3").arg(RoomDialog::serverAesKeyHex, "False", setPasswordData.toHex());
    command = QString("/C python config/cryptographic_tool.py encrypt_aes %1 %2 %3").arg(RoomDialog::serverAesKeyHex, "False", setPasswordData.toHex());

    // encrypt JSON with server key
    process.start("cmd", QStringList(command));

    while(process.state() == QProcess::Running){
        qApp->processEvents();
    }

    QString postData = process.readAllStandardOutput().trimmed();

    QJsonObject objPostData;
    objData["data"] = postData;
    QJsonDocument docPostData(objData);
    QByteArray postDataJson = docPostData.toJson();

    QNetworkRequest request;
    QUrl qurl_address = QUrl(server_url + "/change-password");

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::UserAgentHeader, RoomDialog::user_agent);

    if(RoomDialog::authentication_required){
        // authentication

        qurl_address.setUserName(authentication_username);
        qurl_address.setPassword(authentication_password);
    }

    request.setUrl(qurl_address);

    QNetworkReply *reply_post = manager.post(request, postDataJson);

    while (!reply_post->isFinished())
    {
        qApp->processEvents();
    }


    QByteArray response = reply_post->readAll();

    if(reply_post->error() == QNetworkReply::ConnectionRefusedError){
        QMessageBox::critical(this, "Chyba", "Nelze se připojit k internetu nebo server není dostupný!");

        RoomDialog::disable_widgets(false);
        return;

    } else if(reply_post->error() != QNetworkReply::NoError){
        // Any error

        QMessageBox::critical(this, "Odpověd serveru (chyba)", tr("Nastala neznámá chyba!\nOznačení QNetworkReply chyby: %1\n\nOdpověd serveru: %2").arg(reply_post->error()).arg(response));

        RoomDialog::disable_widgets(false);
        return;
    }


    // get decrypted data
    QStringList decryptedData = getJson(QStringList("status_code"), response);

    if (decryptedData.isEmpty()){

        QMessageBox::critical(this, "Chyba", "Nepodařilo se dešifrovat data!");

        RoomDialog::disable_widgets(false);
        return;
    }

    QString statusCode = decryptedData[0];

    if (statusCode != "1"){
        QMessageBox::critical(this, "Chyba", "Nepodařilo se nastavit heslo!");

        RoomDialog::disable_widgets(false);
        return;
    }
}


QStringList RoomDialog::getJson(QStringList names, QByteArray data)
{
    QJsonDocument jsonResponse;
    QJsonObject jsonObject;
    QString dataHex;


    jsonResponse = QJsonDocument::fromJson(data);
    jsonObject = jsonResponse.object();
    dataHex = jsonObject["data"].toString();

    //QString command = QString("/C python config/cryptographic_tool.exe decrypt_aes %1 %2 %3").arg(RoomDialog::serverAesKeyHex, "False", dataHex);
    QString command = QString("/C python config/cryptographic_tool.py decrypt_aes %1 %2 %3").arg(RoomDialog::serverAesKeyHex, "False", dataHex);

    // decrypt
    QProcess process;
    process.start("cmd", QStringList(command));

    while(process.state() == QProcess::Running){
        qApp->processEvents();
    }

    QByteArray output = process.readAllStandardOutput().trimmed();

    if(output.isEmpty()){
        return QStringList();
    }


    // get decrypted data
    QByteArray decryptedData = QByteArray::fromHex(output);


    decryptedData.replace("\'", "\"");

    jsonResponse = QJsonDocument::fromJson(decryptedData);
    jsonObject = jsonResponse.object();


    QStringList returnData;
    int i;

    for(i=0; i<names.length(); i++){
        returnData.append(jsonObject[names[i]].toString());
    }

    return returnData;
}


void RoomDialog::on_lineEdit_3_textEdited(const QString &arg1)
{
    int textLength = arg1.length();

    if(arg1.length() == 33){
        // length of ID is more than 32 chars

        QString text = arg1;

        if (text.back() != '\n'){
            QMessageBox::critical(this, "Upozornění", "Délka ID musí být přesně 32 znaků!");
        }

        text.chop(1);
        ui->lineEdit_3->setText(text);


    } else if (textLength == 0 || ui->lineEdit_4->text().length() == 0){
        ui->pushButton->setDisabled(true);

    } else{
        ui->pushButton->setDisabled(false);
    }
}

void RoomDialog::on_lineEdit_4_textEdited(const QString &arg1)
{
    int textLength = arg1.length();

    if(textLength == 26){
        // length of username is more than 25 chars

        QString text = arg1;
        text.chop(1);

        ui->lineEdit_4->setText(text);

        QMessageBox::critical(this, "Upozornění", "Délka jména nemůže přesáhnout 25 znaků!");

    } else if (textLength == 0 || ui->lineEdit_3->text().length() == 0){
        ui->pushButton->setDisabled(true);

    } else{
        ui->pushButton->setDisabled(false);
    }
}


void RoomDialog::on_pushButton_clicked()
{
    if(RoomDialog::createRoom){
        RoomDialog::createRoomFunc();

    } else{
        RoomDialog::joinRoomFunc();
    }
}


void RoomDialog::on_lineEdit_3_returnPressed()
{
    RoomDialog::on_pushButton_clicked();
}

void RoomDialog::on_lineEdit_4_returnPressed()
{
    RoomDialog::on_pushButton_clicked();
}

void RoomDialog::on_lineEdit_returnPressed()
{
    RoomDialog::on_pushButton_clicked();
}


void RoomDialog::on_pushButton_2_clicked()
{
    if(RoomDialog::createRoom){

        this->setWindowTitle("Připojení do chatovací místnosti");

        ui->lineEdit_3->setDisabled(false);
        ui->lineEdit_3->setReadOnly(false);
        ui->lineEdit_3->clear();

        ui->lineEdit_3->setFocus();

        ui->pushButton->setText(" Připojit se do místnosti ");

        RoomDialog::createRoom = false;
        QMessageBox::information(this, "Oznámení", "Přepnuto na připojení do existující místnosti");

    } else{

        this->setWindowTitle("Vytvoření chatovací místnosti");

        ui->lineEdit_3->setDisabled(true);
        ui->lineEdit_3->setReadOnly(true);
        ui->lineEdit_3->setText(RoomDialog::generateId());

        ui->lineEdit_2->setFocus();

        ui->pushButton->setText(" Vytvořit místnost ");

        RoomDialog::createRoom = true;
        QMessageBox::information(this, "Oznámení", "Přepnuto na vytváření nové místnosti");
    }
}


void RoomDialog::on_checkBox_clicked()
{
    if(ui->checkBox->isChecked()){
        ui->lineEdit->setHidden(false);
        ui->lineEdit->setFocus();

    } else{
        ui->lineEdit->setHidden(true);
    }
}


void RoomDialog::on_pushButton_3_clicked()
{
    ui->lineEdit_3->setText(RoomDialog::generateId());
}

