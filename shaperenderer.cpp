#include "shaperenderer.h"
#include <QPolygonF>

void ShapeRenderer::render(QPainter &painter, const Shape &shape) {
    if (!shape.isValid()) {
        return;
    }

    painter.setPen(QPen(shape.getColor(), shape.getThickness(),
                       Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);

    switch (shape.getType()) {
    case LineShape:
        renderLine(painter, shape);
        break;
    case RectangleShape:
        renderRectangle(painter, shape);
        break;
    case EllipseShape:
        renderEllipse(painter, shape);
        break;
    case TriangleShape:
        renderTriangle(painter, shape);
        break;
    case PolygonShape:
        renderPolygon(painter, shape);
        break;
    case PolylineShape:
        renderFreehand(painter, shape);
        break;
    default:
        break;
    }
}

void ShapeRenderer::renderLine(QPainter &painter, const Shape &shape) {
    if (shape.pointCount() >= 2) {
        painter.drawLine(shape.firstPoint(), shape.lastPoint());
    }
}

void ShapeRenderer::renderRectangle(QPainter &painter, const Shape &shape) {
    if (shape.pointCount() >= 2) {
        QRectF rect(shape.firstPoint(), shape.lastPoint());
        painter.drawRect(rect);
    }
}

void ShapeRenderer::renderEllipse(QPainter &painter, const Shape &shape) {
    if (shape.pointCount() >= 2) {
        QRectF rect(shape.firstPoint(), shape.lastPoint());
        painter.drawEllipse(rect);
    }
}

void ShapeRenderer::renderTriangle(QPainter &painter, const Shape &shape) {

    const int point_a = 0;
    const int point_b = 1;
    const int point_c = 2;
    if (shape.pointCount() >= 2) {
        QVector<QPointF> points = shape.getPoints();
        QPolygonF triangle;
        triangle << points[point_a] << points[point_b] << points[point_c];
        painter.drawPolygon(triangle);
    }
}

void ShapeRenderer::renderPolygon(QPainter &painter, const Shape &shape) {
    auto points = shape.getPoints();
    if (points.size() > 1) {
        painter.drawPolyline(points);
        if (points.size() > 2) {
            painter.drawLine(points.last(), points.first());
        }
    }
}

void ShapeRenderer::renderFreehand(QPainter &painter, const Shape &shape) {
    auto points = shape.getPoints();
    if (points.size() > 1) {
        for (int i = 1; i < points.size(); ++i) {
            painter.drawLine(points[i-1], points[i]);
        }
    }
}
