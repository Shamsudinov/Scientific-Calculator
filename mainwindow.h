#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QDebug>
#include <QPushButton>
#include <QGroupBox>
#include <QGridLayout>

#include <stdexcept>

#include "expressioncalculator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

const int BTN_0 = 0;
const int BTN_1 = 1;
const int BTN_2 = 2;
const int BTN_3 = 3;
const int BTN_4 = 4;
const int BTN_5 = 5;
const int BTN_6 = 6;
const int BTN_7 = 7;
const int BTN_8 = 8;
const int BTN_9 = 9;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:

    void onBtnClearClicked();
    void onBtnDotClicked();
    void onBtnBackspaceClicked();
protected:
   virtual void keyPressEvent(QKeyEvent *);

private:
    void appendDigit(int);
    void appendOperator(const QString &);
    void appendFunction(const QString &);
    void calculateResult();
    void updateStatusBar(const QString&);
    // ... существующие переменные ...
    void addToHistory(const QString&, double);
    void clearHistory();
    void useHistoryItem();

    Ui::MainWindow *ui;
    QString text_buffer;
    ExpressionCalculator calculator;
    // Добавьте константы для истории
    const int MAX_HISTORY_ITEMS = 20;
};
#endif // MAINWINDOW_H
