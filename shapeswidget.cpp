#include "ShapesWidget.h"
#include "shaperenderer.h"
#include "drawingcontroller.h"
#include <QPainter>
#include <QWheelEvent>
#include <QFile>
#include <QDataStream>

ShapesWidget::ShapesWidget(QWidget *parent)
    : QWidget(parent)
    , controller(new DrawingController())
    , isDrawing(false)
    , isPanning(false)
{
    setupUI();
}

ShapesWidget::~ShapesWidget() {
    delete controller;
}

void ShapesWidget::setupUI() {
    setCurrentShape(LineShape);
    setCurrentColor(Qt::black);
    setCurrentThickness(2);

    setMouseTracking(true);
    setMinimumSize(400, 300);

    setAutoFillBackground(true);
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, Qt::white);
    setPalette(palette);
}

void ShapesWidget::setCurrentShape(ShapeType shape) {
    currentShape = Shape(shape);
}

void ShapesWidget::setCurrentColor(const QColor &color) {
    currentShape = Shape(currentShape.getType(), color, currentShape.getThickness());
}

void ShapesWidget::setCurrentThickness(int thickness) {
    currentShape = Shape(currentShape.getType(), currentShape.getColor(), thickness);
}

void ShapesWidget::clearAllShapes() {
    controller->clearAllShapes();
    update();
    emit shapesCleared();
}

void ShapesWidget::undoLastShape() {
    controller->removeLastShape();
    update();
}

QVector<Shape> ShapesWidget::getShapes() const {
    return controller->getShapes();
}

void ShapesWidget::setShapes(const QVector<Shape> &newShapes) {
    controller->setShapes(newShapes);
    update();
}

bool ShapesWidget::saveToFile(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_15);

    auto shapes = controller->getShapes();
    out << quint32(shapes.size());

    for (const Shape &shape : shapes) {
        out << quint32(shape.getType());
        out << shape.getColor();
        out << shape.getThickness();

        auto points = shape.getPoints();
        out << quint32(points.size());
        for (const QPointF &point : points) {
            out << point;
        }
    }

    file.close();
    return true;
}

bool ShapesWidget::loadFromFile(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_15);

    QVector<Shape> loadedShapes;
    quint32 shapeCount;
    in >> shapeCount;

    for (quint32 i = 0; i < shapeCount; ++i) {
        quint32 type;
        QColor color;
        int thickness;
        quint32 pointCount;

        in >> type >> color >> thickness >> pointCount;
        Shape shape(static_cast<ShapeType>(type), color, thickness);

        for (quint32 j = 0; j < pointCount; ++j) {
            QPointF point;
            in >> point;
            shape.addPoint(point);
        }

        loadedShapes.append(shape);
    }

    file.close();
    controller->setShapes(loadedShapes);
    update();
    return true;
}

void ShapesWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.save();
    painter.translate(transformer.getOffset());
    painter.scale(transformer.getScale(), transformer.getScale());

    drawShapes(painter);

    painter.restore();
    drawDebugInfo(painter);
}

void ShapesWidget::drawShapes(QPainter &painter) {
    auto shapes = controller->getShapes();
    for (const Shape &shape : shapes) {
        ShapeRenderer::render(painter, shape);
    }

    if (controller->isDrawing()) {
        ShapeRenderer::render(painter, controller->getCurrentShape());
    }
}

void ShapesWidget::drawDebugInfo(QPainter &painter) {
    painter.setPen(Qt::blue);
    painter.drawText(10, 20, QString("Масштаб: %1").arg(transformer.getScale(), 0, 'f', 2));
    painter.drawText(10, 40, QString("Фигур: %1").arg(controller->shapeCount()));
}

void ShapesWidget::mousePressEvent(QMouseEvent *event) {
    QPointF worldPoint = transformer.screenToWorld(event->pos());

    if (event->button() == Qt::RightButton) {
        isPanning = true;
        lastPanPoint = event->pos();
    } else if (event->button() == Qt::LeftButton && drawingEnabled) {
        isDrawing = true;
        controller->startDrawing(currentShape.getType(), worldPoint);
        update();
    }
}

void ShapesWidget::mouseMoveEvent(QMouseEvent *event) {
    QPointF worldPoint = transformer.screenToWorld(event->pos());

    if (isPanning) {
        QPoint delta = event->pos() - lastPanPoint;
        transformer.pan(delta);
        lastPanPoint = event->pos();
        update();
    } else if (isDrawing) {
        controller->updateDrawing(worldPoint);
        update();
    }
}

void ShapesWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        isPanning = false;
    } else if (event->button() == Qt::LeftButton && isDrawing) {
        isDrawing = false;
        controller->finishDrawing();
        emit shapeAdded();
        update();
    }
}

void ShapesWidget::wheelEvent(QWheelEvent *event) {
    qreal zoomFactor = 1.1;
    if (event->angleDelta().y() < 0) {
        zoomFactor = 1.0 / zoomFactor;
    }

    transformer.zoom(zoomFactor, event->position());
    update();
}

void ShapesWidget::resetView() {
    transformer.setScale(1.0);
    transformer.setOffset(QPointF(0, 0));
    update();
    emit viewChanged();
}

void ShapesWidget::zoomIn() {
    transformer.zoom(1.1, rect().center());
    update();
    emit viewChanged();
}

void ShapesWidget::zoomOut() {
    transformer.zoom(1.0/1.1, rect().center());
    update();
    emit viewChanged();
}

void ShapesWidget::fitToView() {
    auto shapes = controller->getShapes();
    if (shapes.isEmpty()) return;

    QRectF boundingRect;
    for (const Shape &shape : shapes) {
        boundingRect = boundingRect.united(shape.boundingRect());
    }

    if (!boundingRect.isEmpty()) {
        // Расчет масштаба и смещения для отображения всех фигур
        qreal scaleX = width() / boundingRect.width();
        qreal scaleY = height() / boundingRect.height();
        qreal scale = qMin(scaleX, scaleY) * 0.9; // 10% отступ

        transformer.setScale(scale);
        transformer.setOffset(QPointF(
            (width() - boundingRect.width() * scale) / 2 - boundingRect.left() * scale,
            (height() - boundingRect.height() * scale) / 2 - boundingRect.top() * scale
        ));

        update();
        emit viewChanged();
    }
}


// Новые методы
Shape ShapesWidget::getCurrentShape() const {
    return currentShape;
}

void ShapesWidget::updateCurrentShape(const Shape &shape) {
    currentShape = shape;
}

void ShapesWidget::setDrawingEnabled(bool enabled) {
    drawingEnabled = enabled;
}

bool ShapesWidget::isDrawingEnabled() const {
    return drawingEnabled;
}

void ShapesWidget::addShapeWithParameters(ShapeType type,
                                          const QVector<QPointF> &points,
                                          const QColor &color,
                                          int thickness) {
    if (points.size() < getRequiredPointsCount(type)) {
        QMessageBox::warning(this, "Ошибка",
            QString("Для фигуры '%1' требуется минимум %2 точек")
            .arg(Shape::shapeTypeToString(type))
            .arg(getRequiredPointsCount(type)));
        return;
    }

    Shape shape(type, color, thickness);
    for (const QPointF &point : points) {
        shape.addPoint(point);
    }

    controller->addShape(shape);
    update();
    emit shapeAdded();
}

QVector<QString> ShapesWidget::getAvailableShapes() {
    return {
        "Линия",
        "Прямоугольник",
        "Эллипс",
        "Треугольник",
        "Многоугольник",
        "Ломаная линия",
        "Текст"
    };
}

int ShapesWidget::getRequiredPointsCount(ShapeType type) {
    switch (type) {
    case LineShape: return 2;
    case RectangleShape: return 2;
    case EllipseShape: return 2;
    case TriangleShape: return 3;
    case PolygonShape: return 3; // Минимум треугольник
    case PolylineShape: return 2;
    case TextShape: return 1;
    default: return 1;
    }
}
