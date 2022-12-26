#ifndef JOINROOMDIALOG_H
#define JOINROOMDIALOG_H

#include <QDialog>

namespace Ui {
class JoinRoomDialog;
}

class JoinRoomDialog : public QDialog
{
    Q_OBJECT

public:
    explicit JoinRoomDialog(QWidget *parent = nullptr, QString server_url = "");
    ~JoinRoomDialog();

    QString server_url;
    QString app_version;
    QByteArray user_agent;
    QString room_id;
    QString username;

    bool authentication_required = false;
    QString authentication_username;
    QString authentication_password;

    bool joined = false;
    bool deleleFolder = true;

private slots:
    void on_pushButton_clicked();

    void on_lineEdit_3_textEdited(const QString &arg1);

    void on_toolButton_clicked();

private:
    Ui::JoinRoomDialog *ui;
};

#endif // JOINROOMDIALOG_H
