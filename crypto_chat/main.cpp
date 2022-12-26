#include "logindialog.h"
#include "createroomdialog.h"
#include "chatwindow.h"
#include "joinroomdialog.h"

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

    QString user_name;

    if(ld.create_room){
        // user choose to create room on server

        CreateRoomDialog crd(nullptr, server_url, ld.room_id);
        crd.app_version = app_version;
        crd.user_agent = user_agent;

        if(ld.authentication_required){
            crd.authentication_required = true;
            crd.authentication_username = ld.authentication_username;
            crd.authentication_password = ld.authentication_password;
        }

        crd.exec();

        if(!crd.created){
            QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
            return 0;
        }

        user_name = crd.username;

    } else{

        JoinRoomDialog jrd(nullptr, server_url);
        jrd.app_version = app_version;
        jrd.user_agent = user_agent;

        if(ld.authentication_required){
            jrd.authentication_required = true;
            jrd.authentication_username = ld.authentication_username;
            jrd.authentication_password = ld.authentication_password;
        }

        jrd.exec();

        if(!jrd.joined){
            QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
            return 0;
        }
    }




    ChatWindow chw(nullptr, server_url, user_name);
    chw.app_version = app_version;
    chw.user_agent = user_agent;
    chw.room_id = ld.room_id;
    chw.authentication_required = ld.authentication_required;
    chw.authentication_username = ld.authentication_username;
    chw.authentication_password = ld.authentication_password;
    chw.show();

    return a.exec();
}
