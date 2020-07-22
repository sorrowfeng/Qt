/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButtonLoad;
    QPushButton *pushButtonSave;
    QPushButton *pushButtonClose;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));
        Widget->resize(400, 300);
        verticalLayout = new QVBoxLayout(Widget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label = new QLabel(Widget);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);
        QFont font;
        font.setFamily(QString::fromUtf8("\351\273\221\344\275\223"));
        font.setPointSize(10);
        label->setFont(font);

        verticalLayout->addWidget(label);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        pushButtonLoad = new QPushButton(Widget);
        pushButtonLoad->setObjectName(QString::fromUtf8("pushButtonLoad"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(pushButtonLoad->sizePolicy().hasHeightForWidth());
        pushButtonLoad->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(pushButtonLoad);

        pushButtonSave = new QPushButton(Widget);
        pushButtonSave->setObjectName(QString::fromUtf8("pushButtonSave"));

        horizontalLayout->addWidget(pushButtonSave);

        pushButtonClose = new QPushButton(Widget);
        pushButtonClose->setObjectName(QString::fromUtf8("pushButtonClose"));
        sizePolicy1.setHeightForWidth(pushButtonClose->sizePolicy().hasHeightForWidth());
        pushButtonClose->setSizePolicy(sizePolicy1);
        pushButtonClose->setFont(font);
        pushButtonClose->setLayoutDirection(Qt::LeftToRight);

        horizontalLayout->addWidget(pushButtonClose);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(Widget);
        QObject::connect(pushButtonClose, SIGNAL(clicked()), Widget, SLOT(close()));

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QApplication::translate("Widget", "Widget", nullptr));
        label->setText(QApplication::translate("Widget", "TextLabel", nullptr));
        pushButtonLoad->setText(QApplication::translate("Widget", "\345\212\240\350\275\275", nullptr));
        pushButtonSave->setText(QApplication::translate("Widget", "\344\277\235\345\255\230", nullptr));
        pushButtonClose->setText(QApplication::translate("Widget", "Close", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
