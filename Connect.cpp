#include "Connect.h"
#include "Gline.h"
#include "GraphicsView.h"

void BaseConnect::getPos(double& x, double& y) const
{
    x = m_x;
    y = m_y;

}
void BaseConnect::setPos(double x, double y)
{
    m_x = x;
    m_y = y;
}
void BaseConnect::addCoinPoint(const GCS::Point& pt)
{
    for (auto& tmp : m_coinPoints)
    {
        if (mathUtils::isEqualCompletely(tmp, pt))
            return;
    }

    m_coinPoints.emplace_back(pt);
}

bool SimpleConnect::isStraight() const
{
    // todo:

    return mathUtils::isEqual(fabs(*m_line1->getLineAngle()), fabs(*m_line2->getLineAngle()), mathUtils::s_anglePre);
}

bool SimpleConnect::isValid() const
{
    if (!m_line1 || !m_line2)
        return false;

    return true;
}

TConnect::TConnect(GLine* line1, GLine* line2, GLine* line3)
{
    if (line1->isParallel(*line2))
    {
        m_line1 = line1;
        m_line2 = line2;
        m_tLine = line3;
    }
    else if (line1->isParallel(*line3))
    {
        m_line1 = line1;
        m_line2 = line3;
        m_tLine = line2;
    }
    else if (line2->isParallel(*line3))
    {
        m_line1 = line2;
        m_line2 = line3;
        m_tLine = line1;
    }
}

bool TConnect::isValid() const
{
    if (!m_line1 || !m_line2 || !m_tLine)
        return false;

    return mathUtils::isEqual(fabs(*m_line1->getLineAngle()), fabs(*m_line2->getLineAngle()), mathUtils::s_anglePre);
}

bool CrossConnect::isValid() const
{
    if (!m_line1 || !m_line2 || !m_line3 || m_line4)
        return false;

    bool straight1 = mathUtils::isEqual(*m_line1->getLineAngle(), *m_line2->getLineAngle());
    bool straight2 = mathUtils::isEqual(*m_line3->getLineAngle(), *m_line4->getLineAngle());
    return straight1 && straight2;
}


ConnectManager* ConnectManager::getInstance()
{
    static ConnectManager instance;
    return &instance;
}

ConnectManager::~ConnectManager()
{
}
void ConnectManager::addConnect(std::unique_ptr<BaseConnect> pConnect)
{
    m_connects.emplace_back(std::move(pConnect));
}

void ConnectManager::removeConnect(BaseConnect* pConnect)
{
    auto it = std::find_if(m_connects.begin(), m_connects.end(), [pConnect](const std::unique_ptr<BaseConnect>& up) {return up.get() == pConnect; });
    if (it != m_connects.end())
        m_connects.erase(it);
}

void ConnectManager::clearConnect()
{
    m_connects.clear();
}

