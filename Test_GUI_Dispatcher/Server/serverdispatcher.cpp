#include "serverdispatcher.h"

using namespace Symposium;

ServerDispatcher::ServerDispatcher()
{
}

void ServerDispatcher::startServer()
{
    int port = 1234;

    if(!this->listen(QHostAddress::Any, port))
    {
        qDebug() << "Could not start server";
    }
    else
    {
        qDebug() << "Listening to port " << port << "...";
    }
}

void ServerDispatcher::incomingConnection(qintptr socketDescriptor){
    //abbiamo una nuova connessione
    qDebug() << socketDescriptor << " New Connection";

    //creiamo il socket per la nuova connessione
    QTcpSocket* temp = new QTcpSocket();

    //attacchiamo il socketdescriptor
    temp->setSocketDescriptor(socketDescriptor);

    //inseriamo la nuova connessione nella coda dei client connessi
    Connected_Clients.insert(socketDescriptor, temp);

    //quando un socket connesso ad un client riceve dei dati, mi emette il segnale di readyRead, che mi chiama lo slot readyRead()
    connect(Connected_Clients.value(socketDescriptor), SIGNAL(readyRead()), this, SLOT(readyRead()));

    //mettiamo in temp il socket appena creato
    this->tmp = temp;

}

void ServerDispatcher::readyRead(){
    qDebug() << "Entrato ";
    //questa funzione viene chiamata quando un qualche client ha inviato qualcosa
    //otteniamo il socket che ha ricevuto i dati
    QTcpSocket* readSocket = qobject_cast<QTcpSocket*>(sender());
    //associamo il textstream al socket ha ricevuto dei dati
    QTextStream stream(readSocket);
    //leggiamo quanto ricevuto
    QString line = stream.readAll();
    qDebug() << "socketdescriptor - " << readSocket->socketDescriptor() << ": " << line << "\n";

    if(line == "mario mario"){
        this->sendMessage("OK");
    }else{
        this->sendMessage("KO");
    }

    //inviamo la risposta
    //this->sendMessage();
}

void ServerDispatcher::sendMessage(QString s){
    QTextStream out(this->tmp);
    //inviamo un messaggio
    out << s;
}
