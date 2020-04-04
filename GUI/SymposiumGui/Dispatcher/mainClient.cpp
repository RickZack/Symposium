#include <iostream>

#include "clientdispatcher.h"



int main(int argc, char *argv[]) {

    QCoreApplication a(argc, argv);

    Symposium::clientdispatcher client;
    //client.logIn("mario","mario");

    client.signUp("","","","");

    return a.exec();
}
