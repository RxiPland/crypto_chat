/********************************************************************************
** Form generated from reading UI file 'createroomdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CREATEROOMDIALOG_H
#define UI_CREATEROOMDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>

QT_BEGIN_NAMESPACE

class Ui_CreateRoomDialog
{
public:

    void setupUi(QDialog *CreateRoomDialog)
    {
        if (CreateRoomDialog->objectName().isEmpty())
            CreateRoomDialog->setObjectName("CreateRoomDialog");
        CreateRoomDialog->resize(640, 480);

        retranslateUi(CreateRoomDialog);

        QMetaObject::connectSlotsByName(CreateRoomDialog);
    } // setupUi

    void retranslateUi(QDialog *CreateRoomDialog)
    {
        CreateRoomDialog->setWindowTitle(QCoreApplication::translate("CreateRoomDialog", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CreateRoomDialog: public Ui_CreateRoomDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CREATEROOMDIALOG_H
