#include <QApplication>
#include "mainwindow.h"

#include <iostream>
using namespace std;

double evaluateExpression(const std::string& expression) {
    ExpressionCalculator calculator;
    return calculator.calculate(expression);
}

int main(int argc, char *argv[])
{

//    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();
//    return a.exec();

//     Примеры использования
    std::string expressions[] = {
        "2 + 3 * 4",
        "(2 + 3) * 4",
        "10 / 2 - 3",
        "2^3 + 4 * 5",
        "(-5 + 2) * 3",
        "3.5 * 2 + 1.5",
        "10 / (2 + 3)",
        "2 + 3 * (4 - 1)^2",
        "sin(0)",
        "sin(pi/2)",
        "sin(pi)",
        "sin(3*(pi)/2)",
        "sin(2*pi)",
        "cos(0)",
        "cos(pi/2)",
        "cos(pi)",
        "cos(3*(pi)/2)",
        "cos(2*pi)"
    };

    std::cout << "Arithmetic Expression Evaluator\n";
    std::cout << "===============================\n";

    for (const auto& expr : expressions) {
        try {
            double result = evaluateExpression(expr);
            std::cout << expr << " = " << result << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error evaluating '" << expr << "': " << e.what() << std::endl;
        }
    }
    return 0;
}

