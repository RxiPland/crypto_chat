/********************************************************************************
** Form generated from reading UI file 'chatwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHATWINDOW_H
#define UI_CHATWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChatWindow
{
public:
    QAction *action_room_1;
    QAction *action_room_2;
    QAction *action_zpravy_1;
    QAction *action_room_3;
    QAction *action_zpravy_3;
    QAction *action_zpravy_4;
    QAction *action_zpravy_2_1;
    QAction *action_zpravy_3_1;
    QAction *action_zpravy_3_2;
    QAction *action_zpravy_3_3;
    QAction *action_advanced_1;
    QAction *action_advanced_2;
    QAction *action_room_4;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_3;
    QSpacerItem *verticalSpacer_4;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QVBoxLayout *verticalLayout;
    QTextBrowser *textBrowser;
    QSpacerItem *verticalSpacer_2;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *lineEdit;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_3;
    QVBoxLayout *verticalLayout_2;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *pushButton_4;
    QLabel *label;
    QSpacerItem *verticalSpacer_3;
    QPushButton *pushButton_5;
    QSpacerItem *verticalSpacer;
    QSpacerItem *horizontalSpacer_2;
    QMenuBar *menubar;
    QMenu *menuMistnost;
    QMenu *menuZpravy;
    QMenu *menuZpravy_2;
    QMenu *menuZpravy_3;
    QMenu *menuAdvanced;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ChatWindow)
    {
        if (ChatWindow->objectName().isEmpty())
            ChatWindow->setObjectName("ChatWindow");
        ChatWindow->resize(690, 401);
        ChatWindow->setFocusPolicy(Qt::StrongFocus);
        action_room_1 = new QAction(ChatWindow);
        action_room_1->setObjectName("action_room_1");
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/delete.ico"), QSize(), QIcon::Normal, QIcon::Off);
        action_room_1->setIcon(icon);
        action_room_2 = new QAction(ChatWindow);
        action_room_2->setObjectName("action_room_2");
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/images/copy.ico"), QSize(), QIcon::Normal, QIcon::Off);
        action_room_2->setIcon(icon1);
        action_zpravy_1 = new QAction(ChatWindow);
        action_zpravy_1->setObjectName("action_zpravy_1");
        action_zpravy_1->setEnabled(false);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/images/email.ico"), QSize(), QIcon::Normal, QIcon::Off);
        action_zpravy_1->setIcon(icon2);
        action_room_3 = new QAction(ChatWindow);
        action_room_3->setObjectName("action_room_3");
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/images/power_off.ico"), QSize(), QIcon::Normal, QIcon::Off);
        action_room_3->setIcon(icon3);
        action_zpravy_3 = new QAction(ChatWindow);
        action_zpravy_3->setObjectName("action_zpravy_3");
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/images/clean.ico"), QSize(), QIcon::Normal, QIcon::Off);
        action_zpravy_3->setIcon(icon4);
        action_zpravy_4 = new QAction(ChatWindow);
        action_zpravy_4->setObjectName("action_zpravy_4");
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/images/refresh.ico"), QSize(), QIcon::Normal, QIcon::Off);
        action_zpravy_4->setIcon(icon5);
        action_zpravy_2_1 = new QAction(ChatWindow);
        action_zpravy_2_1->setObjectName("action_zpravy_2_1");
        action_zpravy_3_1 = new QAction(ChatWindow);
        action_zpravy_3_1->setObjectName("action_zpravy_3_1");
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/images/zoom_in.ico"), QSize(), QIcon::Normal, QIcon::Off);
        action_zpravy_3_1->setIcon(icon6);
        action_zpravy_3_2 = new QAction(ChatWindow);
        action_zpravy_3_2->setObjectName("action_zpravy_3_2");
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/images/zoom_out.ico"), QSize(), QIcon::Normal, QIcon::Off);
        action_zpravy_3_2->setIcon(icon7);
        action_zpravy_3_3 = new QAction(ChatWindow);
        action_zpravy_3_3->setObjectName("action_zpravy_3_3");
        action_advanced_1 = new QAction(ChatWindow);
        action_advanced_1->setObjectName("action_advanced_1");
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/images/github.ico"), QSize(), QIcon::Normal, QIcon::Off);
        action_advanced_1->setIcon(icon8);
        action_advanced_2 = new QAction(ChatWindow);
        action_advanced_2->setObjectName("action_advanced_2");
        action_advanced_2->setEnabled(false);
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/images/settings.ico"), QSize(), QIcon::Normal, QIcon::Off);
        action_advanced_2->setIcon(icon9);
        action_room_4 = new QAction(ChatWindow);
        action_room_4->setObjectName("action_room_4");
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
        textBrowser = new QTextBrowser(centralwidget);
        textBrowser->setObjectName("textBrowser");
        QFont font;
        font.setPointSize(9);
        textBrowser->setFont(font);
        textBrowser->setFocusPolicy(Qt::ClickFocus);
        textBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        textBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        textBrowser->setUndoRedoEnabled(false);
        textBrowser->setReadOnly(true);
        textBrowser->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard|Qt::LinksAccessibleByMouse|Qt::TextBrowserInteraction|Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);

        verticalLayout->addWidget(textBrowser);

        verticalSpacer_2 = new QSpacerItem(0, 7, QSizePolicy::Minimum, QSizePolicy::Fixed);

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
        lineEdit->setFocusPolicy(Qt::ClickFocus);
        lineEdit->setToolTipDuration(-1);
        lineEdit->setMaxLength(100000);
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

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalLayout_4->setContentsMargins(-1, -1, 0, -1);
        pushButton_4 = new QPushButton(centralwidget);
        pushButton_4->setObjectName("pushButton_4");
        pushButton_4->setMinimumSize(QSize(110, 30));

        horizontalLayout_4->addWidget(pushButton_4);

        label = new QLabel(centralwidget);
        label->setObjectName("label");
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy2);
        label->setMinimumSize(QSize(24, 24));

        horizontalLayout_4->addWidget(label);


        verticalLayout_2->addLayout(horizontalLayout_4);

        verticalSpacer_3 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_2->addItem(verticalSpacer_3);

        pushButton_5 = new QPushButton(centralwidget);
        pushButton_5->setObjectName("pushButton_5");
        pushButton_5->setEnabled(false);
        pushButton_5->setMinimumSize(QSize(110, 30));

        verticalLayout_2->addWidget(pushButton_5);

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
        menuZpravy_2 = new QMenu(menuZpravy);
        menuZpravy_2->setObjectName("menuZpravy_2");
        menuZpravy_3 = new QMenu(menuZpravy);
        menuZpravy_3->setObjectName("menuZpravy_3");
        menuAdvanced = new QMenu(menubar);
        menuAdvanced->setObjectName("menuAdvanced");
        ChatWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(ChatWindow);
        statusbar->setObjectName("statusbar");
        ChatWindow->setStatusBar(statusbar);

        menubar->addAction(menuMistnost->menuAction());
        menubar->addAction(menuZpravy->menuAction());
        menubar->addAction(menuAdvanced->menuAction());
        menuMistnost->addAction(action_room_1);
        menuMistnost->addAction(action_room_4);
        menuMistnost->addSeparator();
        menuMistnost->addAction(action_room_2);
        menuMistnost->addAction(action_room_3);
        menuZpravy->addAction(action_zpravy_1);
        menuZpravy->addAction(menuZpravy_2->menuAction());
        menuZpravy->addSeparator();
        menuZpravy->addAction(action_zpravy_3);
        menuZpravy->addAction(action_zpravy_4);
        menuZpravy->addSeparator();
        menuZpravy->addAction(menuZpravy_3->menuAction());
        menuZpravy_2->addAction(action_zpravy_2_1);
        menuZpravy_3->addAction(action_zpravy_3_1);
        menuZpravy_3->addAction(action_zpravy_3_2);
        menuZpravy_3->addSeparator();
        menuZpravy_3->addAction(action_zpravy_3_3);
        menuAdvanced->addAction(action_advanced_1);
        menuAdvanced->addSeparator();
        menuAdvanced->addAction(action_advanced_2);

        retranslateUi(ChatWindow);

        QMetaObject::connectSlotsByName(ChatWindow);
    } // setupUi

    void retranslateUi(QMainWindow *ChatWindow)
    {
        ChatWindow->setWindowTitle(QCoreApplication::translate("ChatWindow", "ChatWindow", nullptr));
        action_room_1->setText(QCoreApplication::translate("ChatWindow", "Smazat m\303\255stnost", nullptr));
        action_room_2->setText(QCoreApplication::translate("ChatWindow", "Kop\303\255rovat ID m\303\255stnosti", nullptr));
        action_zpravy_1->setText(QCoreApplication::translate("ChatWindow", "Po\304\215et zobrazen\303\275ch: 0", nullptr));
        action_room_3->setText(QCoreApplication::translate("ChatWindow", "Odpojit se", nullptr));
        action_zpravy_3->setText(QCoreApplication::translate("ChatWindow", "Vy\304\215istit chat", nullptr));
#if QT_CONFIG(tooltip)
        action_zpravy_3->setToolTip(QCoreApplication::translate("ChatWindow", "Vy\304\215istit chat", nullptr));
#endif // QT_CONFIG(tooltip)
        action_zpravy_4->setText(QCoreApplication::translate("ChatWindow", "Aktualizovat", nullptr));
#if QT_CONFIG(tooltip)
        action_zpravy_4->setToolTip(QCoreApplication::translate("ChatWindow", "Aktualizovat", nullptr));
#endif // QT_CONFIG(tooltip)
        action_zpravy_2_1->setText(QCoreApplication::translate("ChatWindow", "Nastavit vlastn\303\255", nullptr));
        action_zpravy_3_1->setText(QCoreApplication::translate("ChatWindow", "Zv\304\233t\305\241it", nullptr));
#if QT_CONFIG(shortcut)
        action_zpravy_3_1->setShortcut(QCoreApplication::translate("ChatWindow", "Ctrl++", nullptr));
#endif // QT_CONFIG(shortcut)
        action_zpravy_3_2->setText(QCoreApplication::translate("ChatWindow", "Zmen\305\241it", nullptr));
#if QT_CONFIG(shortcut)
        action_zpravy_3_2->setShortcut(QCoreApplication::translate("ChatWindow", "Ctrl+-", nullptr));
#endif // QT_CONFIG(shortcut)
        action_zpravy_3_3->setText(QCoreApplication::translate("ChatWindow", "Reset", nullptr));
        action_advanced_1->setText(QCoreApplication::translate("ChatWindow", "Zdrojov\303\275 k\303\263d", nullptr));
        action_advanced_2->setText(QCoreApplication::translate("ChatWindow", "Nastaven\303\255", nullptr));
        action_room_4->setText(QCoreApplication::translate("ChatWindow", "Vygenerovat nov\303\275 kl\303\255\304\215", nullptr));
        textBrowser->setHtml(QCoreApplication::translate("ChatWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><meta charset=\"utf-8\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"hr { height: 1px; border-width: 0; }\n"
"li.unchecked::marker { content: \"\\2610\"; }\n"
"li.checked::marker { content: \"\\2612\"; }\n"
"</style></head><body style=\" font-family:'Segoe UI'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", nullptr));
        textBrowser->setPlaceholderText(QCoreApplication::translate("ChatWindow", "Zde budou zpr\303\241vy z konverzace", nullptr));
        lineEdit->setPlaceholderText(QCoreApplication::translate("ChatWindow", "Napsat zpr\303\241vu", nullptr));
#if QT_CONFIG(tooltip)
        pushButton->setToolTip(QString());
#endif // QT_CONFIG(tooltip)
        pushButton->setText(QCoreApplication::translate("ChatWindow", "Odeslat", nullptr));
        pushButton_2->setText(QCoreApplication::translate("ChatWindow", "Informace", nullptr));
        pushButton_3->setText(QCoreApplication::translate("ChatWindow", "Nov\303\241 p\305\231ezd\303\255vka", nullptr));
        pushButton_4->setText(QCoreApplication::translate("ChatWindow", "Zm\304\233nit barvu", nullptr));
        label->setText(QString());
        pushButton_5->setText(QCoreApplication::translate("ChatWindow", "Soubory", nullptr));
        menuMistnost->setTitle(QCoreApplication::translate("ChatWindow", "M\303\255stnost", nullptr));
        menuZpravy->setTitle(QCoreApplication::translate("ChatWindow", "Zpr\303\241vy", nullptr));
        menuZpravy_2->setTitle(QCoreApplication::translate("ChatWindow", "Aktualizace za 5s", nullptr));
        menuZpravy_3->setTitle(QCoreApplication::translate("ChatWindow", "Velikost p\303\255sma", nullptr));
        menuAdvanced->setTitle(QCoreApplication::translate("ChatWindow", "Mo\305\276nosti", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ChatWindow: public Ui_ChatWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHATWINDOW_H
