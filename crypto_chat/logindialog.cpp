#include "logindialog.h"
#include "ui_logindialog.h"
#include "threadfunctions.h"

#include <QMessageBox>
#include <QDir>
#include <QTextCodec>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QCloseEvent>
#include <windows.h>


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


    QDir config_file(QDir::currentPath() + "/config");

    if(!config_file.exists()){
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

    if(LoginDialog::deleleFolder && room_id != ""){

        QDir roomFolder(QDir::tempPath() + "/" + room_id);

        if(roomFolder.exists()){
            roomFolder.removeRecursively();
        }
    }

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

    QMessageBox msgBox;
    msgBox.setWindowIcon(QIcon("://images/hacker.ico"));
    msgBox.setWindowTitle("Postup");
    msgBox.setText("");
    msgBox.setModal(true);
    QAbstractButton* pButtonCreate = msgBox.addButton(" Vytvořit místnost ", QMessageBox::YesRole);
    QAbstractButton* pButtonJoin = msgBox.addButton(" Připojit se do místnosti ", QMessageBox::YesRole);

    QString previousText;

    pButtonCreate->setHidden(true);
    pButtonJoin->setHidden(true);


    QString url_address = ui->lineEdit->text();

    if(url_address == ""){
        QMessageBox::critical(this, "Chyba", "Pole pro URL adresu nemůže být prázdné!");

    } else if (!url_address.contains("http") || !url_address.contains("://") || !url_address.contains(".")){
        QMessageBox::critical(this, "Chyba", "Zadejte kompletní URL adresu!\n\nPř. https://www.google.com");

    } else{
        // validate version && exchange server's AES key

        msgBox.show();

        QStringList temp_list;
        int i;

        msgBox.setText(msgBox.text() + "1/5 Čištění URL adresy");
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

        msgBox.setText(msgBox.text() + "2/5 Ověřování verze aplikace s verzí serveru");
        previousText = msgBox.text();
        msgBox.setText(msgBox.text() + "<span style=\"color:orange;\"> [Probíhá]<br></span>");

        QNetworkReply *reply_get = manager.get(request);

        while (!reply_get->isFinished())
        {
            qApp->processEvents();
        }

        QString response = QString(reply_get->readAll());

        if(reply_get->error() == QNetworkReply::ConnectionRefusedError){
            QMessageBox::critical(this, "Chyba", "Nelze se připojit k internetu nebo server není dostupný!");

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
            // Any error

            QMessageBox::critical(this, "Odpověd serveru (chyba)", tr("Nastala neznámá chyba!\nOznačení QNetworkReply chyby: %1\n\nOdpověd serveru: %2").arg(reply_get->error()).arg(response));

        } else{
            // No error

            if(!response.contains("crypto-chat")){
                QMessageBox::critical(this, "Odpověd serveru (chyba)", tr("Nastala nezmámá chyba!\n\nOčekáváná odpověď: crypto-chat %1\nOdpověď stránky: %2").arg(app_version).arg(response));

            } else if (!response.contains(app_version)){
                // Version is wrong

                QMessageBox::critical(this, "Odpověd serveru (chyba)", tr("Verze serveru se neshoduje s verzí klienta! Aktualizujte na novou verzi.\n\nOčekáváná odpověď: crypto-chat %1\nOdpověď stránky: %2").arg(app_version).arg(response));

            } else{
                // Validation succesful

                msgBox.setText(previousText + "<span style=\"color:green;\"> [Dokončeno]<br></span>");


                msgBox.setText(msgBox.text() + "3/5 Generace veřejného a soukromého klíče (RSA)");
                previousText = msgBox.text();
                msgBox.setText(msgBox.text() + "<span style=\"color:orange;\"> [Probíhá]<br></span>");


                QString rsaBits = ui->comboBox->currentText();
                rsaBits.replace(" (defaultní)", "");


                //std::wstring command = QString("/C config/cryptography_tool.exe generate_rsa " + rsaBits).toStdWString();
                std::wstring command = QString("/C python config/cryptography_tool.py generate_rsa " + rsaBits).toStdWString();

                // generate
                ThreadFunctions shellThread;
                shellThread.operation = 2;  // Thread func
                shellThread.ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
                shellThread.ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
                shellThread.ShExecInfo.hwnd = NULL;
                shellThread.ShExecInfo.lpVerb = L"open";
                shellThread.ShExecInfo.lpFile = L"cmd.exe";
                shellThread.ShExecInfo.lpParameters = command.c_str();
                shellThread.ShExecInfo.lpDirectory = QDir::currentPath().toStdWString().c_str();
                shellThread.ShExecInfo.nShow = SW_HIDE;
                shellThread.ShExecInfo.hInstApp = NULL;

                shellThread.start();

                // wait for thread to complete
                while(shellThread.isRunning()){
                    qApp->processEvents();
                }

                msgBox.setText(previousText + "<span style=\"color:green;\"> [Dokončeno]<br></span>");

                QFile hexFile("config/temp/hex_id");

                if(!hexFile.exists()){
                    // python did not create the file
                    QMessageBox::critical(this, "Chyba", "Program nenašel soubor s náhodně vygenerovaným ID!");

                } else{

                    hexFile.open(QIODevice::ReadOnly);
                    QByteArray content = hexFile.readAll();
                    hexFile.close();

                    room_id = (QTextCodec::codecForMib(106)->toUnicode(content)).split("\r\n")[0].trimmed();

                    if(room_id == ""){
                        // python did not create the file
                        QMessageBox::critical(this, "Chyba", "ID je prázdné!");

                    } else{

                        QFile public_pem(QDir::tempPath() + "/" + room_id + "/public_key.pem");

                        if(!public_pem.exists()){
                            QMessageBox::critical(this, "Chyba", "Program nenašel soubor s veřejným klíčem!");

                        } else {

                            public_pem.open(QIODevice::ReadOnly);
                            content = public_pem.readAll();
                            public_pem.close();

                            if(room_id == ""){
                                // python did not create the file
                                QMessageBox::critical(this, "Chyba", "Veřejný klíč je prázdný!");

                            } else{

                                qurl_address = QUrl(url_address + "/get-key");

                                msgBox.setText(msgBox.text() + "4/5 Získávání symetrického klíče (AES) serveru pomocí veřejného klíče (RSA)");
                                previousText = msgBox.text();
                                msgBox.setText(msgBox.text() + "<span style=\"color:orange;\"> [Probíhá]<br></span>");

                                if(ui->checkBox->isChecked()){
                                    // authentication

                                    qurl_address.setUserName(authentication_username);
                                    qurl_address.setPassword(authentication_password);
                                }

                                request.setUrl(qurl_address);
                                request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");


                                QJsonObject obj;
                                obj["rsa_pem"] = (QString)QUrl::toPercentEncoding(content);
                                QJsonDocument doc(obj);
                                QByteArray data = doc.toJson();

                                QNetworkReply *reply_post = manager.post(request, data);


                                while (!reply_post->isFinished())
                                {
                                    qApp->processEvents();
                                }

                                QJsonDocument jsonResponse = QJsonDocument::fromJson(reply_post->readAll());
                                QJsonObject jsonObject = jsonResponse.object();

                                QString encryptedAesKeyHex = jsonObject["aes_key"].toString();

                                if(encryptedAesKeyHex == ""){
                                    QMessageBox::critical(this, "Chyba", "Server neposlal zašifrovaný symetrický klíč! (AES)");

                                } else{

                                    msgBox.setText(previousText + "<span style=\"color:green;\"> [Dokončeno]<br></span>");


                                    QFile aes_key(QDir::tempPath() + "/" + room_id + "/symetric_key_server");
                                    aes_key.open(QIODevice::WriteOnly);
                                    aes_key.write(encryptedAesKeyHex.toStdString().c_str());
                                    aes_key.close();

                                    if(!aes_key.exists()){
                                        QMessageBox::critical(this, "Chyba", "Nepodařilo se zapsat zašifrovaný symetrický klíč do souboru!");

                                    } else{

                                        msgBox.setText(msgBox.text() + "5/5 Dešiforvání přijatého symetrického klíče (AES) pomocí privátního klíče (RSA)");
                                        previousText = msgBox.text();
                                        msgBox.setText(msgBox.text() + "<span style=\"color:orange;\"> [Probíhá]<br></span>");


                                        //command = QString("/C config/cryptography_tool.exe decrypt_rsa " + room_id).toStdWString();
                                        command = QString("/C python config/cryptography_tool.py decrypt_rsa " + room_id).toStdWString();

                                        shellThread.ShExecInfo.lpParameters = command.c_str();
                                        shellThread.start();

                                        // wait for thread to complete
                                        while(shellThread.isRunning()){
                                            qApp->processEvents();
                                        }

                                        msgBox.setText(previousText + "<span style=\"color:green;\"> [Dokončeno]<br></span>");


                                        msgBox.setText(msgBox.text() + "<br>Symetrický klíč serveru byl úspěšně přijat. Vyberte zda chcete vytvořit novou místnost, nebo se připojit už k existující:");

                                        pButtonCreate->setHidden(false);
                                        pButtonJoin->setHidden(false);

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

                                        LoginDialog::deleleFolder = false;

                                        msgBox.close();
                                        this->close();

                                        return;
                                    }
                                }
                            }
                        }
                    }
                }
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
    QMessageBox::information(this, "Nápověda URL", "Zadejte kompletní URL adresu, na které běží <a href=\"https://github.com/RxiPland/crypto_chat\">crypto-chat server</a> (včetně protokolu!)<br><br>Př. https://google.com");
}

void LoginDialog::on_toolButton_2_clicked()
{
    QMessageBox::information(this, "Nápověda autentizace", "V hostingu lze omezit přístup na webovou stránku jménem a heslem. Vyplňte, pokud stránka přihlášení vyžaduje.");
}

void LoginDialog::on_toolButton_4_clicked()
{
    QMessageBox::information(this, "Nápověda RSA", "Výměna <a href=\"https://cs.wikipedia.org/wiki/Symetrick%C3%A1_%C5%A1ifra\">symetrického klíče</a> pomocí <a href=\"https://cs.wikipedia.org/wiki/Asymetrick%C3%A1_kryptografie\">asymetrické kryptografie</a>.<br><br>Asymetrická kryptografie (RSA) funguje na principu dvou klíčů (veřejný a privátní), které spolu souvisí. Větší velikost klíče snižuje šanci na prolomení při útocích <a href=\"https://cs.wikipedia.org/wiki/%C3%9Atok_hrubou_silou\">hrubou silou</a>, ale zvyšuje tím <a href=\"https://stuvel.eu/python-rsa-doc/usage.html#time-to-generate-a-key\">čas potřebný k vytvoření</a>.<br><br>V tomto případě se vyšle vygenerovaný veřejný klíč (RSA) na server, kde se využije k zašifrování symetrického klíče (AES), který vytvořil server. Zašifrovaný symetrický klíč se pošle zpět uživateli. Uživatel si symetrický klíč (AES) dešifruje svým privátním klíčem (RSA) a tím obě strany (server+uživatel) mají stejný symetrický klíč.");

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

