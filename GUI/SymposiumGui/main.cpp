#include "mainwindow.h"
#include "Dispatcher/clientdispatcher.h"
#include <QApplication>
#include "notepad.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFontDatabase database;
    database.addApplicationFont(":/resources/font/baskvil.TTF");
    MainWindow w;
    Symposium::clientdispatcher *cl= new Symposium::clientdispatcher();
    w.setClientDispatcher(cl);
    w.show();
    notepad notepadWindow(nullptr, 2, Symposium::privilege::owner, Symposium::privilege::owner, "");
    notepadWindow.show();
    notepadWindow.showLabels();

    int ret=a.exec();
    delete cl;
    return ret;
}
