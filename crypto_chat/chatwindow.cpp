#include "chatwindow.h"
#include "ui_chatwindow.h"
#include "logindialog.h"
#include "colordialog.h"

ChatWindow::ChatWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ChatWindow)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/images/hacker.ico"));
    this->setWindowTitle("crypto-chat  |  přezdívka  |  místnost");
    this->setWindowFlags(windowFlags() &(~Qt::WindowMaximizeButtonHint));
    this->show();

    // login to server
    LoginDialog lw;
    lw.setModal(true);
    lw.app_version = app_version;
    lw.user_agent = user_agent;
    lw.exec();

    if(!lw.successful_login){
        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return;
    }


    ui->lineEdit->setFocus();
}

ChatWindow::~ChatWindow()
{
    delete ui;
}


void ChatWindow::on_pushButton_4_clicked()
{
    ColorDialog cd;
    cd.setModal(true);
    cd.exec();
}

