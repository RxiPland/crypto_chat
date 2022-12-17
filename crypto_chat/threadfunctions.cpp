#include "threadfunctions.h"

ThreadFunctions::ThreadFunctions()
{

}

void ThreadFunctions::stopLoop(){
    ThreadFunctions::continueLoop = false;
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
        // update count on message refresh

        int i;

        do{
            for(i=sleep_time; i>=0 && continueLoop; i--){

                QThread::sleep(1);
                actionObject->setText(tr("Aktualizace za %1s").arg(i));
                qInfo() << i;
            }

        }while(continueLoop);
    }
}
