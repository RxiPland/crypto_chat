#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    QString app_version;
    QByteArray user_agent;

private slots:
    void on_checkBox_stateChanged();
    void on_pushButton_clicked();
    void on_lineEdit_textEdited();
    void on_lineEdit_3_textEdited();
    void on_toolButton_3_pressed();
    void on_toolButton_3_released();
    void on_lineEdit_returnPressed();
    void on_toolButton_clicked();
    void on_toolButton_2_clicked();
    void on_lineEdit_2_returnPressed();
    void on_lineEdit_3_returnPressed();

private:
    Ui::LoginDialog *ui;
    QNetworkAccessManager manager;

    void hide_widgets(bool hide=true);
    void disable_widgets(bool disable=true);
};

#endif // LOGINDIALOG_H
