#include <iostream>
#include <cmath>
#include <stdexcept>
#include <vector>
#include <QPointF>

class Triangle {
private:
    double side_a;
    double side_b;
    double side_c;

    bool isValidTriangle(double a, double b, double c) const;

public:
    Triangle();

    // Конструктор для создания треугольника по длине трёх сторок
    Triangle(double a, double b, double c);

    // Конструктор для создания равностороннего треугольника
    Triangle(double side);

    // Методы для получения длины нужной стороны
    double getSideA() const;
    double getSideB() const;
    double getSideC() const;

    // Методы для устновки длины нужной стороны
    void setSideA(double a);
    void setSideB(double b);
    void setSideC(double c);

    void setSides(double a, double b, double c);

    // Метод для вычисления периметра треугольника
    double perimeter() const;

    // Метод для вычисления площади треугольника через полупериметр
    double area() const;

    std::string type() const;

    bool isCongruent(const Triangle& other) const;

    void printInfo() const;

    // Метод для вычисления координат вершин треугольника
    // Возвращает три точки, образующие треугольник
    std::vector<QPointF> getVertices(double centerX = 0, double centerY = 0, double scale = 50) const;

    // Статические методы
    static Triangle createEquilateral(double side); // ?

    static Triangle createRightIsosceles(double leg); // ?
};
