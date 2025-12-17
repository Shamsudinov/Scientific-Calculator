#include "ShapesWidget.h"
#include "shaperenderer.h"
#include "drawingcontroller.h"
#include <QPainter>
#include <QWheelEvent>
#include <QFile>
#include <QDataStream>
#include <QPen>
#include <QResizeEvent>

ShapesWidget::ShapesWidget(QWidget *parent)
    : QWidget(parent)
    , controller(new DrawingController())
    , isDrawing(false)
    , isPanning(false)
    , gridEnabled(true)  // сетка включена по умолчанию
    , gridStep(1.0)      // 1 см по умолчанию
    , gridColor(QColor(200, 200, 200, 150))  // светло-серый с прозрачностью
    , axesColor(Qt::black)
    , worldSize(50.0)    // 50 см по умолчанию
{
    setupUI();
    updateViewForWorldSize();
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

    // Настройка шрифта для координатных подписей
    coordinateFont = QFont("Arial", 8);
}

void ShapesWidget::updateViewForWorldSize() {
    // При масштабе 1.0 виджет должен показывать worldSize сантиметров
    // Настраиваем трансформер соответствующим образом
    transformer.setScale(1.0);

    // Центрируем начало координат
    QPointF offset(width() / 2.0, height() / 2.0);
    transformer.setOffset(offset);

    update();
}

void ShapesWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    updateViewForWorldSize();
}

QPointF ShapesWidget::screenToWorldWithOffset(const QPoint &screenPoint) const {
    // Преобразуем экранные координаты в мировые с учетом размера области
    QPointF centeredScreen(screenPoint.x() - width() / 2.0,
                          screenPoint.y() - height() / 2.0);

    // Масштабируем: worldSize см соответствуют меньшей стороне виджета
    qreal widgetSize = qMin(width(), height());
    qreal pixelsPerCm = widgetSize / worldSize;

    return QPointF(centeredScreen.x() / pixelsPerCm,
                   -centeredScreen.y() / pixelsPerCm);  // Инвертируем Y
}

QPointF ShapesWidget::worldToScreenWithOffset(const QPointF &worldPoint) const {
    // Преобразуем мировые координаты в экранные
    qreal widgetSize = qMin(width(), height());
    qreal pixelsPerCm = widgetSize / worldSize;

    QPointF screenPoint(worldPoint.x() * pixelsPerCm,
                       -worldPoint.y() * pixelsPerCm);  // Инвертируем Y

    return QPointF(screenPoint.x() + width() / 2.0,
                   screenPoint.y() + height() / 2.0);
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

    // Рисуем координатную сетку (в координатах экрана)
    if (gridEnabled) {
        drawCoordinateGrid(painter);
    }

    // Рисуем фигуры с использованием текущего трансформера
    painter.save();
    qreal widgetSize = qMin(width(), height());
    qreal pixelsPerCm = widgetSize / worldSize;

    painter.translate(width() / 2.0, height() / 2.0);
    painter.scale(pixelsPerCm, -pixelsPerCm);  // Инвертируем ось Y

    drawShapes(painter);
    painter.restore();

    drawDebugInfo(painter);
}

void ShapesWidget::drawCoordinateGrid(QPainter &painter) {
    // Рисуем оси координат
    drawAxes(painter);

    // Рисуем линии сетки
    drawGridLines(painter);
}

void ShapesWidget::drawAxes(QPainter &painter) {
    painter.save();

    QPen axesPen(axesColor, 2);
    painter.setPen(axesPen);

    // Начало координат в центре виджета
    int centerX = width() / 2;
    int centerY = height() / 2;

    // Ось X (горизонтальная)
    painter.drawLine(0, centerY, width(), centerY);

    // Ось Y (вертикальная)
    painter.drawLine(centerX, 0, centerX, height());

    // Стрелки на осях
    int arrowSize = 10;

    // Стрелка оси X
    painter.drawLine(width() - arrowSize, centerY - arrowSize/2,
                     width(), centerY);
    painter.drawLine(width() - arrowSize, centerY + arrowSize/2,
                     width(), centerY);

    // Стрелка оси Y
    painter.drawLine(centerX - arrowSize/2, arrowSize,
                     centerX, 0);
    painter.drawLine(centerX + arrowSize/2, arrowSize,
                     centerX, 0);

    // Подписи осей
    painter.setFont(coordinateFont);
    painter.drawText(width() - 20, centerY - 10, "X");
    painter.drawText(centerX + 10, 20, "Y");

    // Ноль в начале координат
    painter.drawText(centerX + 5, centerY - 5, "0");

    painter.restore();
}

void ShapesWidget::drawGridLines(QPainter &painter) {
    painter.save();

    QPen gridPen(gridColor, 1, Qt::DotLine);
    painter.setPen(gridPen);

    qreal widgetSize = qMin(width(), height());
    qreal pixelsPerCm = widgetSize / worldSize;

    // Шаг сетки в пикселях
    qreal gridStepPixels = gridStep * pixelsPerCm;

    if (gridStepPixels < 5) {
        // Слишком мелкая сетка, не рисуем
        painter.restore();
        return;
    }

    int centerX = width() / 2;
    int centerY = height() / 2;

    // Рисуем вертикальные линии (слева и справа от оси Y)
    for (int i = 1; centerX + i * gridStepPixels < width(); i++) {
        qreal x = centerX + i * gridStepPixels;
        painter.drawLine(x, 0, x, height());

        // Подпись для положительных X
        if (gridStepPixels > 20) {
            painter.setPen(axesColor);
            painter.drawText(x - 10, centerY + 15, QString::number(i));
            painter.setPen(gridPen);
        }
    }

    for (int i = 1; centerX - i * gridStepPixels >= 0; i++) {
        qreal x = centerX - i * gridStepPixels;
        painter.drawLine(x, 0, x, height());

        // Подпись для отрицательных X
        if (gridStepPixels > 20) {
            painter.setPen(axesColor);
            painter.drawText(x - 10, centerY + 15, QString::number(-i));
            painter.setPen(gridPen);
        }
    }

    // Рисуем горизонтальные линии (выше и ниже оси X)
    for (int i = 1; centerY + i * gridStepPixels < height(); i++) {
        qreal y = centerY + i * gridStepPixels;
        painter.drawLine(0, y, width(), y);

        // Подпись для отрицательных Y (в декартовой системе)
        if (gridStepPixels > 20) {
            painter.setPen(axesColor);
            painter.drawText(centerX + 5, y + 5, QString::number(-i));
            painter.setPen(gridPen);
        }
    }

    for (int i = 1; centerY - i * gridStepPixels >= 0; i++) {
        qreal y = centerY - i * gridStepPixels;
        painter.drawLine(0, y, width(), y);

        // Подпись для положительных Y (в декартовой системе)
        if (gridStepPixels > 20) {
            painter.setPen(axesColor);
            painter.drawText(centerX + 5, y + 5, QString::number(i));
            painter.setPen(gridPen);
        }
    }

    painter.restore();
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
    painter.drawText(10, 60, QString("Область: %1 см").arg(worldSize, 0, 'f', 0));

    // Отображаем текущие координаты мыши в сантиметрах
    if (underMouse()) {
        QPoint mousePos = mapFromGlobal(QCursor::pos());
        QPointF worldPos = screenToWorldWithOffset(mousePos);
        painter.drawText(10, 80, QString("X: %1 см, Y: %2 см")
                         .arg(worldPos.x(), 0, 'f', 1)
                         .arg(worldPos.y(), 0, 'f', 1));
    }
}

void ShapesWidget::mousePressEvent(QMouseEvent *event) {
    QPointF worldPoint = screenToWorldWithOffset(event->pos());

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
    QPointF worldPoint = screenToWorldWithOffset(event->pos());

    if (isPanning) {
        // Панорамирование реализуем через изменение worldSize
        QPoint delta = event->pos() - lastPanPoint;
        // Здесь можно реализовать панорамирование если нужно
        lastPanPoint = event->pos();
        update();
    } else if (isDrawing) {
        controller->updateDrawing(worldPoint);
        update();
    }

    // Обновляем для отображения координат мыши
    update();
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
    // Масштабирование через изменение размера отображаемой области
    qreal zoomFactor = 1.1;
    if (event->angleDelta().y() < 0) {
        // Увеличиваем область (уменьшаем масштаб)
        worldSize *= zoomFactor;
    } else {
        // Уменьшаем область (увеличиваем масштаб)
        worldSize /= zoomFactor;
    }

    // Ограничиваем минимальный и максимальный размер области
    worldSize = qMax(1.0, qMin(500.0, worldSize));

    update();
    emit viewChanged();
}

void ShapesWidget::resetView() {
    worldSize = 50.0;
    updateViewForWorldSize();
    emit viewChanged();
}

void ShapesWidget::zoomIn() {
    worldSize /= 1.1;
    worldSize = qMax(1.0, worldSize);
    update();
    emit viewChanged();
}

void ShapesWidget::zoomOut() {
    worldSize *= 1.1;
    worldSize = qMin(500.0, worldSize);
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
        // Находим максимальную координату по модулю
        qreal maxCoord = qMax(qAbs(boundingRect.left()), qAbs(boundingRect.right()));
        maxCoord = qMax(maxCoord, qAbs(boundingRect.top()));
        maxCoord = qMax(maxCoord, qAbs(boundingRect.bottom()));

        // Устанавливаем worldSize так, чтобы все фигуры помещались
        // с запасом 20%
        worldSize = maxCoord * 2.4;  // *2 для обеих сторон, *1.2 для запаса

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

// Методы для управления координатной сеткой
void ShapesWidget::setGridEnabled(bool enabled) {
    gridEnabled = enabled;
    update();
}

bool ShapesWidget::isGridEnabled() const {
    return gridEnabled;
}

void ShapesWidget::setGridStep(qreal step) {
    gridStep = step;
    update();
}

qreal ShapesWidget::getGridStep() const {
    return gridStep;
}

void ShapesWidget::setGridColor(const QColor &color) {
    gridColor = color;
    update();
}

QColor ShapesWidget::getGridColor() const {
    return gridColor;
}

void ShapesWidget::setAxesColor(const QColor &color) {
    axesColor = color;
    update();
}

QColor ShapesWidget::getAxesColor() const {
    return axesColor;
}

// Методы для управления размером отображаемой области
void ShapesWidget::setWorldSize(qreal size) {
    worldSize = qMax(1.0, size);
    update();
    emit viewChanged();
}

qreal ShapesWidget::getWorldSize() const {
    return worldSize;
}
