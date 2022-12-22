/********************************************************************************
** Form generated from reading UI file 'intervaldialog.ui'
**
** Created by: Qt User Interface Compiler version 6.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INTERVALDIALOG_H
#define UI_INTERVALDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>

QT_BEGIN_NAMESPACE

class Ui_IntervalDialog
{
public:

    void setupUi(QDialog *IntervalDialog)
    {
        if (IntervalDialog->objectName().isEmpty())
            IntervalDialog->setObjectName("IntervalDialog");
        IntervalDialog->resize(332, 145);

        retranslateUi(IntervalDialog);

        QMetaObject::connectSlotsByName(IntervalDialog);
    } // setupUi

    void retranslateUi(QDialog *IntervalDialog)
    {
        IntervalDialog->setWindowTitle(QCoreApplication::translate("IntervalDialog", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class IntervalDialog: public Ui_IntervalDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INTERVALDIALOG_H
