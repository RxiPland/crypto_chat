/*
Program for making threads
*/

#include "threadfunctions.h"
#include "ui_chatwindow.h"

#include <QProcess>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDir>
#include <QScrollBar>
#include <QTime>


ThreadFunctions::ThreadFunctions()
{

}

void ThreadFunctions::stopLoop()
{
    ThreadFunctions::i = -1;
    ThreadFunctions::continueLoop = false;
}

void ThreadFunctions::reload()
{
    ThreadFunctions::continueLoop = true;
    ThreadFunctions::i = ThreadFunctions::sleep_time;
}

QList<QJsonValue> ThreadFunctions::getJson(QStringList keys, QByteArray data)
{
    QJsonDocument jsonResponse;
    QJsonObject jsonObject;
    QString jsonData;


    data.replace("\'", "\"");
    jsonResponse = QJsonDocument::fromJson(data);
    jsonObject = jsonResponse.object();
    jsonData = jsonObject["data"].toString();


    //QString command = "/C config/cryptographic_tool.exe decrypt_aes_server " + room_id + " " + jsonData;
    QString command = "/C python config/cryptographic_tool.py decrypt_aes_server " + room_id + " " + jsonData;

    // decrypt
    QProcess process;
    process.start("cmd", QStringList(command));
    process.waitForFinished(-1); // will wait forever until finished

    QList<QJsonValue> returnData;
    QByteArray decryptedData = process.readAllStandardOutput().trimmed();

    if (decryptedData.isEmpty()){
        return returnData;
    }

    decryptedData = QByteArray::fromHex(decryptedData);

    if(decryptedData.isEmpty() || decryptedData.contains("error")){
        return returnData;
    }

    decryptedData.replace("\'", "\"");

    jsonResponse = QJsonDocument::fromJson(decryptedData);
    jsonObject = jsonResponse.object();

    int i;

    for(i=0; i<keys.length(); i++){
        returnData.append(jsonObject[keys[i]]);
    }

    return returnData;
}

void ThreadFunctions::appendMessage(QString messageHtml)
{
    // load number of messages as int
    int messagesNumber = ui->action_zpravy_1->text().split(" ").back().toInt();

    // move scrollbar to end
    QTextCursor c = ui->textEdit->textCursor();
    c.movePosition(QTextCursor::End);
    ui->textEdit->setTextCursor(c);

    if (messagesNumber == 0){
        // add first message without new line at front
        ui->textEdit->insertHtml(messageHtml);

    } else {
        // add message to text edit
        ui->textEdit->insertHtml("<br></br>" + messageHtml);
    }


    // increment and set back
    ui->action_zpravy_1->setText(tr("Počet zobrazených: %1").arg(messagesNumber + 1));


    // move scrollbar to end
    c.movePosition(QTextCursor::End);
    ui->textEdit->setTextCursor(c);

    ThreadFunctions::recievedMessagesCount += 1;
}


QString ThreadFunctions::decryptMessage(QString encryptedMessage)
{
    QByteArray decryptedMessage;

    encryptedMessage = encryptedMessage.trimmed();

    //QString command = "/C config/cryptographic_tool.exe decrypt_aes_room " + room_id + " " + encryptedMessage;
    QString command = "/C python config/cryptographic_tool.py decrypt_aes_room " + room_id + " " + encryptedMessage;

    QProcess process;
    process.start("cmd", QStringList(command));
    process.waitForFinished(-1); // will wait forever until finished


    decryptedMessage = process.readAllStandardOutput().trimmed();

    if(decryptedMessage.isEmpty()){
        return decryptedMessage;

    }

    decryptedMessage = QByteArray::fromHex(decryptedMessage);

    if (decryptedMessage.contains("error")){

        return QString();

    } else{

        return decryptedMessage;
    }

}

void ThreadFunctions::getMessages()
{
    // checking new messages

    QJsonObject objMessage;
    objMessage["room_id"] = ThreadFunctions::room_id;
    objMessage["room_password"] = ThreadFunctions::room_password;
    objMessage["user_messages_count"] = ThreadFunctions::recievedMessagesCount;
    QJsonDocument docMessage(objMessage);
    QString postData = docMessage.toJson().toHex();  // in hex

    //QString = "/C config/cryptographic_tool.exe encrypt_aes_server " + room_id + " " + postData;
    QString command = "/C python config/cryptographic_tool.py encrypt_aes_server " + room_id + " " + postData;


    // encrypt postData (json) with server symetric key
    QProcess process;
    process.start("cmd", QStringList(command));
    process.waitForFinished(-1); // will wait forever until finished

    QByteArray encryptedData = process.readAllStandardOutput().trimmed();


    QJsonObject objData;
    objData["data"] = (QString)encryptedData;
    QJsonDocument docData(objData);
    QByteArray getMessageData = docData.toJson();

    // create url;
    QUrl endpointUrl = QUrl(server_url + "/get-messages");

    if(ThreadFunctions::authentication_required){
        // authentication

        endpointUrl.setUserName(authentication_username);
        endpointUrl.setPassword(authentication_password);
    }

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::UserAgentHeader, ThreadFunctions::user_agent);

    if(ThreadFunctions::authentication_required){
        // authentication

        endpointUrl.setUserName(authentication_username);
        endpointUrl.setPassword(authentication_password);
    }

    request.setUrl(endpointUrl);

    QNetworkAccessManager manager;
    QNetworkReply *reply_post = manager.post(request, getMessageData);


    while (!reply_post->isFinished())
    {
        qApp->processEvents();
    }

    QByteArray response = reply_post->readAll();

    if (reply_post->error() != QNetworkReply::NoError || response.isEmpty()){
        // network error
        return;
    }

    QStringList keys;
    keys.append("status_code");
    keys.append("server_messages_count");
    keys.append("messages");

    QList<QJsonValue> responseData = getJson(keys, response);

    if (responseData.isEmpty()){

        ThreadFunctions::appendMessage(tr("<span style=\"background-color: #ff0000;\">%1</span>").arg(QString("(%1) Nepodařilo se dešifrovat data! (zpráva byla pravděpodobně moc dlouhá)").arg(QTime::currentTime().toString()).toHtmlEscaped()));
        return;
    }

    QString statusCode = responseData[0].toString();

    if (statusCode == "4"){
        ThreadFunctions::stopLoop();
        ThreadFunctions::appendMessage(tr("<span style=\"background-color: #ff0000;\"><br></br>%1</span>").arg(QString("(%1) Místnost byla smazána! Nebude možné již odesílat další zprávy.").arg(QTime::currentTime().toString()).toHtmlEscaped()));

        ui->action_room_1->setDisabled(true);
        return;

    } else if (statusCode != "1"){
        // server error
        return;
    }

    int messagesCountServer = responseData[1].toInt();

    if (messagesCountServer == ThreadFunctions::recievedMessagesCount){
        // no new messages
        return;
    }

    //ThreadFunctions::recievedMessagesCount = messagesCountServer;

    QStringList encryptedMessages = responseData[2].toVariant().toStringList();

    QString decryptedMessage;
    int j;
    for(j=0; j<encryptedMessages.size() && continueLoop; j++){

        decryptedMessage = ThreadFunctions::decryptMessage(encryptedMessages[j]);

        if (decryptedMessage.isEmpty()){
            int messagesNumber = ui->action_zpravy_1->text().split(" ").back().toInt();

            QString messageHtmlEscaped = (tr("(%1) Zprávu #%2 se nepodařilo dešifrovat!").arg(QTime::currentTime().toString()).arg(messagesNumber+1)).toHtmlEscaped();

            ThreadFunctions::appendMessage(tr("<span style=\"background-color: #ff0000;\">%1</span>").arg(messageHtmlEscaped));

        } else{
            ThreadFunctions::appendMessage(decryptedMessage);
        }
    }
}

void ThreadFunctions::run()
{

    if(operation == 1){
        // "sleep" in thread

        QThread::sleep(sleep_time);

    } else if (operation == 2){
        // "shell execute" in thread

        ShellExecuteEx(&ShExecInfo);
        WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

        CloseHandle(ShExecInfo.hProcess);

    } else if (operation == 3){
        // update the count for the next message update
        // every 100ms
        // get info about new messages

        do{
            // request for messages
            ThreadFunctions::getMessages();

            // sleep
            for(i=sleep_time; i>=0.0 && continueLoop; i -= 0.1){

                // sleep 1/10 of second
                QThread::msleep(100);
                ui->menuZpravy_2->menuAction()->setText(tr("Aktualizace za %1s").arg((int)i));
            }

        } while(continueLoop);

        ui->menuZpravy_2->menuAction()->setText("Aktualizace za Nikdy");
    }
}
