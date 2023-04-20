
#include "mainwindow.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("L3 Report Generator");
    MainWindow w;
    w.setWindowTitle("L3 Report Generator");
    w.setWindowState(Qt::WindowMaximized);
    w.show();
    return a.exec();
}
