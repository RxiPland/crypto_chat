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
    explicit RoomDialog(QWidget *parent = nullptr, bool createRoom = true, QString server_url = "", QString room_id = "");
    ~RoomDialog();

    bool createRoom = true;

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

private:
    Ui::RoomDialog *ui;
    QNetworkAccessManager manager;

    void closeEvent(QCloseEvent *bar = nullptr);

    void disable_widgets(bool disable=true);

    QByteArray readTempFile(QString filename);
    QString getJson(QString name, QByteArray data);
};

#endif // ROOMDIALOG_H
