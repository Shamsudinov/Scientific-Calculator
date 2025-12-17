#ifndef SHAPESWIDGET_H
#define SHAPESWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QVector>
#include <QPointF>
#include <QColor>

enum ShapeType {
    NoShape,
    LineShape,
    RectangleShape,
    EllipseShape,
    TriangleShape,
    PolygonShape,
    FreehandShape
};

struct Shape {
    ShapeType type;
    QVector<QPointF> points;
    QColor color;
    int thickness;

    Shape(ShapeType t = NoShape,
          const QColor &c = Qt::black,
          int th = 2)
        : type(t), color(c), thickness(th) {}
};

class ShapesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ShapesWidget(QWidget *parent = nullptr);

    // Методы для управления фигурами
    void setCurrentShape(ShapeType shape);
    void setCurrentColor(const QColor &color);
    void setCurrentThickness(int thickness);
    void clearAllShapes();
    void undoLastShape();

    // Сохранение и загрузка
    bool saveToFile(const QString &fileName);
    bool loadFromFile(const QString &fileName);

    QVector<Shape> getShapes() const { return shapes; }
    void setShapes(const QVector<Shape> &newShapes);

signals:
    void shapeAdded();
    void shapesCleared();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void drawShape(QPainter &painter, const Shape &shape);
    void drawLine(QPainter &painter, const Shape &shape);
    void drawRectangle(QPainter &painter, const Shape &shape);
    void drawEllipse(QPainter &painter, const Shape &shape);
    void drawTriangle(QPainter &painter, const Shape &shape);
    void drawPolygon(QPainter &painter, const Shape &shape);
    void drawFreehand(QPainter &painter, const Shape &shape);

    QPointF applyTransform(const QPointF &point);
    void updateScale(qreal delta, const QPointF &mousePos);

private:
    QVector<Shape> shapes;
    Shape currentShape;
    bool isDrawing;

    // Трансформация для масштабирования и перемещения
    qreal scaleFactor;
    QPointF offset;

    // Для перемещения холста
    bool isPanning;
    QPoint lastPanPoint;
};

#endif // SHAPESWIDGET_H
