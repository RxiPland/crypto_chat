#include "colordialog.h"
#include "ui_colordialog.h"

ColorDialog::ColorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColorDialog)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/images/hacker.ico"));
    this->setWindowTitle("Změna barvy");
}

ColorDialog::~ColorDialog()
{
    delete ui;
}

void ColorDialog::on_comboBox_currentTextChanged(const QString &arg1)
{

    QLabel* pLabel = new QLabel;
    pLabel->setStyleSheet("QLabel { background-color : red }");

    if(arg1 == "Červená"){
        ui->label->
    }
}

