#include "chatwindow.h"
#include "ui_chatwindow.h"
#include "logindialog.h"
#include "colordialog.h"
#include "namechangedialog.h"

ChatWindow::ChatWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ChatWindow)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/images/hacker.ico"));
    this->setWindowTitle("crypto-chat  |  přezdívka  |  místnost");
    this->setWindowFlags(windowFlags() &(~Qt::WindowMaximizeButtonHint));
    this->show();

    ui->label->setStyleSheet(tr("QLabel { background-color : %1 }").arg(user_color));

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
    cd.user_color = user_color;
    cd.setModal(true);
    cd.exec();
}


void ChatWindow::on_pushButton_3_clicked()
{
    NameChangeDialog nchd;
    nchd.setModal(true);
    nchd.exec();
}

