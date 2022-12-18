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
    explicit CreateRoomDialog(QWidget *parent = nullptr, QString version="v1.0.0", QByteArray userAgent = "crypt-chat");
    ~CreateRoomDialog();

    bool created = false;
    QString server_url;

    void set_info(QString server_url);

private slots:
    void on_checkBox_clicked();

private:
    Ui::CreateRoomDialog *ui;
};

#endif // CREATEROOMDIALOG_H
