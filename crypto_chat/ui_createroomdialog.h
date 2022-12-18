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
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_CreateRoomDialog
{
public:
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer_3;
    QVBoxLayout *verticalLayout_2;
    QSpacerItem *verticalSpacer_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QLineEdit *lineEdit_2;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QLineEdit *lineEdit_3;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_3;
    QLineEdit *lineEdit_4;
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QCheckBox *checkBox;
    QSpacerItem *horizontalSpacer;
    QLineEdit *lineEdit;
    QSpacerItem *verticalSpacer_3;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *horizontalSpacer_4;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QDialog *CreateRoomDialog)
    {
        if (CreateRoomDialog->objectName().isEmpty())
            CreateRoomDialog->setObjectName("CreateRoomDialog");
        CreateRoomDialog->resize(400, 232);
        horizontalLayout_4 = new QHBoxLayout(CreateRoomDialog);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalSpacer_3 = new QSpacerItem(5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_3);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalSpacer_2 = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_2->addItem(verticalSpacer_2);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        label = new QLabel(CreateRoomDialog);
        label->setObjectName("label");

        horizontalLayout_2->addWidget(label);

        lineEdit_2 = new QLineEdit(CreateRoomDialog);
        lineEdit_2->setObjectName("lineEdit_2");
        lineEdit_2->setReadOnly(true);

        horizontalLayout_2->addWidget(lineEdit_2);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        label_2 = new QLabel(CreateRoomDialog);
        label_2->setObjectName("label_2");

        horizontalLayout_3->addWidget(label_2);

        lineEdit_3 = new QLineEdit(CreateRoomDialog);
        lineEdit_3->setObjectName("lineEdit_3");
        lineEdit_3->setMaxLength(100);
        lineEdit_3->setReadOnly(true);

        horizontalLayout_3->addWidget(lineEdit_3);


        verticalLayout_2->addLayout(horizontalLayout_3);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        horizontalLayout_6->setContentsMargins(-1, 0, -1, -1);
        label_3 = new QLabel(CreateRoomDialog);
        label_3->setObjectName("label_3");

        horizontalLayout_6->addWidget(label_3);

        lineEdit_4 = new QLineEdit(CreateRoomDialog);
        lineEdit_4->setObjectName("lineEdit_4");
        lineEdit_4->setMaxLength(25);
        lineEdit_4->setClearButtonEnabled(true);

        horizontalLayout_6->addWidget(lineEdit_4);


        verticalLayout_2->addLayout(horizontalLayout_6);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        verticalSpacer = new QSpacerItem(20, 30, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        checkBox = new QCheckBox(CreateRoomDialog);
        checkBox->setObjectName("checkBox");

        horizontalLayout->addWidget(checkBox);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        lineEdit = new QLineEdit(CreateRoomDialog);
        lineEdit->setObjectName("lineEdit");
        lineEdit->setMaxLength(100);
        lineEdit->setClearButtonEnabled(true);

        verticalLayout->addWidget(lineEdit);

        verticalSpacer_3 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_3);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        horizontalLayout_5->setContentsMargins(-1, 0, -1, -1);
        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_4);

        pushButton = new QPushButton(CreateRoomDialog);
        pushButton->setObjectName("pushButton");

        horizontalLayout_5->addWidget(pushButton);


        verticalLayout->addLayout(horizontalLayout_5);


        verticalLayout_2->addLayout(verticalLayout);


        horizontalLayout_4->addLayout(verticalLayout_2);

        horizontalSpacer_2 = new QSpacerItem(5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);


        retranslateUi(CreateRoomDialog);

        QMetaObject::connectSlotsByName(CreateRoomDialog);
    } // setupUi

    void retranslateUi(QDialog *CreateRoomDialog)
    {
        CreateRoomDialog->setWindowTitle(QCoreApplication::translate("CreateRoomDialog", "Dialog", nullptr));
        label->setText(QCoreApplication::translate("CreateRoomDialog", "Server:", nullptr));
        label_2->setText(QCoreApplication::translate("CreateRoomDialog", "ID m\303\255stnosti:", nullptr));
        label_3->setText(QCoreApplication::translate("CreateRoomDialog", "Va\305\241e p\305\231ezd\303\255vka:", nullptr));
        checkBox->setText(QCoreApplication::translate("CreateRoomDialog", "Heslo m\303\255stnosti", nullptr));
        lineEdit->setPlaceholderText(QCoreApplication::translate("CreateRoomDialog", "Bez hesla", nullptr));
        pushButton->setText(QCoreApplication::translate("CreateRoomDialog", "Vytvo\305\231it m\303\255stnost", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CreateRoomDialog: public Ui_CreateRoomDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CREATEROOMDIALOG_H
