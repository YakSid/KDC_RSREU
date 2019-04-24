#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowFlag(Qt::WindowMinimizeButtonHint);
    w.show();

    return a.exec();
}
