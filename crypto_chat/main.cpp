#include "chatwindow.h"
#include "logindialog.h"

#include <QApplication>

QByteArray user_agent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/108.0.0.0 Safari/537.36";
QString app_version = "v1.0.0";

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /*ChatWindow w;
    w.show();
    */
    LoginDialog ww;
    ww.app_version = app_version;
    ww.user_agent = user_agent;
    ww.show();

    return a.exec();
}
