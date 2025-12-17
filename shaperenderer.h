#ifndef SHAPERENDERER_H
#define SHAPERENDERER_H

#include "shape.h"
#include <QPainter>

class ShapeRenderer {
public:
    static void render(QPainter &painter, const Shape &shape);

private:
    static void renderLine(QPainter &painter, const Shape &shape);
    static void renderRectangle(QPainter &painter, const Shape &shape);
    static void renderEllipse(QPainter &painter, const Shape &shape);
    static void renderTriangle(QPainter &painter, const Shape &shape);
    static void renderPolygon(QPainter &painter, const Shape &shape);
    static void renderFreehand(QPainter &painter, const Shape &shape);
};

#endif // SHAPERENDERER_H
