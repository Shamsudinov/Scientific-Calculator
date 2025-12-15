#ifndef EXPRESSIONCALCULATOR_H
#define EXPRESSIONCALCULATOR_H

#include <string>
#include <stack>
#include <cctype>
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <map>
#include <functional>

class ExpressionCalculator
{
    std::map<std::string, std::function<double(double)>> functions;
private:
    // Удаление пробелов из строки
    std::string removeSpaces(const std::string&) const;
    // Проверка, является ли символ оператором
    bool isOperator(char ) const;
    // Получаем приоритет оператора
    int getPrecedence(char );
    // Применяем оператор к двум операндам
    double applyOperation(double , double , char ) const;
    double applyFunction(const std::string&, double) const;
    bool isFunction(const std::string &str) const;
    // Проверяем, является ли строка числом
    bool isNumber(const std::string&) const;
    bool isLetter(char) const;
    // Конвертируем выражение в обратную польскую нотацию (ОПН)
    std::vector<std::string> toRPN(const std::string&);
    // Вычисляем выражение в ОПН
    double evaluateRPN(const std::vector<std::string>&) const;
public:
    ExpressionCalculator();

    // Основная функция для вычисления выражения
    double calculate(const std::string&);
};

#endif // EXPRESSIONCALCULATOR_H
