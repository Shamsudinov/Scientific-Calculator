#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QDebug>
#include <QPushButton>

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

    void pbtn_sum_clicked();
    void pbtn_diff_clicked();
    void pbtn_div_clicked();
    void pbtn_mult_clicked();
    void onBtnClearClicked();

protected:
   virtual void keyPressEvent(QKeyEvent *event);

private:
    void appendDigit(int digit);

    Ui::MainWindow *ui;
    QString text_buffer;
};
#endif // MAINWINDOW_H
