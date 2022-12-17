#include "threadfunctions.h"

ThreadFunctions::ThreadFunctions()
{

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
    }
}
