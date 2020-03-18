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


ServerDispatcher::ServerDispatcher() : temp(msgType::login,{"a","a"}){
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

}

void ServerDispatcher::readyRead(){
    //questa funzione viene chiamata quando un qualche client ha inviato qualcosa
    //stream di stringa che conterrà i dati che abbiamo ricevuto, prima di essere de-serializzati
    std::stringstream accumulo;
    uri c;
    //boost::archive::text_iarchive ia(accumulo);
    qDebug() << "\n" << "boost creato\n";
    //creiamo la variabile che conterrà il messaggio che abbiamo ricevuto dal client
    std::shared_ptr<clientMessage> mes;
    //otteniamo il socket che ha ricevuto i dati
    QTcpSocket* readSocket = qobject_cast<QTcpSocket*>(sender());
    //associamo il textstream al socket ha ricevuto dei dati
    QTextStream stream(readSocket);
    //leggiamo la prima linea di dati ricevuti
    QString line = stream.readLine();
    //la salviamo nello stream
    accumulo << line.toLocal8Bit().constData();
    //leggiamo tutti i dati ricevuti e ricostruiamo lo stream che è stato inviato dal client
    do{
        qDebug() << "socketdescriptor - " << readSocket->socketDescriptor() << ": " << line;
        line = stream.readLine();
        if(!line.isNull())
            accumulo << "\n" << line.toLocal8Bit().constData();
    }while(!line.isNull());
    //nella variabile accumulo abbiamo lo stream inviato dal client
    qDebug() << "\n" << "ricevuto: " << QString::fromStdString(accumulo.str());
    boost::archive::text_iarchive ia(accumulo);
    ia >> c;
    //mes->invokeMethod(this->server);
}

/*void ServerDispatcher::sendMessage(const std::shared_ptr<serverMessage> MessageToSend, int siteID){
    
}*/
