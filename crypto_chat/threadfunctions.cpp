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
#include <QTemporaryFile>


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


QList<QJsonValue> ThreadFunctions::decryptRsa(QStringList jsonKeys, QByteArray response)
{
    // decrypt data_rsa json

    QList<QJsonValue> returnData;

    QJsonDocument jsonResponse;
    QJsonObject jsonObject;
    QString dataHex;

    jsonResponse = QJsonDocument::fromJson(response);
    jsonObject = jsonResponse.object();
    dataHex = jsonObject["data_rsa"].toString();

    QString command = QString("/C cd ./config & cryptographic_tool.exe decrypt_rsa %1 %2").arg(ThreadFunctions::rsaPrivateKeyPemHex, dataHex);
    //QString command = QString("/C python config/cryptographic_tool.py decrypt_rsa %1 %2").arg(ThreadFunctions::rsaPrivateKeyPemHex, dataHex);

    // decrypt
    QProcess process;
    process.start("cmd", QStringList(command));
    process.waitForFinished(-1); // will wait forever until finished

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


QList<QJsonValue> ThreadFunctions::decryptAes(QStringList jsonKeys, QString symetricKeyHex, QByteArray response)
{
    // decrypt data_aes json

    QList<QJsonValue> returnData;

    QJsonDocument jsonResponse;
    QJsonObject jsonObject;
    QString dataHex;

    jsonResponse = QJsonDocument::fromJson(response);
    jsonObject = jsonResponse.object();
    dataHex = jsonObject["data_aes"].toString();


    QTemporaryFile tempFile;
    tempFile.open();
    tempFile.write(dataHex.toUtf8());
    tempFile.close();

    QString fileName = tempFile.fileName().split('/').back();

    QString command = QString("/C cd ./config & cryptographic_tool.exe decrypt_aes %1 %2 %3").arg(symetricKeyHex, "true", fileName);
    //QString command = QString("/C python config/cryptographic_tool.py decrypt_aes %1 %2 %3").arg(symetricKeyHex, "true", fileName);

    // decrypt
    QProcess process;
    process.start("cmd", QStringList(command));
    process.waitForFinished(-1); // will wait forever until finished

    // get encrypted data
    tempFile.open();
    QByteArray output = tempFile.readAll().trimmed();
    tempFile.close();

    // delete temp file
    tempFile.remove();


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


void ThreadFunctions::appendMessage(QString messageHtml)
{

    // add message to text edit
    QObject signalSource;
    QObject::connect(&signalSource, &QObject::destroyed, qApp, [=](QObject *){

        // load number of messages as int
        int messagesNumber = ui->action_zpravy_1->text().split(" ").back().toInt();

        // move scrollbar to end
        QTextCursor c = ui->textBrowser->textCursor();
        c.movePosition(QTextCursor::End);
        ui->textBrowser->setTextCursor(c);


        if (messagesNumber == 0){
            // add first message without new line at front

            ui->textBrowser->insertHtml(QString("Touto místností prošlo od založení celkem %1 zpráv").arg(recievedMessagesCount-1).toHtmlEscaped() + "<br></br>");
            messagesNumber++;
        }

        c = ui->textBrowser->textCursor();
        c.movePosition(QTextCursor::End);
        ui->textBrowser->setTextCursor(c);

        ui->textBrowser->insertHtml("<br></br>" + messageHtml);

        // increment and set back
        ui->action_zpravy_1->setText(QString("Počet zobrazených: %1").arg(messagesNumber + 1));


        // move scrollbar to end
        c = ui->textBrowser->textCursor();
        c.movePosition(QTextCursor::End);
        ui->textBrowser->setTextCursor(c);
    });

    ThreadFunctions::recievedMessagesCount += 1;
}


QString ThreadFunctions::decryptMessage(QString encryptedMessageHex)
{
    QByteArray decryptedMessage;


    QTemporaryFile tempFile;
    tempFile.open();
    tempFile.write(encryptedMessageHex.trimmed().toUtf8());
    tempFile.close();

    QString fileName = tempFile.fileName().split('/').back();

    QString command = QString("/C cd ./config & cryptographic_tool.exe decrypt_aes %1 %2 %3").arg(ThreadFunctions::roomAesKeyHex, "True", fileName);
    //QString command = QString("/C python config/cryptographic_tool.py decrypt_aes %1 %2 %3").arg(ThreadFunctions::roomAesKeyHex, "True", fileName);


    QProcess process;
    process.start("cmd", QStringList(command));
    process.waitForFinished(-1); // will wait forever until finished


    // get decrypted data
    tempFile.open();
    decryptedMessage = tempFile.readAll().trimmed();
    tempFile.close();

    // delete temp file
    tempFile.remove();

    if(decryptedMessage.isEmpty()){
        return decryptedMessage;
    }

    decryptedMessage = QByteArray::fromHex(decryptedMessage);


    return decryptedMessage;
}


void ThreadFunctions::getMessages()
{
    // checking new messages

    QJsonObject objData;
    objData["room_id"] = ThreadFunctions::room_id;
    objData["room_password"] = ThreadFunctions::room_password;
    objData["user_messages_count"] = ThreadFunctions::recievedMessagesCount;
    QJsonDocument docData(objData);
    QString dataRsaHex = docData.toJson().toHex();  // in hex


    QString command = QString("/C cd ./config & cryptographic_tool.exe encrypt_rsa %1 %2").arg(ThreadFunctions::serverPublicKeyPemHex, dataRsaHex);
    //QString command = QString("/C python config/cryptographic_tool.py encrypt_rsa %1 %2").arg(ThreadFunctions::serverPublicKeyPemHex, dataRsaHex);

    // encrypt data_rsa json
    QProcess process;
    process.start("cmd", QStringList(command));
    process.waitForFinished(-1); // will wait forever until finished

    // get encrypted data
    QByteArray encryptedDataRsaHex = process.readAllStandardOutput().trimmed();


    objData = QJsonObject();
    objData["rsa_pem"] = ThreadFunctions::rsaPublicKeyPemHex;
    objData["data_rsa"] = (QString)encryptedDataRsaHex;
    docData = QJsonDocument(objData);
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

    QStringList jsonKeys;
    jsonKeys.append("status_code");
    jsonKeys.append("server_messages_count");
    jsonKeys.append("skipped_messages");
    jsonKeys.append("symetric_key");


    QList<QJsonValue> responseData = decryptRsa(jsonKeys, response);

    if (responseData.isEmpty()){

        ThreadFunctions::appendMessage(QString("<span style=\"background-color: #ff0000;\">%1</span>").arg(QString("(%1) Nepodařilo se dešifrovat data! (zpráva byla pravděpodobně moc dlouhá)").arg(QTime::currentTime().toString()).toHtmlEscaped()));
        return;
    }

    QString statusCode = responseData[0].toString();

    if (statusCode == "4"){
        ThreadFunctions::stopLoop();
        ThreadFunctions::appendMessage(QString("<span style=\"background-color: #ff0000;\"><br></br>%1</span>").arg(QString("(%1) Místnost byla smazána! Nebude možné již odesílat další zprávy.").arg(QTime::currentTime().toString()).toHtmlEscaped()));

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

    int skipped_messages = responseData[2].toInt();

    if (skipped_messages > 0){
        ThreadFunctions::recievedMessagesCount -= 1;
        ThreadFunctions::appendMessage(QString("(%1 zpráv bylo přeskočeno) ...").arg(skipped_messages).toHtmlEscaped());
    }

    // list of "encrypted" messages
    //QStringList encryptedMessages = responseData[3].toVariant().toStringList();
    QString tempSymetricKeyHex = responseData[3].toString();

    jsonKeys = QStringList();
    jsonKeys.append("messages");

    QList<QJsonValue> decryptedData = ThreadFunctions::decryptAes(jsonKeys, tempSymetricKeyHex, response);

    QStringList encryptedHexMessages = decryptedData[0].toVariant().toStringList();


    QString decryptedMessage;
    int j;

    for(j=0; j<encryptedHexMessages.size() && continueLoop; j++){

        decryptedMessage = ThreadFunctions::decryptMessage(encryptedHexMessages[j]);

        if (decryptedMessage.isEmpty()){
            int messagesNumber = ui->action_zpravy_1->text().split(" ").back().toInt();

            QString messageHtmlEscaped = (QString("(%1) Zprávu #%2 se nepodařilo dešifrovat!").arg(QTime::currentTime().toString()).arg(messagesNumber+1)).toHtmlEscaped();

            ThreadFunctions::appendMessage(QString("<span style=\"background-color: #ff0000;\">%1</span>").arg(messageHtmlEscaped));

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
        // get info if there are new messages

        do{
            ui->menuZpravy_2->menuAction()->setText("Probíhá aktualizace ...");

            // request for messages
            ThreadFunctions::getMessages();

            // sleep
            for(i=sleep_time; i>=0.0 && continueLoop; i -= 0.1){

                // sleep 1/10 of second
                QThread::msleep(100);

                // update text
                ui->menuZpravy_2->menuAction()->setText(QString("Aktualizace za %1s").arg((int)i));
            }

        } while(continueLoop);

        ui->menuZpravy_2->menuAction()->setText("Aktualizace za Nikdy");
    }
}
