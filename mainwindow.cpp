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
    connect(ui->pbtn_lparen, &QPushButton::clicked, [this]() {
        appendOperator("(");
    });
    connect(ui->pbtn_rparen, &QPushButton::clicked, [this]() {
        appendOperator(")");
    });

    connect(ui->pbtn_clear,&QPushButton::clicked,this,&MainWindow::onBtnClearClicked);
    // Подключаем backspace
    connect(ui->pbtn_backspace, &QPushButton::clicked, this, &MainWindow::onBtnBackspaceClicked);
    // Подключаем равно
    connect(ui->pbtn_equal, &QPushButton::clicked, this, &MainWindow::calculateResult);
    // Подключаем точку
    connect(ui->pbtn_dot, &QPushButton::clicked, this, &MainWindow::onBtnDotClicked);


    QList<QPushButton*> func_buttons_list = {
        ui->pbtn_sin, ui->pbtn_cos, ui->pbtn_tan, ui->pbtn_asin, ui->pbtn_acos,
        ui->pbtn_atan, ui->pbtn_sinh, ui->pbtn_cosh, ui->pbtn_tanh, ui->pbtn_log,ui->pbtn_ln,ui->pbtn_exp,
        ui->pbtn_sqrt,ui->pbtn_mod,ui->pbtn_pi,ui->pbtn_exp
    };

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
    for (int i = 0; i < func_buttons_list.size(); ++i) {
        connect(func_buttons_list[i], &QPushButton::clicked, [this, func = functions[i].first]() {
            appendFunction(func);
        });
    }

    // Настройка таймера для отложенного перерасчета
    updateTimer = new QTimer();
    updateTimer->setSingleShot(true);
    connect(updateTimer, &QTimer::timeout, this, &MainWindow::onUpdateTimerTimeout);

    // Подключаем сигналы истории
    connect(ui->historyList, &QListWidget::itemChanged, this, &MainWindow::onHistoryItemChanged);
    connect(ui->historyList, &QListWidget::itemDoubleClicked,this, &MainWindow::onHistoryItemDoubleClicked);
    connect(ui->historyEdit, &QLineEdit::textChanged,this, &MainWindow::onHistoryEditTextChanged);

    connect(ui->pbtn_clear_history, &QPushButton::clicked, this, &MainWindow::clearHistory);
    connect(ui->pbtn_use_history, &QPushButton::clicked, this, &MainWindow::useHistoryItem);
    connect(ui->historyList, &QListWidget::itemDoubleClicked, this, &MainWindow::useHistoryItem);
    connect(ui->pbtn_recalculate, &QPushButton::clicked,this, &MainWindow::recalculateHistoryItem);

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::onAddShapeClicked);
    connect(ui->cbox_shapetype, &QComboBox::currentIndexChanged, this, &MainWindow::onShapeTypeChanged);

    // Подключаем сигналы для изменения цвета и толщины
    connect(ui->cbox_color, &QComboBox::currentIndexChanged, this, &MainWindow::onColorChanged);
    connect(ui->spin_thickness, &QSpinBox::valueChanged, this, &MainWindow::onThicknessChanged);

    // Подключаем сигналы изменения значений в полях ввода точек
    connect(ui->edit_x1, &QLineEdit::textChanged, this, &MainWindow::onPointValueChanged);
    connect(ui->edit_y1, &QLineEdit::textChanged, this, &MainWindow::onPointValueChanged);
    connect(ui->edit_x2, &QLineEdit::textChanged, this, &MainWindow::onPointValueChanged);
    connect(ui->edit_y2, &QLineEdit::textChanged, this, &MainWindow::onPointValueChanged);
    connect(ui->edit_x3, &QLineEdit::textChanged, this, &MainWindow::onPointValueChanged);
    connect(ui->edit_y3, &QLineEdit::textChanged, this, &MainWindow::onPointValueChanged);
    connect(ui->edit_x4, &QLineEdit::textChanged, this, &MainWindow::onPointValueChanged);
    connect(ui->edit_y4, &QLineEdit::textChanged, this, &MainWindow::onPointValueChanged);
    connect(ui->edit_x5, &QLineEdit::textChanged, this, &MainWindow::onPointValueChanged);
    connect(ui->edit_y5, &QLineEdit::textChanged, this, &MainWindow::onPointValueChanged);

    // Инициализируем начальные значения
    ui->widget->setDrawingEnabled(false); // Отключаем рисование мышью
    updateShapeSettings();
}

MainWindow::~MainWindow(){

    delete updateTimer;
    delete ui;
}

void MainWindow::onBtnClearClicked(){
    text_buffer.clear();
    ui->browser->setText(text_buffer);
}

void MainWindow::onBtnDotClicked(){
    // Проверяем, не заканчивается ли выражение на точку
        if (text_buffer.endsWith('.')) {
            return; // Уже есть точка
        }

        // Проверяем, есть ли уже точка в текущем числе
        bool hasDotInCurrentNumber = false;
        int pos = text_buffer.length() - 1;

        // Идем от конца строки к началу, пока находим цифры или точку
        while (pos >= 0) {
            QChar c = text_buffer[pos];
            if (c.isDigit()) {
                pos--;
                continue;
            } else if (c == '.') {
                hasDotInCurrentNumber = true;
                break;
            } else {
                // Наткнулись на не-цифру и не-точку (оператор, функция и т.д.)
                break;
            }
        }

        // Если в текущем числе уже есть точка, не добавляем новую
        if (hasDotInCurrentNumber) {
            return;
        }

        // Если строка пустая или последний символ - оператор/скобка, добавляем "0."
        if (text_buffer.isEmpty() ||
            text_buffer.endsWith('+') || text_buffer.endsWith('-') ||
            text_buffer.endsWith('*') || text_buffer.endsWith('/') ||
            text_buffer.endsWith('(') || text_buffer.endsWith('^') ||
            text_buffer.endsWith(' ')) {
            text_buffer += "0.";
        } else {
            // Просто добавляем точку
            text_buffer += ".";
        }

        ui->browser->setText(text_buffer);
}

void MainWindow::onBtnBackspaceClicked(){
    if (!text_buffer.isEmpty()) {
        text_buffer.chop(1);
        ui->browser->setText(text_buffer);
        updateStatusBar("Удален последний символ");
    }
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
    case Qt::Key_Period:
    case Qt::Key_Comma:{ onBtnDotClicked(); break; }
    case Qt::Key_Backspace:{ onBtnBackspaceClicked(); break; }
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
        std::string expression = text_buffer.toStdString();
        double result = calculator.calculate(expression);

        // Сохраняем в историю
        addToHistory(text_buffer, result);

        // Показываем результат
        text_buffer = QString::number(result, 'g', 12);
        ui->browser->setText(text_buffer);
        updateStatusBar("Вычислено успешно");
    } catch (const std::exception& e) {
        ui->browser->setText("Ошибка: " + QString(e.what()));
        updateStatusBar("Ошибка вычисления");
    }
}

// Добавьте новые функции управления историей:
void MainWindow::addToHistory(const QString& expression, double result) {
    // Форматируем запись
    HistoryItem item;
    item.expression = expression;
    item.result = result;
    item.originalText = formatHistoryItem(expression, result);

    // Добавляем в начало
    historyData.prepend(item);

    // Ограничиваем размер
    if (historyData.size() > MAX_HISTORY_ITEMS) {
        historyData.removeLast();
    }

    // Обновляем отображение
    updateHistoryDisplay();
}

// Обновление отображения истории
void MainWindow::updateHistoryDisplay() {
    ui->historyList->clear();

    for (const HistoryItem& item : historyData) {
        QListWidgetItem* listItem = new QListWidgetItem(item.originalText);
        listItem->setFlags(listItem->flags() | Qt::ItemIsEditable);
        ui->historyList->addItem(listItem);
    }

    // Скрываем поля редактирования, если история пуста
    bool hasHistory = !historyData.isEmpty();
    ui->historyEdit->setVisible(hasHistory);
    ui->historyResultLabel->setVisible(hasHistory);
    ui->historyResultBrowser->setVisible(hasHistory);
    ui->pbtn_recalculate->setVisible(hasHistory);
}

// Обработчик изменения текста в поле редактирования
void MainWindow::onHistoryEditTextChanged(const QString& text) {
    // Запускаем отложенный перерасчет
    updateTimer->start(UPDATE_DELAY_MS);
}
// Слот изменения типа фигуры
void MainWindow::onShapeTypeChanged(int index){

    // Устанавливаем текущую фигуру
    ui->widget->setCurrentShape(static_cast<ShapeType>(index));

    // Обновляем настройки UI
    updateShapeSettings();
}
// Обновление настроек ввода точек
void MainWindow::updateShapeSettings() {

    ShapeType currentType = ui->widget->getCurrentShape().getType();
    int requiredPoints = ShapesWidget::getRequiredPointsCount(currentType);

    // Включаем/отключаем поля ввода в зависимости от типа фигуры
    ui->label_vertex1->setEnabled(requiredPoints >= 1);
    ui->edit_x1->setEnabled(requiredPoints >= 1);
    ui->edit_y1->setEnabled(requiredPoints >= 1);

    ui->label_vertex2->setEnabled(requiredPoints >= 2);
    ui->edit_x2->setEnabled(requiredPoints >= 2);
    ui->edit_y2->setEnabled(requiredPoints >= 2);

    ui->label_vertex3->setEnabled(requiredPoints >= 3);
    ui->edit_x3->setEnabled(requiredPoints >= 3);
    ui->edit_y3->setEnabled(requiredPoints >= 3);

    ui->label_vertex4->setEnabled(requiredPoints >= 4);
    ui->edit_x4->setEnabled(requiredPoints >= 4);
    ui->edit_y4->setEnabled(requiredPoints >= 4);

    ui->label_vertex5->setEnabled(requiredPoints >= 5);
    ui->edit_x5->setEnabled(requiredPoints >= 5);
    ui->edit_y5->setEnabled(requiredPoints >= 5);

    // Задаем placeholder'ы
    ui->edit_x1->setPlaceholderText("X1");
    ui->edit_y1->setPlaceholderText("Y1");
    ui->edit_x2->setPlaceholderText("X2");
    ui->edit_y2->setPlaceholderText("Y2");
    ui->edit_x3->setPlaceholderText("X3");
    ui->edit_y3->setPlaceholderText("Y3");
    ui->edit_x4->setPlaceholderText("X4");
    ui->edit_y4->setPlaceholderText("Y4");
    ui->edit_x5->setPlaceholderText("X5");
    ui->edit_y5->setPlaceholderText("Y5");

}
// Очистка полей ввода точек
void MainWindow::clearPointInputs() {
    ui->edit_x1->clear();
    ui->edit_y1->clear();
    ui->edit_x2->clear();
    ui->edit_y2->clear();
    ui->edit_x3->clear();
    ui->edit_y3->clear();
    ui->edit_x4->clear();
    ui->edit_y4->clear();
    ui->edit_x5->clear();
    ui->edit_y5->clear();
}

// Обновленный метод получения точек
QVector<QPointF> MainWindow::getPointsFromInputs() const {
    QVector<QPointF> points;

    ShapeType currentType = ui->widget->getCurrentShape().getType();
    int requiredPoints = ShapesWidget::getRequiredPointsCount(currentType);

    // Добавляем точки в зависимости от требуемого количества
    if (requiredPoints >= 1 && !ui->edit_x1->text().isEmpty() && !ui->edit_y1->text().isEmpty()) {
        double x1 = ui->edit_x1->text().toDouble();
        double y1 = ui->edit_y1->text().toDouble();
        points.append(QPointF(x1, y1));
    }

    if (requiredPoints >= 2 && !ui->edit_x2->text().isEmpty() && !ui->edit_y2->text().isEmpty()) {
        double x2 = ui->edit_x2->text().toDouble();
        double y2 = ui->edit_y2->text().toDouble();
        points.append(QPointF(x2, y2));
    }

    if (requiredPoints >= 3 && !ui->edit_x3->text().isEmpty() && !ui->edit_y3->text().isEmpty()) {
        double x3 = ui->edit_x3->text().toDouble();
        double y3 = ui->edit_y3->text().toDouble();
        points.append(QPointF(x3, y3));
    }

    if (requiredPoints >= 4 && !ui->edit_x4->text().isEmpty() && !ui->edit_y4->text().isEmpty()) {
        double x4 = ui->edit_x4->text().toDouble();
        double y4 = ui->edit_y4->text().toDouble();
        points.append(QPointF(x4, y4));
    }

    if (requiredPoints >= 5 && !ui->edit_x5->text().isEmpty() && !ui->edit_y5->text().isEmpty()) {
        double x5 = ui->edit_x5->text().toDouble();
        double y5 = ui->edit_y5->text().toDouble();
        points.append(QPointF(x5, y5));
    }

    return points;
}

// Обновленный слот изменения цвета
void MainWindow::onColorChanged() {
    int index = ui->cbox_color->currentIndex();
    QColor color;

    switch(index) {
        case 0: color = Qt::red; break;
        case 1: color = Qt::green; break;
        case 2: color = Qt::blue; break;
        case 3: color = Qt::yellow; break;
        case 4: color = Qt::black; break;
        default: color = Qt::black;
    }

    ui->widget->setCurrentColor(color);
}
// Слот изменения значений точек
void MainWindow::onPointValueChanged() {
    // Можно добавить предпросмотр фигуры
}
// Слот переключения режима рисования
void MainWindow::onDrawingModeToggled(bool checked) {
    ShapesWidget *shapesWidget = ui->widget;
    shapesWidget->setDrawingEnabled(checked);
    ui->statusbar->showMessage(
        checked ? "Режим рисования мышью включен" : "Режим рисования мышью выключен",
        2000);
}
// Обновленный слот добавления фигуры
void MainWindow::onAddShapeClicked() {
    ShapeType currentType = ui->widget->getCurrentShape().getType();
    int requiredPoints = ShapesWidget::getRequiredPointsCount(currentType);
    QVector<QPointF> points = getPointsFromInputs();

    // Проверяем, что введено достаточное количество точек
    if (points.size() < requiredPoints) {
        QMessageBox::warning(this, "Ошибка",
            QString("Для фигуры '%1' требуется %2 точек. Введено только %3.")
            .arg(Shape::shapeTypeToString(currentType))
            .arg(requiredPoints)
            .arg(points.size()));
        return;
    }

    // Получаем цвет из ComboBox
    int colorIndex = ui->cbox_color->currentIndex();
    QColor color;
    switch(colorIndex) {
        case 0: color = Qt::red; break;
        case 1: color = Qt::green; break;
        case 2: color = Qt::blue; break;
        case 3: color = Qt::yellow; break;
        case 4: color = Qt::black; break;
        default: color = Qt::black;
    }

    // Получаем толщину
    int thickness = ui->spin_thickness->value();

    // Добавляем фигуру
    ui->widget->addShapeWithParameters(currentType, points, color, thickness);

    ui->statusbar->showMessage(
        QString("Фигура '%1' добавлена")
        .arg(Shape::shapeTypeToString(currentType)),
        2000);
}

// Слот изменения толщины
void MainWindow::onThicknessChanged(int value) {

    ui->widget->setCurrentThickness(value);
}

// Таймер для отложенного перерасчета
void MainWindow::onUpdateTimerTimeout() {
    calculateEditedExpression();
}

// Перерасчет отредактированного выражения
void MainWindow::calculateEditedExpression() {
    QString expression = ui->historyEdit->text().trimmed();

    if (expression.isEmpty()) {
        ui->historyResultBrowser->setText("");
        return;
    }

    try {
        double result = calculator.calculate(expression.toStdString());
        ui->historyResultBrowser->setText(QString::number(result, 'g', 12));
        ui->historyResultBrowser->setStyleSheet(
            "QTextBrowser { color: #00ff00; }"
        );
    } catch (const std::exception& e) {
        ui->historyResultBrowser->setText("Ошибка: " + QString(e.what()));
        ui->historyResultBrowser->setStyleSheet(
            "QTextBrowser { color: #ff5555; }"
        );
    }
}

// Форматирование элемента истории
QString MainWindow::formatHistoryItem(const QString& expression, double result) const {
    return QString("%1 = %2").arg(expression).arg(result, 0, 'g', 12);
}

// Очистка истории
void MainWindow::clearHistory() {
    historyData.clear();
    ui->historyList->clear();
    ui->historyEdit->clear();
    ui->historyResultBrowser->clear();
    updateStatusBar("История очищена");
}

void MainWindow::useHistoryItem() {
    QListWidgetItem* item = ui->historyList->currentItem();
    if (item) {
        int row = ui->historyList->row(item);
        if (row >= 0 && row < historyData.size()) {
            text_buffer = historyData[row].expression;
            ui->browser->setText(text_buffer);
            updateStatusBar("Выражение загружено из истории");
        }
    }
}

// Пересчет выбранного элемента
void MainWindow::recalculateHistoryItem() {
    QListWidgetItem* item = ui->historyList->currentItem();
    if (item) {
        int row = ui->historyList->row(item);
        if (row >= 0 && row < historyData.size()) {
            try {
                double result = calculator.calculate(historyData[row].expression.toStdString());
                historyData[row].result = result;
                historyData[row].originalText = formatHistoryItem(historyData[row].expression, result);

                // Обновляем отображение
                item->setText(historyData[row].originalText);
                ui->historyResultBrowser->setText(QString::number(result, 'g', 12));
                ui->historyResultBrowser->setStyleSheet(
                    "QTextBrowser { color: #00ff00; }"
                );

                updateStatusBar("Выражение пересчитано");
            } catch (const std::exception& e) {
                ui->historyResultBrowser->setText("Ошибка: " + QString(e.what()));
                ui->historyResultBrowser->setStyleSheet(
                    "QTextBrowser { color: #ff5555; }"
                );
            }
        }
    }
}

// Двойной клик по элементу истории
void MainWindow::onHistoryItemDoubleClicked(QListWidgetItem* item) {
    if (!item) return;

    int row = ui->historyList->row(item);
    if (row >= 0 && row < historyData.size()) {
        // Показываем выражение для редактирования
        ui->historyEdit->setText(historyData[row].expression);
        ui->historyResultBrowser->setText(QString::number(historyData[row].result, 'g', 12));
        ui->historyResultBrowser->setStyleSheet(
            "QTextBrowser { color: #00ff00; }"
        );
    }
}

// Изменение элемента в списке (прямое редактирование в списке)
void MainWindow::onHistoryItemChanged(QListWidgetItem* item) {
    if (!item) return;

    int row = ui->historyList->row(item);
    if (row >= 0 && row < historyData.size()) {
        QString newText = item->text().trimmed();

        // Пытаемся разобрать "выражение = результат"
        int eqPos = newText.indexOf("=");
        if (eqPos != -1) {
            QString expr = newText.left(eqPos).trimmed();
            QString resultStr = newText.mid(eqPos + 1).trimmed();

            bool ok;
            double result = resultStr.toDouble(&ok);

            if (ok) {
                // Сохраняем изменения
                historyData[row].expression = expr;
                historyData[row].result = result;
                historyData[row].originalText = newText;

                // Обновляем поле редактирования
                ui->historyEdit->setText(expr);
                ui->historyResultBrowser->setText(resultStr);
            }
        }
    }
}

void MainWindow::updateStatusBar(const QString& message){
    ui->statusbar->showMessage(message, 3000); // Показываем 3 секунды
}


















