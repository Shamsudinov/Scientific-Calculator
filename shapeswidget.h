#ifndef SHAPESWIDGET_H
#define SHAPESWIDGET_H

#include <QWidget>
#include <QVector>
#include "shape.h"
#include "viewtransformer.h"

class ShapeRenderer;
class DrawingController;

class ShapesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ShapesWidget(QWidget *parent = nullptr);
    ~ShapesWidget();

    // Управление рисованием
    void setCurrentShape(ShapeType shape);
    void setCurrentColor(const QColor &color);
    void setCurrentThickness(int thickness);

    // Управление фигурами
    void clearAllShapes();
    void undoLastShape();
    QVector<Shape> getShapes() const;
    void setShapes(const QVector<Shape> &newShapes);

    // Файловые операции
    bool saveToFile(const QString &fileName);
    bool loadFromFile(const QString &fileName);

    // Управление видом
    void resetView();
    void zoomIn();
    void zoomOut();
    void fitToView();

signals:
    void shapeAdded();
    void shapesCleared();
    void viewChanged();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void setupUI();
    void drawShapes(QPainter &painter);
    void drawCurrentShape(QPainter &painter);
    void drawDebugInfo(QPainter &painter);

    class DrawingController *controller;
    ViewTransformer transformer;
    Shape currentShape;
    bool isDrawing;
    bool isPanning;
    QPoint lastPanPoint;
};

#endif // SHAPESWIDGET_H
