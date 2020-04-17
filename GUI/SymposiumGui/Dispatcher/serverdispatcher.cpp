/*
 * Copyright (c) 2019 Riccardo Zaccone, Ksenia Del Conte Akimova, Cristian Gianetto, Martina Bellissimo
 *
 * This file is part of Symposium.
 * Symposium is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Symposium is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Symposium.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * File:   serverdispatcher.cpp
 * Project: Symposium
 * Authors:
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Cristian Gianetto <s265387 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 17 Dicembre 2019, 13.07
 */

#include "serverdispatcher.h"


using namespace Symposium;


ServerDispatcher::ServerDispatcher(){
    //settiamo i booleani entrambe a false per non effettuare il salvataggio degli utente e il loro recupero
    this->server = SymServer(false,false);
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
    //qDebug() << socketDescriptor << " New Connection";

    //creiamo il socket per la nuova connessione
    QTcpSocket* temp = new QTcpSocket();

    //impostiamo il keep_alive
    temp->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

    //attacchiamo il socketdescriptor
    temp->setSocketDescriptor(socketDescriptor);

    //inseriamo la nuova connessione nella coda dei client connessi
    Connected_Clients.insert(socketDescriptor, temp);

    //quando un socket connesso ad un client riceve dei dati, mi emette il segnale di readyRead, che mi chiama lo slot readyRead()
    connect(Connected_Clients.value(socketDescriptor), SIGNAL(readyRead()), this, SLOT(readyRead()));

    //quando il socket si sconnette, chiamiamo il metodo clientDisconnected()
    connect(Connected_Clients.value(socketDescriptor), SIGNAL(disconnected()), this, SLOT(clientDisconnected()));

    //settiamo il nome del socket con il socketdescriptor
    Connected_Clients.value(socketDescriptor)->setObjectName(QString::number(socketDescriptor) + "-0" );

    //notifichiamo
    qDebug() << "New connection insert into Connected_Clients. SocketDescriptor: " << socketDescriptor;
}

void ServerDispatcher::readyRead(){
    //questa funzione viene chiamata quando un qualche client ha inviato qualcosa
    //stream di stringa che conterrà i dati che abbiamo ricevuto, prima di essere de-serializzati
    std::stringstream accumulo;
    //creiamo il QByteArray che conterrà ciò che riceviamo dal socket
    QByteArray byteArray;
    //creiamo la variabile che conterra il puntatore al messaggio ricevuto dal socket
    std::shared_ptr<clientMessage> da;
    //otteniamo il socket che ha ricevuto i dati
    QTcpSocket* readSocket = qobject_cast<QTcpSocket*>(sender());
    //associamo il datastream al socket che ha ricevuto dei dati
    QDataStream in(readSocket);
    //facciamo partire la transazione
    in.startTransaction();
    //leggiamo i dati ricevuti
    in >> byteArray;
    //controlliamo se ci sono stati errori
    if (!in.commitTransaction()){
        //errore di ricezione
    }
    //eseguiamo la conversione in stringa
    std::string ricevuto(byteArray.constData(), byteArray.length());
    //inseriamo quanto ricevuto nel stringstream
    accumulo << ricevuto;
    //deserializziamo il messaggio ricevuto
    boost::archive::text_iarchive ia(accumulo);
    ia >> da;
    try {
        da->invokeMethod(this->server);
        if((da->getAction()==msgType::login) || (da->getAction()==msgType::registration)){
            Connected_SymUser.insert(this->server.getSiteIdOfUser(da->getActionOwner().first), readSocket->socketDescriptor());
            qDebug() << "User inserted in Connected_SymUser. Users in queue: " << Connected_SymUser.size();
            //aggiungiamo al nome del socket il siteID dell'utente
            readSocket->setObjectName(QString::number(readSocket->socketDescriptor()) + "-" + QString::number(this->server.getSiteIdOfUser(da->getActionOwner().first)));
        }
        controlMessageQueue();
    } catch (SymposiumException& e) {
        qDebug() << "Action from socketdescriptor " <<  readSocket->socketDescriptor() << " FAILED. Send notification error.";
        //costruiamo il messaggio di fallimento
        std::shared_ptr<serverMessage> er(new serverMessage(da->getAction(), msgOutcome::failure, da->getMsgId()));
        er->setErrDescr(e.getErrorCodeMsg());
        //inviamo il messaggio
        try {
            sendMessage(er,readSocket);
            qDebug() << "Message error sended to socketdescriptor: " << readSocket->socketDescriptor() ;
        } catch (sendFailure) {
            //impossibile inviare

            // COSA FACCIAMO??
        }
    }

}

void ServerDispatcher::clientDisconnected(){
    qDebug() << "--- METODO CLIENTDISCONNECTED ---";
    QString nome_socket;
    QStringList ls;
    int sok_desc, stID;
    //otteniamo il socket che si è disconnesso
    QTcpSocket* readSocket = qobject_cast<QTcpSocket*>(sender());
    //preleviamo il suo nome
    nome_socket = readSocket->objectName();
    qDebug() << "socketdescriptor " << nome_socket<< " disconnected!";
    //eseguiamo lo split
    ls = nome_socket.split("-");
    //separiamo il socketdescriptor
    sok_desc = ls[0].toUInt();
    //ed il siteID dell'utente associato al socket
    stID = ls[1].toUInt();
    //dobbiamo controllare se era loggato oppure no
    if(stID != 0){
        //utente loggato, quindi eseguiamo l'hard logout
        this->server.hardLogout(stID);
        //eliminiamo l'utente dalla mappa Connected_SymUser
        this->Connected_SymUser.remove(stID);
        qDebug() << "Logged User DELETED from Symposium online users";
    }
    //eliminiamo l'utente dalla mappa Connected_Clients
    this->Connected_Clients.remove(sok_desc);

    qDebug() << "Socket DELETED from connected clients";
    qDebug() << "Symposium users online: " << this->Connected_SymUser.size();
    qDebug() << "Connected clients: " << this->Connected_Clients.size();
    qDebug() << "--- FINE METODO CLIENTDISCONNECTED ---";
}

void ServerDispatcher::logoutUser(uint_positive_cnt::type siteID){
    qDebug() << "--- METODO LOGOUTUSER ---";
    QMap<uint_positive_cnt::type, int>::iterator it = this->Connected_SymUser.find(siteID);
    int socdsc = it.value();
    this->Connected_SymUser.remove(siteID);
    qDebug() << "Logged User DELETED from Symposium online users";
    QMap<int, QTcpSocket*>::iterator cu = this->Connected_Clients.find(socdsc);
    cu.value()->setObjectName(QString::number(cu.value()->socketDescriptor()) + "-0");
    qDebug() << "Symposium users online: " << this->Connected_SymUser.size();
    qDebug() << "Connected clients: " << this->Connected_Clients.size();
    qDebug() << "--- FINE METODO LOGOUTUSER ---";
}

void ServerDispatcher::sendMessage(const std::shared_ptr<serverMessage> MessageToSend, uint_positive_cnt::type siteID){
    std::stringstream ofs;
    boost::archive::text_oarchive oa(ofs);
    QMap<int, QTcpSocket*>::iterator cu;
    QMap<uint_positive_cnt::type, int>::iterator it;
    //recuperiamo il socketdescriptor associato al siteID
    it = Connected_SymUser.find(siteID);
    if(it != this->Connected_SymUser.end()){
        //recuperiamo il QTcpSocket associato al socketdescriptor
        cu = Connected_Clients.find(it.value());
    }else{
        //utente non collegato

        //CHE FACCIAMO????
    }
    QDataStream uscita(cu.value());
    //serializziamo il messaggio
    oa << MessageToSend;
    //eseguiamo la conversione in QByteArray
    QByteArray byteArray(ofs.str().c_str(), ofs.str().length());
    //inviamo il messaggio
    uscita << byteArray;
    if (uscita.status() != QDataStream::Ok){
        throw sendFailure();
    }
    qDebug() << "Sended to socketdescriptor " << cu.value()->socketDescriptor() << ": " << QString::fromStdString(ofs.str());

    //se il messaggio inviato è una conferma di logout oppure una conferma eliminazione utente, dobbiamo togliere l'utente dalla lista degli utenti connessi
    if((MessageToSend->getAction()==msgType::logout) || ((MessageToSend->getAction()==msgType::removeUser) & (MessageToSend->getResult()==msgOutcome::success))){
        qDebug() << "Logout Logged user...";
        logoutUser(siteID);
    }

}

void ServerDispatcher::sendMessage(const std::shared_ptr<serverMessage> MessageToSend, QTcpSocket* socket){
    std::stringstream ofs;
    boost::archive::text_oarchive oa(ofs);
    QDataStream uscita(socket);
    //serializziamo il messaggio
    oa << MessageToSend;
    //eseguiamo la conversione in QByteArray
    QByteArray byteArray(ofs.str().c_str(), ofs.str().length());
    //inviamo il messaggio
    uscita << byteArray;
    if (uscita.status() != QDataStream::Ok){
        throw sendFailure();
    }
    qDebug() << "Sended to socketdescriptor " << socket->socketDescriptor() << ": " << QString::fromStdString(ofs.str());
}

void ServerDispatcher::controlMessageQueue(){
    qDebug() << "Control message to send...";
    std::pair<int, std::shared_ptr<serverMessage>> mes = this->server.extractNextMessage();
    while(mes.first!=0){
        sendMessage(mes.second,mes.first);
        qDebug() << "Message to send to user with siteID: " << mes.first;
        mes = this->server.extractNextMessage();
    }
}
