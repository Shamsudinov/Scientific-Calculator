#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QFont f("Arial", 14, QFont::Bold);
    ui->browser->setFont(f);

    QList<QPushButton*> digit_buttons_list = {
        ui->pbtn_0, ui->pbtn_1, ui->pbtn_2, ui->pbtn_3, ui->pbtn_4,
        ui->pbtn_5, ui->pbtn_6, ui->pbtn_7, ui->pbtn_8, ui->pbtn_9
    };

    for (int i = 0; i < digit_buttons_list.size(); ++i) {
        connect(digit_buttons_list[i], &QPushButton::clicked, [this, i]() {
            appendDigit(i);
        });
    }

    connect(ui->pbtn_sum,&QPushButton::clicked,this,&MainWindow::pbtn_sum_clicked);
    connect(ui->pbtn_diff,&QPushButton::clicked,this,&MainWindow::pbtn_diff_clicked);
    connect(ui->pbtn_div,&QPushButton::clicked,this,&MainWindow::pbtn_div_clicked);
    connect(ui->pbtn_mult,&QPushButton::clicked,this,&MainWindow::pbtn_mult_clicked);

    connect(ui->pbtn_clear,&QPushButton::clicked,this,&MainWindow::onBtnClearClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::pbtn_sum_clicked(){
    text_buffer += "+";
    ui->browser->setText(text_buffer);
}

void MainWindow::pbtn_diff_clicked(){
    text_buffer += "-";
    ui->browser->setText(text_buffer);
}

void MainWindow::pbtn_div_clicked(){
    text_buffer += "/";
    ui->browser->setText(text_buffer);
}

void MainWindow::pbtn_mult_clicked(){
    text_buffer += "*";
    ui->browser->setText(text_buffer);
}

void MainWindow::onBtnClearClicked(){
    text_buffer.clear();
    ui->browser->setText(text_buffer);
}

void MainWindow::keyPressEvent(QKeyEvent *e){

    switch (e->key()) {

    case Qt::Key_0:{ appendDigit(BTN_0); break; }
    case Qt::Key_1:{ appendDigit(BTN_1); break; }
    case Qt::Key_2:{ appendDigit(BTN_2); break; }
    case Qt::Key_3:{ appendDigit(BTN_3); break; }
    case Qt::Key_4:{ appendDigit(BTN_4); break; }
    case Qt::Key_5:{ appendDigit(BTN_5); break; }
    case Qt::Key_6:{ appendDigit(BTN_6); break; }
    case Qt::Key_7:{ appendDigit(BTN_7); break; }
    case Qt::Key_8:{ appendDigit(BTN_8); break; }
    case Qt::Key_9:{ appendDigit(BTN_9); break; }
    case Qt::Key_Plus:{
        text_buffer += "+";
        break;
    }
    case Qt::Key_Minus:{
        text_buffer += "-";
        break;
    }
    case Qt::Key_Slash:{
        text_buffer += "/";
        break;
    }
    case Qt::Key_Asterisk:{
        text_buffer += "*";
        break;
    }

    }

    ui->browser->setText(text_buffer);
}

void MainWindow::appendDigit(int digit){

    text_buffer += QString::number(digit);
    ui->browser->setText(text_buffer);
}






















