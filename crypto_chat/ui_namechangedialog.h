/********************************************************************************
** Form generated from reading UI file 'namechangedialog.ui'
**
** Created by: Qt User Interface Compiler version 6.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NAMECHANGEDIALOG_H
#define UI_NAMECHANGEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_NameChangeDialog
{
public:
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_2;
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *lineEdit;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_2;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_3;

    void setupUi(QDialog *NameChangeDialog)
    {
        if (NameChangeDialog->objectName().isEmpty())
            NameChangeDialog->setObjectName("NameChangeDialog");
        NameChangeDialog->resize(367, 116);
        horizontalLayout_3 = new QHBoxLayout(NameChangeDialog);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalSpacer_2 = new QSpacerItem(5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        verticalSpacer_2 = new QSpacerItem(20, 5, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout->addItem(verticalSpacer_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        label = new QLabel(NameChangeDialog);
        label->setObjectName("label");

        horizontalLayout->addWidget(label);

        lineEdit = new QLineEdit(NameChangeDialog);
        lineEdit->setObjectName("lineEdit");
        lineEdit->setMaxLength(25);

        horizontalLayout->addWidget(lineEdit);


        verticalLayout->addLayout(horizontalLayout);

        verticalSpacer = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalSpacer = new QSpacerItem(66, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        pushButton_2 = new QPushButton(NameChangeDialog);
        pushButton_2->setObjectName("pushButton_2");

        horizontalLayout_2->addWidget(pushButton_2);

        pushButton = new QPushButton(NameChangeDialog);
        pushButton->setObjectName("pushButton");

        horizontalLayout_2->addWidget(pushButton);


        verticalLayout->addLayout(horizontalLayout_2);


        horizontalLayout_3->addLayout(verticalLayout);

        horizontalSpacer_3 = new QSpacerItem(5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);


        retranslateUi(NameChangeDialog);

        QMetaObject::connectSlotsByName(NameChangeDialog);
    } // setupUi

    void retranslateUi(QDialog *NameChangeDialog)
    {
        NameChangeDialog->setWindowTitle(QCoreApplication::translate("NameChangeDialog", "Dialog", nullptr));
        label->setText(QCoreApplication::translate("NameChangeDialog", "Nov\303\251 jm\303\251no:", nullptr));
        pushButton_2->setText(QCoreApplication::translate("NameChangeDialog", "Zru\305\241it", nullptr));
        pushButton->setText(QCoreApplication::translate("NameChangeDialog", "Ok", nullptr));
    } // retranslateUi

};

namespace Ui {
    class NameChangeDialog: public Ui_NameChangeDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NAMECHANGEDIALOG_H
