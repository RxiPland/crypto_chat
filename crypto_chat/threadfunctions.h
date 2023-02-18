#ifndef THREADFUNCTIONS_H
#define THREADFUNCTIONS_H

#include <QThread>
#include "chatwindow.h"


#include <QNetworkAccessManager>
#include <windows.h>
#include <QJsonValueRef>
#include <QNetworkReply>

class ThreadFunctions : public QThread
{

public:
    ThreadFunctions();
    void run();
    void stopLoop();
    void reload();

    void (*appendMessageChat) = NULL;

    float i;
    bool continueLoop = false;
    int operation = 3;  // 1=sleep; 2=shell; 3=sleep(infinite)

    float sleep_time = 1.0;  // sleep time in seconds
    SHELLEXECUTEINFO ShExecInfo = {0};  // shell parameters
    Ui::ChatWindow *ui;

    int recievedMessagesCount = 0;

    // for comunicating with server
    bool authentication_required = false;
    QString authentication_username = "";
    QString authentication_password = "";

    QString rsaPublicKeyPemHex;
    QString rsaPrivateKeyPemHex;
    QString serverPublicKeyPemHex;

    QString roomAesKeyHex;

    QByteArray user_agent;
    QString server_url;
    QString room_id;
    QString room_password;


private:
    void getMessages();

    void appendMessage(QString messageHtml);
    void writeTempFile(QString filename, QByteArray content);
    QList<QJsonValue> decryptRsa(QStringList jsonKeys, QByteArray response);
    QList<QJsonValue> decryptAes(QStringList jsonKeys, QString symetricKeyHex, QByteArray response);
    QByteArray readTempFile(QString filename);

    QString decryptMessage(QString encryptedMessage);


};

#endif // THREADFUNCTIONS_H
