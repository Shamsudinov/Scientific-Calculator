#include "ShapesWidget.h"
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <QMessageBox>
#include <QWheelEvent>

ShapesWidget::ShapesWidget(QWidget *parent)
    : QWidget(parent)
    , isDrawing(false)
    , scaleFactor(1.0)
    , offset(0, 0)
    , isPanning(false)
{
    setCurrentShape(LineShape);
    setCurrentColor(Qt::black);
    setCurrentThickness(2);

    // Включаем отслеживание мыши для движения без нажатия
    setMouseTracking(true);

    // Устанавливаем минимальный размер
    setMinimumSize(400, 300);

    // Белый фон
    setAutoFillBackground(true);
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, Qt::white);
    setPalette(palette);
}

void ShapesWidget::setCurrentShape(ShapeType shape)
{
    currentShape.type = shape;
}

void ShapesWidget::setCurrentColor(const QColor &color)
{
    currentShape.color = color;
}

void ShapesWidget::setCurrentThickness(int thickness)
{
    currentShape.thickness = thickness;
}

void ShapesWidget::clearAllShapes()
{
    shapes.clear();
    update();
    emit shapesCleared();
}

void ShapesWidget::undoLastShape()
{
    if (!shapes.isEmpty()) {
        shapes.removeLast();
        update();
    }
}

bool ShapesWidget::saveToFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_15);

    // Сохраняем количество фигур
    out << quint32(shapes.size());

    // Сохраняем каждую фигуру
    for (const Shape &shape : shapes) {
        out << quint32(shape.type);
        out << shape.color;
        out << shape.thickness;
        out << quint32(shape.points.size());

        for (const QPointF &point : shape.points) {
            out << point;
        }
    }

    file.close();
    return true;
}

bool ShapesWidget::loadFromFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_15);

    shapes.clear();

    quint32 shapeCount;
    in >> shapeCount;

    for (quint32 i = 0; i < shapeCount; ++i) {
        Shape shape;
        quint32 type;
        quint32 pointCount;

        in >> type;
        in >> shape.color;
        in >> shape.thickness;
        in >> pointCount;

        shape.type = static_cast<ShapeType>(type);

        for (quint32 j = 0; j < pointCount; ++j) {
            QPointF point;
            in >> point;
            shape.points.append(point);
        }

        shapes.append(shape);
    }

    file.close();
    update();
    return true;
}

void ShapesWidget::setShapes(const QVector<Shape> &newShapes)
{
    shapes = newShapes;
    update();
}

void ShapesWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Сохраняем текущее состояние
    painter.save();

    // Применяем трансформации
    painter.translate(offset);
    painter.scale(scaleFactor, scaleFactor);

    // Отрисовываем все фигуры
    for (const Shape &shape : shapes) {
        drawShape(painter, shape);
    }

    // Отрисовываем текущую фигуру (если рисуем)
    if (isDrawing && !currentShape.points.isEmpty()) {
        drawShape(painter, currentShape);
    }

    // Восстанавливаем состояние
    painter.restore();

    // Отладочная информация (можно убрать)
    painter.setPen(Qt::blue);
    painter.drawText(10, 20, QString("Масштаб: %1").arg(scaleFactor, 0, 'f', 2));
    painter.drawText(10, 40, QString("Фигур: %1").arg(shapes.size()));
}

void ShapesWidget::drawShape(QPainter &painter, const Shape &shape)
{
    painter.setPen(QPen(shape.color, shape.thickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);

    switch (shape.type) {
    case LineShape:
        drawLine(painter, shape);
        break;
    case RectangleShape:
        drawRectangle(painter, shape);
        break;
    case EllipseShape:
        drawEllipse(painter, shape);
        break;
    case TriangleShape:
        drawTriangle(painter, shape);
        break;
    case PolygonShape:
        drawPolygon(painter, shape);
        break;
    case FreehandShape:
        drawFreehand(painter, shape);
        break;
    default:
        break;
    }
}

void ShapesWidget::drawLine(QPainter &painter, const Shape &shape)
{
    if (shape.points.size() >= 2) {
        painter.drawLine(shape.points.first(), shape.points.last());
    }
}

void ShapesWidget::drawRectangle(QPainter &painter, const Shape &shape)
{
    if (shape.points.size() >= 2) {
        QRectF rect(shape.points.first(), shape.points.last());
        painter.drawRect(rect);
    }
}

void ShapesWidget::drawEllipse(QPainter &painter, const Shape &shape)
{
    if (shape.points.size() >= 2) {
        QRectF rect(shape.points.first(), shape.points.last());
        painter.drawEllipse(rect);
    }
}

void ShapesWidget::drawTriangle(QPainter &painter, const Shape &shape)
{
    if (shape.points.size() >= 2) {
        QPointF p1 = shape.points.first();
        QPointF p2 = shape.points.last();

        QPointF p3(p1.x() + (p2.x() - p1.x()) / 2, p1.y());
        QPolygonF triangle;
        triangle << p1 << p2 << p3 << p1;

        painter.drawPolygon(triangle);
    }
}

void ShapesWidget::drawPolygon(QPainter &painter, const Shape &shape)
{
    if (shape.points.size() > 1) {
        painter.drawPolyline(shape.points.data(), shape.points.size());
        if (shape.points.size() > 2) {
            painter.drawLine(shape.points.last(), shape.points.first());
        }
    }
}

void ShapesWidget::drawFreehand(QPainter &painter, const Shape &shape)
{
    if (shape.points.size() > 1) {
        for (int i = 1; i < shape.points.size(); ++i) {
            painter.drawLine(shape.points[i-1], shape.points[i]);
        }
    }
}

QPointF ShapesWidget::applyTransform(const QPointF &point)
{
    return (point - offset) / scaleFactor;
}

void ShapesWidget::mousePressEvent(QMouseEvent *event)
{
    QPointF transformedPoint = applyTransform(event->pos());

    if (event->button() == Qt::RightButton) {
        // Правая кнопка - перемещение холста
        isPanning = true;
        lastPanPoint = event->pos();
    } else if (event->button() == Qt::LeftButton) {
        // Левая кнопка - рисование
        isDrawing = true;
        currentShape.points.clear();
        currentShape.points.append(transformedPoint);

        // Для некоторых фигур добавляем первую точку дважды
        if (currentShape.type == LineShape ||
            currentShape.type == RectangleShape ||
            currentShape.type == EllipseShape ||
            currentShape.type == TriangleShape) {
            currentShape.points.append(transformedPoint);
        }
    }

    update();
}

void ShapesWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPointF transformedPoint = applyTransform(event->pos());

    if (isPanning) {
        // Перемещение холста
        QPoint delta = event->pos() - lastPanPoint;
        offset += delta;
        lastPanPoint = event->pos();
        update();
    } else if (isDrawing && !currentShape.points.isEmpty()) {
        // Обновление текущей фигуры
        if (currentShape.type == FreehandShape ||
            currentShape.type == PolygonShape) {
            currentShape.points.append(transformedPoint);
        } else {
            // Для фигур с двумя точками обновляем последнюю
            currentShape.points.last() = transformedPoint;
        }
        update();
    }
}

void ShapesWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        isPanning = false;
    } else if (event->button() == Qt::LeftButton && isDrawing) {
        isDrawing = false;

        // Добавляем фигуру в список, если она валидна
        if (!currentShape.points.isEmpty() && currentShape.type != NoShape) {
            shapes.append(currentShape);
            emit shapeAdded();
        }

        // Очищаем текущую фигуру
        currentShape.points.clear();
        update();
    }
}

void ShapesWidget::wheelEvent(QWheelEvent *event)
{
    // Масштабирование колесиком мыши
    qreal zoomFactor = 1.1;
    if (event->angleDelta().y() < 0) {
        zoomFactor = 1.0 / zoomFactor;
    }

    QPointF mousePos = event->position();
    updateScale(zoomFactor, mousePos);
    update();
}

void ShapesWidget::updateScale(qreal delta, const QPointF &mousePos)
{
    qreal oldScale = scaleFactor;
    scaleFactor *= delta;

    // Ограничиваем масштаб
    if (scaleFactor < 0.1) scaleFactor = 0.1;
    if (scaleFactor > 10.0) scaleFactor = 10.0;

    // Корректируем offset для масштабирования относительно курсора
    QPointF mouseBefore = (mousePos - offset) / oldScale;
    offset = mousePos - mouseBefore * scaleFactor;
}
