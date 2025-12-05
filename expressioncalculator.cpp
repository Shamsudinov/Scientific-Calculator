#include "expressioncalculator.h"

ExpressionCalculator::ExpressionCalculator()
{

}

double ExpressionCalculator::calculate(const std::string &expression) {

    std::string cleaned = removeSpaces(expression);

    // Проверяем баланс скобок
    int balance = 0;
    for (char c : cleaned) {
        if (c == '(') balance++;
        else if (c == ')') balance--;
        if (balance < 0) {
            throw std::runtime_error("Unbalanced parentheses");
        }
    }
    if (balance != 0) {
        throw std::runtime_error("Unbalanced parentheses");
    }

    // Конвертируем в ОПН и вычисляем
    std::vector<std::string> rpn = toRPN(cleaned);
    return evaluateRPN(rpn);
}

std::string ExpressionCalculator::removeSpaces(const std::string &str) const{

    std::string result;
    for (char c : str) {
        if (!std::isspace(c)) {
            result += c;
        }
    }
    return result;
}

bool ExpressionCalculator::isOperator(char c) const {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

int ExpressionCalculator::getPrecedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;
    return 0;
}

double ExpressionCalculator::applyOperation(double a, double b, char op) const {

    switch (op) {
    case '+': return a + b;
    case '-': return a - b;
    case '*': return a * b;
    case '/':
        if (b == 0) {
            throw std::runtime_error("Division by zero");
        }
        return a / b;
    case '^': return std::pow(a, b);
    default: throw std::runtime_error("Invalid operator");
    }
}

bool ExpressionCalculator::isNumber(const std::string &str) const {

    std::istringstream iss(str);
    double d;
    char c;
    return iss >> d && !(iss >> c);
}

std::vector<std::string> ExpressionCalculator::toRPN(const std::string &expression) {

    std::vector<std::string> output;
    std::stack<char> operators;
    std::string number;

    for (size_t i = 0; i < expression.length(); i++) {
        char c = expression[i];

        // Если символ - цифра или точка, собираем число
        if (std::isdigit(c) || c == '.') {
            number += c;
            // Если следующий символ не цифра и не точка, добавляем число в вывод
            if (i + 1 == expression.length() ||
                    (!std::isdigit(expression[i + 1]) && expression[i + 1] != '.')) {
                output.push_back(number);
                number.clear();
            }
        }
        // Если символ - открывающая скобка
        else if (c == '(') {
            operators.push(c);
        }
        // Если символ - закрывающая скобка
        else if (c == ')') {
            while (!operators.empty() && operators.top() != '(') {
                output.push_back(std::string(1, operators.top()));
                operators.pop();
            }
            if (!operators.empty()) {
                operators.pop(); // Удаляем открывающую скобку
            }
        }
        // Если символ - оператор
        else if (isOperator(c)) {
            // Обрабатываем унарный минус
            if (c == '-' && (i == 0 || expression[i - 1] == '(' || isOperator(expression[i - 1]))) {
                // Для унарного минуса добавляем 0 перед ним
                if (i == 0 || expression[i - 1] == '(') {
                    output.push_back("0");
                }
            }

            while (!operators.empty() && getPrecedence(operators.top()) >= getPrecedence(c)) {
                output.push_back(std::string(1, operators.top()));
                operators.pop();
            }
            operators.push(c);
        }
    }

    // Добавляем оставшиеся операторы
    while (!operators.empty()) {
        output.push_back(std::string(1, operators.top()));
        operators.pop();
    }

    return output;
}

double ExpressionCalculator::evaluateRPN(const std::vector<std::string> &rpn) const {

    std::stack<double> values;

    for (const std::string& token : rpn) {
        if (isNumber(token)) {
            values.push(std::stod(token));
        } else if (token.length() == 1 && isOperator(token[0])) {
            if (values.size() < 2) {
                throw std::runtime_error("Invalid expression");
            }
            double b = values.top(); values.pop();
            double a = values.top(); values.pop();
            values.push(applyOperation(a, b, token[0]));
        } else {
            throw std::runtime_error("Invalid token: " + token);
        }
    }

    if (values.size() != 1) {
        throw std::runtime_error("Invalid expression");
    }

    return values.top();
}
