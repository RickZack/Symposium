//#include "mainwindow.h"
#include "Dispatcher/clientdispatcher.h"
//#include <QApplication>
#include "notepad.h"

int main(int argc, char *argv[])
{
    //QApplication a(argc, argv);
    Symposium::clientdispatcher cl;
    cl.run(argc,argv);

    //MainWindow w;
    //w.setClientDispatcher(cl);
    //w.show();
}
