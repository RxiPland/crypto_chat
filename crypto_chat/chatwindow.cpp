#include "chatwindow.h"
#include "./ui_chatwindow.h"

ChatWindow::ChatWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ChatWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("crypto-chat  |  přezdívka  |  místnost");
    this->setWindowFlags(windowFlags() &(~Qt::WindowMaximizeButtonHint));
    ui->lineEdit->setFocus();
}

ChatWindow::~ChatWindow()
{
    delete ui;
}

