#include "intervaldialog.h"
#include "ui_intervaldialog.h"

IntervalDialog::IntervalDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IntervalDialog)
{
    ui->setupUi(this);
}

IntervalDialog::~IntervalDialog()
{
    delete ui;
}
