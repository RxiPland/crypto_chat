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
    explicit CreateRoomDialog(QWidget *parent = nullptr, QString server_url = "");
    ~CreateRoomDialog();

    QString server_url;
    QString app_version;
    QByteArray user_agent;

    bool created = false;


private slots:
    void on_checkBox_clicked();

private:
    Ui::CreateRoomDialog *ui;
};

#endif // CREATEROOMDIALOG_H
