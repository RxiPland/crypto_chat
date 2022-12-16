/********************************************************************************
** Form generated from reading UI file 'chatwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHATWINDOW_H
#define UI_CHATWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChatWindow
{
public:
    QAction *action_room_1;
    QAction *action_room_2;
    QAction *action_zpravy_1;
    QAction *action_zpravy_2;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_3;
    QSpacerItem *verticalSpacer_4;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QVBoxLayout *verticalLayout;
    QPlainTextEdit *plainTextEdit;
    QSpacerItem *verticalSpacer_2;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *lineEdit;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_3;
    QVBoxLayout *verticalLayout_2;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QSpacerItem *verticalSpacer_3;
    QPushButton *pushButton_4;
    QSpacerItem *verticalSpacer;
    QSpacerItem *horizontalSpacer_2;
    QMenuBar *menubar;
    QMenu *menuMistnost;
    QMenu *menuZpravy;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ChatWindow)
    {
        if (ChatWindow->objectName().isEmpty())
            ChatWindow->setObjectName("ChatWindow");
        ChatWindow->resize(690, 401);
        action_room_1 = new QAction(ChatWindow);
        action_room_1->setObjectName("action_room_1");
        action_room_2 = new QAction(ChatWindow);
        action_room_2->setObjectName("action_room_2");
        action_zpravy_1 = new QAction(ChatWindow);
        action_zpravy_1->setObjectName("action_zpravy_1");
        action_zpravy_2 = new QAction(ChatWindow);
        action_zpravy_2->setObjectName("action_zpravy_2");
        centralwidget = new QWidget(ChatWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout_3 = new QVBoxLayout(centralwidget);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalSpacer_4 = new QSpacerItem(20, 5, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_3->addItem(verticalSpacer_4);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer = new QSpacerItem(17, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        plainTextEdit = new QPlainTextEdit(centralwidget);
        plainTextEdit->setObjectName("plainTextEdit");
        plainTextEdit->setReadOnly(true);

        verticalLayout->addWidget(plainTextEdit);

        verticalSpacer_2 = new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout->addItem(verticalSpacer_2);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        lineEdit = new QLineEdit(centralwidget);
        lineEdit->setObjectName("lineEdit");
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lineEdit->sizePolicy().hasHeightForWidth());
        lineEdit->setSizePolicy(sizePolicy);
        lineEdit->setMinimumSize(QSize(0, 30));
        lineEdit->setToolTipDuration(-1);
        lineEdit->setMaxLength(200);
        lineEdit->setClearButtonEnabled(true);

        horizontalLayout_2->addWidget(lineEdit);

        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName("pushButton");
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(pushButton->sizePolicy().hasHeightForWidth());
        pushButton->setSizePolicy(sizePolicy1);
        pushButton->setMinimumSize(QSize(130, 30));
        pushButton->setToolTipDuration(-1);

        horizontalLayout_2->addWidget(pushButton);


        verticalLayout->addLayout(horizontalLayout_2);


        horizontalLayout->addLayout(verticalLayout);

        horizontalSpacer_3 = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        pushButton_2 = new QPushButton(centralwidget);
        pushButton_2->setObjectName("pushButton_2");
        pushButton_2->setMinimumSize(QSize(110, 30));

        verticalLayout_2->addWidget(pushButton_2);

        pushButton_3 = new QPushButton(centralwidget);
        pushButton_3->setObjectName("pushButton_3");
        pushButton_3->setMinimumSize(QSize(110, 30));

        verticalLayout_2->addWidget(pushButton_3);

        verticalSpacer_3 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_2->addItem(verticalSpacer_3);

        pushButton_4 = new QPushButton(centralwidget);
        pushButton_4->setObjectName("pushButton_4");
        pushButton_4->setMinimumSize(QSize(110, 30));

        verticalLayout_2->addWidget(pushButton_4);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);


        horizontalLayout->addLayout(verticalLayout_2);

        horizontalSpacer_2 = new QSpacerItem(17, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout_3->addLayout(horizontalLayout);

        ChatWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ChatWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 690, 21));
        menuMistnost = new QMenu(menubar);
        menuMistnost->setObjectName("menuMistnost");
        menuZpravy = new QMenu(menubar);
        menuZpravy->setObjectName("menuZpravy");
        ChatWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(ChatWindow);
        statusbar->setObjectName("statusbar");
        ChatWindow->setStatusBar(statusbar);

        menubar->addAction(menuMistnost->menuAction());
        menubar->addAction(menuZpravy->menuAction());
        menuMistnost->addAction(action_room_1);
        menuMistnost->addSeparator();
        menuMistnost->addAction(action_room_2);
        menuZpravy->addAction(action_zpravy_1);
        menuZpravy->addSeparator();
        menuZpravy->addAction(action_zpravy_2);

        retranslateUi(ChatWindow);

        QMetaObject::connectSlotsByName(ChatWindow);
    } // setupUi

    void retranslateUi(QMainWindow *ChatWindow)
    {
        ChatWindow->setWindowTitle(QCoreApplication::translate("ChatWindow", "ChatWindow", nullptr));
        action_room_1->setText(QCoreApplication::translate("ChatWindow", "Smazat m\303\255stnost", nullptr));
        action_room_2->setText(QCoreApplication::translate("ChatWindow", "Odpojit se", nullptr));
        action_zpravy_1->setText(QCoreApplication::translate("ChatWindow", "Po\304\215et: 0", nullptr));
        action_zpravy_2->setText(QCoreApplication::translate("ChatWindow", "Aktualizace za 5s", nullptr));
        plainTextEdit->setPlaceholderText(QCoreApplication::translate("ChatWindow", "Zde budou zpr\303\241vy z konverzace", nullptr));
        lineEdit->setPlaceholderText(QCoreApplication::translate("ChatWindow", "Napsat zpr\303\241vu", nullptr));
#if QT_CONFIG(tooltip)
        pushButton->setToolTip(QString());
#endif // QT_CONFIG(tooltip)
        pushButton->setText(QCoreApplication::translate("ChatWindow", "Odeslat", nullptr));
        pushButton_2->setText(QCoreApplication::translate("ChatWindow", "Informace", nullptr));
        pushButton_3->setText(QCoreApplication::translate("ChatWindow", "Nov\303\241 p\305\231ezd\303\255vka", nullptr));
        pushButton_4->setText(QCoreApplication::translate("ChatWindow", "Soubory", nullptr));
        menuMistnost->setTitle(QCoreApplication::translate("ChatWindow", "M\303\255stnost", nullptr));
        menuZpravy->setTitle(QCoreApplication::translate("ChatWindow", "Zpr\303\241vy", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ChatWindow: public Ui_ChatWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHATWINDOW_H
