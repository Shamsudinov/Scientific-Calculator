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
    , gridEnabled(true)      // сетка включена по умолчанию
    , autoAdjustGrid(true)   // автоматическая подстройка сетки
    , gridStep(1.0)         // 1 см по умолчанию
    , gridDensity(1.0)      // нормальная плотность
    , gridColor(QColor(200, 200, 200, 150))  // светло-серый с прозрачностью
    , axesColor(Qt::black)
    , worldSize(50.0)       // 50 см по умолчанию
    , effectiveGridStep(1.0)
{
    setupUI();
    updateViewForWorldSize();
    updateGridParameters();
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
    transformer.setScale(1.0);
    QPointF offset(width() / 2.0, height() / 2.0);
    transformer.setOffset(offset);
    updateGridParameters();
    update();
}

void ShapesWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    updateViewForWorldSize();
}

QPointF ShapesWidget::screenToWorldWithOffset(const QPoint &screenPoint) const {
    QPointF centeredScreen(screenPoint.x() - width() / 2.0,
                          screenPoint.y() - height() / 2.0);

    qreal widgetSize = qMin(width(), height());
    qreal pixelsPerCm = widgetSize / worldSize;

    return QPointF(centeredScreen.x() / pixelsPerCm,
                   -centeredScreen.y() / pixelsPerCm);
}

QPointF ShapesWidget::worldToScreenWithOffset(const QPointF &worldPoint) const {
    qreal widgetSize = qMin(width(), height());
    qreal pixelsPerCm = widgetSize / worldSize;

    QPointF screenPoint(worldPoint.x() * pixelsPerCm,
                       -worldPoint.y() * pixelsPerCm);

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

    // Рисуем координатную сетку (в мировых координатах)
    if (gridEnabled) {
        painter.save();

        // Применяем те же преобразования, что и для фигур
        qreal widgetSize = qMin(width(), height());
        qreal pixelsPerCm = widgetSize / worldSize;

        painter.translate(width() / 2.0, height() / 2.0);
        painter.scale(pixelsPerCm, -pixelsPerCm);

        drawCoordinateGrid(painter);
        painter.restore();
    }

    // Рисуем фигуры
    painter.save();
    qreal widgetSize = qMin(width(), height());
    qreal pixelsPerCm = widgetSize / worldSize;

    painter.translate(width() / 2.0, height() / 2.0);
    painter.scale(pixelsPerCm, -pixelsPerCm);

    drawShapes(painter);
    painter.restore();

    // Рисуем подписи координат (в экранных координатах)
    if (gridEnabled) {
        painter.save();
        drawGridLabels(painter);
        painter.restore();
    }

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

    QPen axesPen(axesColor, 0.1); // Толщина в мировых координатах
    painter.setPen(axesPen);

    // Рассчитываем границы видимой области в мировых координатах
    qreal halfWorldSize = worldSize / 2.0;
    qreal visibleLeft = -halfWorldSize;
    qreal visibleRight = halfWorldSize;
    qreal visibleTop = halfWorldSize;
    qreal visibleBottom = -halfWorldSize;

    // Ось X
    painter.drawLine(QPointF(visibleLeft, 0), QPointF(visibleRight, 0));

    // Ось Y
    painter.drawLine(QPointF(0, visibleTop), QPointF(0, visibleBottom));

    // Стрелки на осях
    qreal arrowSize = 0.5; // Размер стрелок в см

    // Стрелка оси X
    painter.drawLine(QPointF(visibleRight - arrowSize, arrowSize/2),
                     QPointF(visibleRight, 0));
    painter.drawLine(QPointF(visibleRight - arrowSize, -arrowSize/2),
                     QPointF(visibleRight, 0));

    // Стрелка оси Y
    painter.drawLine(QPointF(-arrowSize/2, visibleTop - arrowSize),
                     QPointF(0, visibleTop));
    painter.drawLine(QPointF(arrowSize/2, visibleTop - arrowSize),
                     QPointF(0, visibleTop));

    painter.restore();
}

void ShapesWidget::drawGridLines(QPainter &painter) {
    if (effectiveGridStep <= 0) return;

    painter.save();

    QPen gridPen(gridColor, 0.02, Qt::SolidLine); // Тонкие линии
    painter.setPen(gridPen);

    // Рассчитываем границы видимой области в мировых координатах
    qreal halfWorldSize = worldSize / 2.0;
    qreal visibleLeft = -halfWorldSize;
    qreal visibleRight = halfWorldSize;
    qreal visibleTop = halfWorldSize;
    qreal visibleBottom = -halfWorldSize;

    // Вертикальные линии
    QVector<qreal> xPositions = getGridLinePositions(visibleLeft, visibleRight, effectiveGridStep);
    for (qreal x : xPositions) {
        // Пропускаем ось Y (x=0)
        if (qAbs(x) < 0.001 * effectiveGridStep) continue;

        painter.drawLine(QPointF(x, visibleTop), QPointF(x, visibleBottom));
    }

    // Горизонтальные линии
    QVector<qreal> yPositions = getGridLinePositions(visibleBottom, visibleTop, effectiveGridStep);
    for (qreal y : yPositions) {
        // Пропускаем ось X (y=0)
        if (qAbs(y) < 0.001 * effectiveGridStep) continue;

        painter.drawLine(QPointF(visibleLeft, y), QPointF(visibleRight, y));
    }

    painter.restore();
}
qreal ShapesWidget::calculateOptimalGridStep() const {
    // Рассчитываем оптимальный шаг сетки в мировых координатах
    // в зависимости от текущего масштаба

    // Размер виджета в пикселях
    qreal widgetSize = qMin(width(), height());

    // Масштаб: сколько сантиметров видно на виджете
    qreal visibleCm = worldSize;

    // Желаемый минимальный размер клетки в пикселях
    const qreal minCellPixels = 40.0;

    // Вычисляем оптимальный шаг сетки в сантиметрах
    qreal optimalStep = (minCellPixels * visibleCm) / widgetSize;

    // Округляем до красивых значений: 0.1, 0.2, 0.5, 1, 2, 5, 10 и т.д.
    qreal magnitude = qPow(10, qFloor(qLn(optimalStep) / qLn(10)));
    qreal normalized = optimalStep / magnitude;

    if (normalized < 0.15) {
        return 0.1 * magnitude;
    } else if (normalized < 0.35) {
        return 0.2 * magnitude;
    } else if (normalized < 0.75) {
        return 0.5 * magnitude;
    } else if (normalized < 1.5) {
        return 1.0 * magnitude;
    } else if (normalized < 3.5) {
        return 2.0 * magnitude;
    } else {
        return 5.0 * magnitude;
    }
}
QVector<qreal> ShapesWidget::getGridLinePositions(qreal min, qreal max, qreal step) const {
    QVector<qreal> positions;

    // Находим первую линию
    qreal first = qCeil(min / step) * step;

    // Добавляем все линии в диапазоне
    for (qreal pos = first; pos <= max; pos += step) {
        positions.append(pos);
    }

    return positions;
}

void ShapesWidget::drawGridLabels(QPainter &painter) {
    painter.save();

    QPen textPen(axesColor);
    painter.setPen(textPen);
    painter.setFont(coordinateFont);

    // Рассчитываем видимую область
    qreal halfWorldSize = worldSize / 2.0;
    qreal visibleLeft = -halfWorldSize;
    qreal visibleRight = halfWorldSize;
    qreal visibleTop = halfWorldSize;
    qreal visibleBottom = -halfWorldSize;

    // Получаем позиции линий сетки
    QVector<qreal> xPositions = getGridLinePositions(visibleLeft, visibleRight, effectiveGridStep);
    QVector<qreal> yPositions = getGridLinePositions(visibleBottom, visibleTop, effectiveGridStep);

    // Подписи для вертикальных линий (ось X)
    for (qreal x : xPositions) {
        // Пропускаем 0 - он рисуется отдельно
        if (qAbs(x) < 0.001 * effectiveGridStep) continue;

        QPointF screenPos = worldToScreenWithOffset(QPointF(x, 0));

        // Определяем, где рисовать текст (сверху или снизу от оси X)
        QString text = QString::number(x, 'f', effectiveGridStep < 0.1 ? 2 :
                                                   effectiveGridStep < 1 ? 1 : 0);
        QRect textRect = painter.fontMetrics().boundingRect(text);

        // Рисуем под осью X
        painter.drawText(screenPos.x() - textRect.width()/2,
                        screenPos.y() + textRect.height() + 5,
                        text);
    }

    // Подписи для горизонтальных линий (ось Y)
    for (qreal y : yPositions) {
        // Пропускаем 0 - он рисуется отдельно
        if (qAbs(y) < 0.001 * effectiveGridStep) continue;

        QPointF screenPos = worldToScreenWithOffset(QPointF(0, y));

        QString text = QString::number(y, 'f', effectiveGridStep < 0.1 ? 2 :
                                                   effectiveGridStep < 1 ? 1 : 0);
        QRect textRect = painter.fontMetrics().boundingRect(text);

        // Рисуем слева от оси Y
        painter.drawText(screenPos.x() - textRect.width() - 5,
                        screenPos.y() + textRect.height()/4,
                        text);
    }

    // Подписи осей
    QPointF originScreen = worldToScreenWithOffset(QPointF(0, 0));
    painter.drawText(originScreen.x() + 5, originScreen.y() - 5, "0");

    // Подписи X и Y в углах
    QPointF xEndScreen = worldToScreenWithOffset(QPointF(visibleRight * 0.9, 0));
    painter.drawText(xEndScreen.x() - 10, xEndScreen.y() - 10, "X");

    QPointF yEndScreen = worldToScreenWithOffset(QPointF(0, visibleTop * 0.9));
    painter.drawText(yEndScreen.x() + 10, yEndScreen.y() + 15, "Y");

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
    painter.drawText(10, 80, QString("Шаг сетки: %1 см").arg(effectiveGridStep, 0, 'f', 2));

    if (underMouse()) {
        QPoint mousePos = mapFromGlobal(QCursor::pos());
        QPointF worldPos = screenToWorldWithOffset(mousePos);
        painter.drawText(10, 100, QString("X: %1 см, Y: %2 см")
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
void ShapesWidget::updateGridParameters() {
    if (autoAdjustGrid) {
        effectiveGridStep = calculateOptimalGridStep() * gridDensity;
    } else {
        effectiveGridStep = gridStep;
    }
}
void ShapesWidget::wheelEvent(QWheelEvent *event) {
    qreal zoomFactor = 1.1;
    if (event->angleDelta().y() < 0) {
        worldSize *= zoomFactor;
    } else {
        worldSize /= zoomFactor;
    }

    worldSize = qMax(1.0, qMin(500.0, worldSize));

    updateGridParameters();
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

void ShapesWidget::addShapeWithParameters(ShapeType type,const QVector<QPointF> &points,const QColor &color,qreal thickness) {

    if (points.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Нет точек для создания фигуры");
        return;
    }
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
