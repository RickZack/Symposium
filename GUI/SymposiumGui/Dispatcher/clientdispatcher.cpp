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

clientdispatcher::clientdispatcher(QObject *parent) : QObject(parent), appIsClosing(false)
{
    this->client.setClientDispatcher(this);
    connect(&(this->timer), &QTimer::timeout, this, &clientdispatcher::TimerExpired);
    connect(&(this->timer_to_read), &QTimer::timeout, this, &clientdispatcher::continueReadyRead);
    this->userIsLogged = false;
    this->connectionClosed = true;
}

int clientdispatcher::run(int argc, char **argv){
    QApplication a(argc, argv);
    QFontDatabase database;
    database.addApplicationFont(":/resources/font/baskvil.TTF");
    MainWindow w(nullptr, this->winmanager, *this);
    (this->winmanager).setActive(w);
    w.show();
    return a.exec();
}

void clientdispatcher::setServerAddress(std::string address, std::string port){
    this->svAddress = QHostAddress(QString::fromStdString(address));
    this->svPort = std::stoi(port);
}

std::pair<std::string, std::string> clientdispatcher::getServerAddress(){
    return (std::make_pair<std::string, std::string>(this->svAddress.toString().toStdString(), std::to_string(this->svPort)));
}

void clientdispatcher::openConnection(){
    if (this->socket.state()==QAbstractSocket::UnconnectedState){
        qDebug() << "Connecting the socket to the server...";
        //impostiamo il keep_alive
        this->socket.setSocketOption(QAbstractSocket::KeepAliveOption, 1);
        //connettiamo il socket all'indirizzo del server
        this->socket.connectToHost(svAddress, svPort);
        //controlliamo se la connessione è andata a buon fine
        if(!this->socket.waitForConnected()){
            this->winmanager.activeWindow().failure("-1");
        }else{
            this->connectionClosed = false;
            //quando riceviamo qualcosa eseguiamo la funzione di lettura (readyRead)
            connect(&(this->socket), &QIODevice::readyRead, this, &clientdispatcher::readyRead);
            //quando il socket si sconnette, chiamiamo il metodo connectionLost()
            connect(&(this->socket), SIGNAL(disconnected()), this, SLOT(connectionLost()));
            qDebug() << "Connection Successful";
        }

    }
}

void clientdispatcher::readyRead(){
    //questa funzione viene chiamata quando il server ci ha inviato qualcosa
    //creiamo la variabile che conta quanti messaggi sono processati nel ciclo
    int messageProcessed = 0;
    //creiamo la variabile che conterra il puntatore al messaggio ricevuto dal socket
    std::shared_ptr<serverMessage> mes;
    //creiamo il QByteArray che conterrà ciò che riceviamo dal socket
    QByteArray byteArray;
    //associamo il datastream al socket
    QDataStream in(&(this->socket));
    //vediamo quanti byte sono disponibili per la lettura
    while(this->socket.bytesAvailable()){
        //se i messaggi già processati sono più di 20, usciamo ed emettiamo il segnale
        //per poi rientrare nella readyRead
        if(messageProcessed > 20){
            qDebug() << "Numero massimo di pacchetti processato;";
            this->timer_to_read.start(TEMPOMAXREADYREAD);
            return;
        }
        qDebug() << "Pacchetto " << messageProcessed+1 << " - Bytes da leggere: " << this->socket.bytesAvailable();
        //stream di stringa che conterrà i dati che abbiamo ricevuto, prima di essere de-serializzati
        std::stringstream accumulo;
        //facciamo partire la transazione
        in.startTransaction();
        //leggiamo i dati ricevuti
        in >> byteArray;
        //controlliamo se ci sono stati errori
        if ((!in.commitTransaction()) && (in.status()!=QDataStream::Ok)){
            //errore di ricezione
            this->socket.flush();
            return;
        }
        //eseguiamo la conversione in stringa
        std::string ricevuto(byteArray.constData(), byteArray.length());
        //inseriamo quanto ricevuto nel stringstream
        accumulo << ricevuto;
        try {
            //deserializziamo il messaggio ricevuto
            boost::archive::text_iarchive ia(accumulo);
            ia >> mes;
        } catch (boost::archive::archive_exception b) {
            qDebug() << "Deserialization ERROR: " << b.what();
        }

        try {
            /*
             * When the app is closing no need to notify the user with action's outcome.
             * Can be dangerous to do so because is case of logout a success() invocation is expected to happen
             */
            if(!appIsClosing)
                mes->invokeMethod(this->client);
            if(mes->getAction()==msgType::login){
                this->userIsLogged = true;
            }else if(mes->getAction()==msgType::logout){
                this->userIsLogged = false;
            }
        } catch (messageException& e) {
            //eccezione di insuccesso dell'operazione
            this->winmanager.activeWindow().failure(QString::fromStdString(mes->getErrDescr()));
            this->userpwd="";

        } catch (SymClientException& e){
            //eccezione di relatedMessage non trovato

            //DA VEDERE CHE MESSAGGIO DEVE DARE SULLA FINESTRA

        }
        messageProcessed++;
    }

}

void clientdispatcher::sendMessage(const std::shared_ptr<clientMessage> MessageToSend){
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
        if(!appIsClosing && !connectionClosed)
            throw sendFailure();
    }else{
        std::chrono::milliseconds tempo = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        TimerStart(tempo);
        qDebug() << "Sended to server: " << QString::fromStdString(ofs.str());
    }
}

void clientdispatcher::connectionLost(){
    if(!appIsClosing){
        this->connectionClosed = true;
        this->winmanager.activeWindow().failure("-1");
    }
}

void clientdispatcher::continueReadyRead(){
    this->timer_to_read.stop();
    emit readyRead();
}

void clientdispatcher::TimerStart(std::chrono::milliseconds timeToSend){
    //se chiudiamo l'applicazione non impostiamo il timer in quanto inutile
    if(this->appIsClosing==false){
        if(this->timer.isActive()){
            //timer già attivo, quindi inseriamo in coda
            attese.push(timeToSend);
        }else{
            std::chrono::milliseconds difftemp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()) - timeToSend;
            int numero = TEMPOATTESA - difftemp.count();
            this->timer.start(numero);
        }
    }
}

void clientdispatcher::signUp(const std::string &username, const std::string &pwd, const std::string &nickname, const std::string &iconPath){
    this->userIsLogged = false;
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
        this->closeApp();
        //dobbiamo notificare alla GUI
        this->winmanager.activeWindow().failure("-1");
    }
}

void clientdispatcher::logIn(const std::string &username, const std::string &pwd) {
    this->userIsLogged = false;
    std::shared_ptr<clientMessage> mess = std::make_shared<clientMessage>(this->client.logIn(username,pwd));
    //Colleghiamo il client al server
    this->openConnection();
    try {
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        //chiudiamo la connessione
        this->closeApp();
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
        this->closeApp();
        //dobbiamo notificare alla GUI
        this->winmanager.activeWindow().failure("-1");
    }
}

void clientdispatcher::openNewSource(const std::string &absolutePath, privilege reqPriv, const std::string &destPath, const std::string& destName) {
    qDebug() << "resourceID: " << QString::fromStdString(absolutePath) << ", destPath: " << QString::fromStdString(destPath) << ", destName: " << QString::fromStdString(destName);
    std::shared_ptr<askResMessage> mess = std::make_shared<askResMessage>(this->client.openNewSource(absolutePath,reqPriv,destPath,destName));
    try {
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        this->closeApp();
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
        this->closeApp();
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
        this->closeApp();
        //dobbiamo notificare alla GUI
        this->winmanager.activeWindow().failure("-1");
    }
}

void clientdispatcher::localInsert(uint_positive_cnt::type resourceId, const symbol &newSym, const std::pair<int, int> &index) {
    std::shared_ptr<symbolMessage> mess = std::make_shared<symbolMessage>(this->client.localInsert(resourceId,newSym,index));
    try {
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        this->closeApp();
        //dobbiamo notificare alla GUI
        this->winmanager.activeWindow().failure("-1");
    }
}

void clientdispatcher::localRemove(uint_positive_cnt::type resourceId, const std::pair<int, int> indexes) {
    std::shared_ptr<symbolMessage> mess = std::make_shared<symbolMessage>(this->client.localRemove(resourceId,indexes));
    try {
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        this->closeApp();
        //dobbiamo notificare alla GUI
        this->winmanager.activeWindow().failure("-1");
    }
}

void clientdispatcher::remoteInsert(uint_positive_cnt::type resourceId, const symbol &newSym, uint_positive_cnt::type siteId, std::pair<unsigned int, unsigned int> index){
    this->winmanager.getNotepad(resourceId).remoteInsert(newSym,siteId,index);
}

void clientdispatcher::remoteRemove(uint_positive_cnt::type resourceId, uint_positive_cnt::type siteId, std::pair<int, int> indexes){
    this->winmanager.getNotepad(resourceId).remoteDelete(indexes,siteId);
}

void clientdispatcher::verifySymbol(uint_positive_cnt::type resId, const symbol &newSym, std::pair<int, int> indexes){
    this->winmanager.getNotepad(resId).verifySymbol(newSym,indexes);
}

Color clientdispatcher::getColor(uint_positive_cnt::type documentID, uint_positive_cnt::type siteID){
    return this->client.colorOfUser(documentID, siteID);
}

Color clientdispatcher::getColorbyUsername(uint_positive_cnt::type documentID, const std::string& username){
    return this->client.colorOfUserbyUsername(documentID, username);
}

void clientdispatcher::editPrivilege(const std::string &targetUser, std::string &resPath, privilege newPrivilege, uint_positive_cnt::type documentID) {
    std::shared_ptr<privMessage> mess = std::make_shared<privMessage>(this->client.editPrivilege(targetUser,resPath, std::to_string(documentID), newPrivilege));
    try {
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        this->closeApp();
        //dobbiamo notificare alla GUI
        this->winmanager.activeWindow().failure("-1");
    }
}

void clientdispatcher::shareResource(const std::string &resPath, const std::string &resName, const uri &newPrefs) {
    std::shared_ptr<uriMessage> mess = std::make_shared<uriMessage>(this->client.shareResource(resPath,resName,newPrefs));
    try {
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        this->closeApp();
        //dobbiamo notificare alla GUI
        this->winmanager.activeWindow().failure("-1");
    }
}

void clientdispatcher::renameResource(const std::string &resPath, const std::string &resName, const std::string &newName) {
    std::shared_ptr<askResMessage> mess = std::make_shared<askResMessage>(this->client.renameResource(resPath,resName,newName));
    try {
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        this->closeApp();
        //dobbiamo notificare alla GUI
        this->winmanager.activeWindow().failure("-1");
    }
}

void clientdispatcher::removeResource(const std::string &resPath, const std::string &resName) {
    if(this->client.controlFileIsActive(std::stoi(resName))){
        failureRemovedResource();
    }else{
        std::shared_ptr<askResMessage> mess = std::make_shared<askResMessage>(this->client.removeResource(resPath,resName));
        try {
            //inviamo il messaggio
            sendMessage(mess);
        } catch (clientdispatcher::sendFailure) {
            //errore nell'invio del messaggio
            this->closeApp();
            //dobbiamo notificare alla GUI
            this->winmanager.activeWindow().failure("-1");
        }
    }
}

void clientdispatcher::closeSource(uint_positive_cnt::type resourceId) {
    std::shared_ptr<updateDocMessage> mess = std::make_shared<updateDocMessage>(this->client.closeSource(resourceId));
    try {
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        this->closeApp();
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
        this->closeApp();
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
        this->closeApp();
        //dobbiamo notificare alla GUI
        this->winmanager.activeWindow().failure("-1");
    }
}

void clientdispatcher::logout() {
    if(this->userIsLogged){
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
}

void clientdispatcher::mapSiteIdToUser(const document &currentDoc) {
    std::shared_ptr<updateDocMessage> mess = std::make_shared<updateDocMessage>(this->client.mapSiteIdToUser(currentDoc));
    try {
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        this->closeApp();
        //dobbiamo notificare alla GUI
        this->winmanager.activeWindow().failure("-1");
    }
}

void clientdispatcher::moveMyCursor(uint_positive_cnt::type resId, int block, int column){
    std::shared_ptr<cursorMessage> mess = std::make_shared<cursorMessage>(this->client.updateCursorPos(resId, block, column));
    try {
        //inviamo il messaggio
        sendMessage(mess);
    } catch (clientdispatcher::sendFailure) {
        //errore nell'invio del messaggio
        this->closeApp();
        //dobbiamo notificare alla GUI
        this->winmanager.activeWindow().failure("-1");
    }
}

void clientdispatcher::moveUserCursor(uint_positive_cnt::type resId, int block, int column, uint_positive_cnt::type siteId){
    this->winmanager.getNotepad(resId).moveUserCursor(siteId,block,column);
}

void clientdispatcher::addUserCursor(uint_positive_cnt::type siteID, std::string username, uint_positive_cnt::type resourceID){
    this->winmanager.getNotepad(resourceID).addUserCursor(siteID,username);
}

void clientdispatcher::addUserReadOnly(uint_positive_cnt::type resourceID){
    this->winmanager.getNotepad(resourceID).insertusers();
}

void clientdispatcher::removeUserCursor(uint_positive_cnt::type siteID, uint_positive_cnt::type resourceID){
    this->winmanager.getNotepad(resourceID).removeUserCursor(siteID);
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


void clientdispatcher::updateRequestDocFileandSuccess(uint_positive_cnt::type docID, uint_positive_cnt::type fileID){
    this->openDocumentID = docID;
    this->openFileID = fileID;
    this->successAction();
}

const document& clientdispatcher::getOpenDocument(){
    return this->client.getActiveDocumenttoOpenbyID(this->openDocumentID);
}

uint_positive_cnt::type clientdispatcher::getOpenFileID(){
    return this->openFileID;
}

privilege clientdispatcher::getMyPrivilegeOnFileOpen(){
    return this->client.getActiveFiletoOpenbyID(this->openFileID).getUserPrivilege(this->getUser().getUsername());
}

void clientdispatcher::closeApp(){
    winmanager.closeAllNotepads();
    appIsClosing=true;
    qApp->quit();
}

const std::forward_list<std::pair<const user *, sessionData>> clientdispatcher::onlineUser(uint_positive_cnt::type documentID){
    return (this->client.onlineUsersonDocument(documentID));
}

std::unordered_map<std::string, privilege> clientdispatcher::allUser(uint_positive_cnt::type documentID){
    return (this->client.allUsersonDocument(documentID));
}

void clientdispatcher::successSetUserColors(uint_positive_cnt::type docID){
    this->winmanager.getNotepad(docID).success();
}

const user& clientdispatcher::getUser(){
    return this->client.userData();
}

uint_positive_cnt::type clientdispatcher::getHomeIDofCurrentUser(){
    return this->client.userData().getHome()->getId();
}

std::string clientdispatcher::showHome(){
    return this->client.showDir();
}

std::string clientdispatcher::showDir(bool recursive){
    return this->client.showDir(recursive);
}

void clientdispatcher::TimerExpired(){
    this->timer.stop();
    qDebug() << "Timer scaduto\n";
    //dobbiamo notificare alla GUI.
    if(this->userIsLogged)
        logout();
    this->winmanager.activeWindow().failure("-1");
}

void clientdispatcher::failureRemovedResource(){
    this->winmanager.activeWindow().failure("You need to close the document first");
}

clientdispatcher::~clientdispatcher() {
    logout();
}

void clientdispatcher::localEditLineStyle(uint_positive_cnt::type docId,
                                          const std::pair<alignType, unsigned int> &oldLineStyle,
                                          const std::pair<alignType, unsigned int> &newLineStyle, unsigned int row) {
//TODO: implement
}

void clientdispatcher::remoteEditLineStyle(uint_positive_cnt::type docId,
                                           const std::pair<alignType, unsigned int> &newLineStyle, unsigned int row) {
//TODO: implement
}
