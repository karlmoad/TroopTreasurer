#include <QApplication>
#include "ui/mainwindow.h"

/* Application entry point */
int main(int argc, char*argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    a.setActiveWindow(&w);
    return a.exec();
}
