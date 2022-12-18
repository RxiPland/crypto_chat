#include "chatwindow.h"
#include "ui_chatwindow.h"
#include "logindialog.h"
#include "colordialog.h"
#include "namechangedialog.h"
#include "threadfunctions.h"
#include "createroomdialog.h"

#include <QMessageBox>
#include <QCloseEvent>

ThreadFunctions refreshChatLoop;


ChatWindow::ChatWindow(QWidget *parent, QString version, QByteArray userAgent)
    : QMainWindow(parent)
    , ui(new Ui::ChatWindow)
{

    ChatWindow::app_version = version;
    ChatWindow::user_agent = userAgent;

    // login to server
    LoginDialog lw(nullptr, ChatWindow::app_version, ChatWindow::user_agent);
    lw.setModal(true);
    lw.exec();

    if(!lw.successful_login){
        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return;

    } else{
        ChatWindow::server_url = lw.server_url;
        ChatWindow::authentication_required = lw.authentication_required;

        if(ChatWindow::authentication_required){
            ChatWindow::authentication_username = lw.authentication_username;
            ChatWindow::authentication_password = lw.authentication_password;
        }
    }

    if(lw.create_room){
        CreateRoomDialog crd(nullptr, ChatWindow::app_version, ChatWindow::user_agent);
        crd.set_info(ChatWindow::server_url);
        crd.exec();
    }

    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/images/hacker.ico"));
    this->setWindowTitle(tr("crypto-chat  |  %1  |  %2").arg(server_url).arg(user_name));
    this->setWindowFlags(windowFlags() &(~Qt::WindowMaximizeButtonHint));

    ui->label->setStyleSheet(tr("QLabel { background-color : %1 }").arg(user_color));
    ui->lineEdit->setFocus();

    QApplication::setQuitOnLastWindowClosed(true);

    refreshChatLoop.operation = 3;
    refreshChatLoop.sleep_time = 5;
    refreshChatLoop.actionObject = ui->action_zpravy_2;
    refreshChatLoop.continueLoop = true;
    refreshChatLoop.start();

    this->show();
}

ChatWindow::~ChatWindow()
{
    delete ui;
}

void ChatWindow::closeEvent(QCloseEvent *bar)
{
    // Before application close

    refreshChatLoop.stopLoop();

    this->close();
    bar->accept();

    while(refreshChatLoop.isRunning()){
        // wait for thread to finish
        qApp->processEvents();
    }

    QApplication::quit();
}

QString convert_color(QString color){

    if(color == "black"){
        return "Černá";

    } else if(color == "red"){
        return "Červená";

    } else if(color == "green"){
        return "Zelená";

    } else if(color == "#00BFFF"){
        return "Modrá";

    } else if(color == "#800080"){
        return "Fialová";

    } else if(color == "#FF8C00"){
        return "Oranžová";

    } else if(color == "#FFD700"){
        return "Žlutá";

    } else if(color == "#8B4513"){
        return "Hnědá";

    } else if (color == "Černá"){
        return "black";

    } else if(color == "Červená"){
        return "red";

    } else if (color == "Zelená"){
        return "green";

    } else if(color == "Modrá"){
        return "#00BFFF";

    } else if(color == "Fialová"){
        return "#800080";

    } else if(color == "Oranžová"){
        return "#FF8C00";

    } else if(color == "Žlutá"){
        return "#FFD700";

    } else if(color == "Hnědá"){
        return "#8B4513";
    }

    return "";
}

void ChatWindow::on_pushButton_4_clicked()
{
    // change color

    ColorDialog cd;
    cd.set_color(user_color);
    cd.setModal(true);
    cd.exec();

    if (cd.change_color){
        user_color = cd.user_color;
    }

    ui->label->setStyleSheet(tr("QLabel { background-color : %1 }").arg(user_color));
}


void ChatWindow::on_pushButton_3_clicked()
{
    // change username

    NameChangeDialog nchd;
    nchd.set_name(user_name);
    nchd.setModal(true);
    nchd.exec();

    if(nchd.new_name != ""){
        ChatWindow::user_name = nchd.new_name;
        this->setWindowTitle(tr("crypto-chat  |  %1  |  %2").arg(server_url).arg(user_name));
    }

}


void ChatWindow::on_pushButton_2_clicked()
{
    QMessageBox::about(this, "Informace o místnosti", tr("Verze aplikace:  %1<br><br>URL:  %2<br>ID:  %3<br><br>Vaše přezdívka:  %4<br>Vaše barva:  %5").arg(app_version).arg(server_url).arg(room_id).arg(user_name).arg(convert_color(user_color)));
}

