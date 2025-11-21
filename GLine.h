#ifndef GLINE_H  
#define GLINE_H  

#include <QGraphicsItem>  
#include <vector> // Ensure the vector header is included  
#include "GraphicsView.h"  
#include "PlaneGCS/GCS.h"  
#include "PlaneGCS/Geo.h"  

class BaseConnect;

enum LineConnectType
{
    Not = 0,  // 没有共点
    SS = 1, // 头头相接
    SE = 2, // 头尾相接
    ES = 3, // 尾头相接
    EE = 4  // 尾尾相接
};

class GLine : public QGraphicsLineItem
{
public:
    GLine();
    ~GLine() = default;

    explicit GLine(qreal x1, qreal y1, qreal x2, qreal y2);
    explicit GLine(const QPointF& start, const QPointF& end);

    // 更新坐标并同步所有数据
    void updateLine(const QPointF& start, const QPointF& end);

    // 仅更新坐标
    void justUpdateCoordinates(const QPointF& start, const QPointF& end);

    void updateLineByInnerCorrds();
    std::vector<double*> getCoordinates();

    double* getStartX() { return &m_sx; }
    double* getStartY() { return &m_sy; }
    double* getEndX() { return &m_ex; }
    double* getEndY() { return &m_ey; }

    double* getLineAngle() { return &m_angle; }
    double* getLength() { return &m_length; }

    GCS::Point getStart();
    GCS::Point getEnd();

    void setStartConnect(BaseConnect* pConnect) { m_startConnect = pConnect; }
    void setEndConnect(BaseConnect* pConnect) { m_endConnect = pConnect; };

    BaseConnect* getStartConnect() { return m_startConnect; }
    BaseConnect* getEndConnect() { return m_endConnect; }


    int getId() const { return m_id; }

public:
    bool isParallel(GLine& line);
    LineConnectType getConnectType(GLine& line);

private:
    void calculateAngleAndLength();
    void generateId();

private:
    double m_sx = 0.0;
    double m_sy = 0.0;
    double m_ex = 0.0;
    double m_ey = 0.0;
    double m_angle = 0.0;
    double m_length = 0.0;
    GCS::Point m_start;
    GCS::Point m_end;
    BaseConnect* m_startConnect = nullptr;
    BaseConnect* m_endConnect = nullptr;

    int m_id = 0;
};

#endif