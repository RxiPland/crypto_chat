#include "namechangedialog.h"
#include "ui_namechangedialog.h"

NameChangeDialog::NameChangeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NameChangeDialog)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/images/hacker.ico"));
    this->setWindowTitle("Změna přezdívky");

    ui->lineEdit->setPlaceholderText(original_name);
    ui->pushButton->setDisabled(true);
}

NameChangeDialog::~NameChangeDialog()
{
    delete ui;
}

void NameChangeDialog::set_name(QString name){

    NameChangeDialog::original_name = name;
}

void NameChangeDialog::on_lineEdit_textChanged(const QString &arg1)
{
    if(arg1 == original_name || arg1.trimmed() == ""){
        ui->pushButton->setDisabled(true);

    } else{
        ui->pushButton->setDisabled(false);
    }

}


void NameChangeDialog::on_pushButton_clicked()
{
    new_name = ui->lineEdit->text().trimmed();
}

