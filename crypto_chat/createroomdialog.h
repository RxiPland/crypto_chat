#ifndef CREATEROOMDIALOG_H
#define CREATEROOMDIALOG_H

#include <QDialog>

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

    bool created = false;
    bool deleleFolder = true;


private slots:
    void on_checkBox_clicked();


private:
    Ui::CreateRoomDialog *ui;
    void closeEvent(QCloseEvent *bar = nullptr);
};

#endif // CREATEROOMDIALOG_H
