#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class ChatWindow; }
QT_END_NAMESPACE

class ChatWindow : public QMainWindow
{
    Q_OBJECT

public:
    ChatWindow(QWidget *parent = nullptr);
    ~ChatWindow();

    QString app_version;
    QByteArray user_agent;
    QString user_color = "black";

private slots:
    void on_pushButton_4_clicked();
    void on_pushButton_3_clicked();

private:
    Ui::ChatWindow *ui;
};
#endif // CHATWINDOW_H
