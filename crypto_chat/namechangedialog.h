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

private:
    Ui::NameChangeDialog *ui;
};

#endif // NAMECHANGEDIALOG_H
