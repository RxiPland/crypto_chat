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
#include <QProcess>
#include <QClipboard>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QTemporaryFile>
#include <QRegularExpression>


ThreadFunctions refreshChatLoop;


ChatWindow::ChatWindow(QWidget *parent, QString server_url, QString user_name)
    : QMainWindow(parent)
    , ui(new Ui::ChatWindow)
{

    ChatWindow::server_url = server_url;
    ChatWindow::user_name = user_name;

    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/images/hacker.ico"));
    this->setWindowTitle(tr("crypto-chat  |  %1  |  %2").arg(server_url, user_name));

    ui->label->setStyleSheet(tr("QLabel { background-color : %1 }").arg(user_color));
    ui->lineEdit->setFocus();
    ui->textBrowser->setOpenExternalLinks(true);

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
    qApp->setQuitOnLastWindowClosed(false);

    this->hide();

    refreshChatLoop.stopLoop();

    ChatWindow::quitMessage();


    this->close();

    if(bar != nullptr){
        bar->accept();
    }

    while(refreshChatLoop.isRunning()){
        // wait for thread to finish
        qApp->processEvents();
    }

    QApplication::quit();
}

void ChatWindow::welcomeMessage()
{
    QString message = tr("%1 se připojil/a").arg(ChatWindow::user_name);
    QString messageHtml = ChatWindow::makeHtmlMessage(message, "grey", QTime::currentTime().toString(), "", "Server");
    ChatWindow::sendMessage(messageHtml);
}

void ChatWindow::quitMessage()
{
    if (ui->pushButton->isEnabled()){
        QString message = tr("%1 se odpojil/a").arg(ChatWindow::user_name);
        QString messageHtml = ChatWindow::makeHtmlMessage(message, "grey", QTime::currentTime().toString(), "", "Server");
        ChatWindow::sendMessage(messageHtml, true);
    }
}

void ChatWindow::startRefreshLoop()
{
    // start thread for getting new messages

    refreshChatLoop.authentication_required = ChatWindow::authentication_required;
    refreshChatLoop.authentication_username = ChatWindow::authentication_username;
    refreshChatLoop.authentication_password = ChatWindow::authentication_password;

    refreshChatLoop.roomAesKeyHex = ChatWindow::roomAesKeyHex;
    refreshChatLoop.rsaPublicKeyPemHex = ChatWindow::rsaPublicKeyPemHex;
    refreshChatLoop.rsaPrivateKeyPemHex = ChatWindow::rsaPrivateKeyPemHex;
    refreshChatLoop.serverPublicKeyPemHex = ChatWindow::serverPublicKeyPemHex;

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


QList<QJsonValue> ChatWindow::decryptRsa(QStringList jsonKeys, QByteArray response)
{
    // decrypt data_rsa json

    QList<QJsonValue> returnData;

    QJsonDocument jsonResponse;
    QJsonObject jsonObject;
    QString dataHex;

    jsonResponse = QJsonDocument::fromJson(response);
    jsonObject = jsonResponse.object();
    dataHex = jsonObject["data_rsa"].toString();

    QString command = QString("/C cd ./config & cryptographic_tool.exe decrypt_rsa %1 %2").arg(ChatWindow::rsaPrivateKeyPemHex, dataHex);
    //QString command = QString("/C python config/cryptographic_tool.py decrypt_rsa %1 %2").arg(ChatWindow::rsaPrivateKeyPemHex, dataHex);

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

    for(i=0; i<jsonKeys.length(); i++){
        returnData.append(jsonObject[jsonKeys[i]]);
    }

    return returnData;
}


void ChatWindow::sendMessage(QString message, bool exit)
{
    // send message to server

    ChatWindow::disable_widgets(true);


    QString messageEncrypted;  // encrypted message with room symetric key (in hex)


    QTemporaryFile tempFile;
    tempFile.open();
    tempFile.write(message.toUtf8().toHex());
    tempFile.close();

    QString fileName = tempFile.fileName().split('/').back();

    QString command = QString("/C cd ./config & cryptographic_tool.exe encrypt_aes %1 %2 %3").arg(ChatWindow::roomAesKeyHex, "True", fileName);
    //QString command = QString("/C python config/cryptographic_tool.py encrypt_aes %1 %2 %3").arg(ChatWindow::roomAesKeyHex, "True", fileName);

    // encrypt message with room symetric key
    QProcess process;
    process.start("cmd", QStringList(command));

    while(process.state() == QProcess::Running){
        qApp->processEvents();
    }

    // get encrypted data
    tempFile.open();
    messageEncrypted = tempFile.readAll().trimmed();
    tempFile.close();


    // delete temp file
    tempFile.remove();

    if (messageEncrypted.isEmpty()){

        if(!exit){
            QMessageBox::critical(this, "Upozornění", "Nepodařilo se zašifrovat zprávu! (odesílání zrušeno)\n\nChyba: " + process.readAllStandardError().trimmed());
            ChatWindow::disable_widgets(false);
        }
        return;
    }


    command = QString("/C cd ./config & cryptographic_tool.exe generate_aes");
    //command = QString("/C python config/cryptographic_tool.py generate_aes");

    // generate symetric key
    process.start("cmd", QStringList(command));

    while(process.state() == QProcess::Running){
        qApp->processEvents();
    }

    QString tempSymetricKeyHex = process.readAllStandardOutput().trimmed();

    if (tempSymetricKeyHex.isEmpty()){
        if(!exit){
            QMessageBox::critical(this, "Upozornění", "Nepodařilo se vygenerovat dočasný symetrický klíč! (odesílání zrušeno)\n\nChyba: " + process.readAllStandardError().trimmed());
            ChatWindow::disable_widgets(false);
        }
        return;
    }


    QJsonObject objData;
    objData["room_id"] = ChatWindow::room_id;
    objData["room_password"] = ChatWindow::room_password;
    objData["symetric_key"] = tempSymetricKeyHex;

    QJsonDocument docData(objData);
    QString dataRsaHex = docData.toJson().toHex();  // in hex

    command = QString("/C cd ./config & cryptographic_tool.exe encrypt_rsa %1 %2").arg(ChatWindow::serverPublicKeyPemHex, dataRsaHex);
    //command = QString("/C python config/cryptographic_tool.py encrypt_rsa %1 %2").arg(ChatWindow::serverPublicKeyPemHex, dataRsaHex);

    // encrypt postData (json) with server symetric key
    process.start("cmd", QStringList(command));

    while(process.state() == QProcess::Running){
        qApp->processEvents();
    }

    // get encrypted data
    QString encryptedDataRsaHex = process.readAllStandardOutput().trimmed();

    if (encryptedDataRsaHex.isEmpty()){

        if(!exit){
            QMessageBox::critical(this, "Upozornění", "Nepodařilo se zašifrovat data! (odesílání zrušeno)\n\nChyba: " + process.readAllStandardError().trimmed());
            ChatWindow::disable_widgets(false);
        }
        return;
    }


    objData = QJsonObject();
    objData["message"] = messageEncrypted;

    docData = QJsonDocument(objData);
    QByteArray dataAesHex = docData.toJson().toHex();  // in hex


    QTemporaryFile tempFile2;
    tempFile2.open();
    tempFile2.write(dataAesHex);
    tempFile2.close();

    QString fileName2 = tempFile2.fileName().split('/').back();

    command = QString("/C cd ./config & cryptographic_tool.exe encrypt_aes %1 %2 %3").arg(tempSymetricKeyHex, "true", fileName2);
    //command = QString("/C python config/cryptographic_tool.py encrypt_aes %1 %2 %3").arg(tempSymetricKeyHex, "true", fileName2);

    // encrypt json with temp symetric key
    process.start("cmd", QStringList(command));

    while(process.state() == QProcess::Running){
        qApp->processEvents();
    }

    // get encrypted data
    tempFile2.open();
    QString encryptedDataAesHex = tempFile2.readAll().trimmed();
    tempFile2.close();

    // delete temp file
    tempFile2.remove();

    if (encryptedDataAesHex.isEmpty()){

        if(!exit){
            QMessageBox::critical(this, "Upozornění", "Nepodařilo se zašifrovat zprávu! (odesílání zrušeno)\n\nChyba: " + process.readAllStandardError().trimmed());
            ChatWindow::disable_widgets(false);
        }
        return;
    }


    // prepare json
    objData = QJsonObject();
    objData["rsa_pem"] = ChatWindow::rsaPublicKeyPemHex;
    objData["data_rsa"] = encryptedDataRsaHex;
    objData["data_aes"] = encryptedDataAesHex;
    docData = QJsonDocument(objData);
    QByteArray sendMessageData = docData.toJson();

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
    QNetworkReply *reply_post = manager.post(request, sendMessageData);


    while (!reply_post->isFinished())
    {
        qApp->processEvents();
    }

    QByteArray response = reply_post->readAll().trimmed();

    if(reply_post->error() == QNetworkReply::ConnectionRefusedError){

        if(!exit){
            QMessageBox::critical(this, "Chyba", "Nelze se připojit k internetu nebo server není dostupný! (odesílání zrušeno)");
            ChatWindow::disable_widgets(false);
        }
        return;

    } else if(reply_post->error() != QNetworkReply::NoError){
        // Any error

        if(!exit){
            QMessageBox::critical(this, "Odpověd serveru (chyba)", QString("Nastala neznámá chyba!\n\n%1").arg(reply_post->errorString()));
            ChatWindow::disable_widgets(false);
        }
        return;
    }


    QStringList jsonKeys;
    jsonKeys.append("status_code");

    QList<QJsonValue> decryptedData = ChatWindow::decryptRsa(jsonKeys, response);

    if (decryptedData.isEmpty()){

        if(!exit){
            QMessageBox::critical(this, "Chyba", "Nepodařilo se dešifrovat data! (RSA)");

            ChatWindow::disable_widgets(false);
            return;
        }
    }


    QString statusCode = decryptedData[0].toString();

    if (statusCode.isEmpty() || statusCode == "5"){

        if(!exit){
            QMessageBox::critical(this, "Chyba - symetrický klíč", "Symetrický klíč, který server obdržel, není validní!");

            //ChatWindow::roomNotExist();
            ChatWindow::disable_widgets(false);
        }
        return;

    } else if (statusCode == "4"){

        if(!exit){
            QMessageBox::critical(this, "Chyba - místnost byla smazána", "Místnost, ve které se nacházíte, již neexistuje! Odpojte se prosím.");

            ChatWindow::roomNotExist();
            ChatWindow::disable_widgets(false);
        }
        return;

    } else if (statusCode == "3"){

        if(!exit){
            QMessageBox::critical(this, "Chyba - heslo", "Heslo místnosti bylo změněno! Odpojte se a připojte znovu.");

            ChatWindow::roomNotExist();
            ChatWindow::disable_widgets(false);
        }
        return;

    } else if (statusCode != "1"){

        if(!exit){
            QMessageBox::critical(this, "Chyba", QString("Zprávu se nepodařilo odeslat! Server odpověděl {\"status_code\": %1}").arg(statusCode));
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

    ChatWindow::disable_widgets(true);

    refreshChatLoop.stopLoop();
    ui->action_room_1->setDisabled(true);
    ui->action_zpravy_2_1->setDisabled(true);
    ui->action_zpravy_4->setDisabled(true);
}

QString ChatWindow::makeHtmlMessage(QString message, QString color, QString time, QString prefix, QString nickname)
{
    // convert parameters to message HTML format
    // (hours:minutes:seconds) Prefix <Username>: Message

    QString messageBody;  // user's message
    QString messageHtml;  // message in html format with color

    QRegularExpression rx("(https?://)+(([0-9a-zA-Z_!~*'().&=+$%-]+: )?[0-9a-zA-Z_!~*'().&=+$%-]+@)?(([0-9]{1,3}\\.){3}[0-9]{1,3}|([0-9a-zA-Z_!~*'()-]+\\.)*([0-9a-zA-Z][0-9a-zA-Z-]{0,61})?[0-9a-zA-Z]\\.[a-zA-Z]{2,6})(:[0-9]{1,5})?((/?)|(/[0-9a-zA-Z_!~*'().;?:@&=+$,%#-]+)+/?)($| )");
    QRegularExpressionMatchIterator iterator = rx.globalMatch(message);

    // search for valid url links
    QStringList foundUrls;
    while (iterator.hasNext()){
        QRegularExpressionMatch match = iterator.next();
        QString url = match.captured(0).trimmed();

        // do not allow duplicates
        if (!foundUrls.contains(url)){
            foundUrls << url;
        }
    }

    int i;

    if(foundUrls.length() > 0){

        // sort by length
        foundUrls.sort();
        std::reverse(foundUrls.begin(), foundUrls.end());

        // replace found urls with %i
        for(i=0; i<foundUrls.length(); i++){

            message.replace(foundUrls[i], '%' + QString::number(i+1));
        }
    }

    messageBody = QString("(%1) %2 <%3>: %4").arg(time, prefix, nickname, message.trimmed());
    messageHtml = QString("<span style=\"color:%1;\">%2</span>").arg(color, messageBody.toHtmlEscaped());

    // put links into HTML syntax
    for(i=0; i<foundUrls.length(); i++){
        messageHtml = QString(messageHtml).arg("<a href=\"%1\" target=\"_blank\">%1</a>").arg(foundUrls[i]);
    }

    return messageHtml;
}


void ChatWindow::on_pushButton_4_clicked()
{
    // change color

    ColorDialog cd;
    cd.set_color(ChatWindow::user_color);
    cd.setModal(true);
    cd.exec();

    if (cd.color_changed){
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


    if (nchd.nameChanged && nchd.prefixChanged){

        QString message = QString("(Server) Uživatel <%1> se přejmenoval na <%2>").arg(ChatWindow::user_name, nchd.newName).toHtmlEscaped();
        message += "<br>";
        message += QString("(Server) Uživatel <%1> si změnil prefix na %2").arg(nchd.newName, nchd.newPrefix).toHtmlEscaped();

        ChatWindow::sendMessage(message);

        ChatWindow::user_name = nchd.newName;
        ChatWindow::prefix = nchd.newPrefix;

        this->setWindowTitle(tr("crypto-chat  |  %1  |  %2").arg(ChatWindow::server_url, ChatWindow::user_name));

    } else{

        if (nchd.nameChanged){

            ChatWindow::sendMessage(QString("(Server) Uživatel <%1> se přejmenoval na <%2>").arg(ChatWindow::user_name, nchd.newName).toHtmlEscaped());

            ChatWindow::user_name = nchd.newName;
            this->setWindowTitle(tr("crypto-chat  |  %1  |  %2").arg(ChatWindow::server_url, ChatWindow::user_name));
        }

        if (nchd.prefixChanged){
            ChatWindow::sendMessage(QString("(Server) Uživatel <%1> si změnil prefix na %2").arg(ChatWindow::user_name, nchd.newPrefix).toHtmlEscaped());

            ChatWindow::prefix = nchd.newPrefix;
        }
    }

    ui->lineEdit->setFocus();
}


void ChatWindow::on_pushButton_2_clicked()
{
    // room info

    QString displayMessage = QString("Verze aplikace:  %1<br><br>URL:  %2<br>ID:  %3<br>Interval aktualizace chatu: %4s<br><br>Vaše přezdívka:  %5<br>Váš prefix: %6<br>Vaše barva:  %7");
    displayMessage = displayMessage.arg(app_version);
    displayMessage = displayMessage.arg(server_url);
    displayMessage = displayMessage.arg(room_id);
    displayMessage = displayMessage.arg(refreshInterval);
    displayMessage = displayMessage.arg(user_name);
    displayMessage = displayMessage.arg(prefix);
    displayMessage = displayMessage.arg(user_color);

    QMessageBox::about(this, "Informace o místnosti", displayMessage);

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

    ui->textBrowser->clear();
    ui->textBrowser->insertHtml("<span style=\"color:grey;\">Chat byl vyčištěn ...</span><br></br>");
    ui->action_zpravy_1->setText("Počet zobrazených: 1");

    ui->lineEdit->setFocus();
}


void ChatWindow::on_action_room_3_triggered()
{
    // disconnect room
    QProcess::startDetached(QApplication::applicationFilePath());

    this->close();
}


void ChatWindow::on_action_zpravy_3_1_triggered()
{
    // change font - increase

    QFont fontInfo = ui->textBrowser->font();
    int pointSize = fontInfo.pointSize();

    if(pointSize < 20){
        fontInfo.setPointSize(pointSize + 1);
        ui->textBrowser->setFont(fontInfo);
    }

    // move scrollbar to end
    QTextCursor c = ui->textBrowser->textCursor();
    c.movePosition(QTextCursor::End);
    ui->textBrowser->setTextCursor(c);
}


void ChatWindow::on_action_zpravy_3_2_triggered()
{
    // change font - decrease

    QFont fontInfo = ui->textBrowser->font();
    int pointSize = fontInfo.pointSize();

    if(pointSize > 7){
        fontInfo.setPointSize(pointSize - 1);
        ui->textBrowser->setFont(fontInfo);
    }

    // move scrollbar to end
    QTextCursor c = ui->textBrowser->textCursor();
    c.movePosition(QTextCursor::End);
    ui->textBrowser->setTextCursor(c);
}


void ChatWindow::on_action_zpravy_3_3_triggered()
{
    // reset font - default size 9

    QFont fontInfo = ui->textBrowser->font();
    fontInfo.setPointSize(9);
    ui->textBrowser->setFont(fontInfo);

    // move scrollbar to end
    QTextCursor c = ui->textBrowser->textCursor();
    c.movePosition(QTextCursor::End);
    ui->textBrowser->setTextCursor(c);
}


void ChatWindow::on_action_zpravy_2_1_triggered()
{
    // change refresh interval

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

    if (ui->pushButton->isEnabled()){

        QString message = ui->lineEdit->text().trimmed();

        if (message.isEmpty()){
            return;
        }

        QString messageHtml = ChatWindow::makeHtmlMessage(message, ChatWindow::user_color, QTime::currentTime().toString(), ChatWindow::prefix, ChatWindow::user_name);

        ChatWindow::sendMessage(messageHtml);

        ui->lineEdit->setFocus();
    }
}


void ChatWindow::on_lineEdit_returnPressed()
{
    ChatWindow::on_pushButton_clicked();
}

void ChatWindow::on_action_room_1_triggered()
{
    // delete room button

    ChatWindow::disable_widgets(true);

    QJsonObject objData;
    objData["room_id"] = ChatWindow::room_id;
    objData["room_password"] = ChatWindow::room_password;

    QJsonDocument docData(objData);
    QString dataRsaHex = docData.toJson().toHex();  // in hex

    QString command = QString("/C cd ./config & cryptographic_tool.exe encrypt_rsa %1 %2").arg(ChatWindow::serverPublicKeyPemHex, dataRsaHex);
    //QString command = QString("/C python config/cryptographic_tool.py encrypt_rsa %1 %2").arg(ChatWindow::serverPublicKeyPemHex, dataRsaHex);

    // encrypt data_rsa json
    QProcess process;
    process.start("cmd", QStringList(command));

    while(process.state() == QProcess::Running){
        qApp->processEvents();
    }

    // get encrypted data
    QString encryptedDataRsaHex = process.readAllStandardOutput().trimmed();

    if (encryptedDataRsaHex.isEmpty()){
        QMessageBox::critical(this, "Upozornění", "Nepodařilo se zašifrovat data! (odesílání požadavku zrušeno)\n\nChyba: " + process.readAllStandardError().trimmed());
        ChatWindow::disable_widgets(false);

        return;
    }

    objData = QJsonObject();
    objData["rsa_pem"] = ChatWindow::rsaPublicKeyPemHex;
    objData["data_rsa"] = encryptedDataRsaHex;
    docData = QJsonDocument(objData);
    QByteArray deleteRoomData = docData.toJson();

    QNetworkRequest request;
    QUrl qurl_address = QUrl(server_url + "/delete-room");

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::UserAgentHeader, ChatWindow::user_agent);

    if(ChatWindow::authentication_required){
        // authentication

        qurl_address.setUserName(authentication_username);
        qurl_address.setPassword(authentication_password);
    }

    request.setUrl(qurl_address);
    QNetworkReply *reply_post = manager.post(request, deleteRoomData);


    while (!reply_post->isFinished())
    {
        qApp->processEvents();
    }

    QByteArray response = reply_post->readAll();


    if(reply_post->error() == QNetworkReply::ConnectionRefusedError){


        QMessageBox::critical(this, "Chyba", "Nelze se připojit k internetu nebo server není dostupný! (odesílání požadavku zrušeno)");
        ChatWindow::disable_widgets(false);

        return;

    } else if(reply_post->error() != QNetworkReply::NoError){
        // Any error

        QMessageBox::critical(this, "Odpověd serveru (chyba)", tr("Nastala neznámá chyba!\n\n%1").arg(reply_post->errorString()));
        ChatWindow::disable_widgets(false);

        return;
    }


    QStringList jsonKeys;
    jsonKeys.append("status_code");

    QList<QJsonValue> responseData = ChatWindow::decryptRsa(jsonKeys, response);

    QString statusCode = responseData[0].toString();

    if(statusCode != "4"){
        QMessageBox::critical(this, "Odpověd serveru (chyba)", QString("Nastala neznámá chyba!\n\n%1").arg(response));
        ChatWindow::disable_widgets(false);

        return;

    } else{
        ChatWindow::roomNotExist();
        ui->textBrowser->insertHtml(QString("<br></br><span style=\"background-color: #ff0000;\"><br></br>%1</span>").arg(QString("(%1) Místnost byla smazána! Nebude možné již odesílat další zprávy.").arg(QTime::currentTime().toString()).toHtmlEscaped()));
        QMessageBox::information(this, "Smazání místnosti", "Místnost byla úspěšně smazána, ostatním uživatelům se zobrazí tato informace, jakmile se pokusí získat nové zprávy.");
    }
}
