#include "createroomdialog.h"
#include "ui_createroomdialog.h"

#include <QDir>
#include <QCloseEvent>
#include <QMessageBox>


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


void CreateRoomDialog::on_pushButton_clicked()
{
    QString username = ui->lineEdit_4->text().trimmed();

    if(username == ""){
        QMessageBox::critical(this, "Chyba", "Pole pro jméno nemůže být prázdné!");
        return;
    }



}


void CreateRoomDialog::on_lineEdit_4_textEdited(const QString &arg1)
{
    if(arg1.length() == 26){
        QString text = arg1;
        text.chop(1);

        ui->lineEdit_4->setText(text);

        QMessageBox::critical(this, "Upozornění", "Délka jména nemůže přesáhnout 25 znaků!");

    }
}

