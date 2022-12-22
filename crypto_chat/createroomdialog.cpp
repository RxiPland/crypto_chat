#include "createroomdialog.h"
#include "ui_createroomdialog.h"

#include <QDir>
#include <QCloseEvent>


CreateRoomDialog::CreateRoomDialog(QWidget *parent, QString server_url, QString room_id) :
    QDialog(parent),
    ui(new Ui::CreateRoomDialog)
{
    CreateRoomDialog::room_id = room_id;
    CreateRoomDialog::server_url = server_url;

    ui->setupUi(this);
    this->setWindowIcon(QIcon("://images/hacker.ico"));
    this->setWindowTitle("Vytvoření chatovací místnosti");

    ui->lineEdit->setHidden(true);

    ui->lineEdit_2->setDisabled(true);
    ui->lineEdit_3->setDisabled(true);

    ui->lineEdit_2->setText(server_url);
    ui->lineEdit_3->setText(room_id);

    ui->pushButton->setFocusPolicy(Qt::FocusPolicy::NoFocus);

}

CreateRoomDialog::~CreateRoomDialog()
{
    delete ui;
}

void CreateRoomDialog::closeEvent(QCloseEvent *bar)
{
    // Before application close

    if(deleleFolder){

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

void CreateRoomDialog::on_checkBox_clicked()
{
    if(ui->checkBox->isChecked()){
        ui->lineEdit->setHidden(false);

    } else{
        ui->lineEdit->setHidden(true);
    }
}

