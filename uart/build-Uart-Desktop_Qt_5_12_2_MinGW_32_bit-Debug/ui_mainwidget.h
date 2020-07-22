/********************************************************************************
** Form generated from reading UI file 'mainwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWIDGET_H
#define UI_MAINWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWidget
{
public:
    QGridLayout *gridLayout;
    QLabel *labelComText;
    QComboBox *comboBoxCom;
    QPushButton *pushButtonLink;
    QPushButton *pushButtonDisconnect;
    QPushButton *pushButtonChoose;
    QPushButton *pushButtonSend;

    void setupUi(QWidget *MainWidget)
    {
        if (MainWidget->objectName().isEmpty())
            MainWidget->setObjectName(QString::fromUtf8("MainWidget"));
        MainWidget->resize(400, 300);
        gridLayout = new QGridLayout(MainWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        labelComText = new QLabel(MainWidget);
        labelComText->setObjectName(QString::fromUtf8("labelComText"));

        gridLayout->addWidget(labelComText, 0, 1, 1, 1);

        comboBoxCom = new QComboBox(MainWidget);
        comboBoxCom->setObjectName(QString::fromUtf8("comboBoxCom"));

        gridLayout->addWidget(comboBoxCom, 1, 0, 1, 1);

        pushButtonLink = new QPushButton(MainWidget);
        pushButtonLink->setObjectName(QString::fromUtf8("pushButtonLink"));

        gridLayout->addWidget(pushButtonLink, 1, 1, 1, 1);

        pushButtonDisconnect = new QPushButton(MainWidget);
        pushButtonDisconnect->setObjectName(QString::fromUtf8("pushButtonDisconnect"));

        gridLayout->addWidget(pushButtonDisconnect, 1, 2, 1, 1);

        pushButtonChoose = new QPushButton(MainWidget);
        pushButtonChoose->setObjectName(QString::fromUtf8("pushButtonChoose"));

        gridLayout->addWidget(pushButtonChoose, 2, 0, 1, 1);

        pushButtonSend = new QPushButton(MainWidget);
        pushButtonSend->setObjectName(QString::fromUtf8("pushButtonSend"));

        gridLayout->addWidget(pushButtonSend, 2, 2, 1, 1);


        retranslateUi(MainWidget);

        QMetaObject::connectSlotsByName(MainWidget);
    } // setupUi

    void retranslateUi(QWidget *MainWidget)
    {
        MainWidget->setWindowTitle(QApplication::translate("MainWidget", "MainWidget", nullptr));
        labelComText->setText(QApplication::translate("MainWidget", "\346\234\252\350\277\236\346\216\245", nullptr));
        pushButtonLink->setText(QApplication::translate("MainWidget", "\350\277\236\346\216\245", nullptr));
        pushButtonDisconnect->setText(QApplication::translate("MainWidget", "\346\226\255\345\274\200", nullptr));
        pushButtonChoose->setText(QApplication::translate("MainWidget", "\351\200\211\346\213\251\346\226\207\344\273\266", nullptr));
        pushButtonSend->setText(QApplication::translate("MainWidget", "\345\217\221\351\200\201", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWidget: public Ui_MainWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWIDGET_H
