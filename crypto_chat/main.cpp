#include "chatwindow.h"
#include "logindialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /*ChatWindow w;
    w.show();
    */
    LoginDialog ww;
    ww.show();

    return a.exec();
}
