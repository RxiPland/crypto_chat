#include "logindialog.h"
#include "ui_logindialog.h"

#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("přihlášení k serveru");

    ui->lineEdit->setFocus();
    ui->pushButton->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    ui->checkBox->setChecked(false);
    ui->pushButton->setEnabled(false);
    ui->toolButton_3->setHidden(true);
    hide_widgets(true);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::hide_widgets(bool hide){

    ui->label->setHidden(hide);
    ui->label_2->setHidden(hide);

    ui->lineEdit_2->setHidden(hide);
    ui->lineEdit_3->setHidden(hide);


    on_lineEdit_3_textEdited();
}

void LoginDialog::on_checkBox_stateChanged()
{
    if(ui->checkBox->isChecked()){
        LoginDialog::hide_widgets(false);

    }else{
        LoginDialog::hide_widgets(true);
    }
}


void LoginDialog::on_pushButton_clicked()
{
    // validate URL address & comunicate with server

    QString url_address = ui->lineEdit->text();

    if(url_address == ""){
        QMessageBox::critical(this, "Chyba", "Pole pro URL adresu nemůže být prázdné!");
        return;

    } else if (!url_address.contains("http") || !url_address.contains("://") || !url_address.contains(".")){
        QMessageBox::critical(this, "Chyba", "Zadejte úplnou URL adresu!\n\nPř. https://www.google.com");
        return;
    }

    // send credentials to website
}

void LoginDialog::on_lineEdit_returnPressed()
{
    // "return" pressed when typing URL address
    on_pushButton_clicked();
}

void LoginDialog::on_lineEdit_textEdited()
{
    if(ui->lineEdit->text() != ""){
        ui->pushButton->setEnabled(true);
    } else{
        ui->pushButton->setEnabled(false);
    }
}


void LoginDialog::on_lineEdit_3_textEdited()
{
    if(ui->lineEdit_3->text() != ""){
        ui->toolButton_3->setHidden(false);
    } else{
        ui->toolButton_3->setHidden(true);
    }
}


void LoginDialog::on_toolButton_3_pressed()
{
    ui->lineEdit_3->setEchoMode(QLineEdit::Normal);
}


void LoginDialog::on_toolButton_3_released()
{
    ui->lineEdit_3->setEchoMode(QLineEdit::Password);
}

