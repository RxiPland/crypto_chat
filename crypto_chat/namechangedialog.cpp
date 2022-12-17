#include "namechangedialog.h"
#include "ui_namechangedialog.h"

NameChangeDialog::NameChangeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NameChangeDialog)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/images/hacker.ico"));
    this->setWindowTitle("Změna přezdívky");
    ui->pushButton->setFocus();
}

NameChangeDialog::~NameChangeDialog()
{
    delete ui;
}
