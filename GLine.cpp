#include "GLine.h"
#include "GraphicsView.h"

int Global_MaxId = 1;

GLine::GLine() :QGraphicsLineItem()
{
    generateId();

}

GLine::GLine(qreal x1, qreal y1, qreal x2, qreal y2) :QGraphicsLineItem(x1, y1, x2, y2), m_start(&m_sx, &m_sy), m_end(&m_ex, &m_ey)
{
    m_sx = x1;
    m_sy = y1;
    m_ex = x2;
    m_ey = y2;
    generateId();
    calculateAngleAndLength();

}

GLine::GLine(const QPointF& start, const QPointF& end) :QGraphicsLineItem(start.x(), start.y(), end.x(), end.y()), m_start(&m_sx, &m_sy), m_end(&m_ex, &m_ey)
{
    m_sx = start.x();
    m_sy = start.y();
    m_ex = end.x();
    m_ey = end.y();
    generateId();
    calculateAngleAndLength();

}

void GLine::updateLine(const QPointF& start, const QPointF& end)
{
    m_sx = start.x();
    m_sy = start.y();
    m_ex = end.x();
    m_ey = end.y();
    calculateAngleAndLength();
    setLine(QLineF(start, end));
}

void GLine::justUpdateCoordinates(const QPointF& start, const QPointF& end)
{
    m_sx = start.x();
    m_sy = start.y();
    m_ex = end.x();
    m_ey = end.y();
}

void GLine::updateLineByInnerCorrds()
{
    calculateAngleAndLength();
    setLine(m_sx, m_sy, m_ex, m_ey);
}


std::vector<double*> GLine::getCoordinates()
{
    std::vector<double*> result{ &m_sx,&m_sy,&m_ex, &m_ey };
    return result;
}

GCS::Point GLine::getStart()
{
    return GCS::Point(&m_sx, &m_sy);
}

GCS::Point GLine::getEnd()
{
    return GCS::Point(&m_ex, &m_ey);
}

bool GLine::isParallel(GLine& line)
{
    return mathUtils::isEqual(fabs(*getLineAngle()), fabs(*line.getLineAngle()), mathUtils::s_anglePre);
}

LineConnectType GLine::getConnectType(GLine& line)
{
    auto start2 = line.getStart();
    auto end2 = line.getEnd();
    if (mathUtils::isEqual(getStart(), start2))
        return LineConnectType::SS;
    if (mathUtils::isEqual(getStart(), end2))
        return LineConnectType::SE;
    if (mathUtils::isEqual(getEnd(), start2))
        return LineConnectType::ES;
    if (mathUtils::isEqual(getEnd(), end2))
        return LineConnectType::EE;

    return LineConnectType::Not;
}


void GLine::calculateAngleAndLength()
{
    m_angle = atan2(m_ey - m_sy, m_ex - m_sx);
    m_length = hypot(m_ex - m_sx, m_ey - m_sy);
}

void GLine::generateId()
{
    m_id = Global_MaxId++;
}
