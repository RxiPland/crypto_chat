#include "logindialog.h"
#include "ui_logindialog.h"

#include <QMessageBox>
#include <windows.h>


LoginDialog::LoginDialog(QWidget *parent, QString version, QByteArray userAgent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    LoginDialog::app_version = version;
    LoginDialog::user_agent = userAgent;

    ui->setupUi(this);
    this->setWindowIcon(QIcon("://images/hacker.ico"));
    this->setWindowTitle("Připojení ke crypto-chat serveru");

    ui->lineEdit->setFocus();
    ui->pushButton->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    ui->checkBox->setChecked(false);
    ui->pushButton->setEnabled(false);
    ui->toolButton_3->setHidden(true);
    hide_widgets(true);
    this->show();

    if (QSslSocket::supportsSsl() == false){

        QMessageBox msgBox;
        msgBox.setWindowTitle("crypto-chat (Problém s OpenSSL)");
        msgBox.setText("Verze OpenSSL není platná!<br>Bez ní program nemůže přistupovat na zabezpečené weby s protokolem HTTPs<br><br>Nainstalování verze \"" + QSslSocket::sslLibraryBuildVersionString() + "\", nebo velmi podobné, problém opraví<br>Odkaz na stažení: <a href=\"https://www.filehorse.com/download-openssl-64\">https://www.filehorse.com/download-openssl-64</a><br><br>Před stažením je důležité označit správnou verzi!<br>Vaše aktuální nainstalovaná verze: \"" + QSslSocket::sslLibraryVersionString() + "\"");
        QAbstractButton* pButtonYes = msgBox.addButton("Otevřít odkaz", QMessageBox::YesRole);
        msgBox.addButton("Odejít", QMessageBox::NoRole);
        msgBox.exec();

        if (msgBox.clickedButton()==pButtonYes) {
            ShellExecute(0, 0, L"https://www.filehorse.com/download-openssl-64", 0, 0, SW_HIDE);
        }

        QApplication::exit();
        return;
    }
}

LoginDialog::~LoginDialog()
{
    delete ui;
}


void LoginDialog::hide_widgets(bool hide){
    // hide authentication lines

    ui->label->setHidden(hide);
    ui->label_2->setHidden(hide);

    ui->lineEdit_2->setHidden(hide);
    ui->lineEdit_3->setHidden(hide);


    on_lineEdit_3_textEdited();
}

void LoginDialog::disable_widgets(bool disable){
    // disable widgets when sending network requests

    if(ui->lineEdit->text() != "" && !disable){
        ui->pushButton->setEnabled(true);

    } else{
        ui->pushButton->setEnabled(false);
    }

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
    // authentication checkbox

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

    } else if (!url_address.contains("http") || !url_address.contains("://") || !url_address.contains(".")){
        QMessageBox::critical(this, "Chyba", "Zadejte kompletní URL adresu!\n\nPř. https://www.google.com");

    } else{
        // validate website

        QStringList temp_list;
        int i;

        // convert string to list
        for(i=0; i<url_address.length(); i++){
            temp_list.append(url_address[i]);
        }

        // remove all '/' chars from back
        while(url_address.endsWith("/")){
            temp_list.pop_back();
            url_address = temp_list.join("");
        }

        // set cleaned URL back to lineEdit
        ui->lineEdit->setText(url_address);

        QNetworkRequest request;
        QUrl qurl_address = QUrl(url_address + "/test");

        if(ui->checkBox->isChecked()){
            // authentication

            authentication_required = true;
            authentication_username = ui->lineEdit_2->text();
            authentication_password = ui->lineEdit_3->text();

            qurl_address.setUserName(authentication_username);
            qurl_address.setPassword(authentication_password);

        } else{
            authentication_required = false;
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


        } else if (reply_get->error() == QNetworkReply::HostNotFoundError){
            QMessageBox::critical(this, "Chyba", "Doména neexsistuje!");

        } else if (reply_get->error() == QNetworkReply::AuthenticationRequiredError){
            // Authentication is required or credentials may be wrong


            if(ui->checkBox->isChecked()){
                QMessageBox::critical(this, "Odpověd serveru (chyba)", "Přihlašovací údaje nejsou správné!");

            } else{
                QMessageBox::critical(this, "Odpověd serveru (chyba)", "Server vyžaduje ověření pomocí jména a hesla!");
            }

            ui->checkBox->setChecked(true);

        } else if(reply_get->error() != QNetworkReply::NoError){
            QMessageBox::critical(this, "Odpověd serveru (chyba)", tr("Nastala nezmámá chyba! Označení chyby: %1").arg(reply_get->error()));

        } else{
            // No error

            QString response = QString(reply_get->readAll());


            if(!response.contains("crypto-chat")){
                QMessageBox::critical(this, "Odpověd serveru (chyba)", tr("Nastala nezmámá chyba!\n\nOčekáváná odpověď: crypto-chat %1\nOdpověď stránky: %2").arg(app_version).arg(response));

            } else if (!response.contains(app_version)){
                QMessageBox::critical(this, "Odpověd serveru (chyba)", tr("Verze serveru se neshoduje s verzí klienta! Aktualizujte na novou verzi.\n\nOčekáváná odpověď: crypto-chat %1\nOdpověď stránky: %2").arg(app_version).arg(response));

            } else{

                successful_login = true;
                server_url = ui->lineEdit->text();

                server_url.replace("https://", "");
                server_url.replace("http://", "");
                server_url.replace("//", "/");


                QMessageBox msgBox;
                msgBox.setWindowIcon(QIcon("://images/hacker.ico"));
                msgBox.setWindowTitle("Vyberte akci");
                msgBox.setText("Úspěšně se podařilo navázat spojení s crypto-chat serverem. Vyberte zda chcete vytvořit novou místnost, nebo se připojit už k existující.");
                QAbstractButton* pButtonYes = msgBox.addButton(" Vytvořit místnost ", QMessageBox::YesRole);
                msgBox.addButton(" Připojit se do místnosti ", QMessageBox::YesRole);
                msgBox.exec();

                if (msgBox.clickedButton()==pButtonYes) {
                    create_room = true;
                }

                this->close();
                return;
            }
        }
    }

    LoginDialog::disable_widgets(false);
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
    QMessageBox::information(this, "Nápověda URL", "Zadejte kompletní URL adresu, na které běží <a href=\"https://github.com/RxiPland/crypto_chat\">crypt-chat server</a> (včetně protokolu!)<br><br>Př. https://google.com");
}


void LoginDialog::on_toolButton_2_clicked()
{
    QMessageBox::information(this, "Nápověda autentizace", "V hostingu lze omezit přístup na webovou stránku jménem a heslem. Vyplňte, pokud stránka přihlášení vyžaduje.");
}


void LoginDialog::on_lineEdit_returnPressed()
{
    // "return" pressed when typing URL address
    LoginDialog::on_pushButton_clicked();
}

void LoginDialog::on_lineEdit_2_returnPressed()
{
    // "return" pressed when typing username
    LoginDialog::on_pushButton_clicked();
}


void LoginDialog::on_lineEdit_3_returnPressed()
{
    // "return" pressed when typing password
    LoginDialog::on_pushButton_clicked();
}

