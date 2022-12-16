#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private slots:
    void on_checkBox_stateChanged();
    void on_pushButton_clicked();
    void on_lineEdit_textEdited();
    void on_lineEdit_3_textEdited();
    void on_toolButton_3_pressed();
    void on_toolButton_3_released();

    void on_lineEdit_returnPressed();

private:
    Ui::LoginDialog *ui;

    void hide_widgets(bool hide=true);
};

#endif // LOGINDIALOG_H
