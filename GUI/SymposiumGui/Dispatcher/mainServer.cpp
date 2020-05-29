#include <iostream>
#include <QApplication>
#include "gui_symserver.h"

#include "serverdispatcher.h"



int main(int argc, char *argv[]) {

    QApplication a(argc, argv);

    GUI_SymServer w(nullptr);
    w.show();
    //creiamo un server e facciamolo partire
    //Symposium::ServerDispatcher server;

    //server.startServer();

    return a.exec();
}
