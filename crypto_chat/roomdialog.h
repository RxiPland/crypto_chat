#ifndef ROOMDIALOG_H
#define ROOMDIALOG_H

#include <QDialog>
#include <QNetworkAccessManager>

namespace Ui {
class RoomDialog;
}

class RoomDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RoomDialog(QWidget *parent = nullptr, bool createRoom = false, QString server_url = "", QString room_id = "");
    ~RoomDialog();

    bool createRoom = false;

    QString server_url;
    QString app_version;
    QByteArray user_agent;
    QString room_id;
    QString username;

    bool authentication_required = false;
    QString authentication_username;
    QString authentication_password;

    bool successful = false;
    bool deleleFolder = true;

private slots:
    void on_lineEdit_3_textEdited(const QString &arg1);
    void on_lineEdit_4_textEdited(const QString &arg1);
    void on_pushButton_clicked();
    void on_lineEdit_3_returnPressed();
    void on_lineEdit_4_returnPressed();
    void on_lineEdit_returnPressed();
    void on_pushButton_2_clicked();
    void on_checkBox_clicked();

private:
    Ui::RoomDialog *ui;
    QNetworkAccessManager manager;

    void closeEvent(QCloseEvent *bar = nullptr);

    void disable_widgets(bool disable=true);

    void createRoomFunc();
    void joinRoomFunc();

    QByteArray readTempFile(QString filename);
    void writeTempFile(QString filename, QByteArray content);

    QString getJson(QString name, QByteArray data);
};

#endif // ROOMDIALOG_H
