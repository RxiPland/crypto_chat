#ifndef THREADFUNCTIONS_H
#define THREADFUNCTIONS_H

#include <QThread>
#include <QAction>
#include <windows.h>

#include "chatwindow.h"

class ThreadFunctions : public QThread
{
public:
    ThreadFunctions();
    void run();
    void stopLoop();
    void reload();


    float i;
    bool continueLoop = false;
    int operation = 3;  // 1=sleep; 2=shell; 3=sleep(infinite)

    float sleep_time = 1.0;  // sleep time in seconds
    SHELLEXECUTEINFO ShExecInfo = {0};  // shell parameters
    //QAction *actionObject;
    Ui::ChatWindow *ui;


    // for comunicating with server
    bool authentication_required = false;
    QString authentication_username;
    QString authentication_password;
    QString server_url;
    QString room_id;

private:
    void getMessages();
    void appendMessage(QString messageHtml);



};

#endif // THREADFUNCTIONS_H
