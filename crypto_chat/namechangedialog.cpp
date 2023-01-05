/*
Window for changing user's nickname
*/

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

    ui->lineEdit_2->setHidden(true);
}

NameChangeDialog::~NameChangeDialog()
{
    delete ui;
}

void NameChangeDialog::set_name(QString name)
{

    NameChangeDialog::original_name = name;

    ui->lineEdit->setPlaceholderText(original_name);
}

void NameChangeDialog::on_pushButton_clicked()
{
    // close window (save new name)

    this->hide();

    NameChangeDialog::new_name = ui->lineEdit->text().trimmed();
    NameChangeDialog::prefix = ui->lineEdit_2->text().trimmed();

    if(ui->checkBox->isChecked()){
        QMessageBox::information(this, "Oznámení", "Údaje o jménu a prefixu byly úspěšně změněny.");

    } else{
        QMessageBox::information(this, "Oznámení", "Jméno bylo úspěšně změněno.");
    }


    this->close();
}


void NameChangeDialog::on_pushButton_2_clicked()
{
    // close window (keep old name)
    this->close();
}


void NameChangeDialog::on_lineEdit_returnPressed()
{
    // 'return' pressed when typing name

    QString lineEdit_text = ui->lineEdit->text();

    if (lineEdit_text.trimmed() == ""){
        ui->lineEdit->setText(ui->lineEdit->placeholderText());
    }

    NameChangeDialog::on_pushButton_clicked();
}

void NameChangeDialog::on_lineEdit_textEdited(const QString &arg1)
{
    if(arg1.length() == 26){
        QString text = arg1;
        text.chop(1);

        ui->lineEdit->setText(text);

        QMessageBox::critical(this, "Upozornění", "Délka jména nemůže přesáhnout 25 znaků!");
    }
}


void NameChangeDialog::on_checkBox_clicked()
{
    if(ui->checkBox->isChecked()){
        ui->lineEdit_2->setHidden(false);

    } else{
        ui->lineEdit_2->setHidden(true);
    }
}


void NameChangeDialog::on_lineEdit_2_textEdited(const QString &arg1)
{
    if(arg1.length() == 26){
        QString text = arg1;
        text.chop(1);

        ui->lineEdit_2->setText(text);

        QMessageBox::critical(this, "Upozornění", "Délka prefixu nemůže přesáhnout 25 znaků!");
    }
}


void NameChangeDialog::on_lineEdit_2_returnPressed()
{
    on_lineEdit_returnPressed();
}

