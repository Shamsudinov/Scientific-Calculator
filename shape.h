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
    Shape(ShapeType type = NoShape,const QColor &color = Qt::black,int thickness = 2);

    ShapeType getType() const;
    QVector<QPointF> getPoints() const;
    QColor getColor() const;
    int getThickness() const;

    void addPoint(const QPointF &point);
    void setLastPoint(const QPointF &point);
    void clearPoints();
    bool isValid() const;
    int pointCount() const;
    QPointF firstPoint() const;
    QPointF lastPoint() const;

    QRectF boundingRect() const;

    static QString shapeTypeToString(ShapeType type);
private:
    ShapeType type;
    QVector<QPointF> points;
    QColor color;
    int thickness;
};

#endif // SHAPE_H
