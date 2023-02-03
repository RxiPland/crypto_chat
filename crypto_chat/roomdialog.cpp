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


RoomDialog::RoomDialog(QWidget *parent, bool createRoom, QString server_url) :
    QDialog(parent),
    ui(new Ui::RoomDialog)
{

    ui->setupUi(this);
    this->setWindowIcon(QIcon("://images/hacker.ico"));

    RoomDialog::room_id = RoomDialog::generateId();
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
        ui->lineEdit_3->setText(RoomDialog::room_id);

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

QString RoomDialog::generateId()
{
    // generate random ID (32)

    ui->pushButton_3->setDisabled(true);

    //QString command = QString("/C python config/cryptographic_tool.exe generate_id");
    QString command = QString("/C python config/cryptographic_tool.py generate_id");

    // decrypt RSA encrypted data
    QProcess process;
    process.start("cmd", QStringList(command));

    while(process.state() == QProcess::Running){
        qApp->processEvents();
    }

    ui->pushButton_3->setDisabled(false);


    if (process.readAllStandardError().isEmpty()){

        return process.readAllStandardOutput().trimmed();
    }

    QMessageBox::critical(this, "Chyba", "Nepodařilo se vytvořit ID! Zkuste to znovu.");

    return "";
}


void RoomDialog::createRoomFunc()
{
    // create new room

    RoomDialog::disable_widgets(true);
    RoomDialog::room_id = ui->lineEdit_3->text().trimmed();

    if(RoomDialog::room_id.length() != 32){
        QMessageBox::critical(this, "Chyba", "ID místnosti není validní!");

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

    if (ui->checkBox->isChecked()){
        RoomDialog::room_password = ui->lineEdit->text();

    } else{
        RoomDialog::room_password = "";
    }

    // encrypt json with server public key
    QJsonObject objData;
    objData["room_id"] = RoomDialog::room_id;

    //hash password
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(RoomDialog::room_password.toStdString());

    objData["room_password_sha256"] = (QString)hash.result().toHex();


    QJsonDocument docData(objData);
    QString dataHex = docData.toJson().toHex();

    QProcess process;
    //QString command = QString("/C python config/cryptographic_tool.exe encrypt_rsa %1 %2").arg(serverPublicKeyPemHex, dataHex);
    QString command = QString("/C python config/cryptographic_tool.py encrypt_rsa %1 %2").arg(serverPublicKeyPemHex, dataHex);

    // encrypt data
    process.start("cmd", QStringList(command));

    while(process.state() == QProcess::Running){
        qApp->processEvents();
    }

    // get encrypted data
    QString encryptedDataHex = process.readAllStandardOutput().trimmed();

    if (encryptedDataHex.isEmpty()){
        QMessageBox::critical(this, "Chyba", "Nepodařilo se zašifrovat data veřejným klíčem serveru! (RSA)");

        RoomDialog::disable_widgets(false);
        return;
    }


    // data to send
    objData = QJsonObject();
    objData["rsa_pem"] = RoomDialog::rsaPublicKeyPemHex;
    objData["data_rsa"] = encryptedDataHex;
    docData = QJsonDocument(objData);
    QByteArray createRoomData = docData.toJson();

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

    // post
    QNetworkReply *reply_post = manager.post(request, createRoomData);

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

    QList<QJsonValue> decryptedData = RoomDialog::decryptRsa(QStringList("status_code", "room_aes_key"), response);


    if (decryptedData.isEmpty()){

        QMessageBox::critical(this, "Chyba", "Nepodařilo se dešifrovat data! (RSA)");

        RoomDialog::disable_widgets(false);
        return;
    }

    QString statusCode = decryptedData[0].toString();

    if (statusCode != "1"){

        QMessageBox::critical(this, "Chyba", QString("Hodnota status_code není 1!\nPřijatá hodnota: %1").arg(statusCode));

        RoomDialog::disable_widgets(false);
        return;
    }


    // room symetric key in hex
    RoomDialog::roomAesKeyHex = decryptedData[1].toString();

    if(RoomDialog::roomAesKeyHex.isEmpty()){
        QMessageBox::critical(this, "Chyba", "Server neposlal symetrický klíč místnosti!");

        RoomDialog::disable_widgets(false);
        return;
    }


    RoomDialog::successful = true;
    QMessageBox::information(this, "Oznámení", "Místnost byla úspěšně vytvořena");

    this->close();
}


void RoomDialog::joinRoomFunc()
{
    // join existing room

    RoomDialog::disable_widgets(true);

    QString nickname = ui->lineEdit_4->text().trimmed();
    QString roomId = ui->lineEdit_3->text().trimmed();


    // check for mistakes
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


    QJsonObject objData;
    objData["room_id"] = roomId;

    if (ui->checkBox->isChecked()){
        RoomDialog::room_password = ui->lineEdit->text();
        objData["room_password"] = RoomDialog::room_password;

    } else{
        RoomDialog::room_password = "";
        objData["room_password"] = "";
    }

    QJsonDocument docData(objData);
    QString dataHex = docData.toJson().toHex();

    QProcess process;
    //QString command = QString("/C python config/cryptographic_tool.exe encrypt_rsa %1 %2").arg(serverPublicKeyPemHex, dataHex);
    QString command = QString("/C python config/cryptographic_tool.py encrypt_rsa %1 %2").arg(serverPublicKeyPemHex, dataHex);

    // encrypt data
    process.start("cmd", QStringList(command));

    while(process.state() == QProcess::Running){
        qApp->processEvents();
    }

    // get encrypted data
    QString encryptedDataHex = process.readAllStandardOutput().trimmed();

    if (encryptedDataHex.isEmpty()){
        QMessageBox::critical(this, "Chyba", "Nepodařilo se zašifrovat data veřejným klíčem serveru! (RSA)");

        RoomDialog::disable_widgets(false);
        return;
    }


    // data to send
    objData = QJsonObject();
    objData["rsa_pem"] = RoomDialog::rsaPublicKeyPemHex;
    objData["data_rsa"] = encryptedDataHex;
    docData = QJsonDocument(objData);
    QByteArray joinRoomData = docData.toJson();

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

    QNetworkReply *reply_post = manager.post(request, joinRoomData);

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


    QStringList jsonKeys;
    jsonKeys.append("status_code");
    jsonKeys.append("room_aes_key");
    jsonKeys.append("messages_count");

    QList<QJsonValue> decryptedData = RoomDialog::decryptRsa(jsonKeys, response);

    if (decryptedData.isEmpty()){

        QMessageBox::critical(this, "Chyba", "Nepodařilo se dešifrovat data! (RSA)");

        RoomDialog::disable_widgets(false);
        return;
    }


    QString statusCode = decryptedData[0].toString();


    if (statusCode == "2"){
        QMessageBox::critical(this, "Chyba", "Na serveru neexistuje soubor se symetrickým klíčem místnosti!");

        RoomDialog::disable_widgets(false);
        return;

    } else if (statusCode == "3"){

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

    } else if (statusCode == "4"){
        QMessageBox::critical(this, "Chyba", "Místnost s tímto ID neexistuje!");

        RoomDialog::disable_widgets(false);
        ui->lineEdit_3->setFocus();

        return;

    } else if (statusCode != "1"){
        QMessageBox::critical(this, "Chyba", QString("Nastala neznámá chyba při pokusu o připojení do místnosti!\n\nstatus_code %1").arg(statusCode));

        RoomDialog::disable_widgets(false);
        return;
    }


    // room symetric key in hex
    RoomDialog::roomAesKeyHex = decryptedData[1].toString();

    if(RoomDialog::roomAesKeyHex.isEmpty()){
        QMessageBox::critical(this, "Chyba", "Server neposlal symetrický klíč místnosti!");

        RoomDialog::disable_widgets(false);
        return;
    }

    RoomDialog::username = nickname;
    RoomDialog::room_id = roomId;


    // messages count on server
    QString messagesCount = decryptedData[2].toString();

    if(!messagesCount.isEmpty()){
        RoomDialog::serverMessagesCount = messagesCount.toInt();
    }

    RoomDialog::successful = true;
    QMessageBox::information(this, "Oznámení", "Připojení do místnosti proběhlo úspěšně");

    this->close();
}

/*
void RoomDialog::changePassword(QString password)
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
    QStringList decryptedData = decryptRsa(QStringList("status_code"), response);

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
*/

QList<QJsonValue> RoomDialog::decryptRsa(QStringList json_keys, QByteArray response)
{
    // decrypt data_rsa json

    QList<QJsonValue> returnData;

    QJsonDocument jsonResponse;
    QJsonObject jsonObject;
    QString dataHex;


    jsonResponse = QJsonDocument::fromJson(response);
    jsonObject = jsonResponse.object();
    dataHex = jsonObject["data_rsa"].toString();

    //QString command = QString("/C python config/cryptographic_tool.exe decrypt_rsa %1 %2").arg(RoomDialog::rsaPrivateKeyPemHex, dataHex);
    QString command = QString("/C python config/cryptographic_tool.py decrypt_rsa %1 %2").arg(RoomDialog::rsaPrivateKeyPemHex, dataHex);

    // decrypt
    QProcess process;
    process.start("cmd", QStringList(command));

    while(process.state() == QProcess::Running){
        qApp->processEvents();
    }

    QByteArray output = process.readAllStandardOutput().trimmed();

    if(output.isEmpty()){
        return returnData;
    }

    // get decrypted data
    QByteArray decryptedData = QByteArray::fromHex(output);
    decryptedData.replace("\'", "\"");

    jsonResponse = QJsonDocument::fromJson(decryptedData);
    jsonObject = jsonResponse.object();


    int i;

    for(i=0; i<json_keys.length(); i++){
        returnData.append(jsonObject[json_keys[i]]);
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

