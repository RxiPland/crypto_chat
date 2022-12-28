/********************************************************************************
** Form generated from reading UI file 'namechangedialog.ui'
**
** Created by: Qt User Interface Compiler version 6.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NAMECHANGEDIALOG_H
#define UI_NAMECHANGEDIALOG_H

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
    QSpacerItem *verticalSpacer_3;
    QHBoxLayout *horizontalLayout_5;
    QCheckBox *checkBox;
    QLineEdit *lineEdit_2;
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
        NameChangeDialog->resize(367, 178);
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
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);
        label->setMinimumSize(QSize(0, 24));

        horizontalLayout->addWidget(label);

        lineEdit = new QLineEdit(NameChangeDialog);
        lineEdit->setObjectName("lineEdit");
        lineEdit->setMinimumSize(QSize(0, 24));
        lineEdit->setMaxLength(26);

        horizontalLayout->addWidget(lineEdit);


        verticalLayout->addLayout(horizontalLayout);

        verticalSpacer_3 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        verticalLayout->addItem(verticalSpacer_3);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        checkBox = new QCheckBox(NameChangeDialog);
        checkBox->setObjectName("checkBox");
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(checkBox->sizePolicy().hasHeightForWidth());
        checkBox->setSizePolicy(sizePolicy1);
        checkBox->setMinimumSize(QSize(0, 24));

        horizontalLayout_5->addWidget(checkBox);

        lineEdit_2 = new QLineEdit(NameChangeDialog);
        lineEdit_2->setObjectName("lineEdit_2");
        sizePolicy1.setHeightForWidth(lineEdit_2->sizePolicy().hasHeightForWidth());
        lineEdit_2->setSizePolicy(sizePolicy1);
        lineEdit_2->setMinimumSize(QSize(255, 24));
        lineEdit_2->setMaxLength(26);

        horizontalLayout_5->addWidget(lineEdit_2);


        verticalLayout->addLayout(horizontalLayout_5);

        verticalSpacer = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Minimum);

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
        label->setText(QCoreApplication::translate("NameChangeDialog", "Nov\303\241 p\305\231ezd\303\255vka:", nullptr));
        checkBox->setText(QCoreApplication::translate("NameChangeDialog", "Prefix", nullptr));
        lineEdit_2->setPlaceholderText(QCoreApplication::translate("NameChangeDialog", "Objev\303\255 se p\305\231ed jm\303\251nem", nullptr));
        pushButton_2->setText(QCoreApplication::translate("NameChangeDialog", "Zru\305\241it", nullptr));
        pushButton->setText(QCoreApplication::translate("NameChangeDialog", "Ok", nullptr));
    } // retranslateUi

};

namespace Ui {
    class NameChangeDialog: public Ui_NameChangeDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NAMECHANGEDIALOG_H
