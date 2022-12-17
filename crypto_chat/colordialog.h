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

    QString user_color;
    bool change_color = false;

    void set_color(QString color);

private slots:
    void on_comboBox_textActivated(const QString &arg1);
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    QString convert_color(QString color);


private:
    Ui::ColorDialog *ui;
};

#endif // COLORDIALOG_H
