#ifndef COLORDIALOG_H
#define COLORDIALOG_H

#include <QDialog>

namespace Ui {
class ColorDialog;
}

class ColorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ColorDialog(QWidget *parent = nullptr);
    ~ColorDialog();

    QString user_color = "Černá";

private slots:
    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_pushButton_2_clicked();

private:
    Ui::ColorDialog *ui;
};

#endif // COLORDIALOG_H
