#include "logindialog.h"
#include "ui_logindialog.h"

#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Přihlášení ke crypto-chat serveru");

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

void LoginDialog::disable_widgets(bool disable){

    ui->pushButton->setDisabled(disable);

    ui->toolButton->setDisabled(disable);
    ui->toolButton_2->setDisabled(disable);
    ui->toolButton_3->setDisabled(disable);

    ui->checkBox->setDisabled(disable);

    ui->label->setDisabled(disable);
    ui->label_2->setDisabled(disable);

    ui->lineEdit->setDisabled(disable);
    ui->lineEdit_2->setDisabled(disable);
    ui->lineEdit_3->setDisabled(disable);
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

    LoginDialog::disable_widgets(true);

    QString url_address = ui->lineEdit->text();

    if(url_address == ""){
        QMessageBox::critical(this, "Chyba", "Pole pro URL adresu nemůže být prázdné!");

        LoginDialog::disable_widgets(false);
        return;

    } else if (!url_address.contains("http") || !url_address.contains("://") || !url_address.contains(".")){
        QMessageBox::critical(this, "Chyba", "Zadejte kompletní URL adresu!\n\nPř. https://www.google.com");

        LoginDialog::disable_widgets(false);
        return;
    }

    // send credentials to website

    QNetworkRequest request;
    QUrl qurl_address = QUrl(url_address);


    if(ui->checkBox->isChecked()){
        // authentication is needed

        qurl_address.setUserName(ui->lineEdit_2->text());
        qurl_address.setPassword(ui->lineEdit_3->text());
    }

    request.setUrl(qurl_address);
    request.setRawHeader("User-Agent", user_agent);

    QNetworkReply *reply_get = manager.get(request);

    while (!reply_get->isFinished())
    {
        qApp->processEvents();
    }

    if(reply_get->error() == QNetworkReply::ConnectionRefusedError){
        QMessageBox::critical(this, "Chyba", "Nelze se připojit k internetu!");

        LoginDialog::disable_widgets(false);
        return;

    } else if (reply_get->error() == QNetworkReply::HostNotFoundError){
        QMessageBox::critical(this, "Chyba", "Doména neexsistuje!");

        LoginDialog::disable_widgets(false);
        return;

    } else if (reply_get->error() == QNetworkReply::AuthenticationRequiredError){
        // Authentication is required or credentials may be wrong


        if(ui->checkBox->isChecked()){
            QMessageBox::critical(this, "Odpověd serveru (chyba)", "Přihlašovací údaje nejsou správné!");

        } else{
            QMessageBox::critical(this, "Odpověd serveru (chyba)", "Server vyžaduje ověření pomocí jména a hesla!");
        }

        LoginDialog::disable_widgets(false);
        ui->checkBox->setChecked(true);
        return;

    }

    qInfo() << reply_get->readAll();
    /*

    // url is not working -> replace https with http
    if (reply_headers->error() != QNetworkReply::NoError){

        request.setUrl(QUrl(location.replace("https", "http")));
        reply_headers = manager.head(request);

        while (!reply_headers->isFinished())
        {
            qApp->processEvents();
        }
    }
    */
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
    if(ui->lineEdit_3->text() != "" && ui->checkBox->isChecked()){
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


void LoginDialog::on_toolButton_clicked()
{
    QMessageBox::information(this, "Nápověda URL", "Zadejte kompletní URL adresu, na které běží crypt-chat server (včetně protokolu!)");
}


void LoginDialog::on_toolButton_2_clicked()
{
    QMessageBox::information(this, "Nápověda autentizace", "V hostingu lze omezit přístup na webovou stránku jménem a heslem. Vyplňte, pokud stránka přihlášení vyžaduje.");
}


void LoginDialog::on_lineEdit_returnPressed()
{
    // "return" pressed when typing URL address
    on_pushButton_clicked();
}

void LoginDialog::on_lineEdit_2_returnPressed()
{
    // "return" pressed when typing username
    on_pushButton_clicked();
}


void LoginDialog::on_lineEdit_3_returnPressed()
{
    // "return" pressed when typing password
    on_pushButton_clicked();
}

