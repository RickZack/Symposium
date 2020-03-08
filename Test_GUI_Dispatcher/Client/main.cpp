#include <QCoreApplication>
#include <iostream>
#include "clientdispatcher.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Symposium::Clientdispatcher client;

    qDebug() << "clientdispatcher creato\n";

    client.sendMessage();

    qDebug() << "Messaggio inviato\n";

    return a.exec();
}
