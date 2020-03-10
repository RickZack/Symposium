#include "clientdispatcher.h"

Clientdispatcher::Clientdispatcher(QObject *parent) : QObject(parent)
{
    //connettiamo il socket all'indirizzo del server
    this->socket.connectToHost(svAddress, svPort);
    //quando riceviamo qualcosa eseguiamo la funzione di lettura (readyRead)
    connect(&(this->socket), &QIODevice::readyRead, this, &Clientdispatcher::readyRead);
    qDebug() << "Connection Successful\n";
}

void Clientdispatcher::sendMessage(QString s){
    qDebug() << "Entrato " << s;
    QTextStream out(&(this->socket));
    //inviamo un messaggio
    out << s;
    out.flush();
    qDebug() << "dopo flush";
    if (out.status() != QTextStream::Ok)
        qDebug() << "Errore";
}

void Clientdispatcher::sendMessage(){
    QTextStream out(&(this->socket));
    //inviamo un messaggio
    out << "test";
}

void Clientdispatcher::readyRead(){
    //questa funzione viene chiamata quando arriva qualcosa dal server
    //associamo il textstream al socket
    QTextStream stream(&(this->socket));
    //leggiamo quanto ricevuto
    QString line = stream.readAll();
    qDebug() << "Il server risponde: " << line << "\n";
    if(line == "OK"){
        this->lg->successLogin();
    }else{
        this->lg->errorLogin();
    }
}

void Clientdispatcher::setlg(login* lg){
    this->lg = lg;
}

void Clientdispatcher::logIn(const QString &username, const QString &pwd) {
    /*std::shared_ptr<clientMessage> mess = std::make_shared<clientMessage>(this->client.logIn(username,pwd));
    try {
        sendMessage(mess);
        //facciamo partire il timer

    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio

    }*/
    this->sendMessage(username + " " + pwd);
}
