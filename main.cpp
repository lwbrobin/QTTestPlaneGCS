#include <QApplication>
#include "MainWindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.setWindowTitle("2D Line Editor");
    window.resize(1400, 1000);
    window.show();

    return app.exec();
}