#include "createroomdialog.h"
#include "ui_createroomdialog.h"

CreateRoomDialog::CreateRoomDialog(QWidget *parent, QString server_url) :
    QDialog(parent),
    ui(new Ui::CreateRoomDialog)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon("://images/hacker.ico"));
    this->setWindowTitle("Vytvoření chatovací místnosti");

    ui->lineEdit->setHidden(true);

    ui->lineEdit_2->setDisabled(true);
    ui->lineEdit_3->setDisabled(true);

    ui->lineEdit_2->setText(server_url);
    ui->lineEdit_3->setText("Vytvoří server");

    ui->pushButton->setFocusPolicy(Qt::FocusPolicy::NoFocus);

}

CreateRoomDialog::~CreateRoomDialog()
{
    delete ui;
}

void CreateRoomDialog::on_checkBox_clicked()
{
    if(ui->checkBox->isChecked()){
        ui->lineEdit->setHidden(false);

    } else{
        ui->lineEdit->setHidden(true);
    }
}

