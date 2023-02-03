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
    explicit RoomDialog(QWidget *parent = nullptr, bool createRoom = false, QString server_url = "");
    ~RoomDialog();

    bool createRoom = false;

    QString server_url;
    QString app_version;
    QByteArray user_agent;
    QString room_id;
    QString room_password;
    QString username;
    int serverMessagesCount=0;

    bool authentication_required = false;
    QString authentication_username;
    QString authentication_password;

    QString rsaPublicKeyPemHex;
    QString rsaPrivateKeyPemHex;
    QString serverPublicKeyPemHex;

    QString roomAesKeyHex;

    bool successful = false;

private slots:
    void on_lineEdit_3_textEdited(const QString &arg1);
    void on_lineEdit_4_textEdited(const QString &arg1);
    void on_pushButton_clicked();
    void on_lineEdit_3_returnPressed();
    void on_lineEdit_4_returnPressed();
    void on_lineEdit_returnPressed();
    void on_pushButton_2_clicked();
    void on_checkBox_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::RoomDialog *ui;
    QNetworkAccessManager manager;

    void closeEvent(QCloseEvent *bar = nullptr);

    void disable_widgets(bool disable=true);

    void createRoomFunc();
    void joinRoomFunc();
    void setPassword(QString password);

    QString generateId();

    QList<QJsonValue> decryptRsa(QStringList json_keys, QByteArray response);
};

#endif // ROOMDIALOG_H
