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
#include <QCryptographicHash>
#include <QScrollBar>

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
    refreshChatLoop.authentication_required = ChatWindow::authentication_required;
    refreshChatLoop.authentication_username = ChatWindow::authentication_username;
    refreshChatLoop.authentication_password = ChatWindow::authentication_password;
    refreshChatLoop.room_id = ChatWindow::room_id;

    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(ChatWindow::room_password.toStdString());

    refreshChatLoop.room_password_sha256 = (QString)hash.result().toHex();
    refreshChatLoop.server_url = ChatWindow::server_url;
    refreshChatLoop.user_agent = ChatWindow::user_agent;

    refreshChatLoop.operation = 3;
    refreshChatLoop.sleep_time = ChatWindow::refreshInterval;
    refreshChatLoop.ui = this->ui;
    refreshChatLoop.continueLoop = true;
    refreshChatLoop.start();
}

QByteArray ChatWindow::readTempFile(QString filename){

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

void ChatWindow::writeTempFile(QString filename, QByteArray content){

    QFile file(QDir::tempPath() + "/" + room_id + "/" + filename);

    file.open(QIODevice::WriteOnly);
    file.write(content);
    file.close();
}

QStringList ChatWindow::getJson(QStringList names, QByteArray data)
{
    QJsonDocument jsonResponse;
    QJsonObject jsonObject;
    QString jsonData;


    jsonResponse = QJsonDocument::fromJson(data);
    jsonObject = jsonResponse.object();
    jsonData = jsonObject["data"].toString();

    // write encrypted JSON to file for python
    ChatWindow::writeTempFile("encrypted_message", jsonData.toUtf8());


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

    QByteArray decrypted_data = ChatWindow::readTempFile("decrypted_message");

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


void ChatWindow::sendMessage(QString color, QString time, QString prefix, QString nickname, QString message, bool silent)
{
    refreshChatLoop.stopLoop();

    while(refreshChatLoop.isRunning()){
        // wait for thread to finish
        qApp->processEvents();
    }

    // prepare for start
    refreshChatLoop.continueLoop = true;


    ChatWindow::disable_widgets(true);

    QString messageText;  // user's message
    QString messageHtml;  // message in html format with color
    QString messageEncrypted;  // encrypted messageHtml with room symetric key (in hex)

    messageText = tr("(%1) %2 <%3>: %4").arg(time).arg(prefix).arg(nickname).arg(message);
    messageHtml = tr("<span style=\"color:%1;\">%2</span>").arg(color).arg(messageText.toHtmlEscaped());

    //std::wstring command = QString("/C python config/cryptographic_tool.exe encrypt_aes_room \"" + room_id + "\" \"" + messageHtml.toUtf8().toHex() + "\"").toStdWString();
    std::wstring command = QString("/C python config/cryptographic_tool.py encrypt_aes_room \"" + room_id + "\" \"" + messageHtml.toUtf8().toHex() + "\"").toStdWString();

    // encrypt with room symetric key
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

    messageEncrypted = readTempFile("encrypted_message");

    if (messageEncrypted.isEmpty()){

        if(!silent){
            QMessageBox::critical(this, "Upozornění", "Nepodařilo se zašifrovat zprávu! (odesílání zrušeno)");
            ChatWindow::disable_widgets(false);
        }
        refreshChatLoop.start();
        return;
    }


    QJsonObject objMessage;
    objMessage["room_id"] = ChatWindow::room_id;
    objMessage["room_password"] = ChatWindow::room_password;
    objMessage["message"] = messageEncrypted;

    QJsonDocument docMessage(objMessage);
    QString postData = docMessage.toJson().toHex();  // in hex


    //command = QString("/C python config/cryptographic_tool.exe encrypt_aes_server \"" + room_id + "\" \"" + postData + "\"").toStdWString();
    command = QString("/C python config/cryptographic_tool.py encrypt_aes_server \"" + room_id + "\" \"" + postData + "\"").toStdWString();

    // encrypt postData (json) with server symetric key
    shellThread.ShExecInfo.lpParameters = command.c_str();
    shellThread.start();

    // wait for thread to complete
    while(shellThread.isRunning()){
        qApp->processEvents();
    }

    QByteArray content = readTempFile("encrypted_message");

    if (content.isEmpty()){

        if(!silent){
            QMessageBox::critical(this, "Upozornění", "Nepodařilo se zašifrovat data! (odesílání zrušeno)");
            ChatWindow::disable_widgets(false);
        }
        refreshChatLoop.start();
        return;
    }

    QJsonObject objData;
    objData["data"] = (QString)content;
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

        if(!silent){
            QMessageBox::critical(this, "Chyba", "Nelze se připojit k internetu nebo server není dostupný! (odesílání zrušeno)");
            ChatWindow::disable_widgets(false);
        }
        refreshChatLoop.start();
        return;

    } else if(reply_post->error() != QNetworkReply::NoError){
        // Any error

        if(!silent){
            QMessageBox::critical(this, "Odpověd serveru (chyba)", tr("Nastala neznámá chyba!\n\n%1").arg(reply_post->errorString()));
            ChatWindow::disable_widgets(false);
        }
        refreshChatLoop.start();
        return;
    }


    QStringList names;
    names.append("status_code");

    QStringList responseData = getJson(names, response);

    if (responseData.isEmpty() || responseData[0] == "5"){

        if(!silent){
            QMessageBox::critical(this, "Chyba - symetrický klíč", "Server byl pravděpodobně restartován a kvůli tomu máte starý symetrický klíč. Pokud problém přetrvává, restartujte program.");

            //ChatWindow::roomNotExist();
            ChatWindow::disable_widgets(false);
        }
        refreshChatLoop.start();
        return;

    } else if (responseData[0] == "4"){

        if(!silent){
            QMessageBox::critical(this, "Chyba - místnost byla smazána", "Místnost, ve které se nacházíte, již neexistuje! Odpojte se prosím.");

            ChatWindow::roomNotExist();
            ChatWindow::disable_widgets(false);
        }
        refreshChatLoop.start();
        return;

    } else if (responseData[0] == "3"){

        if(!silent){
            QMessageBox::critical(this, "Chyba - heslo", "Heslo místnosti bylo změněno! Odpojte se a připojte znovu.");

            ChatWindow::roomNotExist();
            ChatWindow::disable_widgets(false);
        }
        refreshChatLoop.start();
        return;

    } else if (responseData[0] != "1"){

        if(!silent){
            QMessageBox::critical(this, "Chyba", "Zprávu se nepodařilo odeslat! Server odpověděl {\"status_code\": " + responseData[0] + "}");
            ChatWindow::disable_widgets(false);
        }
        refreshChatLoop.start();
        return;
    }

    if(!silent){
        ui->lineEdit->clear();
        ChatWindow::disable_widgets(false);
        ui->lineEdit->setFocus();
    }

    refreshChatLoop.start();
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
}


void ChatWindow::on_pushButton_2_clicked()
{
    QMessageBox::about(this, "Informace o místnosti", tr("Verze aplikace:  %1<br><br>URL:  %2<br>ID:  %3<br>Interval aktualizace chatu: %4s<br><br>Vaše přezdívka:  %5<br>Váš prefix: %6<br>Vaše barva:  %7").arg(app_version).arg(server_url).arg(room_id).arg(refreshInterval).arg(user_name).arg(prefix).arg(user_color));
}


void ChatWindow::on_action_zpravy_4_triggered()
{
    refreshChatLoop.reload();
}


void ChatWindow::on_action_zpravy_3_triggered()
{
    ui->textEdit->clear();
    ui->textEdit->append("<span style=\"color:grey;\">Chat byl vyčištěn ...<br></span>");
    ui->action_zpravy_1->setText("Počet zobrazených: 1");
}


void ChatWindow::on_action_room_3_triggered()
{
    ChatWindow::restart = true;
    this->hide();
    ChatWindow::closeEvent();
}


void ChatWindow::on_action_zpravy_3_1_triggered()
{
    QFont fontInfo = ui->textEdit->font();
    int pointSize = fontInfo.pointSize();

    if(pointSize < 20){
        fontInfo.setPointSize(pointSize + 1);
        ui->textEdit->setFont(fontInfo);
    }
}


void ChatWindow::on_action_zpravy_3_2_triggered()
{
    QFont fontInfo = ui->textEdit->font();
    int pointSize = fontInfo.pointSize();

    if(pointSize > 7){
        fontInfo.setPointSize(pointSize - 1);
        ui->textEdit->setFont(fontInfo);
    }
}


void ChatWindow::on_action_zpravy_3_3_triggered()
{
    QFont fontInfo = ui->textEdit->font();
    fontInfo.setPointSize(9);
    ui->textEdit->setFont(fontInfo);
}


void ChatWindow::on_action_zpravy_2_1_triggered()
{
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
}


void ChatWindow::on_action_room_2_triggered()
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(ChatWindow::room_id);

    QMessageBox::information(this, "Oznámení", "Zkopírováno do schránky");
}


void ChatWindow::on_action_advanced_1_triggered()
{
    ShellExecute(0, 0, L"https://github.com/RxiPland/crypto_chat", 0, 0, SW_HIDE);
}


void ChatWindow::on_pushButton_clicked()
{

    QString message = ui->lineEdit->text().trimmed();

    if (message.isEmpty()){
        return;
    }

    ChatWindow::sendMessage(ChatWindow::user_color, QTime::currentTime().toString(), ChatWindow::prefix, ChatWindow::user_name, message);
}


void ChatWindow::on_lineEdit_returnPressed()
{
    ChatWindow::on_pushButton_clicked();
}

void ChatWindow::on_textEdit_textChanged()
{
    ui->textEdit->horizontalScrollBar()->setValue(ui->textEdit->horizontalScrollBar()->maximum());
}

