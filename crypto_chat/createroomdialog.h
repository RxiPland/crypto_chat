#ifndef CREATEROOMDIALOG_H
#define CREATEROOMDIALOG_H

#include <QDialog>
#include <QNetworkAccessManager>

namespace Ui {
class CreateRoomDialog;
}

class CreateRoomDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateRoomDialog(QWidget *parent = nullptr, QString server_url = "", QString room_id = "");
    ~CreateRoomDialog();

    QString server_url;
    QString app_version;
    QByteArray user_agent;
    QString room_id;

    bool authentication_required = false;
    QString authentication_username;
    QString authentication_password;

    bool created = false;
    bool deleleFolder = true;


private slots:
    void on_checkBox_clicked();
    void on_pushButton_clicked();
    void on_lineEdit_4_textEdited(const QString &arg1);

private:
    Ui::CreateRoomDialog *ui;
    QNetworkAccessManager manager;

    void closeEvent(QCloseEvent *bar = nullptr);
    QByteArray readTempFile(QString filename);
    QString getJson(QString name, QByteArray data);
};

#endif // CREATEROOMDIALOG_H
