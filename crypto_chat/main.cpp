#include "logindialog.h"
#include "chatwindow.h"
#include "roomdialog.h"

#include <QApplication>
#include <QFile>


QString app_version = "v1.0.0";
QByteArray user_agent = "crypto-chat " + QByteArray::fromStdString(app_version.toStdString());


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(true);

    // login to crypto-chat server && obtain server AES key
    LoginDialog ld;
    ld.app_version = app_version;
    ld.user_agent = user_agent;
    ld.setModal(true);
    ld.exec();

    if(!ld.successful_login){
        // unsuccessful -> exit

        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return 0;
    }


    QString server_url = ld.server_url;

    RoomDialog rd(nullptr, ld.create_room, server_url, ld.room_id);
    rd.app_version = app_version;
    rd.user_agent = user_agent;

    if(ld.authentication_required){
        rd.authentication_required = true;
        rd.authentication_username = ld.authentication_username;
        rd.authentication_password = ld.authentication_password;
    }

    rd.exec();

    if(!rd.successful){
        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return 0;
    }


    ChatWindow chw(nullptr, server_url, rd.username);
    chw.app_version = app_version;
    chw.user_agent = user_agent;
    chw.room_id = ld.room_id;
    chw.authentication_required = ld.authentication_required;
    chw.authentication_username = ld.authentication_username;
    chw.authentication_password = ld.authentication_password;
    chw.show();

    return a.exec();
}
