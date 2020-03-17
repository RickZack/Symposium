#include "mainwindow.h"
#include "Dispatcher/clientdispatcher.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    Symposium::clientdispatcher cl;
    w.setClientDispatcher(&cl);
    w.show();

    return a.exec();
}
