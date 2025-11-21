#include "MainWindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUI();
    connectSignals();
}

void MainWindow::setupUI()
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);

    // 创建图形视图
    m_graphicsView = new GraphicsView(this);
    mainLayout->addWidget(m_graphicsView, 1);

    // 创建控制面板
    QWidget* controlPanel = new QWidget(this);
    controlPanel->setFixedWidth(300);
    QVBoxLayout* panelLayout = new QVBoxLayout(controlPanel);

    // 绘制模式控制区域
    QGroupBox* modeGroup = new QGroupBox("绘制模式控制", this);
    QVBoxLayout* modeLayout = new QVBoxLayout(modeGroup);

    drawModeButton = new QPushButton("进入绘制模式", this);
    drawModeButton->setStyleSheet("QPushButton { background-color: #FF9800; color: white; padding: 10px; font-weight: bold; }");

    QLabel* modeStatus = new QLabel("当前模式: 选择模式", this);
    modeStatus->setObjectName("modeStatusLabel");

    modeLayout->addWidget(drawModeButton);
    modeLayout->addWidget(modeStatus);

    // 文件批量加载区域
    QGroupBox* fileGroup = new QGroupBox("加载文件批量绘制", this);
    QVBoxLayout* fileLayout = new QVBoxLayout(fileGroup);

    m_loadFileButton = new QPushButton("选择文件加载坐标", this);
    m_loadFileButton->setStyleSheet("QPushButton { background-color: #9C27B0; color: white; padding: 8px; }");

    QLabel* fileHelp = new QLabel("支持格式：每行四个数字，分别表示起点X、起点Y、终点X、终点Y", this);
    fileHelp->setWordWrap(true);

    fileLayout->addWidget(m_loadFileButton);
    fileLayout->addWidget(fileHelp);

    // 绘制新线区域
    QGroupBox* drawGroup = new QGroupBox("坐标输入绘制", this);
    QVBoxLayout* drawLayout = new QVBoxLayout(drawGroup);

    QLabel* startLabel = new QLabel("起点坐标:", this);
    QHBoxLayout* startLayout = new QHBoxLayout();
    startXEdit = new QLineEdit(this);
    startYEdit = new QLineEdit(this);
    startXEdit->setPlaceholderText("X坐标");
    startYEdit->setPlaceholderText("Y坐标");
    startLayout->addWidget(startXEdit);
    startLayout->addWidget(startYEdit);

    QLabel* endLabel = new QLabel("终点坐标:", this);
    QHBoxLayout* endLayout = new QHBoxLayout();
    endXEdit = new QLineEdit(this);
    endYEdit = new QLineEdit(this);
    endXEdit->setPlaceholderText("X坐标");
    endYEdit->setPlaceholderText("Y坐标");
    endLayout->addWidget(endXEdit);
    endLayout->addWidget(endYEdit);

    drawButton = new QPushButton("通过坐标绘制直线", this);
    drawButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 8px; }");

    drawLayout->addWidget(startLabel);
    drawLayout->addLayout(startLayout);
    drawLayout->addWidget(endLabel);
    drawLayout->addLayout(endLayout);
    drawLayout->addWidget(drawButton);

    // 编辑选中线区域
    QGroupBox* editGroup = new QGroupBox("edit line", this);
    QVBoxLayout* editLayout = new QVBoxLayout(editGroup);

    QLabel* editStartLabel = new QLabel("起点坐标:", this);
    QHBoxLayout* editStartLayout = new QHBoxLayout();
    m_editStartXEdit = new QLineEdit(this);
    m_editStartYEdit = new QLineEdit(this);
    m_editStartXEdit->setPlaceholderText("X坐标");
    m_editStartYEdit->setPlaceholderText("Y坐标");
    editStartLayout->addWidget(m_editStartXEdit);
    editStartLayout->addWidget(m_editStartYEdit);

    QLabel* editEndLabel = new QLabel("终点坐标:", this);
    QHBoxLayout* editEndLayout = new QHBoxLayout();
    m_editEndXEdit = new QLineEdit(this);
    m_editEndYEdit = new QLineEdit(this);
    m_editEndXEdit->setPlaceholderText("X坐标");
    m_editEndYEdit->setPlaceholderText("Y坐标");
    editEndLayout->addWidget(m_editEndXEdit);
    editEndLayout->addWidget(m_editEndYEdit);

    updateButton = new QPushButton("更新直线", this);
    updateButton->setStyleSheet("QPushButton { background-color: #2196F3; color: white; padding: 8px; }");
    updateButton->setEnabled(false);

    deleteButton = new QPushButton("删除直线", this);
    deleteButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 8px; }");

    m_clearButton = new QPushButton("清空场景", this);
    m_clearButton->setStyleSheet("QPushButton { background-color: #122233; color: white; padding: 8px; }");

    editLayout->addWidget(editStartLabel);
    editLayout->addLayout(editStartLayout);
    editLayout->addWidget(editEndLabel);
    editLayout->addLayout(editEndLayout);
    editLayout->addWidget(updateButton);
    editLayout->addWidget(deleteButton);
    editLayout->addWidget(m_clearButton);

    // 操作说明
    QGroupBox* helpGroup = new QGroupBox("操作说明", this);
    QVBoxLayout* helpLayout = new QVBoxLayout(helpGroup);
    QLabel* helpText = new QLabel(
        "-绘制模式: 点击按钮进入绘制模式\n"
        "-鼠标绘制: 在绘制模式下点击两点\n"
        "-坐标输入: 在左侧输入坐标绘制\n"
        "-选择直线: 点击直线进行选择\n"
        "-移动直线: 拖动选中的直线\n"
        "-缩放视图: 鼠标滚轮\n"
        "-平移视图: 按住右键拖动\n"
        "-退出绘制: 按ESC键或完成绘制", this);
    helpText->setWordWrap(true);
    helpLayout->addWidget(helpText);

    panelLayout->addWidget(modeGroup);
    panelLayout->addWidget(fileGroup);      // 文件加载区域
    panelLayout->addWidget(drawGroup);
    panelLayout->addWidget(editGroup);
    panelLayout->addWidget(helpGroup);
    panelLayout->addStretch();

    mainLayout->addWidget(controlPanel);
}

void MainWindow::connectSignals()
{
    connect(drawModeButton, &QPushButton::clicked, this, &MainWindow::onDrawModeButtonClicked);
    connect(drawButton, &QPushButton::clicked, this, &MainWindow::onDrawButtonClicked);
    connect(updateButton, &QPushButton::clicked, this, &MainWindow::onUpdateButtonClicked);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteButtonClicked);
    connect(m_graphicsView, &GraphicsView::lineSelected, this, &MainWindow::onLineSelected);
    connect(m_graphicsView, &GraphicsView::lineDeselected, this, &MainWindow::onLineDeselected);
    connect(m_loadFileButton, &QPushButton::clicked, this, &MainWindow::onLoadFileButtonClicked);
    connect(m_clearButton, &QPushButton::clicked, this, [this]() {m_graphicsView->clearAll(); });
}

void MainWindow::onLoadFileButtonClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "选择坐标文件",
        "",
        "文本文件 (*.txt);;所有文件 (*)");

    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "wrong file", "can't open file:" + fileName);
        return;
    }

    QTextStream in(&file);
    int lineCount = 0;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList coordinates = line.split(' ', QString::SkipEmptyParts);
        if (coordinates.size() == 4) {
            bool ok1, ok2, ok3, ok4;
            double startX = coordinates[0].toDouble(&ok1);
            double startY = coordinates[1].toDouble(&ok2);
            double endX = coordinates[2].toDouble(&ok3);
            double endY = coordinates[3].toDouble(&ok4);

            if (ok1 && ok2 && ok3 && ok4) {
                m_graphicsView->addLineByCoordinates(QPointF(startX, startY), QPointF(endX, endY));
                lineCount++;
            }
        }
    }

    file.close();

    if (lineCount > 0) {
        QMessageBox::information(this, "load successful",
            QString("success load %1 lines").arg(lineCount));
    }
    else {
        QMessageBox::warning(this, "fail load",
            "wrong file");
    }
}


void MainWindow::onDrawModeButtonClicked()
{
    m_graphicsView->setDrawMode(true);
    drawModeButton->setText("exit draw mode");
    drawModeButton->setStyleSheet("QPushButton { background-color: #795548; color: white; padding: 10px; font-weight: bold; }");

    QLabel* statusLabel = findChild<QLabel*>("modeStatusLabel");
    if (statusLabel) {
        statusLabel->setText("当前模式: 绘制模式");
    }
}

void MainWindow::onDrawButtonClicked()
{
    bool ok1, ok2, ok3, ok4;
    double startX = startXEdit->text().toDouble(&ok1);
    double startY = startYEdit->text().toDouble(&ok2);
    double endX = endXEdit->text().toDouble(&ok3);
    double endY = endYEdit->text().toDouble(&ok4);

    if (ok1 && ok2 && ok3 && ok4) {
        m_graphicsView->addLineByCoordinates(QPointF(startX, startY), QPointF(endX, endY));
        startXEdit->clear();
        startYEdit->clear();
        endXEdit->clear();
        endYEdit->clear();
    }
    else {
        QMessageBox::warning(this, "wront input", "Please input valid value");
    }
}

void MainWindow::onUpdateButtonClicked()
{
    // 检查是否有选中的直线
    if (!m_graphicsView->hasSelectedLine()) {
        QMessageBox::warning(this, "warning", "please select a line");
        return;
    }

    // 验证输入坐标的有效性
    bool ok1, ok2, ok3, ok4;
    double startX = m_editStartXEdit->text().toDouble(&ok1);
    double startY = m_editStartYEdit->text().toDouble(&ok2);
    double endX = m_editEndXEdit->text().toDouble(&ok3);
    double endY = m_editEndYEdit->text().toDouble(&ok4);

    if (ok1 && ok2 && ok3 && ok4) {
        // 调用GraphicsView的更新函数
        m_graphicsView->updateSelectedLine(QPointF(startX, startY), QPointF(endX, endY));

        // 可选：清空编辑框
        // editStartXEdit->clear();
        // editStartYEdit->clear();
        // editEndXEdit->clear();
        // editEndYEdit->clear();
    }
    else {
        QMessageBox::warning(this, "wrong input", "please input valid num");
    }
}

void MainWindow::onDeleteButtonClicked()
{
    m_graphicsView->deleteSelectedLine();
}

void MainWindow::onLineSelected(const QPointF& start, const QPointF& end)
{
    // 更新编辑框中的坐标显示
    m_editStartXEdit->setText(QString::number(start.x()));
    m_editStartYEdit->setText(QString::number(start.y()));
    m_editEndXEdit->setText(QString::number(end.x()));
    m_editEndYEdit->setText(QString::number(end.y()));

    updateButton->setEnabled(true);
}

void MainWindow::onLineDeselected()
{
    updateButton->setEnabled(false);
}
