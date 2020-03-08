#include <QCoreApplication>
#include <iostream>
#include "serverdispatcher.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //creiamo un server e facciamolo partire
    Symposium::ServerDispatcher server;

    server.startServer();

    return a.exec();
}
