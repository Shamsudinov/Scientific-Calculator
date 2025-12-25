#include "triangle.h"

Triangle::Triangle() : side_a(3), side_b(4), side_c(5) {

}

Triangle::Triangle(double a, double b, double c) {

    if (!isValidTriangle(a, b, c)) {
        throw std::invalid_argument("Invalid triangle sides");
    }
    side_a = a;
    side_b = b;
    side_c = c;
}

Triangle::Triangle(double side) : Triangle(side, side, side) {

}

bool Triangle::isValidTriangle(double a, double b, double c) const {
    return (a > 0 && b > 0 && c > 0) &&
            (a + b > c) &&
            (a + c > b) &&
            (b + c > a);
}

double Triangle::getSideA() const {
    return side_a;
}

double Triangle::getSideB() const {
    return side_b;
}

double Triangle::getSideC() const {
    return side_c;
}

void Triangle::setSideA(double a) {

    if (!isValidTriangle(a, side_b, side_c)) {
        throw std::invalid_argument("Invalid triangle side");
    }
    side_a = a;
}

void Triangle::setSideB(double b) {

    if (!isValidTriangle(side_a, b, side_c)) {
        throw std::invalid_argument("Invalid triangle side");
    }
    side_b = b;
}

void Triangle::setSideC(double c) {

    if (!isValidTriangle(side_a, side_b, c)) {
        throw std::invalid_argument("Invalid triangle side");
    }
    side_c = c;
}

void Triangle::setSides(double a, double b, double c) {

    if (!isValidTriangle(a, b, c)) {
        throw std::invalid_argument("Invalid triangle sides");
    }
    side_a = a;
    side_b = b;
    side_c = c;
}

double Triangle::perimeter() const {
    return side_a + side_b + side_c;
}

double Triangle::area() const {

    double p = perimeter() / 2;
    return std::sqrt(p * (p - side_a) * (p - side_b) * (p - side_c));
}

std::string Triangle::type() const {

    if (side_a == side_b && side_b == side_c) {
        return "Equilateral";
    }
    if (side_a == side_b || side_a == side_c || side_b == side_c) {
        return "Isosceles";
    }

    double a2 = side_a * side_a;
    double b2 = side_b * side_b;
    double c2 = side_c * side_c;

    if (std::abs(a2 + b2 - c2) < 1e-9 ||
            std::abs(a2 + c2 - b2) < 1e-9 ||
            std::abs(b2 + c2 - a2) < 1e-9) {
        return "Right";
    }

    return "Scalene";
}

bool Triangle::isCongruent(const Triangle &other) const {

    double sides1[3] = {side_a, side_b, side_c};
    double sides2[3] = {other.side_a, other.side_b, other.side_c};

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2 - i; j++) {
            if (sides1[j] > sides1[j + 1]) std::swap(sides1[j], sides1[j + 1]);
            if (sides2[j] > sides2[j + 1]) std::swap(sides2[j], sides2[j + 1]);
        }
    }

    return (std::abs(sides1[0] - sides2[0]) < 1e-9 &&
            std::abs(sides1[1] - sides2[1]) < 1e-9 &&
            std::abs(sides1[2] - sides2[2]) < 1e-9);
}

void Triangle::printInfo() const {

    std::cout << "Triangle sides: " << side_a << ", " << side_b << ", " << side_c << std::endl;
    std::cout << "Type: " << type() << std::endl;
    std::cout << "Perimeter: " << perimeter() << std::endl;
    std::cout << "Area: " << area() << std::endl;
}

std::vector<QPointF> Triangle::getVertices(double centerX, double centerY, double scale) const {

    std::vector<QPointF> vertices;

    // Первая вершина в центре
    QPointF A(centerX, centerY);

    // Вычисляем углы треугольника по теореме косинусов
    double angleA = std::acos((side_b * side_b + side_c * side_c - side_a * side_a) / (2 * side_b * side_c));
    double angleB = std::acos((side_a * side_a + side_c * side_c - side_b * side_b) / (2 * side_a * side_c));

    // Вторая вершина - вдоль оси X
    QPointF B(centerX + side_c * scale, centerY);

    // Третья вершина - вычисляем по углам
    double x = centerX + side_b * scale * std::cos(angleA);
    double y = centerY + side_b * scale * std::sin(angleA);
    QPointF C(x, y);

    vertices.push_back(A);
    vertices.push_back(B);
    vertices.push_back(C);

    return vertices;
}

Triangle Triangle::createEquilateral(double side) {
    return Triangle(side);
}

Triangle Triangle::createRightIsosceles(double leg) {
    double hypotenuse = leg * std::sqrt(2);
    return Triangle(leg, leg, hypotenuse);
}
