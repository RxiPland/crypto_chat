#include "namechangedialog.h"
#include "ui_namechangedialog.h"

#include <QMessageBox.h>

NameChangeDialog::NameChangeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NameChangeDialog)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/images/hacker.ico"));
    this->setWindowTitle("Změna přezdívky");

    ui->pushButton->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    ui->pushButton_2->setFocusPolicy(Qt::FocusPolicy::NoFocus);
}

NameChangeDialog::~NameChangeDialog()
{
    delete ui;
}

void NameChangeDialog::set_name(QString name){

    NameChangeDialog::original_name = name;

    ui->lineEdit->setPlaceholderText(original_name);
    ui->pushButton->setDisabled(true);
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
    // close window (save new name)

    NameChangeDialog::new_name = ui->lineEdit->text().trimmed();

    QMessageBox::information(this, "Oznámení", "Jméno bylo úspěšně změněno.");


    this->close();
    return;
}


void NameChangeDialog::on_pushButton_2_clicked()
{
    // close window (old name)
    this->close();
    return;
}


void NameChangeDialog::on_lineEdit_returnPressed()
{
    QString lineEdit_text = ui->lineEdit->text();

    if(lineEdit_text == NameChangeDialog::original_name){
        QMessageBox::critical(this, "Chyba", "Jméno se shoduje s předchozím!");

    } else if (lineEdit_text.trimmed() == ""){
        ui->lineEdit->clear();
        QMessageBox::critical(this, "Chyba", "Pole pro jméno nemůže být prázdné!");

    } else{
        NameChangeDialog::on_pushButton_clicked();
    }

}

