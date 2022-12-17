#include "chatwindow.h"
#include "ui_chatwindow.h"
#include "logindialog.h"
#include "colordialog.h"
#include "namechangedialog.h"

ChatWindow::ChatWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ChatWindow)
{

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

    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/images/hacker.ico"));
    this->setWindowTitle("crypto-chat  |  přezdívka  |  místnost");
    this->setWindowFlags(windowFlags() &(~Qt::WindowMaximizeButtonHint));

    ui->label->setStyleSheet(tr("QLabel { background-color : %1 }").arg(user_color));
    ui->lineEdit->setFocus();

    this->show();
}

ChatWindow::~ChatWindow()
{
    delete ui;
}


void ChatWindow::on_pushButton_4_clicked()
{
    // change color

    ColorDialog cd;
    cd.set_color(user_color);
    cd.setModal(true);
    cd.exec();

    if (cd.change_color){
        user_color = cd.user_color;
    }

    ui->label->setStyleSheet(tr("QLabel { background-color : %1 }").arg(user_color));
}


void ChatWindow::on_pushButton_3_clicked()
{
    // change username

    NameChangeDialog nchd;
    nchd.set_name(user_name);
    nchd.setModal(true);
    nchd.exec();
}

