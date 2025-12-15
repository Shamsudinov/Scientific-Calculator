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

    // Подключаем операторы
    connect(ui->pbtn_sum, &QPushButton::clicked, [this]() {
        appendOperator("+");
    });
    connect(ui->pbtn_diff, &QPushButton::clicked, [this]() {
        appendOperator("-");
    });
    connect(ui->pbtn_mult, &QPushButton::clicked, [this]() {
        appendOperator("*");
    });
    connect(ui->pbtn_div, &QPushButton::clicked, [this]() {
        appendOperator("/");
    });

    connect(ui->pbtn_clear,&QPushButton::clicked,this,&MainWindow::onBtnClearClicked);

    // Создаем группу для тригонометрических функций
    QGroupBox *functionsGroup = new QGroupBox("Функции", this);
    functionsGroup->setGeometry(270, 10, 300, 331);

    QGridLayout *functionsLayout = new QGridLayout(functionsGroup);

    // Список функций с их отображаемыми именами
    QVector<QPair<QString, QString>> functions = {
        {"sin(", "sin"},
        {"cos(", "cos"},
        {"tan(", "tan"},
        {"asin(", "asin"},
        {"acos(", "acos"},
        {"atan(", "atan"},
        {"sinh(", "sinh"},
        {"cosh(", "cosh"},
        {"tanh(", "tanh"},
        {"log(", "log"},
        {"ln(", "ln"},
        {"exp(", "exp"},
        {"sqrt(", "√"},
        {"abs(", "|x|"},
        {"pi", "π"},
        {"e", "e"}
    };

    // Создаем кнопки для функций
    for (int i = 0; i < functions.size(); ++i) {
        QPushButton *btn = new QPushButton(functions[i].second, functionsGroup);
        btn->setFixedSize(60, 40);
        connect(btn, &QPushButton::clicked, [this, func = functions[i].first]() {
            appendFunction(func);
        });
        functionsLayout->addWidget(btn, i / 3, i % 3);
    }

    functionsGroup->setLayout(functionsLayout);
}

MainWindow::~MainWindow()
{
    delete ui;
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
    case Qt::Key_Plus:{ appendOperator("+"); break; }
    case Qt::Key_Minus:{ appendOperator("-"); break; }
    case Qt::Key_Slash:{ appendOperator("/"); break; }
    case Qt::Key_Asterisk:{ appendOperator("*"); break; }
    case Qt::Key_Enter:{ calculateResult(); break; }
    case Qt::Key_Equal:{ calculateResult(); break; }
    case Qt::Key_ParenLeft:{ appendOperator("("); break; }
    case Qt::Key_ParenRight:{ appendOperator(")"); break; }
    case Qt::Key_P:{
         if (e->modifiers() & Qt::ControlModifier) {
             appendFunction("pi");
         }
         break;
     }


    }

    ui->browser->setText(text_buffer);
}

void MainWindow::appendDigit(int digit){

    text_buffer += QString::number(digit);
    ui->browser->setText(text_buffer);
}

void MainWindow::appendOperator(const QString &op){
    text_buffer += op;
    ui->browser->setText(text_buffer);
}

void MainWindow::appendFunction(const QString &func){

    if (func == "pi" || func == "e") {
        text_buffer += func;
    } else {
        text_buffer += func;
    }
    ui->browser->setText(text_buffer);
}

void MainWindow::calculateResult(){

    try {
        double result = calculator.calculate(text_buffer.toStdString());
        text_buffer = QString::number(result);
        ui->browser->setText(text_buffer);
    } catch (const std::exception& e) {
        ui->browser->setText("Ошибка: " + QString(e.what()));
    }
}




















