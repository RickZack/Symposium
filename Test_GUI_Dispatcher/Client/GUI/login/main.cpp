#include "login.h"
#include "clientdispatcher.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    Clientdispatcher cl;

    QApplication a(argc, argv);
    login w;

    cl.setlg(&w);

    w.setClientDispatcher(&cl);

    cl.sendMessage();

    qDebug() << "prima di show";

    w.show();

    qDebug() << "dopo di show";

    return a.exec();
}
