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

    void set_name(QString name);

private slots:
    void on_lineEdit_textChanged(const QString &arg1);
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_lineEdit_returnPressed();

private:
    Ui::NameChangeDialog *ui;
};

#endif // NAMECHANGEDIALOG_H
