#include "expressioncalculator.h"

ExpressionCalculator::ExpressionCalculator()
{
    functions["sin"] = [](double x) { return std::sin(x); };
    functions["cos"] = [](double x) { return std::cos(x); };
    functions["tan"] = [](double x) { return std::tan(x); };
    functions["tg"] = [](double x) { return std::tan(x); }; // альтернативное обозначение

    functions["asin"] = [](double x) { return std::asin(x); };
    functions["arcsin"] = [](double x) { return std::asin(x); };
    functions["acos"] = [](double x) { return std::acos(x); };
    functions["arccos"] = [](double x) { return std::acos(x); };
    functions["atan"] = [](double x) { return std::atan(x); };
    functions["arctg"] = [](double x) { return std::atan(x); };

    functions["sinh"] = [](double x) { return std::sinh(x); };
    functions["cosh"] = [](double x) { return std::cosh(x); };
    functions["tanh"] = [](double x) { return std::tanh(x); };

    functions["log"] = [](double x) { return std::log10(x); };
    functions["ln"] = [](double x) { return std::log(x); };
    functions["exp"] = [](double x) { return std::exp(x); };
    functions["sqrt"] = [](double x) { return std::sqrt(x); };
    functions["abs"] = [](double x) { return std::abs(x); };
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

double ExpressionCalculator::applyFunction(const std::string& func, double arg) const {

    auto it = functions.find(func);
    if (it != functions.end()) {
        try {
            return it->second(arg);
        } catch (...) {
            throw std::runtime_error("Error evaluating function " + func);
        }
    }
    throw std::runtime_error("Unknown function: " + func);
}

bool ExpressionCalculator::isFunction(const std::string &str) const {
    return functions.find(str) != functions.end();
}

bool ExpressionCalculator::isNumber(const std::string &str) const {

    std::istringstream iss(str);
    double d;
    char c;
    return iss >> d && !(iss >> c);
}

bool ExpressionCalculator::isLetter(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

std::vector<std::string> ExpressionCalculator::toRPN(const std::string &expression) {

    std::vector<std::string> output;
    std::stack<std::string> operators;
    std::string buffer;

    for (size_t i = 0; i < expression.length(); i++) {
        char c = expression[i];

        // Если символ - цифра или точка, собираем число
        if (std::isdigit(c) || c == '.') {
            buffer += c;
            // Если следующий символ не цифра и не точка, добавляем число в вывод
            if (i + 1 == expression.length() ||
                    (!std::isdigit(expression[i + 1]) && expression[i + 1] != '.')) {
                output.push_back(buffer);
                buffer.clear();
            }
        }
        // Если символ - буква, собираем имя функции
        else if (isLetter(c)) {
            buffer += c;
            // Если следующий символ не буква, проверяем, что мы собрали
            if (i + 1 == expression.length() || !isLetter(expression[i + 1])) {
                // Если после имени функции идет открывающая скобка, это функция
                if (i + 1 < expression.length() && expression[i + 1] == '(') {
                    operators.push(buffer);
                } else {
                    // Иначе это константа (например, pi, e)
                    if (buffer == "pi") {
                        output.push_back("3.14159265358979323846");
                    } else if (buffer == "e") {
                        output.push_back("2.71828182845904523536");
                    } else {
                        throw std::runtime_error("Unknown identifier: " + buffer);
                    }
                }
                buffer.clear();
            }
        }
        // Если символ - открывающая скобка
        else if (c == '(') {
            operators.push(std::string(1, c));
        }
        // Если символ - закрывающая скобка
        else if (c == ')') {
            while (!operators.empty() && operators.top() != "(") {
                output.push_back(operators.top());
                operators.pop();
            }
            if (!operators.empty()) {
                operators.pop(); // Удаляем открывающую скобку

                // Если после удаления скобки на вершине стека функция, добавляем ее
                if (!operators.empty() && isFunction(operators.top())) {
                    output.push_back(operators.top());
                    operators.pop();
                }
            }
        }
        // Если символ - оператор
        else if (isOperator(c)) {
            // Обрабатываем унарный минус
            if (c == '-' && (i == 0 || expression[i - 1] == '(' ||
                             isOperator(expression[i - 1]) || expression[i - 1] == ',')) {
                output.push_back("0");
            }

            while (!operators.empty() && operators.top() != "(" &&
                   (isOperator(operators.top()[0]) &&
                    getPrecedence(operators.top()[0]) >= getPrecedence(c))) {
                output.push_back(operators.top());
                operators.pop();
            }
            operators.push(std::string(1, c));
        }
        // Если символ - запятая (разделитель аргументов функции)
        else if (c == ',') {
            while (!operators.empty() && operators.top() != "(") {
                output.push_back(operators.top());
                operators.pop();
            }
        }
    }

    // Добавляем оставшиеся операторы
    while (!operators.empty()) {
        output.push_back(operators.top());
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
        } else if (isFunction(token)) {
            if (values.empty()) {
                throw std::runtime_error("Invalid expression for function " + token);
            }
            double arg = values.top(); values.pop();
            values.push(applyFunction(token, arg));
        } else {
            throw std::runtime_error("Invalid token: " + token);
        }
    }

    if (values.size() != 1) {
        throw std::runtime_error("Invalid expression");
    }

    return values.top();
}
