/*
Program for making threads
*/

#include "threadfunctions.h"
#include "ui_chatwindow.h"

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

void ThreadFunctions::appendMessage(QString messageHtml)
{
    // add message to text edit
    ui->textEdit->append(messageHtml);

    // load number of messages as int
    int messagesNumber = ui->action_zpravy_1->text().split(" ").back().toInt();

    // increment and set back
    ui->action_zpravy_1->setText(tr("Počet zobrazených: %1").arg(messagesNumber + 1));
}

void ThreadFunctions::getMessages()
{
    // checking new messages



    QStringList messages;

    int j;
    for(j=0; j<messages.size(); j++){
        ThreadFunctions::appendMessage(messages[j]);
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
            // sleep
            for(i=sleep_time; i>=0.0 && continueLoop; i -= 0.1){

                // sleep 1/10 of second
                QThread::msleep(100);
                ui->menuZpravy_2->menuAction()->setText(tr("Aktualizace za %1s").arg((int)i));
            }

            // request for messages
            ThreadFunctions::getMessages();

        } while(continueLoop);

        ui->menuZpravy_2->menuAction()->setText("Aktualizace za Nikdy");
    }
}
