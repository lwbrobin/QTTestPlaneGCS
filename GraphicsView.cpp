#include "GraphicsView.h"
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <cmath>
#include <QScrollBar>
#include <QMessageBox>
#include <iostream>

#include "GLine.h"
#include "PlaneGCS/Geo.h"
#include <cmath>
#include "Connect.h"

double mathUtils::s_anglePre = 1e-4;

namespace
{
    void setLineConnectByType(GLine& l1, GLine& l2, BaseConnect* pConnect, LineConnectType type)
    {
        if (!pConnect)
            return;

        GCS::Point pt;
        if (type == LineConnectType::SS)
        {
            l1.setStartConnect(pConnect);
            l2.setStartConnect(pConnect);
            pt = l1.getStart();
            pConnect->addCoinPoint(l1.getStart());
            pConnect->addCoinPoint(l2.getStart());
        }
        else if (type == LineConnectType::SE)
        {
            l1.setStartConnect(pConnect);
            l2.setEndConnect(pConnect);
            pt = l1.getStart();
            pConnect->addCoinPoint(l1.getStart());
            pConnect->addCoinPoint(l2.getEnd());
        }
        else if (type == LineConnectType::ES)
        {
            l1.setEndConnect(pConnect);
            l2.setStartConnect(pConnect);
            pt = l2.getStart();
            pConnect->addCoinPoint(l1.getEnd());
            pConnect->addCoinPoint(l2.getStart());
        }
        else if (type == LineConnectType::EE)
        {
            l1.setEndConnect(pConnect);
            l2.setEndConnect(pConnect);
            pt = l1.getEnd();
            pConnect->addCoinPoint(l1.getEnd());
            pConnect->addCoinPoint(l2.getEnd());
        }

        pConnect->setPos(*pt.x, *pt.y);
    }
}



GraphicsView::GraphicsView(QWidget* parent)
    : QGraphicsView(parent), m_isDrawing(false), isPanning(false),
    drawModeEnabled(false), m_selectedLine(nullptr)
{
    setupScene();
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::RubberBandDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setFocusPolicy(Qt::StrongFocus);
    m_connectMana = ConnectManager::getInstance();
}

void setBackLine(QGraphicsLineItem* pLine)
{
    if (!pLine)
        return;
    // 设置网格线不可选择和移动
    pLine->setFlag(QGraphicsItem::ItemIsSelectable, false);
    pLine->setFlag(QGraphicsItem::ItemIsMovable, false);
}

bool GraphicsView::isDragingLine() const
{
    return m_selectedLine && !m_isDrawing;
}

void setupScene1()
{
    //m_scene = new QGraphicsScene(this);
    //m_scene->setSceneRect(-1000, -1000, 2000, 2000);
    //setScene(m_scene);

    //// 添加网格背景
    //for (int i = -1000; i <= 1000; i += 100) {
    //    auto pl1 = m_scene->addLine(i, -1000, i, 1000, QPen(QColor(50, 200, 200, 100)));
    //    auto pl2 = m_scene->addLine(-1000, i, 1000, i, QPen(QColor(50, 200, 200, 100)));
    //    setBackLine(pl1);
    //    setBackLine(pl2);
    //}

    ////添加坐标轴
    //auto pl1 = m_scene->addLine(-1000, 0, 1000, 0, QPen(Qt::lightGray, 2));
    //auto pl2 = m_scene->addLine(0, -1000, 0, 1000, QPen(Qt::lightGray, 2));
    //setBackLine(pl1);
    //setBackLine(pl2);
}

void GraphicsView::setupScene()
{
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(-1000, -1000, 2000, 2000);
    setScene(m_scene);

    // 创建坐标标签的字体
    QFont coordinateFont;
    coordinateFont.setPointSize(8);
    coordinateFont.setFamily("Arial");

    // 绘制网格线和坐标值
    for (int i = -1000; i <= 1000; i += 100) {
        // 绘制网格线
        m_scene->addLine(i, -1000, i, 1000, QPen(QColor(50, 200, 200, 100)));
        m_scene->addLine(-1000, i, 1000, i, QPen(QColor(50, 200, 200, 100)));

        // 只在主要刻度上显示坐标值（-1000, -500, 0, 500, 1000）
        if (i % 50 == 0) {
            // X轴坐标值（显示在底部）
            QGraphicsTextItem* xLabel = m_scene->addText(QString::number(i));
            xLabel->setFont(coordinateFont);
            xLabel->setDefaultTextColor(QColor(80, 80, 80));
            xLabel->setPos(i - 15, 8);

            // Y轴坐标值（显示在左侧）
            QGraphicsTextItem* yLabel = m_scene->addText(QString::number(i));
            yLabel->setFont(coordinateFont);
            yLabel->setDefaultTextColor(QColor(80, 80, 80));
            yLabel->setPos(-26, i - 8);
        }
    }

    // 添加粗体的主要坐标轴
    m_scene->addLine(-1000, 0, 1000, 0, QPen(Qt::lightGray, 2));
    m_scene->addLine(0, -1000, 0, 1000, QPen(Qt::lightGray, 2));

    // 添加原点特殊标记
    QGraphicsEllipseItem* originDot = m_scene->addEllipse(-3, -3, 6, 6,
        QPen(Qt::red, 1), QBrush(Qt::red));

    // 添加坐标轴箭头
    // X轴箭头
    QPolygonF xArrow;
    xArrow << QPointF(1000, 0) << QPointF(980, -10) << QPointF(980, 10);
    m_scene->addPolygon(xArrow, QPen(Qt::black), QBrush(Qt::black));

    // Y轴箭头
    QPolygonF yArrow;
    yArrow << QPointF(0, 1000) << QPointF(-10, 980) << QPointF(10, 980);
    m_scene->addPolygon(yArrow, QPen(Qt::black), QBrush(Qt::black));

    // 添加坐标轴标签
    QGraphicsTextItem* xAxisLabel = m_scene->addText("X axis");
    xAxisLabel->setPos(920, 40);
    xAxisLabel->setDefaultTextColor(Qt::darkBlue);

    QGraphicsTextItem* yAxisLabel = m_scene->addText("Y axis");
    yAxisLabel->setRotation(-90);
    yAxisLabel->setPos(-60, -920);
    yAxisLabel->setDefaultTextColor(Qt::darkBlue);
}


void GraphicsView::setDrawMode(bool enabled)
{
    drawModeEnabled = enabled;
    if (enabled) {
        setCursor(Qt::CrossCursor);
        setDragMode(QGraphicsView::NoDrag);
        clearSelection();
        setFocus();
    }
    else {
        setCursor(Qt::ArrowCursor);
        setDragMode(QGraphicsView::RubberBandDrag);
    }
}

void GraphicsView::mousePressEvent(QMouseEvent* event)
{
    QPointF scenePos = mapToScene(event->pos());

    if (event->button() == Qt::LeftButton) {
        if (drawModeEnabled) {
            // 绘制模式下开始绘制新直线
            m_isDrawing = true;
            firstPoint = scenePos;
            m_tempLine = new GLine(firstPoint, firstPoint);
            m_tempLine->setPen(QPen(Qt::blue, 2));
            m_scene->addItem(m_tempLine);
        }
        else {
            // 非绘制模式下选择现有直线
            //QGraphicsItem* item = m_scene->itemAt(scenePos, transform());  // 用点，很难拾取
            // 方法1：创建一个小矩形区域来检测附近的直线
            QRectF detectRect(scenePos.x() - 10, scenePos.y() - 10, 20, 20);
            QList<QGraphicsItem*> items = m_scene->items(detectRect);
            // 方法2：使用自定义的直线检测函数
            if (0)
            {
                QGraphicsLineItem* lineItem = findNearestLine(scenePos, 15.0);
            }

            if (!items.empty())
            {
                if (GLine* lineItem = dynamic_cast<GLine*>(items.front()))
                {
                    selectLine(lineItem);
                    //initSolveSystem();
                    m_oriPos = scenePos;
                }
            }
            else {
                clearSelection();
            }
        }
    }
    else if (event->button() == Qt::RightButton) {
        // 开始平移视图
        isPanning = true;
        lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }

    QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    if (m_isDrawing && m_tempLine) {
        QPointF scenePos = mapToScene(event->pos());
        m_tempLine->setLine(QLineF(firstPoint, scenePos));
    }
    else if (isDragingLine())    // 直线拖动过程中更新坐标显示
    {
        auto p1 = m_selectedLine->line().p1();
        auto p2 = m_selectedLine->line().p2();
        auto scep1 = m_selectedLine->mapToScene(p1);
        auto scep2 = m_selectedLine->mapToScene(p2);
        emit lineSelected(scep1, scep2);
    }
    else if (isPanning) {
        QPointF delta = event->pos() - lastMousePos;
        lastMousePos = event->pos();
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
    }

    QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    QPointF scenePos = mapToScene(event->pos());
    if (event->button() == Qt::LeftButton) {

        // 直线拖动完成后更新坐标显示
        if (isDragingLine())
        {
            auto tmpPos = scenePos;
            if (tmpPos == m_oriPos)
                return;
            auto move = tmpPos - m_oriPos;
            updateSystemByVec2(move.x(), move.y());
            auto p1 = m_selectedLine->line().p1();
            auto p2 = m_selectedLine->line().p2();
            auto scep1 = m_selectedLine->mapToScene(p1);
            auto scep2 = m_selectedLine->mapToScene(p2);

            emit lineSelected(scep1, scep2);

        }

        if (m_isDrawing && m_tempLine) {
            m_isDrawing = false;
            QPointF secondPoint = mapToScene(event->pos());

            // 创建最终的直线
            GLine* finalLine = new GLine(firstPoint, secondPoint);
            finalLine->setPen(QPen(Qt::black, 2));
            m_scene->removeItem(m_tempLine);
            m_scene->addItem(finalLine);
            delete m_tempLine;
            m_tempLine = nullptr;

            selectLine(finalLine);

            // 绘制完成后自动退出绘制模式
            exitDrawMode();
        }

    }
    else if (event->button() == Qt::RightButton && isPanning) {
        isPanning = false;
        setCursor(drawModeEnabled ? Qt::CrossCursor : Qt::ArrowCursor);
    }

    QGraphicsView::mouseReleaseEvent(event);
}

void GraphicsView::wheelEvent(QWheelEvent* event)
{
    // 缩放视图
    double scaleFactor = 1.15;
    if (event->angleDelta().y() > 0) {
        scale(scaleFactor, scaleFactor);
    }
    else {
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
}

void GraphicsView::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape && drawModeEnabled) {
        exitDrawMode();
        event->accept();
        return;
    }

    QGraphicsView::keyPressEvent(event);
}

void GraphicsView::addLineByCoordinates(const QPointF& start, const QPointF& end)
{
    clearSelection();
    //QGraphicsLineItem* line = m_scene->addLine(QLineF(start, end), QPen(Qt::black, 2));
    auto newLine = new GLine(start, end);
    m_scene->addItem(newLine);
    selectLine(newLine);

    createConnectByNewLine(newLine); // 创建接头
    m_allLines.insert(newLine);
}

void GraphicsView::deleteSelectedLine()
{
    if (m_selectedLine) {
        m_scene->removeItem(m_selectedLine);
        delete m_selectedLine;
        m_selectedLine = nullptr;
        emit lineDeselected();
    }
    else // 多选时
    {
        auto items = m_scene->selectedItems();
        for (QGraphicsItem* item : items)
        {
            if (GLine* lineItem = dynamic_cast<GLine*>(item))
            {
                m_scene->removeItem(lineItem);
                delete lineItem;
            }
        }
        emit lineDeselected();
    }
}

void GraphicsView::clearAll()
{
    for (auto& line : m_allLines)
    {
        m_scene->removeItem(line);
        delete line;
    }
    m_allLines.clear();
    m_selectedLine = nullptr;
    emit lineDeselected();
    m_connectMana->clearConnect();
}

void GraphicsView::updateSelectedLine(const QPointF& newStart, const QPointF& newEnd)
{
    // 先建立约束
    //initSolveSystem();

    // 更新参数
    auto s = m_selectedLine->mapFromScene(newStart.x(), newStart.y());
    auto e = m_selectedLine->mapFromScene(newEnd.x(), newEnd.y());
    updateSystemByLine(s, e);
    // 重新选中以更新显示（保持红色高亮）
    selectLine(m_selectedLine);
}

void GraphicsView::clearSelection()
{
    if (m_selectedLine) {
        m_selectedLine->setPen(QPen(Qt::black, 2));
        m_selectedLine->setFlag(QGraphicsItem::ItemIsMovable, false);
        m_selectedLine = nullptr;
        emit lineDeselected();
    }
}

void GraphicsView::selectLine(GLine* line)
{
    clearSelection();
    auto p1 = line->line().p1();
    auto p2 = line->line().p2();
    m_selectedLine = line;
    m_selectedLine->setPen(QPen(Qt::red, 3));
    m_selectedLine->setFlag(QGraphicsItem::ItemIsMovable, true);
    m_selectedLine->setFlag(QGraphicsItem::ItemIsSelectable, true);

    auto scep1 = m_selectedLine->mapToScene(p1);
    auto scep2 = m_selectedLine->mapToScene(p2);
    emit lineSelected(scep1, scep2);
}

void GraphicsView::exitDrawMode()
{
    drawModeEnabled = false;
    setCursor(Qt::ArrowCursor);
    setDragMode(QGraphicsView::RubberBandDrag);

    // 通知主窗口更新UI状态
    emit lineDeselected();
}

void GraphicsView::createConnectByNewLine(GLine* newLine)
{
    auto start = newLine->getStart();
    auto end = newLine->getEnd();

    std::vector<std::unique_ptr<BaseConnect>> newConnects;
    std::set<BaseConnect*> connectToRemove;
    bool startHasConnect = false;
    bool endHasConnect = false;

    std::set<GLine*> connectedLines;  // 通过接头已经连接的线（后续不需要再创建接头）
    // 1、检查所有接头，现在只考虑双接头变T接头
    for (auto& connect : m_connectMana->getAllConnects())
    {
        if (!connect)
            continue;

        double cx(0.0), cy(0.0);
        connect->getPos(cx, cy);

        bool startEqual = mathUtils::isEqual(cx, *start.x) && mathUtils::isEqual(cy, *start.y);
        bool endEqual = mathUtils::isEqual(cx, *end.x) && mathUtils::isEqual(cy, *end.y);
        if (startEqual || endEqual)// 起点或终点接头
        {
            if (auto simpleC = dynamic_cast<SimpleConnect*>(connect.get()))
            {
                auto newTc = std::make_unique<TConnect>(simpleC->getLine1(), simpleC->getLine2(), newLine);
                if (newTc->isValid())
                {
                    auto oldL1 = simpleC->getLine1();
                    auto oldL2 = simpleC->getLine2();
                    connectedLines.insert(oldL1);
                    connectedLines.insert(oldL2);
                    auto connectType1 = oldL1->getConnectType(*newLine);
                    setLineConnectByType(*oldL1, *newLine, newTc.get(), connectType1);
                    auto connectType2 = oldL2->getConnectType(*newLine);
                    setLineConnectByType(*oldL2, *newLine, newTc.get(), connectType2);

                    connectToRemove.insert(simpleC);
                    newConnects.emplace_back(std::move(newTc));
                    if (startEqual)
                        startHasConnect = true;
                    if (endEqual)
                        endHasConnect = true;
                }
            }
        }
    }

    if (startHasConnect && endHasConnect)  // 都创建了接头，不用继续检查
        return;

    // 2、检查与所有线的连接情况 ： 现只考虑创建双接头
    for (auto line : m_allLines)
    {
        if (!line)
            continue;
        if (connectedLines.find(line) != connectedLines.end())
            continue;

        auto connectType = line->getConnectType(*newLine);
        if (connectType == LineConnectType::Not)
            continue;

        auto newSc = std::make_unique<SimpleConnect>(line, newLine);
        setLineConnectByType(*line, *newLine, newSc.get(), connectType);
        newConnects.emplace_back(std::move(newSc));
    }

    for (auto connect : connectToRemove)
    {
        m_connectMana->removeConnect(connect);
    }
    for (auto& upConnect : newConnects)
    {
        m_connectMana->addConnect(std::move(upConnect));
    }

}



// 查找最近的直线
QGraphicsLineItem* GraphicsView::findNearestLine(const QPointF& point, double tolerance)
{
    QGraphicsLineItem* nearestLine = nullptr;
    double minDistance = tolerance;

    // 获取场景中的所有直线项
    QList<QGraphicsItem*> items = m_scene->items();
    for (QGraphicsItem* item : items) {
        if (QGraphicsLineItem* lineItem = dynamic_cast<QGraphicsLineItem*>(item)) {
            // 排除临时绘制线
            if (lineItem == m_tempLine) continue;

            // 计算点到直线的距离
            double distance = distanceToLine(point, lineItem->line());

            if (distance < minDistance) {
                minDistance = distance;
                nearestLine = lineItem;
            }
        }

        return nearestLine;
    }
    return nullptr;
}

// ：计算点到直线的距离
double GraphicsView::distanceToLine(const QPointF& point, const QLineF& line)
{
    // 使用数学公式计算点到直线的最短距离
    QPointF p1 = line.p1();
    QPointF p2 = line.p2();

    double numerator = std::abs((p2.y() - p1.y()) * point.x() -
        (p2.x() - p1.x()) * point.y() +
        p2.x() * p1.y() - p2.y() * p1.x());
    double denominator = std::sqrt(std::pow(p2.y() - p1.y(), 2) +
        std::pow(p2.x() - p1.x(), 2));

    return denominator > 0 ? numerator / denominator : std::numeric_limits<double>::max();
}
bool GraphicsView::isPointEqual(const QPointF& qPoint, const GCS::Point& gPoint)const
{
    if (!mathUtils::isEqual(qPoint.x(), *gPoint.x))
        return false;
    if (!mathUtils::isEqual(qPoint.y(), *gPoint.y))
        return false;
    return true;
}


// 加入共点约束，线角度约束
void GraphicsView::initSolveSystem()
{
    return;
    enum StOrEd
    {
        start = 0,
        end = 1
    };
    m_solveSys.clear();
    m_unknowParams.clear();

    std::map<QPointF, std::pair<int, StOrEd>>  pointMaps;
    std::vector<GCS::Point> allPoints;
    auto tryAddP2PCoincidentConstraint = [&allPoints, this](GCS::Point& pt) // 判读是否需要添加共点约束
        {
            bool findSame = false;
            for (auto& tmp : allPoints)
            {
                if (mathUtils::isEqual(tmp, pt))
                {
                    // 是公共顶点（坐标重合）
                    m_solveSys.addConstraintP2PCoincident(tmp, pt, /*tagId=*/0, /*driving=*/false);
                    findSame = true;
                    break;
                }
            }
            if (!findSame)
                allPoints.emplace_back(pt);
        };

    //QList<QGraphicsItem*> items = m_scene->items();
    // 1、保持所有线角度不变
    for (GLine* lineItem : m_allLines)
    {
        //m_allLines.insert(lineItem);
        auto st = lineItem->getStart();
        auto ed = lineItem->getEnd();
        //tryAddP2PCoincidentConstraint(st);
        //tryAddP2PCoincidentConstraint(ed);

        m_solveSys.addConstraintP2PAngle(st, ed, lineItem->getLineAngle(), /*incrAngle=*/0.0, /*tagId=*/0, /*driving=*/false);
    }

    // 2、加入所有共点约束
    for (auto& pConnect : m_connectMana->getAllConnects())
    {
        if (!pConnect)
            continue;

        auto pts = pConnect->getCoinPoints();
        if (pts.size() < 2)
            continue;

        for (int i = 1; i < pts.size(); i++)
            m_solveSys.addConstraintP2PCoincident(pts.front(), pts[i], /*tagId=*/0, /*driving=*/false);

    }
}

void GraphicsView::getDrivenParams(const QPointF& newStart, const QPointF& newEnd, std::vector<double*>& drivenParams)
{
    if (!m_selectedLine)
        return;

    auto dealPoint = [&drivenParams, this](const QPointF& qPoint, const GCS::Point& gPoint)
        {
            if (isPointEqual(qPoint, gPoint)) // 没变加入unknowParams
            {
                m_unknowParams.emplace_back(gPoint.x);
                m_unknowParams.emplace_back(gPoint.y);
            }
            else  // 有变化则加入drivenParams, 并更新参数
            {
                drivenParams.emplace_back(gPoint.x);
                drivenParams.emplace_back(gPoint.y);
                *gPoint.x = qPoint.x();
                *gPoint.y = qPoint.y();
            }

        };

    dealPoint(newStart, m_selectedLine->getStart());
    dealPoint(newEnd, m_selectedLine->getEnd());
}

void GraphicsView::update(GLine* pLine, bool start, bool isFirstLine)
{
    if (!pLine)
        return;

    // 肯定会增加两个未知量（线另一端的xy），同时需要两个约束，1个约束为线定向，另一个约束分情况添加
    // 两种情况：1、另一端无连接，则约束为线定长；2、另一端有连接，则
    // ？line的另一端参数是否应直接加入未知，不需要后面判断（）

    // 传播端的未知参数
    double* nx = nullptr;
    double* ny = nullptr;
    if (start)
    {
        nx = pLine->getStartX();
        ny = pLine->getStartY();
    }
    else
    {
        nx = pLine->getEndX();
        ny = pLine->getEndY();
    }
    m_unknowP.insert(nx);
    m_unknowP.insert(ny);

    // 1、处理当前线的约束与未知参数
    // 定向
    auto st = pLine->getStart();
    auto ed = pLine->getEnd();
    m_solveSys.addConstraintP2PAngle(st, ed, pLine->getLineAngle(), /*incrAngle=*/0.0, /*tagId=*/0, /*driving=*/false);

    auto lenP = pLine->getLength();
    auto startP = pLine->getStart();
    auto endP = pLine->getEnd();
    auto otherEndPt = start ? startP : endP;

    BaseConnect* pConnect = nullptr;
    auto sConnect = pLine->getStartConnect();
    auto eConnect = pLine->getEndConnect();
    if (start && sConnect)
        pConnect = sConnect;
    else if (!start && eConnect)
        pConnect = eConnect;

    if (!pConnect) // 如果传播端无连接，则定长,且返回
    {
        m_solveSys.addConstraintP2PDistance(startP, endP, lenP, /*tagId=*/0, /*driving=*/false);
        return;
    }

    // 处理下一个连接
    // 连接点加入共点约束，且所有参数加入未知（后续是否可优化——不加约束，最后直接相等）
    auto pts = pConnect->getCoinPoints();
    if (pts.size() < 2)
        return;

    for (int i = 1; i < pts.size(); i++)
        m_solveSys.addConstraintP2PCoincident(pts.front(), pts[i], /*tagId=*/0, /*driving=*/false);
    for (auto& p : pts)
    {
        m_unknowP.insert(p.x);
        m_unknowP.insert(p.y);
    }

    // 必须对当前线再加一个约束（上面已有一个定向约束）
    if (auto pSimpleC = dynamic_cast<SimpleConnect*>(pConnect))// 双接头
    {
        auto nextLine = pSimpleC->getLine1();
        if (pLine == nextLine)
        {
            nextLine = pSimpleC->getLine2();
        }
        if (!nextLine)
            return;

        if (pSimpleC->isStraight()) // 平接头:定长并传导
        {
            m_solveSys.addConstraintP2PDistance(startP, endP, lenP, /*tagId=*/0, /*driving=*/false);
            bool isStartConnect = (nextLine->getStartConnect() == pConnect);
            update(nextLine, !isStartConnect); // 平接头需要向平接线的另一头传播
        }
        else // 弯接头
        {
            //m_solveSys.addConstraintPointOnLine(otherEndPt, nextLine->getStart(), nextLine->getEnd(), 0, false); // 不确定是否可以这样用
            m_solveSys.addConstraintP2PAngle(nextLine->getStart(), nextLine->getEnd(), nextLine->getLineAngle(), /*incrAngle=*/0.0, /*tagId=*/0, /*driving=*/false);

        }
    }
    else if (auto pTc = dynamic_cast<TConnect*>(pConnect))  // T接头
    {
        auto tL1 = pTc->getLine1();
        auto tL2 = pTc->getLine2();
        auto tTl = pTc->getTLine();

        GLine* nextLine = nullptr;
        if (pLine == tL1) // 拖的是平接线
        {
            nextLine = tL2;
        }
        else if (pLine == tL2)
        {
            nextLine = tL1;
        }

        if (nextLine)// 拖的是平接线，定长，传导
        {
            m_solveSys.addConstraintP2PDistance(startP, endP, lenP, /*tagId=*/0, /*driving=*/false);

            bool isStartConnect = (nextLine->getStartConnect() == pConnect);
            update(nextLine, !isStartConnect);
            update(tTl, tTl->getStartConnect() != pConnect);
        }
        else // 拖的t线，点在平接线上（平接线位置不动）
        {
            //m_solveSys.addConstraintPointOnLine(otherEndPt, tL1->getStart(), tL1->getEnd(), 0, false); // 不确定是否可以这样用
            m_solveSys.addConstraintP2PAngle(tL1->getStart(), tL1->getEnd(), tL1->getLineAngle(), /*incrAngle=*/0.0, /*tagId=*/0, /*driving=*/false);

        }
    }
    else if (auto pCrossC = dynamic_cast<CrossConnect*>(pConnect))
    {

    }
}

void GraphicsView::updateSystemByVec2(double x, double y)
{
    QPointF scenePos = m_oriPos + QPointF(x, y);
    m_solveSys.clear();
    m_unknowP.clear();
    if (!m_selectedLine)
        return;
    QPointF tmpPos = m_selectedLine->pos();
    auto posx = scenePos.x();
    auto posy = scenePos.y();

    m_selectedLine->setPos(QPointF(0, 0));
    auto sconnect = m_selectedLine->getStartConnect();
    auto econnect = m_selectedLine->getEndConnect();
    if (!sconnect && !econnect) // 独立线,不需要约束求解
    {
        *m_selectedLine->getStartX() += x;
        *m_selectedLine->getStartY() += y;
        *m_selectedLine->getEndX() += x;
        *m_selectedLine->getEndY() += y;
        m_selectedLine->updateLineByInnerCorrds();
        return;
    }

    std::vector<double*> drivenParams;
    double* px = nullptr;
    double* py = nullptr;
    GCS::Point mousePt(&posx, &posy);
    if (sconnect && econnect) // 如果双头连接，分别递归传导
    {
        // 在鼠标处加入虚拟点把m_selectedLine分成两段，分别传导:太难做到
        // todo:
        //drivenParams.emplace_back(&posx);
        //drivenParams.emplace_back(&posy);
        m_solveSys.addConstraintPointOnLine(mousePt, m_selectedLine->getStart(), m_selectedLine->getEnd(), 0, true);

        update(m_selectedLine, true, true);
        update(m_selectedLine, false, true);
    }
    else if (sconnect) // 只有起点接头：终点偏移后作为驱动参数
    {
        *m_selectedLine->getEndX() += x;
        *m_selectedLine->getEndY() += y;
        drivenParams.emplace_back(m_selectedLine->getEndX());
        drivenParams.emplace_back(m_selectedLine->getEndY());
        update(m_selectedLine, true, true);

    }
    else if (econnect) // 只有终点接头：起点偏移后作为驱动参数
    {
        *m_selectedLine->getStartX() += x;
        *m_selectedLine->getStartY() += y;
        drivenParams.emplace_back(m_selectedLine->getStartX());
        drivenParams.emplace_back(m_selectedLine->getStartY());
        update(m_selectedLine, false, true);
    }

    std::vector<double*> vecUnknowParams(m_unknowP.begin(), m_unknowP.end());
    m_solveSys.declareUnknowns(vecUnknowParams);
    m_solveSys.declareDrivenParams(drivenParams);
    m_solveSys.initSolution();

    // 约束求解
    int result = m_solveSys.solve();
    if (result != GCS::Success && result != GCS::Converged)
    {
        QMessageBox::warning(this, "Solve wrong", "Please check!");
        std::cerr << "Solve after moving A returned code: " << result << std::endl;
        return;
    }

    // 重新赋值并同步线数据
    m_solveSys.applySolution(); // 线内部坐标重新赋值
    for (GLine* gLine : m_allLines)
    {
        if (!gLine)
            continue;
        gLine->updateLineByInnerCorrds();
    }
}


void GraphicsView::updateSystemByVec(double x, double y)
{
    if (!m_selectedLine)
        return;

    auto scenePos = m_selectedLine->scenePos();

    auto sconnect = m_selectedLine->getStartConnect();
    auto econnect = m_selectedLine->getEndConnect();

    // 处理选中线：变化的点作为驱动参数更新，没变的点作为未知参数参与求解
    std::vector<double*> drivenParams;
    auto lineParam = m_selectedLine->getCoordinates();
    *m_selectedLine->getStartX() += x;
    *m_selectedLine->getStartY() += y;
    *m_selectedLine->getEndX() += x;
    *m_selectedLine->getEndY() += y;
    m_selectedLine->setPos(QPointF(0, 0));

    if (bool test = true)  // 测试两线相连时，拖动一线的情况
    {
        std::vector<double*> unkownParams;
        auto lenP = m_selectedLine->getLength();
        auto startP = m_selectedLine->getStart();
        auto endP = m_selectedLine->getEnd();
        m_solveSys.addConstraintP2PDistance(startP, endP, lenP, /*tagId=*/0, /*driving=*/false);
        //m_solveSys.addConstraintP2PDistance(startP, endP, lenP, /*tagId=*/0, /*driving=*/false);

        // 把共点约束的其它线端点参数也加入unkownParams
        if (sconnect)
        {
            auto pts = sconnect->getCoinPoints();
            for (auto& p : pts)
            {
                if (mathUtils::isEqualCompletely(p, startP) || mathUtils::isEqualCompletely(p, endP))
                    continue;
                lineParam.emplace_back(p.x);
                lineParam.emplace_back(p.y);
            }
        }
        if (econnect)
        {
            auto pts = econnect->getCoinPoints();
            for (auto& p : pts)
            {
                if (mathUtils::isEqualCompletely(p, startP) || mathUtils::isEqualCompletely(p, endP))
                    continue;
                lineParam.emplace_back(p.x);
                lineParam.emplace_back(p.y);
            }

        }
        m_solveSys.declareUnknowns(lineParam);
        m_solveSys.initSolution();
    }
    else
    {
        // 声名未知变量与驱动变量
        m_solveSys.declareUnknowns(m_unknowParams);
        m_solveSys.declareDrivenParams(lineParam);
        m_solveSys.initSolution();
    }

    // 约束求解
    int result = m_solveSys.solve();
    if (result != GCS::Success && result != GCS::Converged)
    {
        QMessageBox::warning(this, "Solve wrong", "Please check!");
        std::cerr << "Solve after moving A returned code: " << result << std::endl;
        return;
    }

    // 重新赋值并同步线数据
    m_solveSys.applySolution(); // 线内部坐标重新赋值
    for (GLine* gLine : m_allLines)
    {
        if (!gLine)
            continue;
        gLine->updateLineByInnerCorrds();
    }

}

void GraphicsView::updateSystemByLine(const QPointF& newStart, const QPointF& newEnd)
{
    if (!m_selectedLine)
        return;

    // 处理选中线：变化的点作为驱动参数更新，没变的点作为未知参数参与求解
    std::vector<double*> drivenParams;
    getDrivenParams(newStart, newEnd, drivenParams);

    // 声名未知变量与驱动变量
    m_solveSys.declareUnknowns(m_unknowParams);
    m_solveSys.declareDrivenParams(drivenParams);
    m_solveSys.initSolution();

    // 约束求解
    int result = m_solveSys.solve();
    if (result != GCS::Success && result != GCS::Converged)
    {
        QMessageBox::warning(this, "Solve wrong", "Please check!");
        std::cerr << "Solve after moving A returned code: " << result << std::endl;
        return;
    }

    // 重新赋值并同步线数据
    m_solveSys.applySolution(); // 线内部坐标重新赋值
    for (GLine* gLine : m_allLines)
    {
        if (!gLine)
            continue;
        gLine->updateLineByInnerCorrds();
    }
}

bool mathUtils::isEqual(double x, double y, double tol)
{
    return fabs(x - y) < tol;
}
bool mathUtils::isZero(double value, double tol)
{
    return fabs(value) <= tol;
}
bool mathUtils::isGreater(double v1, double v2, double tol)
{
    return (v1 - v2) > tol;
}
bool mathUtils::isGreaterEqual(double v1, double v2, double tol)
{
    return isGreater(v1, v2, tol) || isEqual(v1, v2, tol);
}
bool mathUtils::isLess(double v1, double v2, double tol)
{
    return (v2 - v1) > tol;
}
bool mathUtils::isLessEqual(double v1, double v2, double tol)
{
    return isLess(v1, v2, tol) || isEqual(v1, v2, tol);
}
bool mathUtils::isEqual(const GCS::Point& p1, const GCS::Point& p2, double tol)
{
    if (!mathUtils::isEqual(*p1.x, *p2.x))
        return false;
    if (!mathUtils::isEqual(*p1.y, *p2.y))
        return false;
    return true;
}

bool mathUtils::isEqualCompletely(const GCS::Point& p1, const GCS::Point& p2)
{
    if (p1.x == p2.x && p1.y == p2.y)
        return true;

    return false;
}
