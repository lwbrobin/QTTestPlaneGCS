#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include "GraphicsView.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);

private slots:
    void onDrawModeButtonClicked();
    void onDrawButtonClicked();
    void onUpdateButtonClicked();
    void onDeleteButtonClicked();
    void onLineSelected(const QPointF& start, const QPointF& end);
    void onLineDeselected();
    void onLoadFileButtonClicked();  // 文件加载按钮点击事件


private:
    void setupUI();
    void connectSignals();

    GraphicsView* m_graphicsView;
    QLineEdit* startXEdit;
    QLineEdit* startYEdit;
    QLineEdit* endXEdit;
    QLineEdit* endYEdit;
    QPushButton* drawModeButton;
    QPushButton* drawButton;
    QPushButton* updateButton;
    QPushButton* deleteButton;
    QPushButton* m_loadFileButton;     // 文件加载按钮
    QPushButton* m_clearButton;     // 清空场景按钮

    QLineEdit* m_editStartXEdit;  // 编辑起点X坐标
    QLineEdit* m_editStartYEdit;  // 编辑起点Y坐标
    QLineEdit* m_editEndXEdit;    // 编辑终点X坐标
    QLineEdit* m_editEndYEdit;    // 编辑终点Y坐标
};
#endif