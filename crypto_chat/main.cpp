#include "logindialog.h"
#include "createroomdialog.h"
#include "chatwindow.h"

#include <QApplication>


QString app_version = "v1.0.0";
QByteArray user_agent = "crypt-chat " + QByteArray::fromStdString(app_version.toStdString());

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(true);


    // login to crypto-chat server
    LoginDialog lw;
    lw.app_version = app_version;
    lw.user_agent = user_agent;
    lw.setModal(true);
    lw.exec();


    if(!lw.successful_login){
        // unsuccessful -> exit

        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return 0;
    }


    QString server_url = lw.server_url;

    if(lw.create_room){
        // user choose to create room on server

        CreateRoomDialog crd(nullptr, server_url);
        crd.app_version = app_version;
        crd.user_agent = user_agent;
        crd.exec();
    }

    // TODO: join room ...
    QString user_name;


    ChatWindow chw(nullptr, server_url, user_name);
    chw.app_version = app_version;
    chw.user_agent = user_agent;
    chw.authentication_required = lw.authentication_required;
    chw.authentication_username = lw.authentication_username;
    chw.authentication_password = lw.authentication_password;
    chw.show();

    return a.exec();
}
