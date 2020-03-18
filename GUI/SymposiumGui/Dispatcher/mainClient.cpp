#include <iostream>

#include "serverdispatcher.h"
#include "clientdispatcher.h"



int main(int argc, char *argv[]) {

    QCoreApplication a(argc, argv);

        //creiamo un server e facciamolo partire
        //Symposium::ServerDispatcher server;


        //server.startServer();

        Symposium::clientdispatcher client;

        return a.exec();
}
