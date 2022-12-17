#include "colordialog.h"
#include "ui_colordialog.h"

ColorDialog::ColorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColorDialog)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/images/hacker.ico"));
    this->setWindowTitle("Změna barvy");
    ui->pushButton->setFocus();

    ColorDialog::on_comboBox_currentTextChanged(user_color);
}

ColorDialog::~ColorDialog()
{
    delete ui;
}


void ColorDialog::on_comboBox_currentTextChanged(const QString &arg1)
{
    QString color = "black";

    if(arg1 == "Červená"){
        color = "red";

    } else if (arg1 == "Zelená"){
        color = "green";

    } else if(arg1 == "Modrá"){
        color = "#00BFFF";

    } else if(arg1 == "Fialová"){
        color = "#800080";

    } else if(arg1 == "Oranžová"){
        color = "#FF8C00";

    } else if(arg1 == "Žlutá"){
        color = "#FFD700";

    } else if(arg1 == "Hnědá"){
        color = "#8B4513";
    }

    ui->label->setStyleSheet(tr("QLabel { background-color : %1 }").arg(color));
}


void ColorDialog::on_pushButton_2_clicked()
{
    // close window

    this->close();
    return;
}

