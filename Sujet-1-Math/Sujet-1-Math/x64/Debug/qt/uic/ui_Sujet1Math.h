/********************************************************************************
** Form generated from reading UI file 'Sujet1Math.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SUJET1MATH_H
#define UI_SUJET1MATH_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Sujet1MathClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *Sujet1MathClass)
    {
        if (Sujet1MathClass->objectName().isEmpty())
            Sujet1MathClass->setObjectName("Sujet1MathClass");
        Sujet1MathClass->resize(600, 400);
        menuBar = new QMenuBar(Sujet1MathClass);
        menuBar->setObjectName("menuBar");
        Sujet1MathClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(Sujet1MathClass);
        mainToolBar->setObjectName("mainToolBar");
        Sujet1MathClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(Sujet1MathClass);
        centralWidget->setObjectName("centralWidget");
        Sujet1MathClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(Sujet1MathClass);
        statusBar->setObjectName("statusBar");
        Sujet1MathClass->setStatusBar(statusBar);

        retranslateUi(Sujet1MathClass);

        QMetaObject::connectSlotsByName(Sujet1MathClass);
    } // setupUi

    void retranslateUi(QMainWindow *Sujet1MathClass)
    {
        Sujet1MathClass->setWindowTitle(QCoreApplication::translate("Sujet1MathClass", "Sujet1Math", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Sujet1MathClass: public Ui_Sujet1MathClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SUJET1MATH_H
