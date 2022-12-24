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
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_IntervalDialog
{
public:
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_3;
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer_2;
    QLabel *label;
    QDoubleSpinBox *doubleSpinBox;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_2;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QDialog *IntervalDialog)
    {
        if (IntervalDialog->objectName().isEmpty())
            IntervalDialog->setObjectName("IntervalDialog");
        IntervalDialog->resize(311, 130);
        horizontalLayout_3 = new QHBoxLayout(IntervalDialog);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalSpacer_3 = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        verticalSpacer_2 = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout->addItem(verticalSpacer_2);

        label = new QLabel(IntervalDialog);
        label->setObjectName("label");

        verticalLayout->addWidget(label);

        doubleSpinBox = new QDoubleSpinBox(IntervalDialog);
        doubleSpinBox->setObjectName("doubleSpinBox");
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(doubleSpinBox->sizePolicy().hasHeightForWidth());
        doubleSpinBox->setSizePolicy(sizePolicy);
        doubleSpinBox->setMinimumSize(QSize(64, 0));
        doubleSpinBox->setDecimals(1);
        doubleSpinBox->setMinimum(0.100000000000000);
        doubleSpinBox->setMaximum(999.000000000000000);
        doubleSpinBox->setSingleStep(0.100000000000000);

        verticalLayout->addWidget(doubleSpinBox);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        pushButton_2 = new QPushButton(IntervalDialog);
        pushButton_2->setObjectName("pushButton_2");

        horizontalLayout_2->addWidget(pushButton_2);

        pushButton = new QPushButton(IntervalDialog);
        pushButton->setObjectName("pushButton");

        horizontalLayout_2->addWidget(pushButton);


        verticalLayout->addLayout(horizontalLayout_2);


        horizontalLayout_3->addLayout(verticalLayout);

        horizontalSpacer_2 = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);


        retranslateUi(IntervalDialog);

        QMetaObject::connectSlotsByName(IntervalDialog);
    } // setupUi

    void retranslateUi(QDialog *IntervalDialog)
    {
        IntervalDialog->setWindowTitle(QCoreApplication::translate("IntervalDialog", "Dialog", nullptr));
        label->setText(QCoreApplication::translate("IntervalDialog", "Aktualizace ka\305\276d\303\275ch X vte\305\231in", nullptr));
        doubleSpinBox->setSuffix(QString());
        pushButton_2->setText(QCoreApplication::translate("IntervalDialog", "Zru\305\241it", nullptr));
        pushButton->setText(QCoreApplication::translate("IntervalDialog", "Ok", nullptr));
    } // retranslateUi

};

namespace Ui {
    class IntervalDialog: public Ui_IntervalDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INTERVALDIALOG_H
