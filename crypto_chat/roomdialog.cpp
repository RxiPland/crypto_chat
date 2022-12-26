#include "roomdialog.h"
#include "ui_roomdialog.h"
#include "threadfunctions.h"

#include <QDir>
#include <QCloseEvent>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QNetworkReply>

RoomDialog::RoomDialog(QWidget *parent, bool createRoom, QString server_url, QString room_id) :
    QDialog(parent),
    ui(new Ui::RoomDialog)
{

    ui->setupUi(this);
    this->setWindowIcon(QIcon("://images/hacker.ico"));

    RoomDialog::room_id = room_id;
    RoomDialog::server_url = server_url;

    ui->lineEdit->setHidden(true);
    ui->lineEdit_2->setDisabled(true);
    ui->pushButton->setFocusPolicy(Qt::FocusPolicy::NoFocus);

    if(createRoom){

        this->setWindowTitle("Vytvoření chatovací místnosti");

        ui->lineEdit_3->setDisabled(true);

        ui->lineEdit_2->setText(server_url);
        ui->lineEdit_3->setText(room_id);

        ui->pushButton->setText("Vytvořit místnost");
        ui->pushButton_2->setText("Existující místnost");

    } else{

        this->setWindowTitle("Připojení do chatovací místnosti");
        ui->lineEdit_2->setText(server_url);

        ui->pushButton->setText("Připojit se do místnosti");
        ui->pushButton_2->setText("Vytvořit novou místnost");
    }

}

RoomDialog::~RoomDialog()
{
    delete ui;
}

void RoomDialog::closeEvent(QCloseEvent *bar)
{
    // Before application close

    if(deleleFolder && !successful){
        QDir roomFolder(QDir::tempPath() + "/" + room_id);

        if(roomFolder.exists() && room_id != ""){
            roomFolder.removeRecursively();
        }
    }

    this->close();

    if(bar != nullptr){
        bar->accept();
    }

    QApplication::quit();
}
