#include "login.h"
#include "clientdispatcher.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Clientdispatcher cl;
    login w;

    cl.setlg(&w);

    w.setClientDispatcher(&cl);

    w.show();

    return a.exec();
}
