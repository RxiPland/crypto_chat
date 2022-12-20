#include "threadfunctions.h"

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
            // sleep
            for(i=sleep_time; i>=0.0; i -= 0.1){

                if(!continueLoop){
                    break;
                }

                QThread::msleep(100);
                actionObject->setText(tr("Aktualizace za %1s").arg((int)i));
            }

            // TODO: get messages info

        }while(continueLoop);
    }
}
