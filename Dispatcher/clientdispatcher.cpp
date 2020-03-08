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
 * Created on 9 Febbraio 2020, 16.23
 */

#include "clientdispatcher.h"

using namespace Symposium;

clientdispatcher::clientdispatcher(QObject *parent) : QObject(parent)
{
    //connettiamo il socket all'indirizzo del server
    this->socket.connectToHost(svAddress, svPort);
    //quando riceviamo qualcosa eseguiamo la funzione di lettura (readyRead)
    connect(&(this->socket), &QIODevice::readyRead, this, &clientdispatcher::readyRead);
    qDebug() << "Connection Successful\n";
}

void clientdispatcher::readyRead(){
    //questa funzione viene chiamata quando il server ci ha inviato qualcosa
    //stream di stringa che conterrà i dati che abbiamo ricevuto, prima di essere de-serializzati
    std::stringstream accumulo;
    std::shared_ptr<serverMessage> mes;
    boost::archive::text_iarchive ia(accumulo);
    //associamo il textstream al socket
    QTextStream stream(&(this->socket));
    //leggiamo la prima linea di dati ricevuti
    QString line = stream.readLine();
    //la salviamo nello stream
    accumulo << line.toLocal8Bit().constData();
    //leggiamo tutti i dati ricevuti e ricostruiamo lo stream che è stato inviato dal client
    do{
        qDebug() << "socketdescriptor - " << this->socket.socketDescriptor() << ": " << line;
        line = stream.readLine();
        if(!line.isNull())
            accumulo << "\n" << line.toLocal8Bit().constData();
    }while(!line.isNull());
    //nella variabile accumulo abbiamo lo stream inviato dal server
    qDebug() << "\n" << "ricevuto: " << QString::fromStdString(accumulo.str());
    //ia >> mes;
    //mes->invokeMethod(this->client);
}

void clientdispatcher::sendMessage(const std::shared_ptr<clientMessage> MessageToSend){
    std::stringstream ofs;
    boost::archive::text_oarchive oa(ofs);
    QTextStream out(&(this->socket));

    uri c;

    clientMessage msg = *MessageToSend;
    oa << c;
    qDebug() << QString::fromStdString(ofs.str());
    out << QString::fromStdString(ofs.str());
    if (out.status() != QTextStream::Ok)
        throw sendFailure();
}

signUpMessage clientdispatcher::signUp(const std::string &username, const std::string &pwd, const std::string &nickname, const std::string &iconPath){
    std::shared_ptr<signUpMessage> mess = std::make_shared<signUpMessage>(this->client.signUp(username,pwd,nickname,iconPath));
    try {
        sendMessage(mess);
        //facciamo partire il timer

    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio

    }
}

clientMessage clientdispatcher::logIn(const std::string &username, const std::string &pwd) {
    std::shared_ptr<clientMessage> mess = std::make_shared<clientMessage>(this->client.logIn(username,pwd));
    try {
        sendMessage(mess);
        //facciamo partire il timer

    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio

    }
}

askResMessage clientdispatcher::openSource(const std::string &path, const std::string &name, privilege reqPriv) {
    std::shared_ptr<askResMessage> mess = std::make_shared<askResMessage>(this->client.openSource(path,name,reqPriv));
    try {
        sendMessage(mess);
        //facciamo partire il timer

    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio

    }
}

askResMessage clientdispatcher::openNewSource(const std::string &resourceId, privilege reqPriv, const std::string &destPath, const std::string& destName) {
    std::shared_ptr<askResMessage> mess = std::make_shared<askResMessage>(this->client.openNewSource(resourceId,reqPriv,destPath,destName));
    try {
        sendMessage(mess);
        //facciamo partire il timer

    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio

    }
}

askResMessage clientdispatcher::createNewSource(const std::string &path, const std::string &name) {
    std::shared_ptr<askResMessage> mess = std::make_shared<askResMessage>(this->client.createNewSource(path,name));
    try {
        sendMessage(mess);
        //facciamo partire il timer

    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio

    }
}

askResMessage clientdispatcher::createNewDir(const std::string &path, const std::string &name) {
    std::shared_ptr<askResMessage> mess = std::make_shared<askResMessage>(this->client.createNewDir(path,name));
    try {
        sendMessage(mess);
        //facciamo partire il timer

    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio

    }
}

symbolMessage clientdispatcher::localInsert(int resourceId, const symbol &newSym, const std::pair<int, int> &index) {
    std::shared_ptr<symbolMessage> mess = std::make_shared<symbolMessage>(this->client.localInsert(resourceId,newSym,index));
    try {
        sendMessage(mess);
        //facciamo partire il timer

    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio

    }
}

symbolMessage clientdispatcher::localRemove(int resourceId, const std::pair<int, int> indexes) {
    std::shared_ptr<symbolMessage> mess = std::make_shared<symbolMessage>(this->client.localRemove(resourceId,indexes));
    try {
        sendMessage(mess);
        //facciamo partire il timer

    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio

    }
}

privMessage clientdispatcher::editPrivilege(const std::string &targetUser, const std::string &resPath, const std::string &resName, privilege newPrivilege) {
    std::shared_ptr<privMessage> mess = std::make_shared<privMessage>(this->client.editPrivilege(targetUser,resPath,resName,newPrivilege));
    try {
        sendMessage(mess);
        //facciamo partire il timer

    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio

    }
}

uriMessage clientdispatcher::shareResource(const std::string &resPath, const std::string &resName, const uri &newPrefs) {
    std::shared_ptr<uriMessage> mess = std::make_shared<uriMessage>(this->client.shareResource(resPath,resName,newPrefs));
    try {
        sendMessage(mess);
        //facciamo partire il timer

    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio

    }
}

askResMessage clientdispatcher::renameResource(const std::string &resPath, const std::string &resName, const std::string &newName) {
    std::shared_ptr<askResMessage> mess = std::make_shared<askResMessage>(this->client.renameResource(resPath,resName,newName));
    try {
        sendMessage(mess);
        //facciamo partire il timer

    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio

    }
}

askResMessage clientdispatcher::removeResource(const std::string &resPath, const std::string &resName) {
    std::shared_ptr<askResMessage> mess = std::make_shared<askResMessage>(this->client.removeResource(resPath,resName));
    try {
        sendMessage(mess);
        //facciamo partire il timer

    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio

    }
}

updateDocMessage clientdispatcher::closeSource(int resourceId) {
    std::shared_ptr<updateDocMessage> mess = std::make_shared<updateDocMessage>(this->client.closeSource(resourceId));
    try {
        sendMessage(mess);
        //facciamo partire il timer

    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio

    }
}

userDataMessage clientdispatcher::editUser(user &newUserData) {
    std::shared_ptr<userDataMessage> mess = std::make_shared<userDataMessage>(this->client.editUser(newUserData));
    try {
        sendMessage(mess);
        //facciamo partire il timer

    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio

    }
}

clientMessage clientdispatcher::removeUser() {
    std::shared_ptr<clientMessage> mess = std::make_shared<clientMessage>(this->client.removeUser());
    try {
        sendMessage(mess);
        //facciamo partire il timer

    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio

    }
}

clientMessage clientdispatcher::logout() {
    std::shared_ptr<clientMessage> mess = std::make_shared<clientMessage>(this->client.logout());
    try {
        sendMessage(mess);
        //facciamo partire il timer

    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio

    }
}

updateDocMessage clientdispatcher::mapSiteIdToUser(const document &currentDoc) {
    std::shared_ptr<updateDocMessage> mess = std::make_shared<updateDocMessage>(this->client.mapSiteIdToUser(currentDoc));
    try {
        sendMessage(mess);
        //facciamo partire il timer

    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio

    }
}















/*void clientdispatcher::sendMessage(const std::shared_ptr<clientMessage> MessageToSend){
    //questa funzione invia il messaggio al server
    //QByteArray dat;
    //QDataStream out(&dat, QIODevice::WriteOnly);
    //std::stringstream stream;
    //boost::archive::text_oarchive oa(stream);
    //oa << t;
    //out << t;
    //this->socket.write(&oa,sizeof(oa));
    //this->socket.write("ciao");

    QTextStream stream(&(this->socket));
    int msgaction;
    int msgtype;

    try{
        msgaction = getmsgaction(MessageToSend);
        msgtype = getClassOfMessage(*MessageToSend);

    }catch(Symposium::clientdispatcherException t){
        //messaggio con action non tra quelli possibili oppure classe del messaggio sconosciuta, annulliamo l'invio del messaggio

        //VEDERE COME FARE PER ANNULLARE INVIO DEL MESSAGGIO
    }


    QString straction(msgaction);
    QString strtype(msgtype);

    QString actionOwner(getActionOwner(MessageToSend));

    try {

        switch(msgtype){
        case 1:{
            //messaggio askresmessage
            auto mess = dynamic_cast<const askResMessage*>(MessageToSend.get());
            QString path = QString::fromStdString(mess->getPath());
            QString name = QString::fromStdString(mess->getName());
            QString resID = QString::fromStdString(mess->getResourceId());
            QString msgId = QString::number(mess->getMsgId());
            QString accMod = QString::number(getPrivilege(mess->getaccessMode()));
            //abbiamo tutti i campi del messaggio, componiamo lo stream
            stream << msgtype << ";" << msgaction << ";" << actionOwner << ";" << path << ";" << name << ";" << resID << ";" << accMod << ";" << msgId;
            qDebug() << msgtype << ";" << msgaction << ";" << actionOwner << ";" << path << ";" << name << ";" << resID << ";" << accMod << ";" << msgId << "\n";
            break;
        }case 2:{
            //messaggio signupmessage
            auto mess = dynamic_cast<const signUpMessage*>(MessageToSend.get());
            QString newUser(getnewUser(mess->getNewUser()));
            QString msgId = QString::number(mess->getMsgId());
            //abbiamo tutti i campi del messaggio, componiamo lo stream
            stream << msgtype << ";" << msgaction << ";" << actionOwner << ";" << newUser << ";" << msgId;
            qDebug() << msgtype << ";" << msgaction << ";" << actionOwner << ";" << newUser << ";" << msgId << "\n";
            break;
        }case 3:{
            //messaggio updatedocmessage
            auto mess = dynamic_cast<const updateDocMessage*>(MessageToSend.get());
            QString resID = QString::number(mess->getResourceId());
            QString msgId = QString::number(mess->getMsgId());
            //abbiamo tutti i campi del messaggio, componiamo lo stream
            stream << msgtype << ";" << msgaction << ";" << actionOwner << ";" << resID << ";" << msgId;
            qDebug() << msgtype << ";" << msgaction << ";" << actionOwner << ";" << resID << ";" << msgId << "\n";
            break;
        }case 9:{
            //messaggio privmessage
            auto mess = dynamic_cast<const privMessage*>(MessageToSend.get());
            QString result(getmsgOutcome(mess->getResult()));
            QString resID = QString::fromStdString(mess->getResourceId());
            QString targUser = QString::fromStdString(mess->getTargetUser());
            QString priv = QString::number(getPrivilege(mess->getNewPrivilege()));
            QString msgId = QString::number(mess->getMsgId());
            //abbiamo tutti i campi del messaggio, componiamo lo stream
            stream << msgtype << ";" << msgaction << ";" << actionOwner << ";" << result << ";" << resID << ";" << targUser<< ";" << priv << ";" << msgId;
            qDebug() << msgtype << ";" << msgaction << ";" << actionOwner << ";" << result << ";" << resID << ";" << targUser<< ";" << priv << ";" << msgId << "\n";
            break;
        }case 10:{
            //messaggio symbolmessage
            auto mess = dynamic_cast<const symbolMessage*>(MessageToSend.get());
            QString result(getmsgOutcome(mess->getResult()));
            QString siteID = QString::number(mess->getSiteId());
            QString resID = QString::number(mess->getResourceId());
            QString symbol = getSymbolserialized(mess->getSym());
            QString msgId = QString::number(mess->getMsgId());
            //abbiamo tutti i campi del messaggio, componiamo lo stream
            stream << msgtype << ";" << msgaction << ";" << actionOwner << ";" << result << ";" << siteID << ";" << resID << ";" << symbol << ";" << msgId;
            qDebug() << msgtype << ";" << msgaction << ";" << actionOwner << ";" << result << ";" << siteID << ";" << resID << ";" << symbol << ";" << msgId << "\n";
            break;
        }case 11:{
            //messaggio urimessage
            auto mess = dynamic_cast<const uriMessage*>(MessageToSend.get());
            QString result(getmsgOutcome(mess->getResult()));
            QString path = QString::fromStdString(mess->getPath());
            QString name = QString::fromStdString(mess->getName());
            QString msgId = QString::number(mess->getMsgId());
            //abbiamo tutti i campi del messaggio, componiamo lo stream
            stream << msgtype << ";" << msgaction << ";" << actionOwner << ";" << result << ";" << path << ";" << name << ";" << getUriserialized(mess->getSharingPrefs()) <<
                      ";" << msgId;
            qDebug() << msgtype << ";" << msgaction << ";" << actionOwner << ";" << result << ";" << path << ";" << name << ";" << getUriserialized(mess->getSharingPrefs()) <<
                        ";" << msgId << "\n";
            break;
        }case 12:{
            //messaggio userdatamessage
            auto mess = dynamic_cast<const userDataMessage*>(MessageToSend.get());
            QString result(getmsgOutcome(mess->getResult()));

            //DA VEDERE COME INVIARE L'UTENTE, IN QUANTO CONTIENE ALL'INTERNO TUTTA LA SUA DIRECTORY, QUINDI SAREBBE TUTTA DA SERIALIZZARE!
        }default:{
            //Se cadiamo in questo caso, significa che il messaggio da inviare NON è un tipo di messaggio che il client può inviare, quindi ERRORE
            throw clientdispatcherException(clientdispatcherException::MsgActionNotAllowed, UnpackFileLineFunction());
        }

        }
    }catch (Symposium::clientdispatcherException t){
        //action del messaggio non tra quelle consentite per l'invio da parte del client, quindi notifichiamo l'annullamento dell'invio

        //VEDERE COME FARE PER ANNULLARE INVIO DEL MESSAGGIO
    }

    qDebug() << "Send Succesfull\n";

}*/

void clientdispatcher::test_sendMessage(){
    this->socket.write("ciao");
}

/*int clientdispatcher::getmsgaction(const std::shared_ptr<clientMessage> Message){
    switch(Message->getAction()){
    case Symposium::msgType::registration:
        return 1;
        break;
    case Symposium::msgType::login:
        return 2;
        break;
    case Symposium::msgType::changeUserData:
        return 3;
        break;
    case Symposium::msgType::changeUserPwd:
        return 4;
        break;
    case Symposium::msgType::removeUser:
        return 5;
        break;
    case Symposium::msgType::logout:
        return 6;
        break;
    case Symposium::msgType::createRes:
        return 7;
        break;
    case Symposium::msgType::createNewDir:
        return 8;
        break;
    case Symposium::msgType::openRes:
        return 9;
        break;
    case Symposium::msgType::openNewRes:
        return 10;
        break;
    case Symposium::msgType::changeResName:
        return 11;
        break;
    case Symposium::msgType::removeRes:
        return 12;
        break;
    case Symposium::msgType::mapChangesToUser:
        return 13;
        break;
    case Symposium::msgType::changePrivileges:
        return 14;
        break;
    case Symposium::msgType::shareRes:
        return 15;
        break;
    case Symposium::msgType::insertSymbol:
        return 16;
        break;
    case Symposium::msgType::removeSymbol:
        return 17;
        break;
    case Symposium::msgType::addActiveUser:
        return 18;
        break;
    case Symposium::msgType::removeActiveUser:
        return 19;
        break;
    case Symposium::msgType::closeRes:
        return 20;
        break;
    default:
        throw clientdispatcherException(clientdispatcherException::UnknownMessageAction, UnpackFileLineFunction());
        break;
    }
}

int clientdispatcher::getClassOfMessage(const clientMessage& Message){
    if(dynamic_cast<const Symposium::askResMessage*>(&Message)){
        return 1;
    }else if(dynamic_cast<const Symposium::signUpMessage*>(&Message)){
        return 2;
    }else if(dynamic_cast<const Symposium::updateDocMessage*>(&Message)){
        return 3;
    }else if(dynamic_cast<const Symposium::serverMessage*>(&Message)){
        return 4;
    }else if(dynamic_cast<const Symposium::loginMessage*>(&Message)){
        return 5;
    }else if(dynamic_cast<const Symposium::mapMessage*>(&Message)){
        return 6;
    }else if(dynamic_cast<const Symposium::sendResMessage*>(&Message)){
        return 7;
    }else if(dynamic_cast<const Symposium::updateActiveMessage*>(&Message)){
        return 8;
    }else if(dynamic_cast<const Symposium::privMessage*>(&Message)){
        return 9;
    }else if(dynamic_cast<const Symposium::symbolMessage*>(&Message)){
        return 10;
    }else if(dynamic_cast<const Symposium::uriMessage*>(&Message)){
        return 11;
    }else if(dynamic_cast<const Symposium::userDataMessage*>(&Message)){
        return 12;
    }else if(dynamic_cast<const Symposium::clientMessage*>(&Message)){
        return 13;
    }else{
        throw clientdispatcherException(clientdispatcherException::UnknownClassOfMessage, UnpackFileLineFunction());
    }
}

QString clientdispatcher::getActionOwner(const std::shared_ptr<clientMessage> Message){
    //la funzione restituisce l'actionOwner del messaggio, come user;pass
    QString s = QString::fromStdString(Message->getActionOwner().first + ";" + Message->getActionOwner().second);
    return s;
}

int clientdispatcher::getPrivilege(const privilege priv){
    switch(priv){
    case Symposium::privilege::none:
        return 1;
    case Symposium::privilege::readOnly:
        return 2;
    case Symposium::privilege::modify:
        return 3;
    case Symposium::privilege::owner:
        return 4;
    default:
        return 0;
    }
}

QString clientdispatcher::getnewUser(const user& utente){
    //l'ultimo parametro del nuovo utente sarebbe il puntatore al suo filesystem, ma essendo sempre nullo (perchè utente ancora da creare), lo ometto
    QString s = QString::fromStdString(utente.getUsername() + ";" + utente.getPwdHash() + ";" + utente.getNickname() + ";" + utente.getIconPath() + ";") +
            QString::number(utente.getSiteId());
    return s;
}

int clientdispatcher::getmsgOutcome(const msgOutcome& Message){
    switch(Message){
    case Symposium::msgOutcome::failure:
        return 1;
    case Symposium::msgOutcome::success:
        return 2;
    default:
        return 0;
    }
}

QString clientdispatcher::getSymbolserialized(const symbol& sym){
    wchar_t cr = sym.getCh();
    std::vector<int> vc = sym.getPos();
    QString s = QString::fromWCharArray(&cr) + ";" + sym.getSiteId() + ";" + sym.getCounter() + ";";
    std::vector<int>::iterator it = vc.begin();
    while(it != vc.end()){
        //i vari elementi sono separati da uno spazio " "
        s = s + " " + *it;
        it++;
    }
    s = s + ";" + sym.getVerified();
    return s;
}

QString clientdispatcher::getUriserialized(const uri& uri){
    int actpol;
    QString s;
    switch(uri.getActivePolicy()){
    case Symposium::uriPolicy::inactive:
        actpol = 1;
        break;
    case Symposium::uriPolicy::activeAlways:
        actpol = 2;
        break;
    case Symposium::uriPolicy::activeCount:
        actpol = 3;
        break;
    case Symposium::uriPolicy::activeTimer:
        actpol = 4;
        break;
    default:
        actpol = 0;
        break;
    }
    s = QString::number(actpol) + ";" + QString::number(uri.getSharesLeft()) + ";";
    //convertiamo lo stopTime dell'uri in stringa
    std::time_t t = std::chrono::system_clock::to_time_t(uri.getStopTime());
    std::string ts = std::ctime(&t);
    ts.resize(ts.size()-1);
    s = s + QString::fromStdString(ts) + ";" + QString::number(getPrivilege(uri.getGranted()));
    return s;
}*/


