#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QPointF>
#include "PlaneGCS/GCS.h"
#include <unordered_set>

namespace GCS
{
    class system;
}

class GLine;
class ConnectManager;
class BaseConnect;

class GraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit GraphicsView(QWidget* parent = nullptr);
    void setDrawMode(bool enabled);
    void addLineByCoordinates(const QPointF& start, const QPointF& end);
    void deleteSelectedLine();
    bool hasSelectedLine() const { return m_selectedLine != nullptr; }
    void updateSelectedLine(const QPointF& newStart, const QPointF& newEnd);

    void clearAll();

signals:
    void lineSelected(const QPointF& start, const QPointF& end);
    void lineDeselected();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    bool isDragingLine() const;
    void setupScene();
    void clearSelection();
    void selectLine(GLine* line);
    void exitDrawMode();
    QGraphicsLineItem* findNearestLine(const QPointF& point, double tolerance);
    double distanceToLine(const QPointF& point, const QLineF& line);
    bool isPointEqual(const QPointF& qPoint, const GCS::Point& gPoint) const;

    void createConnectByNewLine(GLine* newLine);

    //////// 更新系统
    void initSolveSystem();
    void getDrivenParams(const QPointF& newStart, const QPointF& newEnd, std::vector<double*>& drivenParams);
    void updateSystemByVec(double x, double y);
    void updateSystemByVec2(double x, double y);
    void updateSystemByLine(const QPointF& newStart, const QPointF& newEnd);

    // 单向递归传播——添加约束及收集参数
    // start表示向前传播还是向后传播
    void update(GLine* pLine, bool start, bool isFirstLine = false);


private:
    QGraphicsScene* m_scene;
    GLine* m_tempLine;
    QPointF firstPoint;
    QPointF lastMousePos;
    bool m_isDrawing;
    bool isPanning;
    bool drawModeEnabled;
    GLine* m_selectedLine;
    QPointF m_oriPos;
    std::set<GLine*> m_allLines;

    ConnectManager* m_connectMana = nullptr;

    /// <summary>
    /// //求解器相关
    /// </summary>
    GCS::VEC_pD m_unknowParams;
    std::unordered_set<double*> m_unknowP;
    GCS::System m_solveSys;
};

class mathUtils
{
public:
    static bool isEqual(double x, double y, double tol = 1e-8);
    static bool isZero(double value, double tol = 1e-8);
    static bool isGreater(double v1, double v2, double tol = 1e-8);
    static bool isGreaterEqual(double v1, double v2, double tol = 1e-8);
    static bool isLess(double v1, double v2, double tol = 1e-8);
    static bool isLessEqual(double v1, double v2, double tol = 1e-8);


    ////////
    static bool isEqualCompletely(const GCS::Point& p1, const GCS::Point& p2); // 参数地址相同

    static bool isEqual(const GCS::Point& p1, const GCS::Point& p2, double tol = 1e-8);
public:
    static double s_anglePre;
};
#endif
