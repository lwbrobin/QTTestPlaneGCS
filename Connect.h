#ifndef GCONNECT_H  
#define GCONNECT_H  

#include <vector> // Ensure the vector header is included  
#include "PlaneGCS/GCS.h"  
#include "PlaneGCS/Geo.h"  

class GLine;

class BaseConnect
{
public:
    BaseConnect() {};
    virtual ~BaseConnect() = default;

public:
    void getPos(double& x, double& y) const;
    void setPos(double x, double y);

    virtual bool isValid() const = 0;
    std::vector<GCS::Point>& getCoinPoints() {
        return m_coinPoints;
    }
    void addCoinPoint(const GCS::Point& pt);
    void setCoinPoints(std::vector<GCS::Point>& points) {
        m_coinPoints = points;
    }

private:
    double m_x = 0.0;
    double m_y = 0.0;
    std::vector<GCS::Point> m_coinPoints; // GLine上共点的点集
};

// 普通双管接头
class SimpleConnect : public BaseConnect
{
public:
    SimpleConnect(GLine* line1, GLine* line2) : m_line1(line1), m_line2(line2) {};
    ~SimpleConnect() = default;

    virtual bool isValid() const override;

    bool isStraight() const;
    GLine* getLine1() { return m_line1; }
    GLine* getLine2() { return m_line2; }



private:
    GLine* m_line1 = nullptr;  // 平接线1
    GLine* m_line2 = nullptr;  // 平接线2
};



// T接头（只支持其中两条线共线的T接头）
class TConnect : public BaseConnect
{
public:
    TConnect(GLine* line1, GLine* line2, GLine* line3);
    ~TConnect() = default;

    // 是否是有效的T接头
    virtual bool isValid() const override;
    GLine* getLine1() { return m_line1; }
    GLine* getLine2() { return m_line2; }
    GLine* getTLine() { return m_tLine; }

private:
    GLine* m_line1 = nullptr;  // 平接线1
    GLine* m_line2 = nullptr;  // 平接线2
    GLine* m_tLine = nullptr;  // T接线

};

// 十接头
class CrossConnect : public BaseConnect
{
public:
    CrossConnect(GLine* line1, GLine* line2, GLine* line3, GLine* line4) :
        m_line1(line1), m_line2(line2), m_line3(line3), m_line4(line4) {
    };
    ~CrossConnect() = default;

    virtual bool isValid() const override;
    GLine* getLine1() { return m_line1; }
    GLine* getLine2() { return m_line2; }
    GLine* getLine3() { return m_line3; }
    GLine* getLine4() { return m_line4; }

private:
    // 1,2构成一路
    GLine* m_line1 = nullptr;  // 平接线1
    GLine* m_line2 = nullptr;  // 平接线2
    // 3，4构成一路
    GLine* m_line3 = nullptr;  // 平接线3
    GLine* m_line4 = nullptr;  // 平接线4
};



class ConnectManager
{
public:
    static ConnectManager* getInstance();

    std::vector<std::unique_ptr<BaseConnect>>& getAllConnects() {
        return m_connects;
    }

    void addConnect(std::unique_ptr<BaseConnect> pConnect);
    void removeConnect(BaseConnect* pConnect);
    void clearConnect();
private:
    ConnectManager() = default;
    ~ConnectManager();
private:

    std::vector<std::unique_ptr<BaseConnect>> m_connects;

};


#endif