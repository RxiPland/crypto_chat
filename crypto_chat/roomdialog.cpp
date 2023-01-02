/*
Window for creating new room OR joining existing room
*/

#include "roomdialog.h"
#include "ui_roomdialog.h"
#include "threadfunctions.h"

#include <QDir>
#include <QCloseEvent>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QNetworkReply>
#include <QCryptographicHash>

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
        ui->lineEdit_3->setText(room_id);

        ui->pushButton->setText("Vytvořit místnost");

    } else{

        this->setWindowTitle("Připojení do chatovací místnosti");

        ui->lineEdit_3->setDisabled(false);
        ui->lineEdit_3->setReadOnly(false);
        ui->lineEdit_3->clear();

        ui->pushButton->setText("Připojit se do místnosti");
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

    if(deleleFolder && !successful){
        QDir roomFolder(QDir::tempPath() + "/" + room_id);

        if(roomFolder.exists() && room_id != ""){
            roomFolder.removeRecursively();
        }
    }

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


void RoomDialog::createRoomFunc()
{
    RoomDialog::disable_widgets(true);
    QString nickname = ui->lineEdit_4->text().trimmed();

    if(nickname == ""){
        QMessageBox::critical(this, "Chyba", "Pole pro jméno nemůže být prázdné!");

        RoomDialog::disable_widgets(false);
        ui->lineEdit_4->setFocus();
        return;
    }

    QCryptographicHash hash(QCryptographicHash::Sha256);

    // data for POST
    QJsonObject objMessage;
    objMessage["room_id"] = ui->lineEdit_3->text();

    if(ui->checkBox->isChecked()){
        hash.addData(ui->lineEdit->text().toStdString());
    } else{
        hash.addData("");
    }

    objMessage["room_password_sha256"] = (QString)hash.result().toHex();

    QJsonDocument docMessage(objMessage);
    QString message_data = docMessage.toJson().toHex();

    //std::wstring command = QString("/C python config/cryptographic_tool.exe encrypt_aes_server \"" + room_id + "\" \"" + message_data + "\"").toStdWString();
    std::wstring command = QString("/C python config/cryptographic_tool.py encrypt_aes_server \"" + room_id + "\" \"" + message_data + "\"").toStdWString();

    // encrypt
    ThreadFunctions shellThread;
    shellThread.operation = 2;  // Thread func
    shellThread.ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    shellThread.ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    shellThread.ShExecInfo.hwnd = NULL;
    shellThread.ShExecInfo.lpVerb = L"open";
    shellThread.ShExecInfo.lpFile = L"cmd.exe";
    shellThread.ShExecInfo.lpParameters = command.c_str();
    shellThread.ShExecInfo.lpDirectory = QDir::currentPath().toStdWString().c_str();
    shellThread.ShExecInfo.nShow = SW_HIDE;
    shellThread.ShExecInfo.hInstApp = NULL;

    shellThread.start();

    // wait for thread to complete
    while(shellThread.isRunning()){
        qApp->processEvents();
    }

    QByteArray content = readTempFile("encrypted_message");

    if (content.isEmpty()){
        RoomDialog::disable_widgets(false);
        return;
    }

    QJsonObject objData;
    objData["data"] = (QString)content;
    QJsonDocument docData(objData);
    QByteArray CreateRoomData = docData.toJson();

    QNetworkRequest request;
    QUrl qurl_address = QUrl(server_url + "/create-room");

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::UserAgentHeader, RoomDialog::user_agent);

    if(ui->checkBox->isChecked()){
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

    QStringList names;
    names.append("status_code");
    names.append("room_aes_key");

    QStringList responseData = getJson(names, response);

    if (responseData.isEmpty() || responseData[0] == "5"){
        QMessageBox::critical(this, "Chyba - symetrický klíč", "Server byl pravděpodobně restartován a kvůli tomu máte starý symetrický klíč. Restartuje program pro získání aktuálního.");

        RoomDialog::disable_widgets(false);
        return;

    } else if (responseData[0] != "1"){
        QMessageBox::critical(this, "Chyba", "Nastala neznámá chyba! Server odpověděl {\"status_code\": " + responseData[0] + "}");

        RoomDialog::disable_widgets(false);
        return;
    }

    QByteArray roomAesKey = QByteArray::fromHex(responseData[1].toUtf8());


    // write decrypted room's AES key to new file
    QFile decryptedSymetricKeyRoom(QDir::tempPath() + "/" + room_id + "/symetric_key_room");
    decryptedSymetricKeyRoom.open(QIODevice::WriteOnly);
    decryptedSymetricKeyRoom.write(roomAesKey);
    decryptedSymetricKeyRoom.close();

    QMessageBox::information(this, "Oznámení", "Místnost byla úspěšně vytvořena");

    RoomDialog::successful = true;
    RoomDialog::username = nickname;

    this->close();
}

void RoomDialog::joinRoomFunc()
{
    RoomDialog::disable_widgets(true);

    QString nickname = ui->lineEdit_4->text().trimmed();
    QString roomId = ui->lineEdit_3->text().trimmed();


    if(roomId == ""){
        QMessageBox::critical(this, "Chyba", "Pole pro ID místnosti nemůže být prázdné!");

        RoomDialog::disable_widgets(false);
        ui->lineEdit_3->setFocus();
        return;

    } else if (roomId.length() != 32){
        QMessageBox::critical(this, "Neplatné ID", "ID místnosti musí mít délku přesně 32 znaků!");

        RoomDialog::disable_widgets(false);
        ui->lineEdit_3->setFocus();
        return;

    } else if (nickname == ""){
        QMessageBox::critical(this, "Chyba", "Pole pro jméno nemůže být prázdné!");

        RoomDialog::disable_widgets(false);
        ui->lineEdit_4->setFocus();
        return;
    }

    QJsonObject objMessage;
    objMessage["room_id"] = roomId;
    objMessage["room_password"] = ui->lineEdit_4->text().trimmed();

    if(ui->checkBox->isChecked()){
        RoomDialog::room_password = ui->lineEdit->text();
        objMessage["room_password"] = RoomDialog::room_password;

    } else{
        RoomDialog::room_password = "";
        objMessage["room_password"] = "";
    }

    QJsonDocument docMessage(objMessage);
    QString message_data = docMessage.toJson().toHex();

    //std::wstring command = QString("/C python config/cryptographic_tool.exe encrypt_aes_server \"" + room_id + "\" \"" + message_data + "\"").toStdWString();
    std::wstring command = QString("/C python config/cryptographic_tool.py encrypt_aes_server \"" + room_id + "\" \"" + message_data + "\"").toStdWString();

    // encrypt
    ThreadFunctions shellThread;
    shellThread.operation = 2;  // Thread func
    shellThread.ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    shellThread.ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    shellThread.ShExecInfo.hwnd = NULL;
    shellThread.ShExecInfo.lpVerb = L"open";
    shellThread.ShExecInfo.lpFile = L"cmd.exe";
    shellThread.ShExecInfo.lpParameters = command.c_str();
    shellThread.ShExecInfo.lpDirectory = QDir::currentPath().toStdWString().c_str();
    shellThread.ShExecInfo.nShow = SW_HIDE;
    shellThread.ShExecInfo.hInstApp = NULL;

    shellThread.start();

    // wait for thread to complete
    while(shellThread.isRunning()){
        qApp->processEvents();
    }

    QByteArray content = readTempFile("encrypted_message");

    if (content.isEmpty()){
        RoomDialog::disable_widgets(false);
        return;
    }

    QJsonObject objData;
    objData["data"] = (QString)content;
    QJsonDocument docData(objData);
    QByteArray JoinRoomData = docData.toJson();

    QNetworkRequest request;
    QUrl qurl_address = QUrl(server_url + "/join-room");

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::UserAgentHeader, RoomDialog::user_agent);

    if(ui->checkBox->isChecked()){
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

    QStringList responseData = getJson(names, response);

    if(responseData.isEmpty() || responseData[0] == "5"){
        QMessageBox::critical(this, "Chyba", "Server byl pravděpodobně restartován a kvůli tomu máte starý symetrický klíč. Restartuje program pro získání nového.");

        RoomDialog::disable_widgets(false);
        return;
    }

    if (responseData[0] == "2"){
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

    QByteArray roomAesKey = QByteArray::fromHex(responseData[1].toUtf8());


    // write decrypted room's AES key to new file
    QFile decryptedSymetricKeyRoom(QDir::tempPath() + "/" + room_id + "/symetric_key_room");
    decryptedSymetricKeyRoom.open(QIODevice::WriteOnly);
    decryptedSymetricKeyRoom.write(roomAesKey);
    decryptedSymetricKeyRoom.close();

    QMessageBox::information(this, "Oznámení", "Připojení do místnosti proběhlo úspěšně");

    RoomDialog::successful = true;
    RoomDialog::username = nickname;

    // folder with new id
    QDir roomFolder(QDir::tempPath() + "/" + roomId);

    if (roomFolder.exists()){
        // delete folder with files
        roomFolder.removeRecursively();
    }

    roomFolder.rename(QDir::tempPath() + "/" + RoomDialog::room_id, QDir::tempPath() + "/" + roomId);

    RoomDialog::room_id = roomId;

    this->close();
}

QByteArray RoomDialog::readTempFile(QString filename){

    QFile file(QDir::tempPath() + "/" + room_id + "/" + filename);
    QByteArray content;

    if(!file.exists()){
        // python did not create the file
        return content;

    } else{
        file.open(QIODevice::ReadOnly);
        content = file.readAll();
        file.close();
    }

    return content;
}

void RoomDialog::writeTempFile(QString filename, QByteArray content){

    QFile file(QDir::tempPath() + "/" + room_id + "/" + filename);

    file.open(QIODevice::WriteOnly);
    file.write(content);
    file.close();
}

QStringList RoomDialog::getJson(QStringList names, QByteArray data)
{
    QJsonDocument jsonResponse;
    QJsonObject jsonObject;
    QString jsonData;


    jsonResponse = QJsonDocument::fromJson(data);
    jsonObject = jsonResponse.object();
    jsonData = jsonObject["data"].toString();

    // write encrypted JSON to file for python
    RoomDialog::writeTempFile("encrypted_message", jsonData.toUtf8());


    //std::wstring command = QString("/C python config/cryptographic_tool.exe decrypt_aes_server \"" + room_id + "\"").toStdWString();
    std::wstring command = QString("/C python config/cryptographic_tool.py decrypt_aes_server \"" + room_id + "\"").toStdWString();

    // decrypt
    ThreadFunctions shellThread;
    shellThread.operation = 2;  // Thread func
    shellThread.ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    shellThread.ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    shellThread.ShExecInfo.hwnd = NULL;
    shellThread.ShExecInfo.lpVerb = L"open";
    shellThread.ShExecInfo.lpFile = L"cmd.exe";
    shellThread.ShExecInfo.lpParameters = command.c_str();
    shellThread.ShExecInfo.lpDirectory = QDir::currentPath().toStdWString().c_str();
    shellThread.ShExecInfo.nShow = SW_HIDE;
    shellThread.ShExecInfo.hInstApp = NULL;

    shellThread.start();

    // wait for thread to complete
    while(shellThread.isRunning()){
        qApp->processEvents();
    }

    QByteArray decrypted_data = RoomDialog::readTempFile("decrypted_message");

    if(decrypted_data.isEmpty()){
        return QStringList();
    }

    decrypted_data.replace("\'", "\"");

    jsonResponse = QJsonDocument::fromJson(decrypted_data);
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
        QString text = arg1;
        text.chop(1);

        ui->lineEdit_3->setText(text);

        QMessageBox::critical(this, "Upozornění", "Délka ID musí být přesně 32 znaků!");

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

        ui->pushButton->setText("Připojit se do místnosti");

        RoomDialog::createRoom = false;
        QMessageBox::information(this, "Oznámení", "Přepnuto na připojení do existující místnosti");

    } else{

        this->setWindowTitle("Vytvoření chatovací místnosti");

        ui->lineEdit_3->setDisabled(true);
        ui->lineEdit_3->setReadOnly(true);
        ui->lineEdit_3->setText(room_id);

        ui->pushButton->setText("Vytvořit místnost");

        RoomDialog::createRoom = true;
        QMessageBox::information(this, "Oznámení", "Přepnuto na vytváření nové místnosti");
    }
}


void RoomDialog::on_checkBox_clicked()
{
    if(ui->checkBox->isChecked()){
        ui->lineEdit->setHidden(false);

    } else{
        ui->lineEdit->setHidden(true);
    }
}

