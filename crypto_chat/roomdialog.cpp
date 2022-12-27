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

    QString username = ui->lineEdit_4->text().trimmed();

    if(username == ""){
        QMessageBox::critical(this, "Chyba", "Pole pro jméno nemůže být prázdné!");

        RoomDialog::disable_widgets(false);
        return;
    }


    // data for POST
    QJsonObject objMessage;
    objMessage["room_id"] = ui->lineEdit_3->text();

    if(ui->checkBox->isChecked()){
        objMessage["room_password"] = ui->lineEdit->text();
    } else{
        objMessage["room_password"] = "";
    }

    QJsonDocument docMessage(objMessage);
    QString message_data = docMessage.toJson().toHex();

    //std::wstring command = QString("/C python config/cryptography_tool.exe encrypt_aes_server \"" + message_data + "\"").toStdWString();
    std::wstring command = QString("/C python config/cryptography_tool.py encrypt_aes_server \"" + message_data + "\"").toStdWString();

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

    QByteArray reponse = reply_post->readAll();

    if(reply_post->error() != QNetworkReply::NoError){
        // Any error

        QMessageBox::critical(this, "Odpověd serveru (chyba)", tr("Nastala neznámá chyba!\nOznačení QNetworkReply chyby: %1\n\nOdpověd serveru: %2").arg(reply_post->error()).arg(reponse));

        RoomDialog::disable_widgets(false);
        return;
    }


    QString roomAesKeyEncryptedHex = getJson("room_aes_key", reponse);

    QFile encryptedMessage(QDir::tempPath() + "/" + room_id + "/encrypted_message");
    encryptedMessage.open(QIODevice::WriteOnly);
    encryptedMessage.write(roomAesKeyEncryptedHex.toStdString().c_str());
    encryptedMessage.close();

    if(!encryptedMessage.exists()){
        QMessageBox::critical(this, "Chyba", "Nepodařilo se zapsat zašifrovaný symetrický klíč do souboru!");

        RoomDialog::disable_widgets(false);
        return;
    }

    //command = QString("/C python config/cryptography_tool.exe decrypt_aes_server").toStdWString();
    command = QString("/C python config/cryptography_tool.py decrypt_aes_server").toStdWString();

    // decrypt room's AES key with server's AES key
    shellThread.ShExecInfo.lpParameters = command.c_str();
    shellThread.start();

    // wait for thread to complete
    while(shellThread.isRunning()){
        qApp->processEvents();
    }

    // read decrypted room's AES key from temp file
    QFile decryptedSymetricKey(QDir::tempPath() + "/" + room_id + "/decrypted_message");
    decryptedSymetricKey.open(QIODevice::ReadOnly);
    QByteArray roomAesKey = decryptedSymetricKey.readAll();
    decryptedSymetricKey.close();

    // write decrypted room's AES key to new file
    QFile decryptedSymetricKeyRoom(QDir::tempPath() + "/" + room_id + "/symetric_key_room");
    decryptedSymetricKeyRoom.open(QIODevice::WriteOnly);
    decryptedSymetricKeyRoom.write(roomAesKey.toStdString().c_str());
    decryptedSymetricKeyRoom.close();

    QMessageBox::information(this, "Oznámení", "Místnost byla úspěšně vytvořena");

    RoomDialog::successful = true;
    RoomDialog::username = ui->lineEdit_4->text();

    this->close();
}

void RoomDialog::joinRoomFunc()
{
    RoomDialog::disable_widgets(true);

    QString username = ui->lineEdit_4->text().trimmed();
    QString roomId = ui->lineEdit_3->text().trimmed();


    if(roomId == ""){
        QMessageBox::critical(this, "Chyba", "Pole pro ID místnosti nemůže být prázdné!");

        RoomDialog::disable_widgets(false);
        return;

    } else if (roomId.length() != 32){
        QMessageBox::critical(this, "Neplatné ID", "ID místnosti musí mít délku přesně 32 znaků!");

        RoomDialog::disable_widgets(false);
        return;

    } else if (username == ""){
        QMessageBox::critical(this, "Chyba", "Pole pro jméno nemůže být prázdné!");

        RoomDialog::disable_widgets(false);
        return;
    }



}

QByteArray RoomDialog::readTempFile(QString filename){

    QFile file(QDir::tempPath() + "/" + room_id + "/" + filename);
    QByteArray content;

    if(!file.exists()){
        // python did not create the file
        QMessageBox::critical(this, "Chyba", "Program nenašel soubor!");

    } else{
        file.open(QIODevice::ReadOnly);
        content = file.readAll();
        file.close();
    }

    return content;
}

QString RoomDialog::getJson(QString name, QByteArray data){

    QJsonDocument jsonResponse = QJsonDocument::fromJson(data);
    QJsonObject jsonObject = jsonResponse.object();
    QString jsonData = jsonObject[name].toString();

    return jsonData;
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
        QMessageBox::information(this, "Oznámení", "Úspěšně přepnuto na připojení do existující místnosti.");

    } else{

        this->setWindowTitle("Vytvoření chatovací místnosti");

        ui->lineEdit_3->setDisabled(true);
        ui->lineEdit_3->setReadOnly(true);
        ui->lineEdit_3->setText(room_id);

        ui->pushButton->setText("Vytvořit místnost");

        RoomDialog::createRoom = true;
        QMessageBox::information(this, "Oznámení", "Úspěšně přepnuto na vytváření nové místnosti.");

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

