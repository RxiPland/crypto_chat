/*
No longer used (DELETE)
*/

#include "createroomdialog.h"
#include "ui_createroomdialog.h"
#include "threadfunctions.h"

#include <QDir>
#include <QCloseEvent>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QNetworkReply>

CreateRoomDialog::CreateRoomDialog(QWidget *parent, QString server_url, QString room_id) :
    QDialog(parent),
    ui(new Ui::CreateRoomDialog)
{
    CreateRoomDialog::room_id = room_id;
    CreateRoomDialog::server_url = server_url;

    ui->setupUi(this);
    this->setWindowIcon(QIcon("://images/hacker.ico"));
    this->setWindowTitle("Vytvoření chatovací místnosti");

    ui->lineEdit->setHidden(true);

    ui->lineEdit_2->setDisabled(true);
    ui->lineEdit_3->setDisabled(true);

    ui->lineEdit_2->setText(server_url);
    ui->lineEdit_3->setText(room_id);

    ui->pushButton->setFocusPolicy(Qt::FocusPolicy::NoFocus);

}

CreateRoomDialog::~CreateRoomDialog()
{
    delete ui;
}

void CreateRoomDialog::closeEvent(QCloseEvent *bar)
{
    // Before application close

    if(deleleFolder && !created){
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

void CreateRoomDialog::disable_widgets(bool disable){
    // disable widgets during network requests, ...

    ui->pushButton->setDisabled(disable);
    ui->lineEdit->setDisabled(disable);
    ui->lineEdit_4->setDisabled(disable);

    ui->checkBox->setDisabled(disable);
}

QByteArray CreateRoomDialog::readTempFile(QString filename){

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

QString CreateRoomDialog::getJson(QString name, QByteArray data){

    QJsonDocument jsonResponse = QJsonDocument::fromJson(data);
    QJsonObject jsonObject = jsonResponse.object();
    QString jsonData = jsonObject[name].toString();

    return jsonData;
}

void CreateRoomDialog::on_checkBox_clicked()
{
    if(ui->checkBox->isChecked()){
        ui->lineEdit->setHidden(false);

    } else{
        ui->lineEdit->setHidden(true);
    }
}


void CreateRoomDialog::on_pushButton_clicked()
{
    CreateRoomDialog::disable_widgets(true);

    QString username = ui->lineEdit_4->text().trimmed();

    if(username == ""){
        QMessageBox::critical(this, "Chyba", "Pole pro jméno nemůže být prázdné!");

        CreateRoomDialog::disable_widgets(false);
        return;
    }


    // data for POST
    QJsonObject objMessage;
    objMessage["room_id"] = ui->lineEdit_3->text();

    if(ui->checkBox->isChecked()){
        objMessage["room_password"] = ui->lineEdit_4->text();
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
        CreateRoomDialog::disable_widgets(false);
        return;
    }

    QJsonObject objData;
    objData["data"] = (QString)content;
    QJsonDocument docData(objData);
    QByteArray CreateRoomData = docData.toJson();

    QNetworkRequest request;
    QUrl qurl_address = QUrl(server_url + "/create-room");

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::UserAgentHeader, CreateRoomDialog::user_agent);

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

        CreateRoomDialog::disable_widgets(false);
        return;
    }


    QString roomAesKeyEncryptedHex = getJson("room_aes_key", reponse);

    QFile encryptedMessage(QDir::tempPath() + "/" + room_id + "/encrypted_message");
    encryptedMessage.open(QIODevice::WriteOnly);
    encryptedMessage.write(roomAesKeyEncryptedHex.toStdString().c_str());
    encryptedMessage.close();

    if(!encryptedMessage.exists()){
        QMessageBox::critical(this, "Chyba", "Nepodařilo se zapsat zašifrovaný symetrický klíč do souboru!");

        CreateRoomDialog::disable_widgets(false);
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

    CreateRoomDialog::created = true;
    CreateRoomDialog::username = ui->lineEdit_4->text();

    this->close();
}


void CreateRoomDialog::on_lineEdit_4_textEdited(const QString &arg1)
{
    int textLength = arg1.length();

    if(textLength == 26){
        QString text = arg1;
        text.chop(1);

        ui->lineEdit_4->setText(text);

        QMessageBox::critical(this, "Upozornění", "Délka jména nemůže přesáhnout 25 znaků!");

    } else if (textLength == 0){
        ui->pushButton->setDisabled(true);

    } else{
        ui->pushButton->setDisabled(false);
    }
}


void CreateRoomDialog::on_lineEdit_4_returnPressed()
{
    CreateRoomDialog::on_pushButton_clicked();
}


void CreateRoomDialog::on_lineEdit_returnPressed()
{
    CreateRoomDialog::on_pushButton_clicked();
}

