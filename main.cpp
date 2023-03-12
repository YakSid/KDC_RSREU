#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator translator;
    QString path = QDir::currentPath() + "/translation";
    translator.load("qt_ru", path);
    a.installTranslator(&translator);
    MainWindow w;
    w.show();

    return a.exec();
}
