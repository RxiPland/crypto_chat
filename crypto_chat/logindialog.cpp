/*
Generate user's RSA key pairs
Window to check user's version with server's version
Obtain server's RSA public key
*/

#include "logindialog.h"
#include "ui_logindialog.h"

#include <QMessageBox>
#include <QDir>
#include <QTextCodec>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QCloseEvent>
#include <windows.h>
#include <QProcess>
#include <QRegExp>


LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{

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


    QDir config_folder(QDir::currentPath() + "/config");

    if(!config_folder.exists()){
        QMessageBox::critical(this, "Chyba", "Složka config neexistuje! Reinstalujte <a href=\"https://github.com/RxiPland/crypto_chat\">program</a>!");
        QApplication::exit();
        return;
    }

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


void LoginDialog::closeEvent(QCloseEvent *bar)
{
    // Before application close

    this->close();

    if(bar != nullptr){
        bar->accept();
    }

    QApplication::quit();
}

void LoginDialog::hide_widgets(bool hide){
    // hide authentication lines

    ui->label->setHidden(hide);
    ui->label_2->setHidden(hide);

    ui->lineEdit_2->setHidden(hide);
    ui->lineEdit_3->setHidden(hide);


    on_lineEdit_3_textEdited(ui->lineEdit_3->text());
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
    ui->toolButton_4->setDisabled(disable);

    ui->checkBox->setDisabled(disable);

    ui->label->setDisabled(disable);
    ui->label_2->setDisabled(disable);

    ui->lineEdit->setDisabled(disable);
    ui->lineEdit_2->setDisabled(disable);
    ui->lineEdit_3->setDisabled(disable);

    ui->comboBox->setDisabled(disable);
}

void LoginDialog::on_checkBox_stateChanged()
{
    // authentication checkbox

    if (ui->checkBox->isChecked()){
        LoginDialog::hide_widgets(false);

    } else{
        LoginDialog::hide_widgets(true);
    }
}


void LoginDialog::on_pushButton_clicked()
{
    // validate URL address & comunicate with server

    LoginDialog::disable_widgets(true);

    QMessageBox msgBox;
    msgBox.setWindowIcon(QIcon("://images/hacker.ico"));
    msgBox.setWindowTitle("Postup");
    msgBox.setText("");
    msgBox.setModal(true);
    QAbstractButton* pButtonCreate = msgBox.addButton(" Vytvořit místnost ", QMessageBox::YesRole);
    QAbstractButton* pButtonJoin = msgBox.addButton(" Připojit se do místnosti ", QMessageBox::YesRole);

    QString previousText;

    pButtonCreate->setDisabled(true);
    pButtonJoin->setDisabled(true);


    QString url_address = ui->lineEdit->text().replace(" ", "");

    // regex for complete URL validation (from: https://stackoverflow.com/questions/3711761/how-to-validate-a-complete-and-valid-url-using-regex)
    QRegExp rx("(https?://)+(([0-9a-z_!~*'().&=+$%-]+: )?[0-9a-z_!~*'().&=+$%-]+@)?(([0-9]{1,3}\\.){3}[0-9]{1,3}|([0-9a-z_!~*'()-]+\\.)*([0-9a-z][0-9a-z-]{0,61})?[0-9a-z]\\.[a-z]{2,6})(:[0-9]{1,5})?((/?)|(/[0-9a-z_!~*'().;?:@&=+$,%#-]+)+/?)$");

    if(url_address == ""){

        msgBox.close();
        QMessageBox::critical(this, "Chyba", "Pole pro URL adresu nemůže být prázdné!");

    } else if (!rx.exactMatch(url_address)){

        msgBox.close();
        QMessageBox::critical(this, "Chyba", "Zadejte kompletní URL adresu!\n\nPř. https://www.google.com");
        LoginDialog::disable_widgets(false);

        ui->lineEdit->setFocus();
        return;

    } else{
        // validate version && get server's RSA public key

        msgBox.show();

        QStringList temp_list;
        int i;

        msgBox.setText(msgBox.text() + "1/4 Čištění URL adresy");
        previousText = msgBox.text();
        msgBox.setText(msgBox.text() + "<span style=\"color:orange;\"> [Probíhá]<br></span>");

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

        msgBox.setText(previousText + "<span style=\"color:green;\"> [Dokončeno]<br></span>");

        QNetworkRequest request;
        QUrl qurl_address = QUrl(url_address + "/version");

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
        request.setHeader(QNetworkRequest::UserAgentHeader, user_agent);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "text/html; charset=utf-8");

        msgBox.setText(msgBox.text() + "2/4 Ověřování verze aplikace s verzí serveru");
        previousText = msgBox.text();
        msgBox.setText(msgBox.text() + "<span style=\"color:orange;\"> [Probíhá]<br></span>");

        QNetworkReply *reply_get = manager.get(request);

        while (!reply_get->isFinished())
        {
            qApp->processEvents();
        }

        QString response = QString(reply_get->readAll());

        if(reply_get->error() == QNetworkReply::ConnectionRefusedError){

            msgBox.close();
            QMessageBox::critical(this, "Chyba", "Nelze se připojit k internetu nebo server není dostupný!");

        } else if (reply_get->error() == QNetworkReply::HostNotFoundError){

            msgBox.close();
            QMessageBox::critical(this, "Chyba", "Doména neexsistuje!");

            LoginDialog::disable_widgets(false);

            ui->lineEdit->setFocus();
            return;

        } else if (reply_get->error() == QNetworkReply::AuthenticationRequiredError){
            // Authentication is required or credentials may be wrong
            msgBox.close();

            if(ui->checkBox->isChecked()){
                QMessageBox::warning(this, "Odpověd serveru (chyba)", "Přihlašovací údaje nejsou správné!");

            } else{
                QMessageBox::warning(this, "Odpověd serveru (chyba)", "Server vyžaduje ověření pomocí jména a hesla!");
            }

            ui->checkBox->setChecked(true);

            LoginDialog::disable_widgets(false);

            ui->lineEdit_2->setFocus();
            return;

        } else if(reply_get->error() != QNetworkReply::NoError){
            // Any error

            msgBox.close();
            QMessageBox::critical(this, "Odpověd serveru (chyba)", tr("Nastala neznámá chyba!\n\n%1").arg(reply_get->errorString()));
            return;

        } else{
            // No error

            if(!response.contains("crypto-chat")){

                msgBox.close();
                QMessageBox::critical(this, "Odpověd serveru (chyba)", QString("Nastala nezmámá chyba!\n\nOčekáváná odpověď: crypto-chat %1\nOdpověď stránky: %2").arg(app_version, response));
                return;

            } else if (!response.contains(app_version)){
                // Version is wrong

                msgBox.close();
                QMessageBox::warning(this, "Odpověd serveru (chyba)", QString("Verze serveru se neshoduje s verzí klienta! Aktualizujte na novou verzi.\n\nOčekáváná odpověď: crypto-chat %1\nOdpověď stránky: %2").arg(app_version, response));
                return;

            } else{
                // Validation succesful

                msgBox.setText(previousText + "<span style=\"color:green;\"> [Dokončeno]<br></span>");

                msgBox.setText(msgBox.text() + "3/4 Získávání veřejného klíče serveru (RSA)");
                previousText = msgBox.text();
                msgBox.setText(msgBox.text() + "<span style=\"color:orange;\"> [Probíhá]<br></span>");


                qurl_address = QUrl(url_address + "/get-public");

                if(ui->checkBox->isChecked()){
                    // authentication

                    qurl_address.setUserName(authentication_username);
                    qurl_address.setPassword(authentication_password);
                }

                request.setUrl(qurl_address);
                reply_get = manager.get(request);

                while (!reply_get->isFinished())
                {
                    qApp->processEvents();
                }

                QString response = reply_get->readAll().trimmed();

                if (response.isEmpty()){
                    msgBox.close();

                    QMessageBox::critical(this, "Nastala chyba", "Server nevrátil svůj veřejný klíč!");
                    LoginDialog::disable_widgets(false);
                    return;
                }

                // save server's RSA public key
                LoginDialog::serverPublicKeyPemHex = response;

                msgBox.setText(previousText + "<span style=\"color:green;\"> [Dokončeno]<br></span>");


                msgBox.setText(msgBox.text() + "4/4 Generování veřejného a soukromého klíče (RSA)");
                previousText = msgBox.text();
                msgBox.setText(msgBox.text() + "<span style=\"color:orange;\"> [Probíhá]<br></span>");

                // get number of bits selected by user
                QString rsaBits = ui->comboBox->currentText();
                rsaBits.replace(" (defaultní)", "");


                QString command = "/C cd ./config & cryptographic_tool.exe generate_rsa " + rsaBits;
                //QString command = "/C python config/cryptographic_tool.py generate_rsa " + rsaBits;

                // generate
                QProcess process;
                process.start("cmd", QStringList(command));
                //process.waitForFinished(-1); // will wait forever until finished

                while(process.state() == QProcess::Running){
                    qApp->processEvents();
                }


                QByteArray output = process.readAllStandardOutput();

                if(output.isEmpty()){
                    msgBox.close();

                    // python did not create the required data
                    QMessageBox::critical(this, "Chyba", "Nepodařilo se vytvořit RSA klíče!");

                    LoginDialog::disable_widgets(false);
                    return;
                }

                QList<QByteArray> cmdOutput = output.split(';');

                // get rsa public key PEM from hex
                LoginDialog::rsaPublicKeyPemHex = cmdOutput[0].trimmed();

                //get rsa private key PEM in hex
                LoginDialog::rsaPrivateKeyPemHex = cmdOutput[1].trimmed();

                msgBox.setText(previousText + "<span style=\"color:green;\"> [Dokončeno]<br></span>");

                msgBox.setText(msgBox.text() + "<br>Vyberte zda chcete vytvořit novou místnost, nebo se připojit už k existující:");

                pButtonCreate->setDisabled(false);
                pButtonJoin->setDisabled(false);

                pButtonJoin->setFocus();

                // wait for user input
                while(!msgBox.isHidden()){
                    qApp->processEvents();
                }

                if (msgBox.clickedButton()==pButtonCreate) {
                    LoginDialog::create_room = true;

                } else{
                    LoginDialog::create_room = false;
                }

                LoginDialog::successful_login = true;
                LoginDialog::server_url = ui->lineEdit->text();


                msgBox.close();
                this->close();

                return;
            }
        }
    }

    msgBox.close();
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


void LoginDialog::on_lineEdit_3_textEdited(const QString &arg1)
{
    if(arg1 != "" && ui->checkBox->isChecked()){
        ui->toolButton_3->setHidden(false);
    } else{
        ui->toolButton_3->setHidden(true);
    }
}


void LoginDialog::on_toolButton_3_pressed()
{
    // show password

    ui->lineEdit_3->setEchoMode(QLineEdit::Normal);
}


void LoginDialog::on_toolButton_3_released()
{
    // hide password

    ui->lineEdit_3->setEchoMode(QLineEdit::Password);
}


void LoginDialog::on_toolButton_clicked()
{
    QMessageBox::information(this, "Nápověda URL", "Zadejte kompletní URL adresu, na které běží <a href=\"https://github.com/RxiPland/crypto_chat\">crypto-chat</a> server (včetně protokolu!)<br><br>Příklady:<br>https://google.com<br>http://127.0.0.1:5000");
}

void LoginDialog::on_toolButton_2_clicked()
{
    QMessageBox::information(this, "Nápověda autentizace", "V hostingu lze omezit přístup na webovou stránku jménem a heslem. Vyplňte, pokud stránka přihlášení vyžaduje.");
}

void LoginDialog::on_toolButton_4_clicked()
{
    QMessageBox::information(this, "Nápověda RSA", "<a href=\"https://cs.wikipedia.org/wiki/Asymetrick%C3%A1_kryptografie\">Asymetrická kryptografie</a> funguje na principu veřejného a privátního klíče. Pro výměnu dat šifrovaně mezi serverem je vyžadován veřejný klíč obou stran. Zašifrovaná data si pak může každá strana dešifrovat pouze svým privátním klíčem. Větší velikost RSA klíčů snižuje šanci na prolomení při útocích <a href=\"https://cs.wikipedia.org/wiki/%C3%9Atok_hrubou_silou\">hrubou silou</a>, ale zvyšuje tím <a href=\"https://stuvel.eu/python-rsa-doc/usage.html#time-to-generate-a-key\">čas potřebný k vytvoření</a>.");
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
