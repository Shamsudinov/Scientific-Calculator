#ifndef DRAWINGCONTROLLER_H
#define DRAWINGCONTROLLER_H

#include "shape.h"
#include <QVector>

class DrawingController {
public:
    DrawingController();

    void startDrawing(ShapeType type, const QPointF &point);

    void initializeShapePoints(const QPointF &point);

    void updateDrawing(const QPointF &point);

    void finishDrawing();

    void cancelDrawing();

    void addShape(const Shape &shape);

    void removeLastShape();

    void clearAllShapes();

    void setShapes(const QVector<Shape> &newShapes);

    QVector<Shape> getShapes() const;

    bool isDrawing() const;
    Shape getCurrentShape() const;
    int shapeCount() const;

private:
    QVector<Shape> shapes;
    Shape currentShape;
    bool drawingInProgress;
};

#endif // DRAWINGCONTROLLER_H
