#include "joinroomdialog.h"
#include "ui_joinroomdialog.h"

#include <QMessageBox>

JoinRoomDialog::JoinRoomDialog(QWidget *parent, QString server_url) :
    QDialog(parent),
    ui(new Ui::JoinRoomDialog)
{

    JoinRoomDialog::server_url = server_url;

    ui->setupUi(this);
    this->setWindowIcon(QIcon("://images/hacker.ico"));
    this->setWindowTitle("Připojení do chatovací místnosti");

    ui->lineEdit->setHidden(true);

    ui->lineEdit_2->setDisabled(true);
    ui->lineEdit_2->setText(server_url);

    ui->pushButton->setFocusPolicy(Qt::FocusPolicy::NoFocus);
}

JoinRoomDialog::~JoinRoomDialog()
{
    delete ui;
}

void JoinRoomDialog::on_pushButton_clicked()
{

}


void JoinRoomDialog::on_lineEdit_3_textEdited(const QString &arg1)
{
    if(arg1.length() == 33){
        QString text = arg1;
        text.chop(1);

        ui->lineEdit_3->setText(text);

        QMessageBox::critical(this, "Upozornění", "Délka ID musí být 32 znaků!");

    }
}


void JoinRoomDialog::on_toolButton_clicked()
{
    QMessageBox::information(this, "Nápověda ID", "Pro získání kontaktujte zakladatele místnosti, který ho získá při vytváření či v záložce (místnost > zkopírovat ID).");
}

