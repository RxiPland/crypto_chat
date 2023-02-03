/*
Main file for handling other windows
*/

#include "logindialog.h"
#include "chatwindow.h"
#include "roomdialog.h"

#include <QApplication>
#include <QFile>


QString app_version = "v1.1.0";
QByteArray user_agent = "crypto-chat " + app_version.toUtf8();


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(true);

    // login to crypto-chat server && obtain asymetric keys
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

    // create OR login to existing room (obtain room symetric key)
    RoomDialog rd(nullptr, ld.create_room, server_url, ld.room_id);
    rd.app_version = app_version;
    rd.user_agent = user_agent;
    rd.serverAesKeyHex = ld.serverAesKeyHex;
    rd.rsaPublicKeyPem = ld.rsaPublicKeyPem;
    rd.rsaPrivateKeyPemHex = ld.rsaPrivateKeyPemHex;

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

    // open chat window
    ChatWindow chw(nullptr, server_url, rd.username);
    chw.app_version = app_version;
    chw.user_agent = user_agent;
    chw.serverAesKeyHex = rd.serverAesKeyHex;
    chw.roomAesKeyHex = rd.roomAesKeyHex;

    chw.room_id = rd.room_id;
    chw.room_password = rd.room_password;
    chw.authentication_required = rd.authentication_required;
    chw.authentication_username = rd.authentication_username;
    chw.authentication_password = rd.authentication_password;
    chw.serverMessagesCount = rd.serverMessagesCount;
    chw.welcomeMessage();
    chw.startRefreshLoop();
    chw.show();

    return a.exec();
}
