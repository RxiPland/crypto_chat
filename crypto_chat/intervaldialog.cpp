/*
Window for changing interval for refreshing chat
*/

#include "intervaldialog.h"
#include "ui_intervaldialog.h"

#include <QMessageBox>


IntervalDialog::IntervalDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IntervalDialog)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/images/hacker.ico"));
    this->setWindowTitle("Změna intervalu");

    ui->pushButton->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    ui->pushButton_2->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    ui->pushButton_3->setFocusPolicy(Qt::FocusPolicy::NoFocus);
}

IntervalDialog::~IntervalDialog()
{
    delete ui;
}

void IntervalDialog::set_interval(float interval)
{
    ui->doubleSpinBox->setValue(interval);
    ui->label->setText(tr("Aktualizace každých %1 vteřin").arg(interval));
}

void IntervalDialog::on_pushButton_2_clicked()
{
    // without saving
    this->close();
}


void IntervalDialog::on_pushButton_clicked()
{
    // save value
    IntervalDialog::interval = ui->doubleSpinBox->value();
    this->close();
}


void IntervalDialog::on_doubleSpinBox_valueChanged(double arg1)
{
    ui->label->setText(tr("Aktualizace každých %1 vteřin").arg(arg1));
}


void IntervalDialog::on_pushButton_3_clicked()
{
    ui->doubleSpinBox->setValue(2.9);
    QMessageBox::information(this, "Oznámení", "Byl nastaven výchozí interval");

}

