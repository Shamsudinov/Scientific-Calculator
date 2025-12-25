#ifndef TRIANGLEGRAPHICSITEM_H
#define TRIANGLEGRAPHICSITEM_H

#include "triangle.h"

#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QPointF>

// Класс для графического представления треугольника
class TriangleGraphicsItem : public QGraphicsItem {
private:
    Triangle triangle;
    QPen pen;
    QBrush brush;
    QPointF position;
    double scale;

public:
    TriangleGraphicsItem();
    TriangleGraphicsItem(const Triangle& tri,
                        QPointF pos = QPointF(0, 0),
                        double scale = 50.0,
                        QPen p = QPen(Qt::black, 2),
                        QBrush b = QBrush(Qt::lightGray));

    // Установка позиции центра треугольника
    void setPosition(const QPointF& pos);

    // Установка масштаба
    void setScale(double s);

    // Установка стилей рисования
    void setPen(const QPen& p);

    void setBrush(const QBrush& b);

    // Получение границ элемента
    QRectF boundingRect() const override;

    // Отрисовка треугольника
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

#endif // TRIANGLEGRAPHICSITEM_H
