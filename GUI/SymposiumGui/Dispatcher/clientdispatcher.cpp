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
 * File:   clientdispatcher.cpp
 * Project: Symposium
 * Authors:
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Cristian Gianetto <s265387 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 9 Febbraio 2020, 16.23
 */

#include "../sigin.h"
#include "../signup.h"
#include "../deleteaccount.h"
#include "../changeuserinfo.h"
#include "../textedit.h"
#include "../onlineusers.h"
#include "../alluser.h"
#include "../../../filesystem.h"
#include "clientdispatcher.h"

using namespace Symposium;

clientdispatcher::clientdispatcher(QObject *parent) : QObject(parent), finestreDocumenti()
{
    //connettiamo il socket all'indirizzo del server
    this->socket.connectToHost(svAddress, svPort);
    //quando riceviamo qualcosa eseguiamo la funzione di lettura (readyRead)
    connect(&(this->socket), &QIODevice::readyRead, this, &clientdispatcher::readyRead);
    qDebug() << "Connection Successful\n";
    this->client.setClientDispatcher(this);
}

void clientdispatcher::readyRead(){
    qDebug() << "Ricevuto qualcosa";
    //questa funzione viene chiamata quando il server ci ha inviato qualcosa
    //stream di stringa che conterrà i dati che abbiamo ricevuto, prima di essere de-serializzati
    std::stringstream accumulo;
    std::shared_ptr<serverMessage> mes;
    serverMessage des(Symposium::msgType::login,Symposium::msgOutcome::success);
    //associamo il textstream al socket
    QTextStream stream(&(this->socket));
    //leggiamo la prima linea di dati ricevuti
    qDebug() << "Leggiamo prima riga";
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
    boost::archive::text_iarchive ia(accumulo);
    ia >> des;
    qDebug() << "server dice: " << QString::fromStdString("ok");
    qDebug() << "id messaggio ricevuto: " << des.getMsgId();
    mes = std::make_shared<serverMessage>(des);
    try {
        mes->invokeMethod(this->client);
    } catch (messageException& e) {
        //eccezione di insuccesso dell'operazione

        switch(currentWindow){
        case 1:{
            this->finestraLogin->errorSignIn();
        }case 2:{
            this->finestraSignup->errorSignUp(mes->getErrDescr());
        }case 3:{
            //QUESTO FUNZIONA SE SI CAMBIA IL TIPO DELLE ECCEZIONI CHE VENGONO LANCIATE IN MESSAGE COME SCRITTO SUL GRUPPO, ALTRIMENTI DA VARIARE

            //this->finestraInsertUri->unsuccessInsert(mes->getErrDescr());
        }case 5:{
            this->finestraEliminaAccount->errorDeleteUser(mes->getErrDescr());
        }case 7:{

            //CHANGEUSERDATA, DA RIVEDERE PERCHE' RICADIAMO IN QUESTO CASO IN 2 ECCEZIONI, CHE DEVO POTER DISCRIMINARE

            //this->finestraModificaUser->errorEditUser(mes->getErrDescr());
        }case 8:{
            this->finestraActiveCounterLink->unsuccessLink(mes->getErrDescr());
        }case 9:{
            this->finestraActiveTimerLink->unsuccessLink(mes->getErrDescr());
        }case 10:{
            this->finestraActiveAlwaysLink->unsuccessLink(mes->getErrDescr());
        }case 13:{
            this->finestraOnlineUser->errorEditPrivilege(mes->getErrDescr());
        }case 14:{
            this->finestraAllUser->errorEditPrivilege(mes->getErrDescr());
        }case 15:{
            this->finestraActiveNonLink->unsuccessLink(mes->getErrDescr());
        }
        }

        //qDebug() << QString::fromStdString(mes->getErrDescr());
    } catch (SymClientException& e){
        //eccezione di relatedMessage non trovato

        //DA VEDERE CHE MESSAGGIO DEVE DARE SULLA FINESTRA

        switch(currentWindow){
        case 1:{
            this->finestraLogin->errorSignIn();
        }case 2:{
            //this->finestraSignup->errorSignUp();
        }case 3:{
            //this->finestraInsertUri->unsuccessInsert();
        }case 5:{
            //this->finestraEliminaAccount->errorDeleteUser();
        }case 7:{
            //this->finestraModificaUser->errorEditUser();
        }case 8:{
            //this->finestraActiveCounterLink->unsuccessLink();
        }case 9:{
            //this->finestraActiveTimerLink->unsuccessLink();
        }case 10:{
            //this->finestraActiveAlwaysLink->unsuccessLink();
        }case 13:{
            //this->finestraOnlineUser->errorEditPrivilege();
        }case 14:{
            //this->finestraAllUser->errorEditPrivilege();
        }case 15:{
            //this->finestraActiveNonLink->unsuccessLink();
        }
        }
    }





    /*if(mes->isRelatedTo(*(this->message))){
        //abbiamo ricevuto la risposta che stavamo aspettando
        this->timer.stop();
        switch(this->message->getAction()){
        case Symposium::msgType::login:{
            //il messaggio ricevuto è la risposta al login
            if(mes->getResult()==Symposium::msgOutcome::success){
                //((sigin*)this->currentWindow)->successSignIn();
            }else{
                //((sigin*)this->currentWindow)->errorSignIn();
            }
        }case Symposium::msgType::registration:{
            //il messaggio ricevuto è la risposta alla registrazione

            //DA VERIFICARE GLI ERRORI

        }case Symposium::msgType::removeUser:{
            //il messaggio ricevuto è la risposta alla rimozione dell'utente
            if(mes->getResult()==Symposium::msgOutcome::success){
                //((deleteAccount*)this->currentWindow)->successDeleteAccount();
            }else{
                //((deleteAccount*)this->currentWindow)->unsuccessDeleteAccount();
            }
        }case Symposium::msgType::changeUserData:{
            //il messaggio ricevuto è la risposta al cambio dei dati dell'utente

            //DA VERIFICARE GLI ERRORI

        }case Symposium::msgType::openNewRes:{
            //il messaggio ricevuto è la risposta alla openNewSource

            //DA VERIFICARE GLI ERRORI
        }case Symposium::msgType::changePrivileges:{
            //il messaggio ricevuto è la risposta al cambio dei privilegi di un utente su un documento

            //DA VERIFICARE GLI ERRORI
        }
        }
    }*/
   // mes->invokeMethod(this->client);
}

void clientdispatcher::sendMessage(const std::shared_ptr<clientMessage> MessageToSend){
    std::stringstream ofs;
    boost::archive::text_oarchive oa(ofs);
    QTextStream out(&(this->socket));
    clientMessage msg = *MessageToSend;
    oa << msg;
    //qDebug() << QString::fromStdString(ofs.str());
    out << QString::fromStdString(ofs.str());
    if (out.status() != QTextStream::Ok){
        throw sendFailure();
    }else{
        this->timer.start(TEMPOATTESA);
        /*if((msg.getAction()==Symposium::msgType::login) || (msg.getAction()==Symposium::msgType::registration) || (msg.getAction()==Symposium::msgType::removeUser) ||
               (msg.getAction()==Symposium::msgType::changeUserData) || (msg.getAction()==Symposium::msgType::openNewRes) || (msg.getAction()==Symposium::msgType::changePrivileges)){
            //messaggio per cui dobbiamo ricevere risposta dal server
            this->timer.start(TEMPOATTESA);
            this->message = MessageToSend;
        }else if(msg.getAction()==Symposium::msgType::logout){
            this->socket.close();
            this->client.logout();
        }*/
    }
}

void clientdispatcher::signUp(const std::string &username, const std::string &pwd, const std::string &nickname, const std::string &iconPath){
    std::shared_ptr<signUpMessage> mess = std::make_shared<signUpMessage>(this->client.signUp(username,pwd,nickname,iconPath));
    try {
        //disconnettiamo il timer da altri eventuali slot
        this->timer.disconnect();
        //connettiamo il timer al giusto metodo
        connect(&(this->timer), &QTimer::timeout, this, &clientdispatcher::signupExpired);
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        this->finestraLogin->errorConnection();
    }
}

void clientdispatcher::logIn(const std::string &username, const std::string &pwd) {
    std::shared_ptr<clientMessage> mess = std::make_shared<clientMessage>(this->client.logIn(username,pwd));
    try {
        //disconnettiamo il timer da altri eventuali slot
        this->timer.disconnect();
        //connettiamo il timer al giusto metodo
        connect(&(this->timer), &QTimer::timeout, this, &clientdispatcher::signinExpired);
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        this->finestraLogin->errorConnection();
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

void clientdispatcher::openNewSource(const std::string &resourceId, privilege reqPriv, const std::string &destPath, const std::string& destName) {
    std::shared_ptr<askResMessage> mess = std::make_shared<askResMessage>(this->client.openNewSource(resourceId,reqPriv,destPath,destName));
    try {
        //disconnettiamo il timer da altri eventuali slot
        this->timer.disconnect();
        //connettiamo il timer al giusto metodo
        connect(&(this->timer), &QTimer::timeout, this, &clientdispatcher::openNewSourceExpired);
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        this->finestraInsertUri->errorConnection();
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

void clientdispatcher::editPrivilege(const std::string &targetUser, std::string &resPath, privilege newPrivilege, int documentID) {
    std::shared_ptr<privMessage> mess = std::make_shared<privMessage>(this->client.editPrivilege(targetUser,resPath, std::to_string(documentID), newPrivilege));
    try {
        //disconnettiamo il timer da altri eventuali slot
        this->timer.disconnect();
        //connettiamo il timer al giusto metodo
        connect(&(this->timer), &QTimer::timeout, this, &clientdispatcher::editPrivilegeExpired);
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        if(this->currentWindow==13){
            this->finestraOnlineUser->errorConnection();
        }else{
            this->finestraAllUser->errorConnection();
        }
    }
}

void clientdispatcher::shareResource(const std::string &resPath, const std::string &resName, const uri &newPrefs) {
    std::shared_ptr<uriMessage> mess = std::make_shared<uriMessage>(this->client.shareResource(resPath,resName,newPrefs));
    try {
        //disconnettiamo il timer da altri eventuali slot
        this->timer.disconnect();
        //connettiamo il timer al giusto metodo
        connect(&(this->timer), &QTimer::timeout, this, &clientdispatcher::shareResourceExpired);
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        if(this->currentWindow==8){
            this->finestraActiveCounterLink->errorConnection();
        }else if(this->currentWindow==9){
            this->finestraActiveTimerLink->errorConnection();
        }else if(this->currentWindow==10){
            this->finestraActiveAlwaysLink->errorConnection();
        }else{
            this->finestraActiveNonLink->errorConnection();
        }
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

void clientdispatcher::editUser(user &newUserData) {
    std::shared_ptr<userDataMessage> mess = std::make_shared<userDataMessage>(this->client.editUser(newUserData));
    try {
        //disconnettiamo il timer da altri eventuali slot
        this->timer.disconnect();
        //connettiamo il timer al giusto metodo
        connect(&(this->timer), &QTimer::timeout, this, &clientdispatcher::editUserExpired);
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        this->finestraModificaUser->errorConnection();
    }
}

void clientdispatcher::removeUser() {
    std::shared_ptr<clientMessage> mess = std::make_shared<clientMessage>(this->client.removeUser());
    try {
        //disconnettiamo il timer da altri eventuali slot
        this->timer.disconnect();
        //connettiamo il timer al giusto metodo
        connect(&(this->timer), &QTimer::timeout, this, &clientdispatcher::removeUserExpired);
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        this->finestraEliminaAccount->errorConnection();
    }
}

void clientdispatcher::logout() {
    std::shared_ptr<clientMessage> mess = std::make_shared<clientMessage>(this->client.logout());
    try {
        //disconnettiamo il timer da altri eventuali slot
        this->timer.disconnect();
        //connettiamo il timer al giusto metodo
        connect(&(this->timer), &QTimer::timeout, this, &clientdispatcher::logoutExpired);
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio ma non facciamo nulla
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

void clientdispatcher::moveMyCursor(int resId, int block, int column){
    std::shared_ptr<cursorMessage> mess = std::make_shared<cursorMessage>(this->client.updateCursorPos(resId, block, column));
    try {
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio

        //COSA FACCIAMO??

    }
}

void clientdispatcher::moveUserCursor(int resId, int block, int column, int siteId){
    notepad* finestra = this->getCorrectNotepadbyResourceID(resId);
    finestra->moveUserCursor(siteId,block,column);
}

void clientdispatcher::addUserCursor(int siteID, std::string username, int resourceID){
    notepad* finestra = this->getCorrectNotepadbyResourceID(resourceID);
    finestra->addUserCursor(siteID,username);
}

void clientdispatcher::removeUserCursor(int siteID, int resourceID){
    notepad* finestra = this->getCorrectNotepadbyResourceID(resourceID);
    finestra->removeUserCursor(siteID);
}



notepad* clientdispatcher::getCorrectNotepadbyResourceID(int resourceID){
    for (std::pair<int,notepad*> it:this->finestreDocumenti){
        if(it.first == resourceID)
            return (it.second);
    }
}

void clientdispatcher::successLogin(){
    this->finestraLogin->successSignIn();
}

void clientdispatcher::successSignUp(){
    this->finestraSignup->successSignUp();
}

void clientdispatcher::successDeleteAccount(){
    this->finestraEliminaAccount->successDeleteAccount();
}

void clientdispatcher::successEditUser(){
    this->finestraModificaUser->successEditUser();
}

void clientdispatcher::successInsertUri(){
    this->finestraInsertUri->successInsert();
}

void clientdispatcher::successEditPrivilege(){
    if(this->currentWindow==13){
       this->finestraOnlineUser->successEditPrivilege();
    }else{
       this->finestraAllUser->successEditPrivilege();
    }
}

void clientdispatcher::successShareResource(std::string path){
    if(this->currentWindow==8){
        this->finestraActiveCounterLink->successLink(path);
    }else if(this->currentWindow==9){
        this->finestraActiveTimerLink->successLink(path);
    }else if(this->currentWindow==10){
        this->finestraActiveAlwaysLink->successLink(path);
    }else{
        this->finestraActiveNonLink->successLink(path);
    }
}

void clientdispatcher::closeConnection(){
    this->socket.close();
}

const std::forward_list<std::pair<const user *, sessionData>> clientdispatcher::onlineUser(int documentID){
    return (this->client.onlineUsersonDocument(documentID));
}

std::unordered_map<std::string, privilege> clientdispatcher::allUser(int documentID){
    return (this->client.allUsersonDocument(documentID));
}

user clientdispatcher::getUser(){
    return this->client.userData();
}

std::string clientdispatcher::showHome(){
    return this->client.showDir();
}

std::string clientdispatcher::showDir(bool recursive){
    return this->client.showDir(recursive);
}

void clientdispatcher::setSignIn(sigin *si){
    this->finestraLogin = si;
    this->currentWindow = 1;
}

void clientdispatcher::setTextEdit(int resourceID, notepad *te){
    this->finestreDocumenti.push_back(std::make_pair(resourceID,te));
}

void clientdispatcher::setSignUp(signup *su){
    this->finestraSignup = su;
    this->currentWindow = 2;
}

void clientdispatcher::setInsertUri(inserturi *iu){
    this->finestraInsertUri = iu;
    this->currentWindow = 3;
}

void clientdispatcher::setHome(home *ho){
    this->finestraHome = ho;
    this->currentWindow = 4;
}

void clientdispatcher::setDirectory(directory *dr){
    this->finestraDirectory = dr;
    this->currentWindow = 12;
}

void clientdispatcher::setDeleteAccount(deleteAccount *da){
    this->finestraEliminaAccount = da;
    this->currentWindow = 5;
}

void clientdispatcher::setChooseDir(choosedir *cd){
    this->finestraSceltaDir = cd;
    this->currentWindow = 6;
}

void clientdispatcher::setChangeUserInfo(changeUserInfo *cui){
    this->finestraModificaUser = cui;
    this->currentWindow = 7;
}

void clientdispatcher::setActiveCounterLink(activecounterlink *acl){
    this->finestraActiveCounterLink = acl;
    this->currentWindow = 8;
}

void clientdispatcher::setActiveTimerLink(activetimerlink *atl){
    this->finestraActiveTimerLink = atl;
    this->currentWindow = 9;
}

void clientdispatcher::setActiveAlwaysLink(activealwayslink *aal){
    this->finestraActiveAlwaysLink = aal;
    this->currentWindow = 10;
}

void clientdispatcher::signinExpired(){
    this->timer.stop();
    this->finestraLogin->errorConnection();
    qDebug() << "Timer scaduto\n";
}

void clientdispatcher::setOnlineUser(onlineusers *ou){
    this->finestraOnlineUser = ou;
    this->currentWindow = 13;
}

void clientdispatcher::setAllUser(alluser *au){
    this->finestraAllUser = au;
    this->currentWindow = 14;
}

void clientdispatcher::setActiveNonLink(activenonlink *anl){
    this->finestraActiveNonLink = anl;
    this->currentWindow = 15;
}

void clientdispatcher::signupExpired(){
    this->timer.stop();
    qDebug() << "Timer scaduto\n";
    //VEDERE SE BISOGNA CHIAMARE QUESTO METODO
    //((signup*)this->currentWindow)->errorConnection();
}

void clientdispatcher::logoutExpired(){
    this->timer.stop();
    qDebug() << "Timer scaduto\n";
    //VEDERE COSA BISOGNA FARE
}

void clientdispatcher::removeUserExpired(){
    this->timer.stop();
    qDebug() << "Timer scaduto\n";
    //VEDERE SE BISOGNA CHIAMARE QUESTO METODO
    //((deleteaccount*)this->currentWindow)->errorConnection();
}

void clientdispatcher::editUserExpired(){
    this->timer.stop();
    qDebug() << "Timer scaduto\n";
    //VEDERE SE BISOGNA CHIAMARE QUESTO METODO
    //((changeuserinfo*)this->currentWindow)->errorConnection();
}

void clientdispatcher::openNewSourceExpired(){
    this->timer.stop();
    qDebug() << "Timer scaduto\n";
    //VEDERE SE BISOGNA CHIAMARE QUESTO METODO
    //((TextEdit*)this->currentWindow)->errorConnection();
}

void clientdispatcher::editPrivilegeExpired(){
    this->timer.stop();
    qDebug() << "Timer scaduto\n";
    //VEDERE SE BISOGNA CHIAMARE QUESTO METODO
    //((TextEdit*)this->currentWindow)->errorConnection();
}

void clientdispatcher::shareResourceExpired(){
    this->timer.stop();
    qDebug() << "Timer scaduto\n";
    //VEDERE SE BISOGNA CHIAMARE QUESTO METODO
    //((TextEdit*)this->currentWindow)->errorConnection();
}
