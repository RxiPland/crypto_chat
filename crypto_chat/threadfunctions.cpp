/*
Program for making threads
*/

#include "threadfunctions.h"
#include "ui_chatwindow.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDir>


ThreadFunctions::ThreadFunctions()
{

}

void ThreadFunctions::stopLoop()
{
    ThreadFunctions::continueLoop = false;
}

void ThreadFunctions::reload()
{
    ThreadFunctions::i = ThreadFunctions::sleep_time;
}

QList<QJsonValue> ThreadFunctions::getJson(QStringList names, QByteArray data)
{
    QJsonDocument jsonResponse;
    QJsonObject jsonObject;
    QString jsonData;


    data.replace("\'", "\"");
    jsonResponse = QJsonDocument::fromJson(data);
    jsonObject = jsonResponse.object();
    jsonData = jsonObject["data"].toString();

    // write encrypted JSON to file for python
    ThreadFunctions::writeTempFile("encrypted_message", QByteArray::fromStdString(jsonData.toStdString()));


    //std::wstring command = QString("/C python config/cryptographic_tool.exe decrypt_aes_server \"" + room_id + "\"").toStdWString();
    std::wstring command = QString("/C python config/cryptographic_tool.py decrypt_aes_server \"" + room_id + "\"").toStdWString();

    // decrypt
    SHELLEXECUTEINFO ShExecInfo;
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = L"open";
    ShExecInfo.lpFile = L"cmd.exe";
    ShExecInfo.lpParameters = command.c_str();
    ShExecInfo.lpDirectory = QDir::currentPath().toStdWString().c_str();
    ShExecInfo.nShow = SW_HIDE;
    ShExecInfo.hInstApp = NULL;

    ShellExecuteEx(&ShExecInfo);
    WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

    CloseHandle(ShExecInfo.hProcess);


    QByteArray decrypted_data = ThreadFunctions::readTempFile("decrypted_message");

    QList<QJsonValue> returnData;

    if(decrypted_data.isEmpty()){

        return returnData;
    }

    decrypted_data.replace("\'", "\"");

    jsonResponse = QJsonDocument::fromJson(decrypted_data);
    jsonObject = jsonResponse.object();

    int i;

    for(i=0; i<names.length(); i++){
        returnData.append(jsonObject[names[i]]);
    }

    /*
    qInfo() << "------------------------\n";
    qInfo() << returnData;
    qInfo() << "------------------------\n";
    */

    return returnData;
}

void ThreadFunctions::appendMessage(QString messageHtml)
{
    // add message to text edit
    ui->textEdit->append(messageHtml);

    // load number of messages as int
    int messagesNumber = ui->action_zpravy_1->text().split(" ").back().toInt();

    // increment and set back
    ui->action_zpravy_1->setText(tr("Počet zobrazených: %1").arg(messagesNumber + 1));
}


QByteArray ThreadFunctions::readTempFile(QString filename){

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

void ThreadFunctions::writeTempFile(QString filename, QByteArray content){

    QFile file(QDir::tempPath() + "/" + room_id + "/" + filename);

    file.open(QIODevice::WriteOnly);
    file.write(content);
    file.close();
}

void ThreadFunctions::getMessages()
{
    // checking new messages

    QJsonObject objMessage;
    objMessage["room_id"] = ThreadFunctions::room_id;
    objMessage["room_password_sha256"] = ThreadFunctions::room_password_sha256;
    objMessage["user_messages_count"] = ThreadFunctions::recievedMessagesCount;
    QJsonDocument docMessage(objMessage);
    QString postData = docMessage.toJson().toHex();  // in hex

    //command = QString("/C python config/cryptographic_tool.exe encrypt_aes_server \"" + room_id + "\" \"" + postData + "\"").toStdWString();
    std::wstring command = QString("/C python config/cryptographic_tool.py encrypt_aes_server \"" + room_id + "\" \"" + postData + "\"").toStdWString();


    // encrypt postData (json) with server symetric key
    SHELLEXECUTEINFO ShExecInfo;
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = L"open";
    ShExecInfo.lpFile = L"cmd.exe";
    ShExecInfo.lpParameters = command.c_str();
    ShExecInfo.lpDirectory = QDir::currentPath().toStdWString().c_str();
    ShExecInfo.nShow = SW_HIDE;
    ShExecInfo.hInstApp = NULL;

    ShellExecuteEx(&ShExecInfo);
    WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

    CloseHandle(ShExecInfo.hProcess);

    //QNetworkRequest request;
    QUrl endpointUrl = QUrl(server_url + "/get-messages");

    if(ThreadFunctions::authentication_required){
        // authentication

        endpointUrl.setUserName(authentication_username);
        endpointUrl.setPassword(authentication_password);
    }


    //command = QString("/C python config/cryptographic_tool.exe get_messages \"" + room_id + "\" \"" + endpointUrl.url() + "\"").toStdWString();
    command = QString("/C python config/cryptographic_tool.py get_messages \"" + room_id + "\" \"" + endpointUrl.url() + "\" \"" + QString::fromStdString(ThreadFunctions::user_agent.toStdString()) + "\"").toStdWString();

    ShExecInfo.lpParameters = command.c_str();
    //ShExecInfo.nShow = SW_HIDE;
    ShellExecuteEx(&ShExecInfo);
    WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

    CloseHandle(ShExecInfo.hProcess);


    QByteArray response = ThreadFunctions::readTempFile("encrypted_message");

    if (response.isEmpty()){
        return;
    }

    QStringList names;
    names.append("status_code");
    names.append("server_messages_count");
    names.append("messages");

    QList<QJsonValue> responseData = getJson(names, response);

    if (responseData.isEmpty()){
        return;
    }

    QString statusCode = responseData[0].toString();

    if (statusCode != "1"){
        return;
    }

    int messagesCount = responseData[1].toInt();

    if (messagesCount == 0){
        return;
    }

    ThreadFunctions::recievedMessagesCount = messagesCount;

    QStringList encryptedMessages = responseData[2].toVariant().toStringList();

    int j;
    for(j=0; j<encryptedMessages.size(); j++){

        ThreadFunctions::appendMessage(encryptedMessages[j]);
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
