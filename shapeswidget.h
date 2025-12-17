#ifndef SHAPESWIDGET_H
#define SHAPESWIDGET_H

#include <QWidget>
#include <QVector>
#include <QMessageBox>
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
    void setCurrentShape(ShapeType);
    void setCurrentColor(const QColor &);
    void setCurrentThickness(int);

    // Управление фигурами
    void clearAllShapes();
    void undoLastShape();
    QVector<Shape> getShapes() const;
    void setShapes(const QVector<Shape> &);

    // Файловые операции
    bool saveToFile(const QString &);
    bool loadFromFile(const QString &);

    // Управление видом
    void resetView();
    void zoomIn();
    void zoomOut();
    void fitToView();


    // Новые методы для работы с параметрами
    Shape getCurrentShape() const;
    void updateCurrentShape(const Shape &);
    void setDrawingEnabled(bool);
    bool isDrawingEnabled() const;

    // Методы для рисования из UI
    void addShapeWithParameters(ShapeType , const QVector<QPointF> &, const QColor &, int);

    // Получить список доступных фигур
    static QVector<QString> getAvailableShapes();

    // Получить требуемое количество точек для фигуры
    static int getRequiredPointsCount(ShapeType);
signals:
    void shapeAdded();
    void shapesCleared();
    void viewChanged();

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void wheelEvent(QWheelEvent *) override;

private:
    void setupUI();
    void drawShapes(QPainter &);
    void drawCurrentShape(QPainter &);
    void drawDebugInfo(QPainter &);

    class DrawingController *controller;
    ViewTransformer transformer;
    Shape currentShape;
    bool isDrawing;
    bool isPanning;
    QPoint lastPanPoint;
    bool drawingEnabled;  // Флаг разрешения рисования
};

#endif // SHAPESWIDGET_H
