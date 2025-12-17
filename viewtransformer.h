#ifndef VIEWTRANSFORMER_H
#define VIEWTRANSFORMER_H

#include <QPointF>

class ViewTransformer {
public:
    ViewTransformer();

    void setScale(qreal scale);
    qreal getScale() const { return scale; }
    void setOffset(const QPointF &offset);
    QPointF getOffset() const { return offset; }

    void zoom(qreal factor, const QPointF &center);
    void pan(const QPointF &delta);

    QPointF screenToWorld(const QPointF &screenPoint) const;
    QPointF worldToScreen(const QPointF &worldPoint) const;

    void constrainScale();

private:
    qreal scale;
    QPointF offset;

    const qreal MIN_SCALE = 0.1;
    const qreal MAX_SCALE = 10.0;
};

#endif // VIEWTRANSFORMER_H
