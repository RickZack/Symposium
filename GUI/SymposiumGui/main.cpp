#include "mainwindow.h"
#include "Dispatcher/clientdispatcher.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    Symposium::clientdispatcher cl;
    //cl.connectionClient();
    w.setClientDispatcher(&cl);
    w.show();

    return a.exec();
}
