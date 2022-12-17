#ifndef THREADFUNCTIONS_H
#define THREADFUNCTIONS_H

#include <QThread>
#include <QAction>
#include <windows.h>

class ThreadFunctions : public QThread
{
public:
    ThreadFunctions();
    void run();
    void stopLoop();

    int operation = 3;  // 1=sleep; 2=shell; 3=sleep(infinite)
    int sleep_time = 1;  // sleep time in seconds
    SHELLEXECUTEINFO ShExecInfo = {0};  // shell parameters

    QAction *actionObject;

    bool continueLoop = false;
};

#endif // THREADFUNCTIONS_H
