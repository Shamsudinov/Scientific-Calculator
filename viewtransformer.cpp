#include "viewtransformer.h"

ViewTransformer::ViewTransformer()
    : scale(1.0), offset(0, 0) {
}

void ViewTransformer::setScale(qreal scale) {
    this->scale = scale;
    constrainScale();
}

void ViewTransformer::setOffset(const QPointF &offset) {
    this->offset = offset;
}

void ViewTransformer::zoom(qreal factor, const QPointF &center) {
    qreal oldScale = scale;
    scale *= factor;
    constrainScale();

    // Корректируем offset для масштабирования относительно центра
    QPointF worldBefore = screenToWorld(center);
    offset = center - worldBefore * scale;
}

void ViewTransformer::pan(const QPointF &delta) {
    offset += delta;
}

QPointF ViewTransformer::screenToWorld(const QPointF &screenPoint) const {
    return (screenPoint - offset) / scale;
}

QPointF ViewTransformer::worldToScreen(const QPointF &worldPoint) const {
    return worldPoint * scale + offset;
}

void ViewTransformer::constrainScale() {
    if (scale < MIN_SCALE) scale = MIN_SCALE;
    if (scale > MAX_SCALE) scale = MAX_SCALE;
}
