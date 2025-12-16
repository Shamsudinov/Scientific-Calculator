#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QDebug>
#include <QPushButton>
#include <QGroupBox>
#include <QGridLayout>
#include <QTimer>
#include <QLineEdit>
#include <QLabel>
#include <QListWidgetItem>
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
    void onHistoryItemChanged(QListWidgetItem*);
    void onHistoryItemDoubleClicked(QListWidgetItem*);
    void onUpdateTimerTimeout();
    void onHistoryEditTextChanged(const QString&);
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
    void startDelayedCalculation();
     void onHistoryTextChanged(const QString&);
     QString formatHistoryItem(const QString&, double) const;
     void calculateEditedExpression();
     void editHistoryItem(int);
     void updateHistoryDisplay();
     void recalculateHistoryItem();

    Ui::MainWindow *ui;

    struct HistoryItem {
        QString expression;
        double result;
        QString originalText;
    };

    QVector<HistoryItem> historyData;
    QTimer* updateTimer; // Таймер для отложенного перерасчета
    QString text_buffer;
    ExpressionCalculator calculator;
    // Добавьте константы для истории
    const int MAX_HISTORY_ITEMS = 20;
    const int UPDATE_DELAY_MS = 500; // Задержка перед перерасчетом в мс
};
#endif // MAINWINDOW_H
