/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *pbtn_1;
    QPushButton *pbtn_2;
    QPushButton *pbtn_3;
    QPushButton *pbtn_5;
    QPushButton *pbtn_4;
    QPushButton *pbtn_6;
    QPushButton *pbtn_8;
    QPushButton *pbtn_7;
    QPushButton *pbtn_9;
    QPushButton *pbtn_0;
    QTextBrowser *browser;
    QPushButton *pbtn_div;
    QPushButton *pbtn_sum;
    QPushButton *pbtn_diff;
    QPushButton *pbtn_mult;
    QPushButton *pbtn_equal;
    QPushButton *pbtn_clear;
    QMenuBar *menubar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(255, 357);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        pbtn_1 = new QPushButton(centralwidget);
        pbtn_1->setObjectName("pbtn_1");
        pbtn_1->setGeometry(QRect(10, 80, 50, 50));
        pbtn_2 = new QPushButton(centralwidget);
        pbtn_2->setObjectName("pbtn_2");
        pbtn_2->setGeometry(QRect(70, 80, 50, 50));
        pbtn_3 = new QPushButton(centralwidget);
        pbtn_3->setObjectName("pbtn_3");
        pbtn_3->setGeometry(QRect(130, 80, 50, 50));
        pbtn_5 = new QPushButton(centralwidget);
        pbtn_5->setObjectName("pbtn_5");
        pbtn_5->setGeometry(QRect(70, 140, 50, 50));
        pbtn_4 = new QPushButton(centralwidget);
        pbtn_4->setObjectName("pbtn_4");
        pbtn_4->setGeometry(QRect(10, 140, 50, 50));
        pbtn_6 = new QPushButton(centralwidget);
        pbtn_6->setObjectName("pbtn_6");
        pbtn_6->setGeometry(QRect(130, 140, 50, 50));
        pbtn_8 = new QPushButton(centralwidget);
        pbtn_8->setObjectName("pbtn_8");
        pbtn_8->setGeometry(QRect(70, 200, 50, 50));
        pbtn_7 = new QPushButton(centralwidget);
        pbtn_7->setObjectName("pbtn_7");
        pbtn_7->setGeometry(QRect(10, 200, 50, 50));
        pbtn_9 = new QPushButton(centralwidget);
        pbtn_9->setObjectName("pbtn_9");
        pbtn_9->setGeometry(QRect(130, 200, 50, 50));
        pbtn_0 = new QPushButton(centralwidget);
        pbtn_0->setObjectName("pbtn_0");
        pbtn_0->setGeometry(QRect(70, 260, 50, 50));
        browser = new QTextBrowser(centralwidget);
        browser->setObjectName("browser");
        browser->setGeometry(QRect(10, 10, 231, 61));
        pbtn_div = new QPushButton(centralwidget);
        pbtn_div->setObjectName("pbtn_div");
        pbtn_div->setGeometry(QRect(190, 260, 50, 50));
        pbtn_sum = new QPushButton(centralwidget);
        pbtn_sum->setObjectName("pbtn_sum");
        pbtn_sum->setGeometry(QRect(190, 80, 50, 50));
        pbtn_diff = new QPushButton(centralwidget);
        pbtn_diff->setObjectName("pbtn_diff");
        pbtn_diff->setGeometry(QRect(190, 140, 50, 50));
        pbtn_mult = new QPushButton(centralwidget);
        pbtn_mult->setObjectName("pbtn_mult");
        pbtn_mult->setGeometry(QRect(190, 200, 50, 50));
        pbtn_equal = new QPushButton(centralwidget);
        pbtn_equal->setObjectName("pbtn_equal");
        pbtn_equal->setGeometry(QRect(130, 260, 50, 50));
        pbtn_clear = new QPushButton(centralwidget);
        pbtn_clear->setObjectName("pbtn_clear");
        pbtn_clear->setGeometry(QRect(10, 260, 50, 50));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 255, 22));
        MainWindow->setMenuBar(menubar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\320\232\320\260\320\273\321\214\320\272\321\203\320\273\321\217\321\202\320\276\321\200", nullptr));
        pbtn_1->setText(QCoreApplication::translate("MainWindow", "1", nullptr));
        pbtn_2->setText(QCoreApplication::translate("MainWindow", "2", nullptr));
        pbtn_3->setText(QCoreApplication::translate("MainWindow", "3", nullptr));
        pbtn_5->setText(QCoreApplication::translate("MainWindow", "5", nullptr));
        pbtn_4->setText(QCoreApplication::translate("MainWindow", "4", nullptr));
        pbtn_6->setText(QCoreApplication::translate("MainWindow", "6", nullptr));
        pbtn_8->setText(QCoreApplication::translate("MainWindow", "8", nullptr));
        pbtn_7->setText(QCoreApplication::translate("MainWindow", "7", nullptr));
        pbtn_9->setText(QCoreApplication::translate("MainWindow", "9", nullptr));
        pbtn_0->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        pbtn_div->setText(QCoreApplication::translate("MainWindow", "/", nullptr));
        pbtn_sum->setText(QCoreApplication::translate("MainWindow", "+", nullptr));
        pbtn_diff->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        pbtn_mult->setText(QCoreApplication::translate("MainWindow", "*", nullptr));
        pbtn_equal->setText(QCoreApplication::translate("MainWindow", "=", nullptr));
        pbtn_clear->setText(QCoreApplication::translate("MainWindow", "C", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
