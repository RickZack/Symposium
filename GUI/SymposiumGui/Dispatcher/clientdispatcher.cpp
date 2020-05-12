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
#include <QApplication>
#include "../textedit.h"
#include "../onlineusers.h"
#include "../alluser.h"
#include "../../../filesystem.h"
#include "../mainwindow.h"
#include "clientdispatcher.h"


using namespace Symposium;

clientdispatcher::clientdispatcher(QObject *parent) : QObject(parent), requestDoc(tp)
{
    this->client.setClientDispatcher(this);
    this->userpwd = "";
    connect(&(this->timer), &QTimer::timeout, this, &clientdispatcher::TimerExpired);
}

int clientdispatcher::run(int argc, char **argv){
    QApplication a(argc, argv);
    QFontDatabase database;
    database.addApplicationFont(":/resources/font/baskvil.TTF");
    MainWindow w(nullptr, this->winmanager, *this);
    (this->winmanager).setActive(w);
    w.show();
    notepad notepadWindow(nullptr, 2, Symposium::privilege::owner, Symposium::privilege::owner, "",this->tp,w);
    notepadWindow.show();
    notepadWindow.showLabels();
    return a.exec();
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

        this->winmanager.activeWindow().failure(QString::fromStdString(mes->getErrDescr()));
        this->userpwd="";

    } catch (SymClientException& e){
        //eccezione di relatedMessage non trovato

        //DA VEDERE CHE MESSAGGIO DEVE DARE SULLA FINESTRA

        switch(currentWindow){
        case 1:{
            //this->finestraLogin->errorSignIn();
            break;
        }case 2:{
            //this->finestraSignup->errorSignUp("eccezione");
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
        TimerStart(tempo);
        qDebug() << "Sended to server: " << QString::fromStdString(ofs.str());
    }
}

void clientdispatcher::TimerStart(std::chrono::milliseconds timeToSend){
    if(this->timer.isActive()){
        //timer già attivo, quindi inseriamo in coda
        attese.push(timeToSend);
    }else{
        std::chrono::milliseconds difftemp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()) - timeToSend;
        int numero = TEMPOATTESA - difftemp.count();
        this->timer.start(numero);
    }
}

void clientdispatcher::signUp(const std::string &username, const std::string &pwd, const std::string &nickname, const std::string &iconPath){
    this->userpwd = pwd;
    this->username = username;
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
        this->winmanager.activeWindow().failure("-1");
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
        this->winmanager.activeWindow().failure("-1");
    }
}

void clientdispatcher::autologIn(){
    logIn(this->username, this->userpwd);
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
        this->winmanager.activeWindow().failure("-1");
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
        this->winmanager.activeWindow().failure("-1");
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
        this->winmanager.activeWindow().failure("-1");
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
        this->winmanager.activeWindow().failure("-1");
    }
}

void clientdispatcher::localInsert(uint_positive_cnt::type resourceId, const symbol &newSym, const std::pair<int, int> &index) {
    std::shared_ptr<symbolMessage> mess = std::make_shared<symbolMessage>(this->client.localInsert(resourceId,newSym,index));
    try {
        //inviamo il messaggio
        sendMessage(mess, resourceId);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        this->closeConnection();
        //dobbiamo notificare alla GUI
        this->winmanager.activeWindow().failure("-1");
    }
}

void clientdispatcher::localRemove(uint_positive_cnt::type resourceId, const std::pair<int, int> indexes) {
    std::shared_ptr<symbolMessage> mess = std::make_shared<symbolMessage>(this->client.localRemove(resourceId,indexes));
    try {
        //inviamo il messaggio
        sendMessage(mess, resourceId);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        this->closeConnection();
        //dobbiamo notificare alla GUI
        this->winmanager.activeWindow().failure("-1");
    }
}
//TODO: Quando arrivate a definire questi metodi ricordatemi di cambiare SymNotepadWinInterface:
// avevo pensato che doveva offrire solo quei tre metodi ma visto che avete già definito degli altri nomi
// ha più senso che lo adeguo quella classe. Essa avrà quindi i metodi: remoteInsert(), remoteDelete(),
// verifySymbol(), moveUserCursor(), addUserCursor() e removeUserCursor().
// In questo modo non sarà necessario usare il cast
void clientdispatcher::remoteInsert(uint_positive_cnt::type resourceId, const symbol &newSym, uint_positive_cnt::type siteId, std::pair<unsigned int, unsigned int> index){
    //FIXME: Usa static_cast
    // notepad& n2=static_cast<notepad&>(this->winmanager.getNotepad(resourceId));
    notepad& n = (notepad&)(this->winmanager.getNotepad(resourceId));
    n.remoteInsert(newSym, siteId, index);
}

void clientdispatcher::remoteRemove(uint_positive_cnt::type resourceId, uint_positive_cnt::type siteId, std::pair<int, int> indexes){
    //FIXME: Usa static_cast
    notepad& n = (notepad&)(this->winmanager.getNotepad(resourceId));
    n.remoteDelete(indexes,siteId);
}

void clientdispatcher::verifySymbol(uint_positive_cnt::type resId, const symbol &newSym, std::pair<int, int> indexes){
    //FIXME: Usa static_cast
    notepad& n = (notepad&)(this->winmanager.getNotepad(resId));
    n.verifySymbol(newSym, indexes);
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
            this->finestraOnlineUser->errorConnectionLogout();
        }else{
            this->finestraAllUser->errorConnectionLogout();
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
            this->finestraActiveCounterLink->errorConnectionLogout();
        }else if(this->currentWindow==9){
            this->finestraActiveTimerLink->errorConnectionLogout();
        }else if(this->currentWindow==10){
            this->finestraActiveAlwaysLink->errorConnectionLogout();
        }else{
            this->finestraActiveNonLink->errorConnectionLogout();
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
        this->winmanager.activeWindow().failure("-1");
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
        this->winmanager.activeWindow().failure("-1");
    }
}

void clientdispatcher::closeSource(uint_positive_cnt::type resourceId) {
    std::shared_ptr<updateDocMessage> mess = std::make_shared<updateDocMessage>(this->client.closeSource(resourceId));
    try {
        //inviamo il messaggio
        sendMessage(mess, resourceId);
        //eliminiamo il riferimento alla finestra del documento che è stato chiuso
        this->winmanager.removeNotepad(resourceId);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        this->closeConnection();
        //dobbiamo notificare alla GUI
        this->winmanager.activeWindow().failure("-1");
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
        this->winmanager.activeWindow().failure("-1");
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
        this->winmanager.activeWindow().failure("-1");
    }
}

void clientdispatcher::logout() {
    std::shared_ptr<clientMessage> mess = std::make_shared<clientMessage>(this->client.logout());
    try {
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        //dobbiamo notificare alla GUI
        this->winmanager.activeWindow().failure("-1");
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
        this->closeConnection();
        //dobbiamo notificare alla GUI
        this->winmanager.activeWindow().failure("-1");
    }
}

void clientdispatcher::moveUserCursor(uint_positive_cnt::type resId, int block, int column, uint_positive_cnt::type siteId){
    //FIXME: Usa static_cast
    notepad& finestra = (notepad&)(this->winmanager.getNotepad(resId));
    finestra.moveUserCursor(siteId,block,column);
}

void clientdispatcher::addUserCursor(uint_positive_cnt::type siteID, std::string username, uint_positive_cnt::type resourceID){
    //FIXME: Usa static_cast
    notepad& finestra = (notepad&)(this->winmanager.getNotepad(resourceID));
    finestra.addUserCursor(siteID,username);
}

void clientdispatcher::removeUserCursor(uint_positive_cnt::type siteID, uint_positive_cnt::type resourceID){
    //FIXME: Usa static_cast
    notepad& finestra = (notepad&)(this->winmanager.getNotepad(resourceID));
    finestra.removeUserCursor(siteID);
}

std::string clientdispatcher::getStr(std::string ID_Cartella, std::string path){
    return this->client.directoryContent(ID_Cartella,path);
}

void clientdispatcher::stopTimer(){
    qDebug() << "entrato in stopTimer";
    this->timer.stop();
    qDebug() << "Timer stoppato";
    if(!this->attese.empty()){
        qDebug() << "Coda attese non vuota, timer reinserito";
        std::chrono::milliseconds estratto = this->attese.front();
        std::chrono::milliseconds tempo = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()) - estratto;
        this->timer.start(TEMPOATTESA - tempo.count());
        this->attese.pop();
    }
}

void clientdispatcher::successAction(){
    this->winmanager.activeWindow().success();
    this->userpwd = "";
}

void clientdispatcher::successEditPrivilege(){
    if(this->currentWindow==13){
       this->finestraOnlineUser->successEditPrivilege();
    }else{
       this->finestraAllUser->successEditPrivilege();
    }
}

void clientdispatcher::successShareResource(std::string path){
    /*if(this->currentWindow==8){
        this->finestraActiveCounterLink->successLink(path);
    }else if(this->currentWindow==9){
        this->finestraActiveTimerLink->successLink(path);
    }else if(this->currentWindow==10){
        this->finestraActiveAlwaysLink->successLink(path);
    }else{
        this->finestraActiveNonLink->successLink(path);
    }*/
}

void clientdispatcher::successOpenSource(document &doc){
    this->requestDoc = doc;
    this->successAction();
}

const document& clientdispatcher::getOpenDocument(){
    return this->requestDoc;
}

void clientdispatcher::successCreateNewSource(const document &doc){
    this->requestDoc = doc;
    this->successAction();
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

void clientdispatcher::setOnlineUser(onlineusers *ou){
    this->finestraOnlineUser = ou;
    this->currentWindow = 13;
}

void clientdispatcher::setAllUser(alluser *au){
    this->finestraAllUser = au;
    this->currentWindow = 14;
}

void clientdispatcher::TimerExpired(){
    this->timer.stop();
    qDebug() << "Timer scaduto\n";
    //chiudiamo la connessione
    this->closeConnection();
    //dobbiamo notificare alla GUI.
    this->winmanager.activeWindow().failure("-1");
}
