#include "drawingcontroller.h"



DrawingController::DrawingController() : drawingInProgress(false) {}

void DrawingController::startDrawing(ShapeType type, const QPointF &point) {
    drawingInProgress = true;
    currentShape = Shape(type);
    initializeShapePoints(point);
}

void DrawingController::initializeShapePoints(const QPointF &point) {
    currentShape.clearPoints();
    currentShape.addPoint(point);

    if (currentShape.getType() == LineShape ||
            currentShape.getType() == RectangleShape ||
            currentShape.getType() == EllipseShape ||
            currentShape.getType() == TriangleShape) {
        currentShape.addPoint(point);
    }
}

void DrawingController::updateDrawing(const QPointF &point) {
    if (!drawingInProgress) return;

    if (currentShape.getType() == PolylineShape ||
            currentShape.getType() == PolygonShape) {
        currentShape.addPoint(point);
    } else {
        currentShape.setLastPoint(point);
    }
}

void DrawingController::finishDrawing() {
    if (drawingInProgress && currentShape.isValid()) {
        shapes.append(currentShape);
    }
    drawingInProgress = false;
    currentShape.clearPoints();
}

void DrawingController::cancelDrawing() {
    drawingInProgress = false;
    currentShape.clearPoints();
}

void DrawingController::addShape(const Shape &shape) {
    shapes.append(shape);
}

void DrawingController::removeLastShape() {
    if (!shapes.isEmpty()) {
        shapes.removeLast();
    }
}

void DrawingController::clearAllShapes() {
    shapes.clear();
}

void DrawingController::setShapes(const QVector<Shape> &newShapes) {
    shapes = newShapes;
}

QVector<Shape> DrawingController::getShapes() const {
    return shapes;
}

int DrawingController::shapeCount() const { return shapes.size(); }

Shape DrawingController::getCurrentShape() const { return currentShape; }

bool DrawingController::isDrawing() const { return drawingInProgress; }
