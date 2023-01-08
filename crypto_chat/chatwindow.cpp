/*
Window for sending messages
*/

#include "chatwindow.h"
#include "ui_chatwindow.h"
#include "colordialog.h"
#include "namechangedialog.h"
#include "threadfunctions.h"
#include "intervaldialog.h"

#include <QMessageBox>
#include <QCloseEvent>
#include <QTime>
#include <QDir>
#include <QProcess>
#include <QClipboard>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>


ThreadFunctions refreshChatLoop;


ChatWindow::ChatWindow(QWidget *parent, QString server_url, QString user_name)
    : QMainWindow(parent)
    , ui(new Ui::ChatWindow)
{

    ChatWindow::server_url = server_url;
    ChatWindow::user_name = user_name;

    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/images/hacker.ico"));
    this->setWindowTitle(tr("crypto-chat  |  %1  |  %2").arg(server_url).arg(user_name));

    ui->label->setStyleSheet(tr("QLabel { background-color : %1 }").arg(user_color));
    ui->lineEdit->setFocus();

    QApplication::setQuitOnLastWindowClosed(true);

    this->show();
}


ChatWindow::~ChatWindow()
{
    delete ui;
}

void ChatWindow::closeEvent(QCloseEvent *bar)
{
    // Before application close

    this->hide();

    // open app as new process
    if(restart){
        ChatWindow::restart = false;
        QProcess::startDetached(QApplication::applicationFilePath());
    }

    refreshChatLoop.stopLoop();

    ChatWindow::quitMessage();

    QDir roomFolder(QDir::tempPath() + "/" + room_id);

    if(roomFolder.exists() && room_id != ""){
        roomFolder.removeRecursively();
    }

    this->close();

    if(bar != nullptr){
        bar->accept();
    }

    while(refreshChatLoop.isRunning()){
        // wait for thread to finish
        qApp->processEvents();
    }


    if(!restart){
        QApplication::quit();
    }
}

void ChatWindow::welcomeMessage()
{
    QString message = tr("%1 se připojil/a").arg(ChatWindow::user_name);
    ChatWindow::sendMessage("grey", QTime::currentTime().toString(), "", "Server", message);
}

void ChatWindow::quitMessage()
{
    QString message = tr("%1 se odpojil/a").arg(ChatWindow::user_name);
    ChatWindow::sendMessage("grey", QTime::currentTime().toString(), "", "Server", message, true);
}

void ChatWindow::startRefreshLoop()
{
    // start thread for getting new messages

    refreshChatLoop.authentication_required = ChatWindow::authentication_required;
    refreshChatLoop.authentication_username = ChatWindow::authentication_username;
    refreshChatLoop.authentication_password = ChatWindow::authentication_password;
    refreshChatLoop.room_id = ChatWindow::room_id;

    refreshChatLoop.room_password = room_password;
    refreshChatLoop.server_url = ChatWindow::server_url;
    refreshChatLoop.user_agent = ChatWindow::user_agent;
    refreshChatLoop.recievedMessagesCount = ChatWindow::serverMessagesCount;

    refreshChatLoop.operation = 3;
    refreshChatLoop.sleep_time = ChatWindow::refreshInterval;
    refreshChatLoop.ui = this->ui;
    refreshChatLoop.continueLoop = true;
    refreshChatLoop.start();
}

QStringList ChatWindow::getJson(QStringList names, QByteArray data)
{
    // decrypt encrypted json

    QJsonDocument jsonResponse;
    QJsonObject jsonObject;
    QString jsonData;


    jsonResponse = QJsonDocument::fromJson(data);
    jsonObject = jsonResponse.object();
    jsonData = jsonObject["data"].toString();


    //QString command = "/C config/cryptographic_tool.exe decrypt_aes_server " + room_id + " " + jsonData;
    QString command = "/C python config/cryptographic_tool.py decrypt_aes_server " + room_id + " " + jsonData;

    // decrypt
    QProcess process;
    process.start("cmd", QStringList(command));

    while(process.state() == QProcess::Running){
        qApp->processEvents();
    }

    QByteArray decryptedData = QByteArray::fromHex(process.readAllStandardOutput().trimmed());


    if(decryptedData.isEmpty() || decryptedData.contains("error")){
        return QStringList();
    }

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


void ChatWindow::sendMessage(QString color, QString time, QString prefix, QString nickname, QString message, bool exit)
{
    // send message to server

    ChatWindow::disable_widgets(true);

    QString messageText;  // user's message
    QString messageHtml;  // message in html format with color
    QString messageEncrypted;  // encrypted messageHtml with room symetric key (in hex)

    messageText = tr("(%1) %2 <%3>: %4").arg(time).arg(prefix).arg(nickname).arg(message.trimmed());
    messageHtml = tr("<span style=\"color:%1;\">%2</span>").arg(color).arg(messageText.toHtmlEscaped());

    //QString command = "/C config/cryptographic_tool.exe encrypt_aes_room " + room_id + " " + messageHtml.toUtf8().toHex();
    QString command = "/C python config/cryptographic_tool.py encrypt_aes_room " + room_id + " " + messageHtml.toUtf8().toHex();

    // encrypt with room symetric key
    QProcess process;
    process.start("cmd", QStringList(command));

    while(process.state() == QProcess::Running){
        qApp->processEvents();
    }

    messageEncrypted = process.readAllStandardOutput().trimmed();

    if (messageEncrypted.isEmpty()){

        if(!exit){
            QMessageBox::critical(this, "Upozornění", "Nepodařilo se zašifrovat zprávu! (odesílání zrušeno)\n\nChyba: " + process.readAllStandardError().trimmed());
            ChatWindow::disable_widgets(false);
        }
        return;
    }


    QJsonObject objMessage;
    objMessage["room_id"] = ChatWindow::room_id;
    objMessage["room_password"] = ChatWindow::room_password;
    objMessage["message"] = messageEncrypted;

    QJsonDocument docMessage(objMessage);
    QString postData = docMessage.toJson().toHex();  // in hex


    //command = "/C config/cryptographic_tool.exe encrypt_aes_server " + room_id + " " + postData;
    command = "/C python config/cryptographic_tool.py encrypt_aes_server " + room_id + " " + postData;

    // encrypt postData (json) with server symetric key
    process.start("cmd", QStringList(command));

    while(process.state() == QProcess::Running){
        qApp->processEvents();
    }

    QString encryptedData = process.readAllStandardOutput().trimmed();


    if (encryptedData.isEmpty()){

        if(!exit){
            QMessageBox::critical(this, "Upozornění", "Nepodařilo se zašifrovat data! (odesílání zrušeno)\n\nChyba: " + process.readAllStandardError().trimmed());
            ChatWindow::disable_widgets(false);
        }
        return;
    }

    QJsonObject objData;
    objData["data"] = encryptedData;
    QJsonDocument docData(objData);
    QByteArray SendMessageData = docData.toJson();

    QNetworkRequest request;
    QUrl qurl_address = QUrl(server_url + "/send-message");

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::UserAgentHeader, ChatWindow::user_agent);

    if(ChatWindow::authentication_required){
        // authentication

        qurl_address.setUserName(authentication_username);
        qurl_address.setPassword(authentication_password);
    }

    request.setUrl(qurl_address);
    QNetworkReply *reply_post = manager.post(request, SendMessageData);


    while (!reply_post->isFinished())
    {
        qApp->processEvents();
    }

    QByteArray response = reply_post->readAll();

    if(reply_post->error() == QNetworkReply::ConnectionRefusedError){

        if(!exit){
            QMessageBox::critical(this, "Chyba", "Nelze se připojit k internetu nebo server není dostupný! (odesílání zrušeno)");
            ChatWindow::disable_widgets(false);
        }
        return;

    } else if(reply_post->error() != QNetworkReply::NoError){
        // Any error

        if(!exit){
            QMessageBox::critical(this, "Odpověd serveru (chyba)", tr("Nastala neznámá chyba!\n\n%1").arg(reply_post->errorString()));
            ChatWindow::disable_widgets(false);
        }
        return;
    }


    QStringList names;
    names.append("status_code");

    QStringList responseData = getJson(names, response);

    if (responseData.isEmpty() || responseData[0] == "5"){

        if(!exit){
            QMessageBox::critical(this, "Chyba - symetrický klíč", "Server byl pravděpodobně restartován a kvůli tomu máte starý symetrický klíč. Pokud problém přetrvává, restartujte program.");

            //ChatWindow::roomNotExist();
            ChatWindow::disable_widgets(false);
        }
        return;

    } else if (responseData[0] == "4"){

        if(!exit){
            QMessageBox::critical(this, "Chyba - místnost byla smazána", "Místnost, ve které se nacházíte, již neexistuje! Odpojte se prosím.");

            ChatWindow::roomNotExist();
            ChatWindow::disable_widgets(false);
        }
        return;

    } else if (responseData[0] == "3"){

        if(!exit){
            QMessageBox::critical(this, "Chyba - heslo", "Heslo místnosti bylo změněno! Odpojte se a připojte znovu.");

            ChatWindow::roomNotExist();
            ChatWindow::disable_widgets(false);
        }
        return;

    } else if (responseData[0] != "1"){

        if(!exit){
            QMessageBox::critical(this, "Chyba", "Zprávu se nepodařilo odeslat! Server odpověděl {\"status_code\": " + responseData[0] + "}");
            ChatWindow::disable_widgets(false);
        }
        return;
    }

    if(!exit){
        ui->lineEdit->clear();
        ChatWindow::disable_widgets(false);
        ui->lineEdit->setFocus();

    } else{
        refreshChatLoop.stopLoop();
    }
}

void ChatWindow::disable_widgets(bool disable)
{
    ui->pushButton->setDisabled(disable);
    ui->pushButton_3->setDisabled(disable);
    ui->pushButton_4->setDisabled(disable);
    //ui->pushButton_5->setDisabled(disable);

    ui->lineEdit->setDisabled(disable);
    ui->lineEdit->setClearButtonEnabled(!disable);
}

void ChatWindow::roomNotExist()
{
    // func to disable chatting

    refreshChatLoop.stopLoop();
    ui->action_zpravy_2_1->setDisabled(true);
    ui->action_zpravy_4->setDisabled(true);
}


void ChatWindow::on_pushButton_4_clicked()
{
    // change color

    ColorDialog cd;
    cd.set_color(ChatWindow::user_color);
    cd.setModal(true);
    cd.exec();

    if (cd.change_color){
        user_color = cd.user_color;
    }

    ui->label->setStyleSheet(tr("QLabel { background-color : %1 }").arg(user_color));

    ui->lineEdit->setFocus();
}


void ChatWindow::on_pushButton_3_clicked()
{
    // change username

    NameChangeDialog nchd;
    nchd.set_name(user_name);
    nchd.setModal(true);
    nchd.exec();

    if(nchd.new_name != ""){
        ChatWindow::user_name = nchd.new_name;
        this->setWindowTitle(tr("crypto-chat  |  %1  |  %2").arg(server_url).arg(user_name));
    }

    if(nchd.prefix != ""){
        ChatWindow::prefix = nchd.prefix;
    }

    ui->lineEdit->setFocus();
}


void ChatWindow::on_pushButton_2_clicked()
{
    // room info

    QMessageBox::about(this, "Informace o místnosti", tr("Verze aplikace:  %1<br><br>URL:  %2<br>ID:  %3<br>Interval aktualizace chatu: %4s<br><br>Vaše přezdívka:  %5<br>Váš prefix: %6<br>Vaše barva:  %7").arg(app_version).arg(server_url).arg(room_id).arg(refreshInterval).arg(user_name).arg(prefix).arg(user_color));

    ui->lineEdit->setFocus();
}


void ChatWindow::on_action_zpravy_4_triggered()
{
    // refresh chat

    refreshChatLoop.stopLoop();

    while(refreshChatLoop.isRunning()){
        qApp->processEvents();
    }

    refreshChatLoop.reload();
    refreshChatLoop.start();

    ui->lineEdit->setFocus();
}


void ChatWindow::on_action_zpravy_3_triggered()
{
    // clear chat

    ui->textEdit->clear();
    ui->textEdit->insertHtml("<span style=\"color:grey;\">Chat byl vyčištěn ...</span><br></br>");
    ui->action_zpravy_1->setText("Počet zobrazených: 1");

    ui->lineEdit->setFocus();
}


void ChatWindow::on_action_room_3_triggered()
{
    // disconnect room

    ChatWindow::restart = true;
    this->hide();
    ChatWindow::closeEvent();
}


void ChatWindow::on_action_zpravy_3_1_triggered()
{
    // change font - increase

    QFont fontInfo = ui->textEdit->font();
    int pointSize = fontInfo.pointSize();

    if(pointSize < 20){
        fontInfo.setPointSize(pointSize + 1);
        ui->textEdit->setFont(fontInfo);
    }
}


void ChatWindow::on_action_zpravy_3_2_triggered()
{
    // change font - decrease

    QFont fontInfo = ui->textEdit->font();
    int pointSize = fontInfo.pointSize();

    if(pointSize > 7){
        fontInfo.setPointSize(pointSize - 1);
        ui->textEdit->setFont(fontInfo);
    }
}


void ChatWindow::on_action_zpravy_3_3_triggered()
{
    // reset font - default size 9

    QFont fontInfo = ui->textEdit->font();
    fontInfo.setPointSize(9);
    ui->textEdit->setFont(fontInfo);
}


void ChatWindow::on_action_zpravy_2_1_triggered()
{
    // change chat refresh interval

    IntervalDialog ind;
    ind.interval = ChatWindow::refreshInterval;
    ind.set_interval(ChatWindow::refreshInterval);
    ind.setModal(true);
    ind.exec();

    if(ChatWindow::refreshInterval != ind.interval){

        ChatWindow::refreshInterval = ind.interval;
        refreshChatLoop.sleep_time = ChatWindow::refreshInterval;
        refreshChatLoop.reload();
    }

    ui->lineEdit->setFocus();
}


void ChatWindow::on_action_room_2_triggered()
{
    // copy room ID to clipboard

    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(ChatWindow::room_id);

    QMessageBox::information(this, "Oznámení", "Zkopírováno do schránky");

    ui->lineEdit->setFocus();
}


void ChatWindow::on_action_advanced_1_triggered()
{
    // open source code of app

    ShellExecute(0, 0, L"https://github.com/RxiPland/crypto_chat", 0, 0, SW_HIDE);

    ui->lineEdit->setFocus();
}


void ChatWindow::on_pushButton_clicked()
{
    // send message

    QString message = ui->lineEdit->text().trimmed();

    if (message.isEmpty()){
        return;
    }

    ChatWindow::sendMessage(ChatWindow::user_color, QTime::currentTime().toString(), ChatWindow::prefix, ChatWindow::user_name, message);

    ui->lineEdit->setFocus();
}


void ChatWindow::on_lineEdit_returnPressed()
{
    ChatWindow::on_pushButton_clicked();
}
