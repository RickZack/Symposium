#include "clientdispatcher.h"

using namespace Symposium;

Clientdispatcher::Clientdispatcher(QObject *parent) : QObject(parent)
{
    //connettiamo il socket all'indirizzo del server
    this->socket.connectToHost(svAddress, svPort);
    //quando riceviamo qualcosa eseguiamo la funzione di lettura (readyRead)
    connect(&(this->socket), &QIODevice::readyRead, this, &Clientdispatcher::readyRead);
    qDebug() << "Connection Successful\n";
}

void Clientdispatcher::sendMessage(){
    QTextStream out(&(this->socket));
    //inviamo un messaggio
    out << "ciao server";
}

void Clientdispatcher::readyRead(){
    //questa funzione viene chiamata quando arriva qualcosa dal server
    //associamo il textstream al socket
    QTextStream stream(&(this->socket));
    //leggiamo quanto ricevuto
    QString line = stream.readAll();
    qDebug() << "Il server risponde: " << line << "\n";
}
