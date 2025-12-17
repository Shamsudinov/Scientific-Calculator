#ifndef SHAPE_H
#define SHAPE_H

#include <QVector>
#include <QPointF>
#include <QRectF>
#include <QColor>

enum ShapeType {
    NoShape,
    LineShape,
    RectangleShape,
    EllipseShape,
    TriangleShape,
    PolygonShape,
    PolylineShape,
    TextShape
};

class Shape {
public:
    Shape(ShapeType type = NoShape,
          const QColor &color = Qt::black,
          int thickness = 2);

    ShapeType getType() const { return type; }
    QVector<QPointF> getPoints() const { return points; }
    QColor getColor() const { return color; }
    int getThickness() const { return thickness; }

    void addPoint(const QPointF &point);
    void setLastPoint(const QPointF &point);
    void clearPoints();
    bool isValid() const;
    int pointCount() const { return points.size(); }
    QPointF firstPoint() const { return points.isEmpty() ? QPointF() : points.first(); }
    QPointF lastPoint() const { return points.isEmpty() ? QPointF() : points.last(); }

    QRectF boundingRect() const;

    static QString shapeTypeToString(ShapeType type);
private:
    ShapeType type;
    QVector<QPointF> points;
    QColor color;
    int thickness;
};

#endif // SHAPE_H
