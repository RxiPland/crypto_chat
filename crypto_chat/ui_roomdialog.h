/********************************************************************************
** Form generated from reading UI file 'roomdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ROOMDIALOG_H
#define UI_ROOMDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_RoomDialog
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
    QPushButton *pushButton_3;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_3;
    QLineEdit *lineEdit_4;
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QCheckBox *checkBox;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_7;
    QLineEdit *lineEdit;
    QToolButton *toolButton;
    QSpacerItem *verticalSpacer_3;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *pushButton_2;
    QSpacerItem *horizontalSpacer_4;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QDialog *RoomDialog)
    {
        if (RoomDialog->objectName().isEmpty())
            RoomDialog->setObjectName("RoomDialog");
        RoomDialog->resize(447, 259);
        horizontalLayout_4 = new QHBoxLayout(RoomDialog);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalSpacer_3 = new QSpacerItem(5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_3);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalSpacer_2 = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_2->addItem(verticalSpacer_2);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        label = new QLabel(RoomDialog);
        label->setObjectName("label");

        horizontalLayout_2->addWidget(label);

        lineEdit_2 = new QLineEdit(RoomDialog);
        lineEdit_2->setObjectName("lineEdit_2");
        lineEdit_2->setReadOnly(true);

        horizontalLayout_2->addWidget(lineEdit_2);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        label_2 = new QLabel(RoomDialog);
        label_2->setObjectName("label_2");

        horizontalLayout_3->addWidget(label_2);

        lineEdit_3 = new QLineEdit(RoomDialog);
        lineEdit_3->setObjectName("lineEdit_3");
        lineEdit_3->setCursor(QCursor(Qt::IBeamCursor));
        lineEdit_3->setMaxLength(100);
        lineEdit_3->setReadOnly(false);

        horizontalLayout_3->addWidget(lineEdit_3);

        pushButton_3 = new QPushButton(RoomDialog);
        pushButton_3->setObjectName("pushButton_3");

        horizontalLayout_3->addWidget(pushButton_3);


        verticalLayout_2->addLayout(horizontalLayout_3);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        horizontalLayout_6->setContentsMargins(-1, 0, -1, -1);
        label_3 = new QLabel(RoomDialog);
        label_3->setObjectName("label_3");

        horizontalLayout_6->addWidget(label_3);

        lineEdit_4 = new QLineEdit(RoomDialog);
        lineEdit_4->setObjectName("lineEdit_4");
        lineEdit_4->setMaxLength(26);
        lineEdit_4->setClearButtonEnabled(false);

        horizontalLayout_6->addWidget(lineEdit_4);


        verticalLayout_2->addLayout(horizontalLayout_6);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        verticalSpacer = new QSpacerItem(20, 14, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        checkBox = new QCheckBox(RoomDialog);
        checkBox->setObjectName("checkBox");

        horizontalLayout->addWidget(checkBox);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        horizontalLayout_7->setContentsMargins(-1, 0, -1, -1);
        lineEdit = new QLineEdit(RoomDialog);
        lineEdit->setObjectName("lineEdit");
        lineEdit->setMaxLength(100);
        lineEdit->setEchoMode(QLineEdit::Password);
        lineEdit->setClearButtonEnabled(true);

        horizontalLayout_7->addWidget(lineEdit);

        toolButton = new QToolButton(RoomDialog);
        toolButton->setObjectName("toolButton");

        horizontalLayout_7->addWidget(toolButton);


        verticalLayout->addLayout(horizontalLayout_7);

        verticalSpacer_3 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_3);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        horizontalLayout_5->setContentsMargins(-1, 0, -1, -1);
        pushButton_2 = new QPushButton(RoomDialog);
        pushButton_2->setObjectName("pushButton_2");

        horizontalLayout_5->addWidget(pushButton_2);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_4);

        pushButton = new QPushButton(RoomDialog);
        pushButton->setObjectName("pushButton");

        horizontalLayout_5->addWidget(pushButton);


        verticalLayout->addLayout(horizontalLayout_5);


        verticalLayout_2->addLayout(verticalLayout);


        horizontalLayout_4->addLayout(verticalLayout_2);

        horizontalSpacer_2 = new QSpacerItem(5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);


        retranslateUi(RoomDialog);

        QMetaObject::connectSlotsByName(RoomDialog);
    } // setupUi

    void retranslateUi(QDialog *RoomDialog)
    {
        RoomDialog->setWindowTitle(QCoreApplication::translate("RoomDialog", "Dialog", nullptr));
        label->setText(QCoreApplication::translate("RoomDialog", "Server:", nullptr));
        lineEdit_2->setPlaceholderText(QCoreApplication::translate("RoomDialog", "https://google.com", nullptr));
        label_2->setText(QCoreApplication::translate("RoomDialog", "ID m\303\255stnosti:", nullptr));
        lineEdit_3->setPlaceholderText(QCoreApplication::translate("RoomDialog", "Nap\305\231.", nullptr));
        pushButton_3->setText(QCoreApplication::translate("RoomDialog", "Generovat", nullptr));
        label_3->setText(QCoreApplication::translate("RoomDialog", "Va\305\241e p\305\231ezd\303\255vka:", nullptr));
        lineEdit_4->setPlaceholderText(QCoreApplication::translate("RoomDialog", "Povinn\303\251", nullptr));
        checkBox->setText(QCoreApplication::translate("RoomDialog", "Heslo m\303\255stnosti", nullptr));
        lineEdit->setPlaceholderText(QCoreApplication::translate("RoomDialog", "Bez hesla", nullptr));
        toolButton->setText(QCoreApplication::translate("RoomDialog", " Uk\303\241zat ", nullptr));
        pushButton_2->setText(QCoreApplication::translate("RoomDialog", "P\305\231epnout", nullptr));
        pushButton->setText(QCoreApplication::translate("RoomDialog", " Vytvo\305\231it m\303\255stnost ", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RoomDialog: public Ui_RoomDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ROOMDIALOG_H
