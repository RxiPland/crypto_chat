#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>

#include <QNetworkAccessManager>

QT_BEGIN_NAMESPACE
namespace Ui { class ChatWindow; }
QT_END_NAMESPACE

class ChatWindow : public QMainWindow
{
    Q_OBJECT

public:
    ChatWindow(QWidget *parent = nullptr, QString server_url = "", QString user_name = "");
    ~ChatWindow();

    void welcomeMessage();

    QString app_version;
    QByteArray user_agent;

    // user
    QString user_color = "black";  // default color
    QString user_name;
    QString prefix;
    QString room_password;

    float refreshInterval = 5.9;

    // server
    bool authentication_required = false;
    QString authentication_username;
    QString authentication_password;
    QString server_url;
    QString room_id;

private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_action_zpravy_4_triggered();
    void on_action_zpravy_3_triggered();
    void on_action_room_3_triggered();
    void on_action_zpravy_3_1_triggered();
    void on_action_zpravy_3_2_triggered();
    void on_action_zpravy_3_3_triggered();
    void on_action_zpravy_2_1_triggered();
    void on_action_room_2_triggered();

    void on_action_advanced_1_triggered();

private:
    void closeEvent(QCloseEvent *bar = nullptr);
    Ui::ChatWindow *ui;
    QNetworkAccessManager manager;

    bool restart = false;

    void sendMessage(QString color="black", QString time="", QString prefix="", QString nickname="", QString message="");

};
#endif // CHATWINDOW_H
