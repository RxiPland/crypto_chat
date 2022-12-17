#include "chatwindow.h"
#include "ui_chatwindow.h"
#include "logindialog.h"
#include "colordialog.h"
#include "namechangedialog.h"
#include "threadfunctions.h"

#include <QMessageBox>
#include <QCloseEvent>

// Threads
//int activeThreads=0;
//QList<ThreadFunctions> runningThreads;

ThreadFunctions sleepLoop;


ChatWindow::ChatWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ChatWindow)
{

    // login to server
    LoginDialog lw;
    lw.setModal(true);
    lw.app_version = app_version;
    lw.user_agent = user_agent;
    lw.exec();

    if(lw.successful_login){
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

    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/images/hacker.ico"));
    this->setWindowTitle(tr("crypto-chat  |  %1  |  %2").arg(server_url).arg(user_name));
    this->setWindowFlags(windowFlags() &(~Qt::WindowMaximizeButtonHint));

    ui->label->setStyleSheet(tr("QLabel { background-color : %1 }").arg(user_color));
    ui->lineEdit->setFocus();

    sleepLoop.operation = 3;
    sleepLoop.sleep_time = 5;
    sleepLoop.actionObject = ui->action_zpravy_2;
    sleepLoop.continueLoop = true;
    sleepLoop.start();

    this->show();
}

ChatWindow::~ChatWindow()
{
    delete ui;
}

void ChatWindow::closeEvent(QCloseEvent *bar)
{
    // Before application close

    sleepLoop.stopLoop();
    bar->accept();

    this->close();

    while(sleepLoop.isRunning()){
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

