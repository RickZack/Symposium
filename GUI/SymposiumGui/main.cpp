#include "mainwindow.h"
#include "Dispatcher/clientdispatcher.h"
#include <QApplication>
#include "notepad.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    Symposium::clientdispatcher *cl= new Symposium::clientdispatcher();
    w.setClientDispatcher(cl);
    w.show();
    //notepad *notepadWindow = new notepad(nullptr, 2, Symposium::privilege::owner, Symposium::privilege::owner, "");
    //notepadWindow->show();

    int ret=a.exec();
    delete cl;
    return ret;
}
