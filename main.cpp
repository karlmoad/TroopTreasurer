#include <QApplication>
#include <QMainWindow>

/* Application entry point */
int main(int argc, char*argv[]) {
    QApplication a(argc, argv);
    QMainWindow w;
    w.show();
    a.setActiveWindow(&w);
    return a.exec();
}
