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

#include <chrono>
#include "../sigin.h"
#include "../signup.h"
#include "../deleteaccount.h"
#include "../changeuserinfo.h"
#include "../textedit.h"
#include "../onlineusers.h"
#include "../alluser.h"
#include "../../../filesystem.h"
#include "../directory.h"
#include "clientdispatcher.h"

using namespace Symposium;

clientdispatcher::clientdispatcher(QObject *parent) : QObject(parent), finestreDocumenti()
{
    this->client.setClientDispatcher(this);
    this->userpwd = "";
}

void clientdispatcher::openConnection(){
    if (this->socket.state()==QAbstractSocket::UnconnectedState){
        qDebug() << "Connecting the socket to the server...";
        //impostiamo il keep_alive
        this->socket.setSocketOption(QAbstractSocket::KeepAliveOption, 1);
        //connettiamo il socket all'indirizzo del server
        this->socket.connectToHost(svAddress, svPort);
        //quando riceviamo qualcosa eseguiamo la funzione di lettura (readyRead)
        connect(&(this->socket), &QIODevice::readyRead, this, &clientdispatcher::readyRead);
        qDebug() << "Connection Successful";
    }
}

void clientdispatcher::readyRead(){
    //questa funzione viene chiamata quando il server ci ha inviato qualcosa
    //stream di stringa che conterrà i dati che abbiamo ricevuto, prima di essere de-serializzati
    std::stringstream accumulo;
    //creiamo la variabile che conterra il puntatore al messaggio ricevuto dal socket
    std::shared_ptr<serverMessage> mes;
    //creiamo il QByteArray che conterrà ciò che riceviamo dal socket
    QByteArray byteArray;
    //associamo il datastream al socket
    QDataStream in(&(this->socket));
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
    ia >> mes;
    try {
        mes->invokeMethod(this->client);
    } catch (messageException& e) {
        //eccezione di insuccesso dell'operazione

        switch(currentWindow){
        case 1:{
            this->finestraLogin->errorSignIn();
            break;
        }case 2:{
            this->finestraSignup->errorSignUp(mes->getErrDescr());
            this->userpwd="";
            break;
        }case 3:{
            this->finestraInsertUri->unsuccessInsert(mes->getErrDescr());
            break;
        }case 5:{
            this->finestraEliminaAccount->errorDeleteUser(mes->getErrDescr());
            break;
        }case 7:{
            this->finestraModificaUser->errorEditUser(mes->getErrDescr());
            break;
        }case 8:{
            this->finestraActiveCounterLink->unsuccessLink(mes->getErrDescr());
            break;
        }case 9:{
            this->finestraActiveTimerLink->unsuccessLink(mes->getErrDescr());
            break;
        }case 10:{
            this->finestraActiveAlwaysLink->unsuccessLink(mes->getErrDescr());
            break;
        }case 12:{
            this->finestraDirectory->failureActionDirectory(mes->getErrDescr());
            break;
        }case 13:{
            this->finestraOnlineUser->errorEditPrivilege(mes->getErrDescr());
            break;
        }case 14:{
            this->finestraAllUser->errorEditPrivilege(mes->getErrDescr());
            break;
        }case 15:{
            this->finestraActiveNonLink->unsuccessLink(mes->getErrDescr());
            break;
        }case 16:{
            this->finestraDirectory->failureActionDirectory(mes->getErrDescr());
            break;
        }
        }

    } catch (SymClientException& e){
        //eccezione di relatedMessage non trovato

        //DA VEDERE CHE MESSAGGIO DEVE DARE SULLA FINESTRA

        switch(currentWindow){
        case 1:{
            this->finestraLogin->errorSignIn();
            break;
        }case 2:{
            this->finestraSignup->errorSignUp("eccezione");
            break;
        }case 3:{
            //this->finestraInsertUri->unsuccessInsert();
            break;
        }case 5:{
            //this->finestraEliminaAccount->errorDeleteUser();
            break;
        }case 7:{
            //this->finestraModificaUser->errorEditUser();
            break;
        }case 8:{
            //this->finestraActiveCounterLink->unsuccessLink();
            break;
        }case 9:{
            //this->finestraActiveTimerLink->unsuccessLink();
            break;
        }case 10:{
            //this->finestraActiveAlwaysLink->unsuccessLink();
            break;
        }case 12:{
            //this->finestraDirectory->failureActionDirectory();
            break;
        }case 13:{
            //this->finestraOnlineUser->errorEditPrivilege();
            break;
        }case 14:{
            //this->finestraAllUser->errorEditPrivilege();
            break;
        }case 15:{
            //this->finestraActiveNonLink->unsuccessLink();
            break;
        }
        }
    }
}

void clientdispatcher::sendMessage(const std::shared_ptr<clientMessage> MessageToSend, uint_positive_cnt::type resourceId){
    std::stringstream ofs;
    boost::archive::text_oarchive oa(ofs);
    QDataStream uscita(&(this->socket));
    //serializziamo il messaggio
    oa << MessageToSend;
    //eseguiamo la conversione in QByteArray
    QByteArray byteArray(ofs.str().c_str(), ofs.str().length());
    //inviamo il messaggio
    uscita << byteArray;
    if (uscita.status() != QDataStream::Ok){
        throw sendFailure();
    }else{
        std::chrono::milliseconds tempo = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        TimerStart(tempo, resourceId);
        qDebug() << "Sended to server: " << QString::fromStdString(ofs.str());
    }
}

void clientdispatcher::TimerStart(std::chrono::milliseconds timeToSend, uint_positive_cnt::type resourceId){
    if(this->timer.isActive()){
        //timer già attivo, quindi inseriamo in coda
        attese.push(std::make_pair(timeToSend, resourceId));
    }else{
        connect(&(this->timer), &QTimer::timeout, this, &clientdispatcher::TimerExpired);
        std::chrono::milliseconds difftemp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()) - timeToSend;
        int numero = TEMPOATTESA - difftemp.count();
        this->timer.start(numero);
        ResIDofWaitingMessage = resourceId;
    }
}

void clientdispatcher::signUp(const std::string &username, const std::string &pwd, const std::string &nickname, const std::string &iconPath){
    this->userpwd = pwd;
    std::shared_ptr<signUpMessage> mess = std::make_shared<signUpMessage>(this->client.signUp(username,pwd,nickname,iconPath));
    //Colleghiamo il client al server
    this->openConnection();
    try {
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        //chiudiamo la connessione
        this->closeConnection();
        //dobbiamo notificare alla GUI
        this->finestraSignup->errorConnection();
    }
}

void clientdispatcher::logIn(const std::string &username, const std::string &pwd) {
    std::shared_ptr<clientMessage> mess = std::make_shared<clientMessage>(this->client.logIn(username,pwd));
    //Colleghiamo il client al server
    this->openConnection();
    try {
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        //chiudiamo la connessione
        this->closeConnection();
        //dobbiamo notificare alla GUI
        this->finestraLogin->errorConnection();
    }
}

void clientdispatcher::autologIn(const std::string &username){
    logIn(username, this->userpwd);
}

bool clientdispatcher::isAutoLogin(){
    if(this->userpwd==""){
        return false;
    }else{
        this->userpwd="";
        return true;
    }
}

void clientdispatcher::openSource(const std::string &path, const std::string &name, privilege reqPriv) {
    std::shared_ptr<askResMessage> mess = std::make_shared<askResMessage>(this->client.openSource(path,name,reqPriv));
    try {
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        this->closeConnection();
        //dobbiamo notificare alla GUI
        this->finestraDirectory->errorConnectionLogout(IMPOSSINVIARE);
    }
}

void clientdispatcher::openNewSource(const std::string &resourceId, privilege reqPriv, const std::string &destPath, const std::string& destName) {
    std::shared_ptr<askResMessage> mess = std::make_shared<askResMessage>(this->client.openNewSource(resourceId,reqPriv,destPath,destName));
    try {
        //inviamo il messaggio
        sendMessage(mess, std::stoi(resourceId));
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        this->closeConnection();
        //dobbiamo notificare alla GUI
        this->finestraInsertUri->errorConnectionLogout(IMPOSSINVIARE);
    }
}

void clientdispatcher::createNewSource(const std::string &path, const std::string &name) {
    std::shared_ptr<askResMessage> mess = std::make_shared<askResMessage>(this->client.createNewSource(path,name));
    try {
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        this->closeConnection();
        //dobbiamo notificare alla GUI
        this->finestraDirectory->errorConnectionLogout(IMPOSSINVIARE);
    }
}

void clientdispatcher::createNewDir(const std::string &path, const std::string &name) {
    std::shared_ptr<askResMessage> mess = std::make_shared<askResMessage>(this->client.createNewDir(path,name));
    try {
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        this->closeConnection();
        //dobbiamo notificare alla GUI
        this->finestraDirectory->errorConnectionLogout(IMPOSSINVIARE);
    }
}

void clientdispatcher::localInsert(uint_positive_cnt::type resourceId, const symbol &newSym, const std::pair<int, int> &index) {
    std::shared_ptr<symbolMessage> mess = std::make_shared<symbolMessage>(this->client.localInsert(resourceId,newSym,index));
    try {
        //inviamo il messaggio
        sendMessage(mess, resourceId);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        this->finestraDirectory->errorConnectionLogout(IMPOSSINVIARE);
    }
}

void clientdispatcher::localRemove(uint_positive_cnt::type resourceId, const std::pair<int, int> indexes) {
    std::shared_ptr<symbolMessage> mess = std::make_shared<symbolMessage>(this->client.localRemove(resourceId,indexes));
    try {
        //inviamo il messaggio
        sendMessage(mess, resourceId);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        this->finestraDirectory->errorConnectionLogout(IMPOSSINVIARE);
    }
}

void clientdispatcher::remoteInsert(uint_positive_cnt::type resourceId, const symbol &newSym, uint_positive_cnt::type siteId, std::pair<unsigned int, unsigned int> index){
    notepad* n = getCorrectNotepadbyResourceID(resourceId);
    n->remoteInsert(newSym, siteId, index);
}

void clientdispatcher::remoteRemove(uint_positive_cnt::type resourceId, uint_positive_cnt::type siteId, std::pair<int, int> indexes){
    notepad* n = getCorrectNotepadbyResourceID(resourceId);
    n->remoteDelete(indexes,siteId);
}

void clientdispatcher::verifySymbol(uint_positive_cnt::type resId, const symbol &newSym, std::pair<int, int> indexes){
    notepad* n = getCorrectNotepadbyResourceID(resId);
    n->verifySymbol(newSym, indexes);
}

Color clientdispatcher::getColor(uint_positive_cnt::type documentID, uint_positive_cnt::type siteID){
    return this->client.colorOfUser(documentID, siteID);
}

void clientdispatcher::editPrivilege(const std::string &targetUser, std::string &resPath, privilege newPrivilege, uint_positive_cnt::type documentID) {
    std::shared_ptr<privMessage> mess = std::make_shared<privMessage>(this->client.editPrivilege(targetUser,resPath, std::to_string(documentID), newPrivilege));
    try {
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        this->closeConnection();
        if(this->currentWindow==13){
            this->finestraOnlineUser->errorConnectionLogout(IMPOSSINVIARE);
        }else{
            this->finestraAllUser->errorConnectionLogout(IMPOSSINVIARE);
        }
    }
}

void clientdispatcher::shareResource(const std::string &resPath, const std::string &resName, const uri &newPrefs) {
    std::shared_ptr<uriMessage> mess = std::make_shared<uriMessage>(this->client.shareResource(resPath,resName,newPrefs));
    try {
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        this->closeConnection();
        if(this->currentWindow==8){
            this->finestraActiveCounterLink->errorConnectionLogout(IMPOSSINVIARE);
        }else if(this->currentWindow==9){
            this->finestraActiveTimerLink->errorConnectionLogout(IMPOSSINVIARE);
        }else if(this->currentWindow==10){
            this->finestraActiveAlwaysLink->errorConnectionLogout(IMPOSSINVIARE);
        }else{
            this->finestraActiveNonLink->errorConnectionLogout(IMPOSSINVIARE);
        }
    }
}

void clientdispatcher::renameResource(const std::string &resPath, const std::string &resName, const std::string &newName) {
    std::shared_ptr<askResMessage> mess = std::make_shared<askResMessage>(this->client.renameResource(resPath,resName,newName));
    try {
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        this->closeConnection();
        //dobbiamo notificare alla GUI
        this->finestraDirectory->errorConnectionLogout(IMPOSSINVIARE);
    }
}

void clientdispatcher::removeResource(const std::string &resPath, const std::string &resName) {
    std::shared_ptr<askResMessage> mess = std::make_shared<askResMessage>(this->client.removeResource(resPath,resName));
    try {
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        this->closeConnection();
        //dobbiamo notificare alla GUI
        this->finestraDirectory->errorConnectionLogout(IMPOSSINVIARE);
    }
}

void clientdispatcher::closeSource(uint_positive_cnt::type resourceId) {
    std::shared_ptr<updateDocMessage> mess = std::make_shared<updateDocMessage>(this->client.closeSource(resourceId));
    try {
        //inviamo il messaggio
        sendMessage(mess, resourceId);
        //eliminiamo il riferimento alla finestra del documento che è stato chiuso
        this->deleteActiveDocument(resourceId);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        this->finestraDirectory->errorConnectionLogout(IMPOSSINVIARE);
    }
}

void clientdispatcher::editUser(user &newUserData) {
    std::shared_ptr<userDataMessage> mess = std::make_shared<userDataMessage>(this->client.editUser(newUserData));
    try {
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        this->closeConnection();
        //dobbiamo notificare alla GUI
        this->finestraModificaUser->errorConnectionLogout(IMPOSSINVIARE);
    }
}

void clientdispatcher::removeUser(const std::string &pwd) {
    std::shared_ptr<clientMessage> mess = std::make_shared<clientMessage>(this->client.removeUser(pwd));
    try {
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        this->closeConnection();
        //dobbiamo notificare alla GUI
        this->finestraEliminaAccount->errorConnectionLogout(IMPOSSINVIARE);
    }
}

void clientdispatcher::logout() {
    std::shared_ptr<clientMessage> mess = std::make_shared<clientMessage>(this->client.logout());
    try {
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        this->finestraHome->errorConnection();
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

void clientdispatcher::moveMyCursor(uint_positive_cnt::type resId, int block, int column){
    std::shared_ptr<cursorMessage> mess = std::make_shared<cursorMessage>(this->client.updateCursorPos(resId, block, column));
    try {
        //inviamo il messaggio
        sendMessage(mess, resId);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        this->finestraDirectory->errorConnectionLogout(IMPOSSINVIARE);
    }
}

void clientdispatcher::moveUserCursor(uint_positive_cnt::type resId, int block, int column, uint_positive_cnt::type siteId){
    notepad* finestra = this->getCorrectNotepadbyResourceID(resId);
    finestra->moveUserCursor(siteId,block,column);
}

void clientdispatcher::addUserCursor(uint_positive_cnt::type siteID, std::string username, uint_positive_cnt::type resourceID){
    notepad* finestra = this->getCorrectNotepadbyResourceID(resourceID);
    finestra->addUserCursor(siteID,username);
}

void clientdispatcher::removeUserCursor(uint_positive_cnt::type siteID, uint_positive_cnt::type resourceID){
    notepad* finestra = this->getCorrectNotepadbyResourceID(resourceID);
    finestra->removeUserCursor(siteID);
}

std::string clientdispatcher::getStr(std::string ID_Cartella, std::string path){
    return this->client.directoryContent(ID_Cartella,path);
}


notepad* clientdispatcher::getCorrectNotepadbyResourceID(uint_positive_cnt::type resourceID){
    for (std::pair<uint_positive_cnt::type,notepad*> it:this->finestreDocumenti){
        if(it.first == resourceID)
            return (it.second);
    }
}

void clientdispatcher::deleteActiveDocument(uint_positive_cnt::type resourceID){
    int i=0,s=-1;
    for (std::pair<uint_positive_cnt::type,notepad*> it:this->finestreDocumenti){
        if(it.first == resourceID)
            s = i;
        i++;
    }
    if(s!=-1)
        this->finestreDocumenti.erase(this->finestreDocumenti.begin()+s);
}

void clientdispatcher::stopTimer(){
    qDebug() << "entrato in stopTimer";
    this->timer.stop();
    qDebug() << "Timer stoppato";
    if(!this->attese.empty()){
        qDebug() << "Coda attese non vuota, timer reinserito";
        std::pair<std::chrono::milliseconds, uint_positive_cnt::type> coppia = this->attese.front();
        std::chrono::milliseconds tempo = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()) - coppia.first;
        this->timer.start(TEMPOATTESA - tempo.count());
        ResIDofWaitingMessage = coppia.second;
        this->attese.pop();
    }
}

void clientdispatcher::successLogin(){
    this->finestraLogin->successSignIn();
}

void clientdispatcher::successLogout(){
    this->finestraHome->successLogout();
}

void clientdispatcher::successSignUp(){
    qDebug() << "entrato in successSignUp";
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

void clientdispatcher::successOpenSource(document &doc){
    this->setTextEdit(doc.getId(),this->finestraChoosePriv->successOpen(doc));
}

void clientdispatcher::successRemoveResource(){
    this->finestraDirectory->successRemove();
}

void clientdispatcher::successCreateNewDir(const std::string ID){
    this->finestraDirectory->successCreate(ID);
}

void clientdispatcher::successCreateNewSource(const std::string ID, document &doc){
    this->setTextEdit(doc.getId(),this->finestraDirectory->successNewSource(ID, doc));
}

void clientdispatcher::successRenameResource(){
    this->finestraDirectory->successRename();
}

void clientdispatcher::closeConnection(){
    this->socket.close();
}

const std::forward_list<std::pair<const user *, sessionData>> clientdispatcher::onlineUser(uint_positive_cnt::type documentID){
    return (this->client.onlineUsersonDocument(documentID));
}

std::unordered_map<std::string, privilege> clientdispatcher::allUser(uint_positive_cnt::type documentID){
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

void clientdispatcher::setSignIn(sigin* si){
    qDebug() << "metodo setSignIn";
    this->finestraLogin = si;
    qDebug() << "metodo setSignIn dopo assegnazione nuovo puntatore";
    this->currentWindow = 1;
    qDebug() << "metodo setSignIn dopo assegnazione a currentWindow";
}

void clientdispatcher::setTextEdit(uint_positive_cnt::type resourceID, notepad *te){
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

void clientdispatcher::setDirectory(::directory *dr){
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

/*void clientdispatcher::signinExpired(){
    this->timer.stop();
    this->finestraLogin->errorConnection();
    qDebug() << "Timer scaduto\n";
}/*/

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

void clientdispatcher::setchoosepriv(choosepriv *pr){
    this->finestraChoosePriv = pr;
    this->currentWindow = 16;
}

/*void clientdispatcher::signupExpired(){
    this->timer.stop();
    qDebug() << "Timer scaduto\n";
    //chiudiamo la connessione
    this->closeConnection();
    //dobbiamo notificare alla GUI
    this->finestraSignup->errorConnection();
}*/

/*void clientdispatcher::removeUserExpired(){
    this->timer.stop();
    qDebug() << "Timer scaduto\n";
    //chiudiamo la connessione
    this->closeConnection();
    //dobbiamo notificare alla GUI
    this->finestraEliminaAccount->errorConnectionLogout(TIMERSCADUTO);
}

void clientdispatcher::editUserExpired(){
    this->timer.stop();
    qDebug() << "Timer scaduto\n";
    //chiudiamo la connessione
    this->closeConnection();
    //dobbiamo notificare alla GUI
    this->finestraModificaUser->errorConnectionLogout(TIMERSCADUTO);
}

void clientdispatcher::openNewSourceExpired(){
    this->timer.stop();
    qDebug() << "Timer scaduto\n";
    //chiudiamo la connessione
    this->closeConnection();
    //dobbiamo notificare alla GUI
    this->finestraInsertUri->errorConnectionLogout(TIMERSCADUTO);
}

void clientdispatcher::openSourceExpired(){
    this->timer.stop();
    qDebug() << "Timer scaduto\n";
    //chiudiamo la connessione
    this->closeConnection();
    //dobbiamo notificare alla GUI
    //this->finestraDirectory->errorConnectionLogout(TIMERSCADUTO);
}

void clientdispatcher::editPrivilegeExpired(){
    this->timer.stop();
    qDebug() << "Timer scaduto\n";
    //chiudiamo la connessione
    this->closeConnection();
    //dobbiamo notificare alla GUI
    if(this->currentWindow==13){
        this->finestraOnlineUser->errorConnectionLogout(TIMERSCADUTO);
    }else{
        this->finestraAllUser->errorConnectionLogout(TIMERSCADUTO);
    }
}

void clientdispatcher::shareResourceExpired(){
    this->timer.stop();
    qDebug() << "Timer scaduto\n";
    //chiudiamo la connessione
    this->closeConnection();
    //dobbiamo notificare alla GUI
    if(this->currentWindow==8){
        this->finestraActiveCounterLink->errorConnectionLogout(TIMERSCADUTO);
    }else if(this->currentWindow==9){
        this->finestraActiveTimerLink->errorConnectionLogout(TIMERSCADUTO);
    }else if(this->currentWindow==10){
        this->finestraActiveAlwaysLink->errorConnectionLogout(TIMERSCADUTO);
    }else{
        this->finestraActiveNonLink->errorConnectionLogout(TIMERSCADUTO);
    }
}

void clientdispatcher::removeResourceExpired(){
    this->timer.stop();
    qDebug() << "Timer scaduto\n";
    //chiudiamo la connessione
    this->closeConnection();
    //dobbiamo notificare alla GUI
    //this->finestraDirectory->errorConnectionLogout(TIMERSCADUTO);
}

void clientdispatcher::createNewDirExpired(){
    this->timer.stop();
    qDebug() << "Timer scaduto\n";
    //chiudiamo la connessione
    this->closeConnection();
    //dobbiamo notificare alla GUI
    //this->finestraDirectory->errorConnectionLogout(TIMERSCADUTO);
}

void clientdispatcher::createNewSourceExpired(){
    this->timer.stop();
    qDebug() << "Timer scaduto\n";
    //chiudiamo la connessione
    this->closeConnection();
    //dobbiamo notificare alla GUI
    //this->finestraDirectory->errorConnectionLogout(TIMERSCADUTO);
}*/

/*void clientdispatcher::localRemoveExpired(){
    this->timer.stop();
    qDebug() << "Timer scaduto\n";

    // COSA FACCIAMO
}*/

/*void clientdispatcher::localInsertExpired(){
    this->timer.stop();
    qDebug() << "Timer scaduto\n";

    // COSA FACCIAMO??
}*/

/*void clientdispatcher::closeSourceExpired(){
    this->timer.stop();

    // COSA FACCIAMO??


    qDebug() << "Timer scaduto\n";
    //chiudiamo la connessione
    this->closeConnection();
    //dobbiamo notificare alla GUI
    //this->finestraDirectory->errorConnectionLogout(TIMERSCADUTO);
}*/

void clientdispatcher::TimerExpired(){
    this->timer.stop();
    qDebug() << "Timer scaduto\n";
    //chiudiamo la connessione
    this->closeConnection();
    //dobbiamo notificare alla GUI. Prendiamo la finestra giusta
    if(this->ResIDofWaitingMessage==0){
        switch (this->currentWindow) {
        case 1:{
            this->finestraLogin->errorConnection();
            break;
        }case 2:{
            this->finestraSignup->errorConnection();
            break;
        }case 3:{
            this->finestraInsertUri->errorConnectionLogout(TIMERSCADUTO);
            break;
        }case 5:{
            this->finestraEliminaAccount->errorConnectionLogout(TIMERSCADUTO);
            break;
        }case 7:{
            this->finestraModificaUser->errorConnectionLogout(TIMERSCADUTO);
            break;
        }case 8:{
            this->finestraActiveCounterLink->errorConnectionLogout(TIMERSCADUTO);
            break;
        }case 9:{
            this->finestraActiveTimerLink->errorConnectionLogout(TIMERSCADUTO);
            break;
        }case 10:{
            this->finestraActiveAlwaysLink->errorConnectionLogout(TIMERSCADUTO);
            break;
        }case 12:{
            this->finestraDirectory->errorConnectionLogout(TIMERSCADUTO);
            break;
        }case 13:{
            this->finestraOnlineUser->errorConnectionLogout(TIMERSCADUTO);
            break;
        }case 14:{
            this->finestraAllUser->errorConnectionLogout(TIMERSCADUTO);
            break;
        }case 15:{
            this->finestraActiveNonLink->errorConnectionLogout(TIMERSCADUTO);
            break;
        }case 16:{

        }
        }
    }else{
        //il timer è scaduto su un messaggio di localinsert, localremove, closesource o movemycursor, notifichiamo l'errore sulla finestra directory
        this->finestraDirectory->errorConnectionLogout(TIMERSCADUTO);
    }
}
