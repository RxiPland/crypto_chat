#ifndef INTERVALDIALOG_H
#define INTERVALDIALOG_H

#include <QDialog>

namespace Ui {
class IntervalDialog;
}

class IntervalDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IntervalDialog(QWidget *parent = nullptr);
    ~IntervalDialog();

    float interval;

    void set_interval(float interval);

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_doubleSpinBox_valueChanged(double arg1);

private:
    Ui::IntervalDialog *ui;
};

#endif // INTERVALDIALOG_H
