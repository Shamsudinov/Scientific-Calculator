#include "shape.h"
#include <algorithm>

Shape::Shape(ShapeType type, const QColor &color, int thickness)
    : type(type), color(color), thickness(thickness) {
}

void Shape::addPoint(const QPointF &point) {
    points.append(point);
}

void Shape::setLastPoint(const QPointF &point) {
    if (!points.isEmpty()) {
        points.last() = point;
    }
}

void Shape::clearPoints() {
    points.clear();
}

bool Shape::isValid() const {
    switch (type) {
    case LineShape:
    case RectangleShape:
    case EllipseShape:
    case TriangleShape:
        return points.size() >= 2;
    case PolygonShape:
    case FreehandShape:
        return points.size() >= 1;
    default:
        return false;
    }
}

QRectF Shape::boundingRect() const {
    if (points.isEmpty()) {
        return QRectF();
    }

    qreal minX = points.first().x();
    qreal maxX = points.first().x();
    qreal minY = points.first().y();
    qreal maxY = points.first().y();

    for (const QPointF &point : points) {
        minX = qMin(minX, point.x());
        maxX = qMax(maxX, point.x());
        minY = qMin(minY, point.y());
        maxY = qMax(maxY, point.y());
    }

    return QRectF(minX, minY, maxX - minX, maxY - minY);
}
