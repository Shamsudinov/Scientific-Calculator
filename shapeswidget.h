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

    // Управление координатной сеткой
    void setGridEnabled(bool enabled);
    bool isGridEnabled() const;
    void setGridStep(qreal step);  // шаг сетки в мировых координатах
    qreal getGridStep() const;
    void setGridColor(const QColor &color);
    QColor getGridColor() const;
    void setAxesColor(const QColor &color);
    QColor getAxesColor() const;

    // Управление размером отображаемой области
    void setWorldSize(qreal size);  // размер области в см при масштабе 1
    qreal getWorldSize() const;
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
    void resizeEvent(QResizeEvent *) override;

private:
    void setupUI();
    void drawShapes(QPainter &);
    void drawCurrentShape(QPainter &);
    void drawDebugInfo(QPainter &);

    void drawCoordinateGrid(QPainter &painter);
    void drawAxes(QPainter &painter);
    void drawGridLines(QPainter &painter);
    void updateViewForWorldSize();
    QPointF screenToWorldWithOffset(const QPoint &screenPoint) const;
    QPointF worldToScreenWithOffset(const QPointF &worldPoint) const;
    QPointF calculateGridOrigin() const;

    class DrawingController *controller;
    ViewTransformer transformer;
    Shape currentShape;
    bool isDrawing;
    bool isPanning;
    QPoint lastPanPoint;
    bool drawingEnabled;  // Флаг разрешения рисования

    // Параметры координатной сетки
    bool gridEnabled;
    qreal gridStep;  // шаг сетки в мировых координатах (1.0 = 1 см)
    QColor gridColor;
    QColor axesColor;
    QFont coordinateFont;
    qreal worldSize;  // по умолчанию 50 см
};

#endif // SHAPESWIDGET_H
