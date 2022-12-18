#include "chatwindow.h"

#include <QApplication>

QString app_version = "v1.0.0";
QByteArray user_agent = "crypt-chat " + QByteArray::fromStdString(app_version.toStdString());

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(true);

    ChatWindow chw(nullptr, app_version, user_agent);
    chw.show();

    return a.exec();
}
