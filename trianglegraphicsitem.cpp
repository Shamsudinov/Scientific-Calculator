#include "trianglegraphicsitem.h"

TriangleGraphicsItem::TriangleGraphicsItem()
{

}

TriangleGraphicsItem::TriangleGraphicsItem(const Triangle &tri, QPointF pos, double scale, QPen p, QBrush b)
    : triangle(tri), position(pos), scale(scale), pen(p), brush(b) {
}

void TriangleGraphicsItem::setPosition(const QPointF &pos) {
    position = pos;
    update();
}

void TriangleGraphicsItem::setScale(double s) {
    scale = s;
    update();
}

void TriangleGraphicsItem::setPen(const QPen &p) {
    pen = p;
    update();
}

void TriangleGraphicsItem::setBrush(const QBrush &b) {
    brush = b;
    update();
}

QRectF TriangleGraphicsItem::boundingRect() const {
    auto vertices = triangle.getVertices(position.x(), position.y(), scale);
    double minX = vertices[0].x();
    double maxX = vertices[0].x();
    double minY = vertices[0].y();
    double maxY = vertices[0].y();

    for (const auto& vertex : vertices) {
        minX = std::min(minX, vertex.x());
        maxX = std::max(maxX, vertex.x());
        minY = std::min(minY, vertex.y());
        maxY = std::max(maxY, vertex.y());
    }

    // Добавляем отступ для толщины пера
    double padding = pen.widthF() / 2.0;
    return QRectF(minX - padding, minY - padding,
                  maxX - minX + 2 * padding, maxY - minY + 2 * padding);
}

void TriangleGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    auto vertices = triangle.getVertices(position.x(), position.y(), scale);

    QPolygonF polygon;
    for (const auto& vertex : vertices) {
        polygon << vertex;
    }

    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawPolygon(polygon);

    // Рисуем вершины точками
    painter->setBrush(Qt::red);
    for (const auto& vertex : vertices) {
        painter->drawEllipse(vertex, 3, 3);
    }

    // Подписываем стороны
    QFont font = painter->font();
    font.setPointSize(8);
    painter->setFont(font);
    painter->setPen(Qt::darkBlue);

    QString sideA = QString("a=%1").arg(triangle.getSideA(), 0, 'f', 1);
    QString sideB = QString("b=%1").arg(triangle.getSideB(), 0, 'f', 1);
    QString sideC = QString("c=%1").arg(triangle.getSideC(), 0, 'f', 1);

    // Подписи около сторон
    QPointF midAB((vertices[0].x() + vertices[1].x()) / 2,
            (vertices[0].y() + vertices[1].y()) / 2);
    QPointF midBC((vertices[1].x() + vertices[2].x()) / 2,
            (vertices[1].y() + vertices[2].y()) / 2);
    QPointF midCA((vertices[2].x() + vertices[0].x()) / 2,
            (vertices[2].y() + vertices[0].y()) / 2);

    painter->drawText(midAB, sideC);
    painter->drawText(midBC, sideA);
    painter->drawText(midCA, sideB);
}
