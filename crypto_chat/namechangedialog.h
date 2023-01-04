#ifndef NAMECHANGEDIALOG_H
#define NAMECHANGEDIALOG_H

#include <QDialog>

namespace Ui {
class NameChangeDialog;
}

class NameChangeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NameChangeDialog(QWidget *parent = nullptr);
    ~NameChangeDialog();

    QString original_name;
    QString new_name = "";
    QString prefix;

    void set_name(QString name);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_lineEdit_returnPressed();
    void on_lineEdit_textEdited(const QString &arg1);
    void on_checkBox_clicked();
    void on_lineEdit_2_textEdited(const QString &arg1);
    void on_lineEdit_2_returnPressed();

private:
    Ui::NameChangeDialog *ui;
};

#endif // NAMECHANGEDIALOG_H
