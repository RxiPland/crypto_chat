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

private:
    Ui::IntervalDialog *ui;
};

#endif // INTERVALDIALOG_H
