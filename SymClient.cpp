/*
 * Copyright (c) 2019 Riccardo Zaccone, Ksenia Del Conte Akimova, Alice Morano, Martina Bellissimo
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
 * File:   SymClient.cpp
 * Project: Symposium
 * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Alice Morano <s259158 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 24 Giugno 2019, 19.25
 */

//#define DISPATCHER_ON

#include "SymClient.h"
#include "uri.h"
#include "message.h"
#include "filesystem.h"
#include "SymposiumException.h"
#ifdef DISPATCHER_ON
#include "GUI/SymposiumGui/Dispatcher/clientdispatcher.h"
#endif

using namespace Symposium;

SymClient::SymClient(): loggedUser("username", "P@assW0rd!!", "noempty", "", 0, nullptr), activeFile(), activeDoc(), userColors(), unanswered(){
}

void SymClient::setLoggedUser(const user &loggedUser) {
    this->loggedUser = loggedUser;
}

signUpMessage SymClient::signUp(const std::string &username, const std::string &pwd, const std::string &nickname,
                                const std::string &iconPath) {
    std::shared_ptr<signUpMessage> mess(new signUpMessage(msgType::registration, {username, pwd}, user(username, pwd, nickname, iconPath, 0, nullptr)));
    unanswered.push_front(mess);
    return *mess;
}

void SymClient::signUp(const user &logged) {
    setLoggedUser(logged);
    //facciamo partire il flusso del login
    #ifdef DISPATCHER_ON
    this->dispatcher->autologIn();
    #endif
}

clientMessage SymClient::logIn(const std::string &username, const std::string &pwd) {
    std::shared_ptr<clientMessage> mess(new clientMessage(msgType::login, {username, pwd}));
    unanswered.push_front(mess);
    return *mess;
}

void SymClient::logIn(const user &logged) {
    setLoggedUser(logged);
    #ifdef DISPATCHER_ON
    //notifichiamo alla gui il successo
    this->dispatcher->successAction();
    /*if(this->dispatcher->isAutoLogin())
        this->dispatcher->successSignUp();
    else
        this->dispatcher->successLogin();*/
    #endif
}

askResMessage SymClient::openSource(const std::string &path, const std::string &name, privilege reqPriv) {
    std::shared_ptr<askResMessage> mess(new askResMessage(msgType::openRes, {SymClient::getLoggedUser().getUsername(),""}, path, name, "", reqPriv, 0));
    unanswered.push_front(mess);
    return *mess;
}

void SymClient::openSource(const std::string &path, const std::string &name, const std::shared_ptr<file> fileAsked,
                           privilege reqPriv) {
    document& doc = fileAsked->access(this->getLoggedUser(), reqPriv);
    auto p=getLoggedUser().openFile(path, name, reqPriv);
    p->replacement(fileAsked);
    activeFile.push_front(fileAsked);
    colorGen c;
    activeDoc.push_front({&doc, c});
    this->userColors.insert(std::pair<std::pair<uint_positive_cnt::type, uint_positive_cnt::type>, std::pair<user, Color>>
    (std::make_pair(this->getLoggedUser().getSiteId(),doc.getId()),std::make_pair(this->getLoggedUser(),c())));
    //notifichiamo alla gui il successo
    #ifdef DISPATCHER_ON
    this->dispatcher->successOpenSource(doc);
    #endif
}

askResMessage
SymClient::openNewSource(const std::string &resourceId, privilege reqPriv, const std::string &destPath, const std::string& destName) {
    std::shared_ptr<askResMessage> mess(new askResMessage(msgType::openNewRes, {SymClient::getLoggedUser().getUsername(), ""}, destPath, destName, resourceId, reqPriv, 0));
    unanswered.push_front(mess);
    return *mess;
}

void SymClient::openNewSource(const std::string &resId, privilege reqPriv, const std::string &destPath,
                              const std::string &destName,
                              uint_positive_cnt::type idToAssign, const std::shared_ptr<file> fileAsked) {
    std::string filePath = resId.substr(0,resId.find_last_of("/"));
    std::string fileName = resId.substr(resId.find_last_of("/")+1);
    (this->getLoggedUser().getHome()).get()->addLink(destPath, destName, filePath, fileName, idToAssign);
    activeFile.push_front(fileAsked);
    document& doc = fileAsked->access(this->getLoggedUser(), reqPriv);
    colorGen c;
    activeDoc.push_front({&doc,c});
    this->userColors.insert(std::pair<std::pair<uint_positive_cnt::type, uint_positive_cnt::type>, std::pair<user, Color>>
                                    (std::make_pair(this->getLoggedUser().getSiteId(),doc.getId()),std::make_pair(this->getLoggedUser(),c())));
	//notifichiamo alla gui il successo
    #ifdef DISPATCHER_ON
    this->dispatcher->successInsertUri();
    #endif
}

askResMessage SymClient::createNewSource(const std::string &path, const std::string &name) {
    std::shared_ptr<askResMessage> mess(new askResMessage(msgType::createRes, {SymClient::getLoggedUser().getUsername(), ""}, path, name, "", uri::getDefaultPrivilege(), 0));
    unanswered.push_front(mess);
    return *mess;
}

void SymClient::createNewSource(const std::string &path, const std::string &name, uint_positive_cnt::type idToAssign) {
    std::shared_ptr<file> file = this->getLoggedUser().newFile(name, path, idToAssign);
    document& docReq = file->access(this->getLoggedUser(), privilege::owner);
    activeFile.push_front(file);
    colorGen c;
    activeDoc.push_front({&docReq,c});
    this->userColors.insert(std::pair<std::pair<uint_positive_cnt::type, uint_positive_cnt::type>, std::pair<user, Color>>
                                    (std::make_pair(this->getLoggedUser().getSiteId(),docReq.getId()),std::make_pair(this->getLoggedUser(),c())));
    //notifichiamo alla gui il successo
    #ifdef DISPATCHER_ON
    //this->dispatcher->successCreateNewSource(std::to_string(idToAssign), docReq);
    this->dispatcher->successCreateNewSource(docReq);
    #endif
}

askResMessage SymClient::createNewDir(const std::string &path, const std::string &name) {
    std::shared_ptr<askResMessage> mess(new askResMessage(msgType::createNewDir, {SymClient::getLoggedUser().getUsername(), ""}, path, name, "", uri::getDefaultPrivilege(), 0));
    unanswered.push_front(mess);
    return *mess;
}

void SymClient::createNewDir(const std::string &path, const std::string &name, uint_positive_cnt::type idToAssign) {
    this->getLoggedUser().newDirectory(name,path,idToAssign);
    //notifichiamo alla gui il successo
    #ifdef DISPATCHER_ON
    //this->dispatcher->successCreateNewDir(std::to_string(idToAssign));
    this->dispatcher->successAction();
    #endif
}


symbolMessage SymClient::localInsert(uint_positive_cnt::type resourceId, const symbol &newSym, const std::pair<unsigned int, unsigned int> &index) {
    document* d = this->getActiveDocumentbyID(resourceId);
    d->localInsert(index, const_cast<symbol &>(newSym));
    std::shared_ptr<symbolMessage> mess (new symbolMessage(msgType::insertSymbol, {SymClient::getLoggedUser().getUsername(), ""}, msgOutcome::success, SymClient::getLoggedUser().getSiteId(), resourceId, newSym));
    this->unanswered.push_front(mess);
    return *mess;
}

symbolMessage SymClient::localRemove(uint_positive_cnt::type resourceId, const std::pair<unsigned int, unsigned int> indexes) {
    document* d = this->getActiveDocumentbyID(resourceId);
    symbol s = d->localRemove(indexes, getLoggedUser().getSiteId());
    std::shared_ptr<symbolMessage> mess (new symbolMessage(msgType::removeSymbol, {SymClient::getLoggedUser().getUsername(), ""}, msgOutcome::success, SymClient::getLoggedUser().getSiteId(), resourceId, s));
    this->unanswered.push_front(mess);
    return *mess;
}

void SymClient::remoteInsert(uint_positive_cnt::type siteId, uint_positive_cnt::type resourceId, const symbol &newSym) {
    document* d = this->getActiveDocumentbyID(resourceId);
    std::pair<unsigned int, unsigned int> p = d->remoteInsert(siteId, newSym);
    //notifica alla gui
    #ifdef DISPATCHER_ON
    this->dispatcher->remoteInsert(resourceId,newSym, siteId, p);
    #endif
}

void SymClient::remoteRemove(uint_positive_cnt::type siteId, uint_positive_cnt::type resourceId, const symbol &rmSym) {
    document* d = this->getActiveDocumentbyID(resourceId);
    std::pair<unsigned int, unsigned int> p = d->remoteRemove(siteId, rmSym);
    //notifica alla gui
    #ifdef DISPATCHER_ON
    this->dispatcher->remoteRemove(resourceId, siteId, p);
    #endif
}

privMessage SymClient::editPrivilege(const std::string &targetUser, const std::string &resPath, const std::string &resName,
                                     privilege newPrivilege) {
    std::shared_ptr<privMessage> mess (new privMessage(msgType::changePrivileges, {this->getLoggedUser().getUsername(),""}, msgOutcome::success, resPath + "/" + resName, targetUser, newPrivilege));
    unanswered.push_front(mess);
    return *mess;
}

privilege SymClient::editPrivilege(const std::string &targetUser, const std::string &resPath, const std::string &resName,
                                   privilege newPrivilege, bool msgRcv) {
    privilege* p = new privilege(this->getLoggedUser().editPrivilege(targetUser, resPath, resName, newPrivilege));
    //notifichiamo alla gui il successo
    #ifdef DISPATCHER_ON
    this->dispatcher->successEditPrivilege();
    #endif
    return *p;
}

uriMessage SymClient::shareResource(const std::string &resPath, const std::string &resName, const uri &newPrefs) {    std::shared_ptr<uriMessage> mess (new uriMessage(msgType::shareRes, {SymClient::getLoggedUser().getUsername(), ""}, msgOutcome::success, resPath, resName, newPrefs, 0));
    unanswered.push_front(mess);
    return *mess;
}

std::shared_ptr<filesystem> SymClient::shareResource(const std::string &resPath, const std::string &resName, const uri &newPrefs, bool msgRcv) {    //TODO: modified this method, return the file: FATTO
    std::shared_ptr<filesystem> fil (this->getLoggedUser().shareResource(resPath, resName, newPrefs));
    //notifichiamo alla gui il successo
    #ifdef DISPATCHER_ON
    this->dispatcher->successShareResource("/" + resPath);
    #endif
    return fil;
}

askResMessage
SymClient::renameResource(const std::string &resPath, const std::string &resName, const std::string &newName) {
    std::shared_ptr<askResMessage> mess(new askResMessage(msgType::changeResName, {SymClient::getLoggedUser().getUsername(), ""}, resPath, resName, newName, uri::getDefaultPrivilege(), 0));
    unanswered.push_front(mess);
    return *mess;
}

std::shared_ptr<filesystem>
SymClient::renameResource(const std::string &resPath, const std::string &resName, const std::string &newName,
                          bool msgRcv) {
    std::shared_ptr<filesystem> f (getLoggedUser().renameResource(resPath, resName, newName));
    //notifichiamo alla gui il successo
    #ifdef DISPATCHER_ON
    //this->dispatcher->successRenameResource();
    this->dispatcher->successAction();
    #endif
    return f;
}

askResMessage SymClient::removeResource(const std::string &resPath, const std::string &resName) {
    std::shared_ptr<askResMessage> mess(new askResMessage(msgType::removeRes, {SymClient::getLoggedUser().getUsername(), ""}, resPath, resName, "", uri::getDefaultPrivilege(), 0));
    unanswered.push_front(mess);
    return *mess;
}

std::shared_ptr<filesystem>
SymClient::removeResource(const std::string &resPath, const std::string &resName, bool msgRcv) {
    std::shared_ptr<filesystem> f = this->getLoggedUser().removeResource(resPath,resName);
    //notifichiamo alla gui il successo
    #ifdef DISPATCHER_ON
    //this->dispatcher->successRemoveResource();
    this->dispatcher->successAction();
    #endif
    return f;
}

std::string SymClient::showDir(bool recursive) const {
    return getLoggedUser().showDir(recursive);
}

updateDocMessage SymClient::closeSource(uint_positive_cnt::type resourceId) {
    document* d = getActiveDocumentbyID(resourceId);
    activeFile.remove_if([resourceId](std::shared_ptr<file> it){return (it->getDoc().getId() == resourceId);});
    activeDoc.remove_if([&](auto that){return that.first==d;});
    d->close(this->getLoggedUser());
    std::shared_ptr<updateDocMessage> mess (new updateDocMessage(msgType::closeRes,{SymClient::getLoggedUser().getUsername(), ""}, resourceId));
    unanswered.push_front(mess);
    return *mess;
}

userDataMessage SymClient::editUser(user &newUserData) {
    std::shared_ptr<userDataMessage> mess (new userDataMessage(msgType::changeUserData, std::pair(SymClient::getLoggedUser().getUsername(), ""), msgOutcome::success, newUserData));
    unanswered.push_front(mess);
    return *mess;
}

const user SymClient::editUser(user &newUserData, bool msgRcv) {
    (const_cast<user&>(this->getLoggedUser())).setNewData(newUserData);
    //notifichiamo alla gui il successo
    #ifdef DISPATCHER_ON
    this->dispatcher->successAction();
    #endif
    return this->getLoggedUser();
}

clientMessage SymClient::removeUser(const std::string &pwd) {
    std::shared_ptr<clientMessage> mess (new clientMessage(msgType::removeUser, {SymClient::getLoggedUser().getUsername(), pwd}));
    this->unanswered.push_front(mess);
    return *mess;
}

clientMessage SymClient::logout() {
    std::shared_ptr<clientMessage> mess (new clientMessage(msgType::logout, {this->getLoggedUser().getUsername(), ""}));
    this->unanswered.push_front(mess);
    return *mess;
}

void SymClient::logout(bool msgRcv){
    std::shared_ptr<user> u(new user());
    this->setLoggedUser(*u);
    this->userColors.clear();
    //notifichiamo alla GUI il successo
    #ifdef DISPATCHER_ON
    this->dispatcher->successAction();
    #endif
}

updateDocMessage SymClient::mapSiteIdToUser(const document &currentDoc) {
    std::shared_ptr<updateDocMessage> mess (new updateDocMessage(msgType::mapChangesToUser, {SymClient::getLoggedUser().getUsername(), ""}, currentDoc.getId()));
    unanswered.push_front(mess);
    return *mess;
}

void SymClient::setUserColors(uint_positive_cnt::type resId, const std::map<uint_positive_cnt::type, user> &siteIdToUser) {
    //prendiamo il generatore di colore associato al documento
    colorGen gen = this->getColorGeneratorbyDocumentiID(resId);
    //iteratore alla mappa ricevuta
    std::map<uint_positive_cnt::type, user>::const_iterator it = siteIdToUser.begin();
    //iteratore alla mappa {siteId, documentId}->{user, color}
    //std::map<std::pair<uint_positive_cnt::type, uint_positive_cnt::type>, std::pair<user, Color>>::iterator mp;
    //scorriamo la mappa
    while(it != siteIdToUser.end()){
        //controlliamo se c'è già il pair, altrimenti inseriamo
        if(this->userColors.find(std::make_pair(it->first,resId)) == this->userColors.end()){
            //non è stato trovato, quindi inseriamo
            this->userColors.insert(std::pair<std::pair<uint_positive_cnt::type, uint_positive_cnt::type>, std::pair<user, Color>>
                                            (std::make_pair(it->first,resId),std::make_pair(it->second,gen())));
        }
        it++;
    }
}

void SymClient::addActiveUser(uint_positive_cnt::type resourceId, user &targetUser, privilege Priv) {
    getActiveDocumentbyID(resourceId)->access(targetUser, Priv);
    //recuperiamo il generatore di colore associato al documento
    colorGen c = getColorGeneratorbyDocumentiID(resourceId);
    //generiamo un colore per il nuovo utente
    std::shared_ptr<Color> col (new Color(c()));
    //inseriamolo nella mappa
    this->userColors.insert(std::pair<std::pair<uint_positive_cnt::type, uint_positive_cnt::type>, std::pair<user, Color>>
                                    (std::make_pair(targetUser.getSiteId(),resourceId),std::make_pair(targetUser,*col)));
    //dobbiamo aggiungiamo il cursore alla GUI, se necessario
    #ifdef DISPATCHER_ON
    if(Priv!=privilege::readOnly){
        this->dispatcher->addUserCursor(targetUser.getSiteId(),targetUser.getUsername(),resourceId);
    }
    #endif
}

void SymClient::removeActiveUser(uint_positive_cnt::type resourceId, user &targetUser) {
    getActiveDocumentbyID(resourceId)->close(targetUser);
    //dobbiamo rimuovere il cursore dalla GUI, se il cursore non era presente, la GUI non fa niente
    #ifdef DISPATCHER_ON
    this->dispatcher->removeUserCursor(targetUser.getSiteId(),resourceId);
    #endif
}

const user& SymClient::getLoggedUser() const{
    return loggedUser;
}

std::shared_ptr<clientMessage> SymClient::retrieveRelatedMessage(const serverMessage& smex) {
    for (std::shared_ptr<clientMessage> it:unanswered){
        if(smex.isRelatedTo(*it)){
            unanswered.remove(it);
			//fermiamo il timer di attesa nel dispatcher
            #ifdef DISPATCHER_ON
            this->dispatcher->stopTimer();
            #endif
            return it;
        }
    }
    throw SymClientException(SymClientException::noRelatedMessage, UnpackFileLineFunction());
}

void SymClient::setClientDispatcher(clientdispatcher *cl){
    #ifdef DISPATCHER_ON
    this->dispatcher = cl;
    #endif
}

void SymClient::verifySymbol(uint_positive_cnt::type resourceId, const symbol &sym) {
    document* d = this->getActiveDocumentbyID(resourceId);
    //Il metodo in verifySymbol mi deve restituire il pair delle coordinate (già detto a Martina che farà la modifica)
    std::pair<unsigned int, unsigned int> p = d->verifySymbol(sym);
    //notifichiamo alla GUI
    #ifdef DISPATCHER_ON
    this->dispatcher->verifySymbol(resourceId, sym, p);
    #endif
}

filterShared::filterShared(const user &currentUser): currentUser{currentUser} {
    //TODO: to implement
}

const std::forward_list<std::pair<const user *, sessionData>> SymClient::onlineUsersonDocument(int documentID){
    return ((this->getActiveDocumentbyID(documentID))->getActiveUsers());
}

const std::unordered_map<std::string, privilege> SymClient::allUsersonDocument(int documentID){
    return ((this->getFilebyDocumentID(documentID))->getUsers());
}

const std::shared_ptr<file> SymClient::getFilebyDocumentID(int id){
    for (std::shared_ptr<file> it:this->activeFile){
        if((it->getDoc()).getId() == id)
            return (it);
    }
}

bool filterShared::operator()(std::shared_ptr<file> file) {
    //TODO: to implement
    return false;
}

filterPrivilege::filterPrivilege(const user &currentUser, privilege filter): currentUser{currentUser} {
    //TODO: to implement
}

bool filterPrivilege::operator()(std::shared_ptr<file> file) {
    //TODO: to implement
    return false;
}

document* SymClient::getActiveDocumentbyID(uint_positive_cnt::type id){
    for (std::pair<document*, colorGen> it:this->activeDoc){
        if((it.first->getId() == id))
            return it.first;
    }
    throw SymClientException(SymClientException::noActiveDocument, UnpackFileLineFunction());
}

colorGen SymClient::getColorGeneratorbyDocumentiID(uint_positive_cnt::type id){
    for (std::pair<document*, colorGen> it:this->activeDoc){
        if((it.first->getId() == id))
            return (it.second);
    }
}

const user SymClient::userData(){
    return this->getLoggedUser();
}

cursorMessage SymClient::updateCursorPos(uint_positive_cnt::type resourceId, unsigned int row, unsigned int col) {
    std::shared_ptr<cursorMessage> mess (new cursorMessage(msgType::updateCursor, {this->getLoggedUser().getUsername(),""}, msgOutcome::success, this->getLoggedUser().getSiteId(), resourceId, row, col));
    this->unanswered.push_front(mess);
    return *mess;
}

void SymClient::updateCursorPos(uint_positive_cnt::type userSiteId, uint_positive_cnt::type resourceId, unsigned int row, unsigned int col){
    document* d = getActiveDocumentbyID(resourceId);
    d->updateCursorPos(userSiteId,row,col);
    #ifdef DISPATCHER_ON
    this->dispatcher->moveUserCursor(resourceId,row,col,userSiteId);
    #endif
}

std::string SymClient::directoryContent(std::string &ID_Cartella, std::string &path){
    return this->getLoggedUser().getHome()->getDir(path,ID_Cartella)->print(this->getLoggedUser().getUsername(),false);
}

void SymClient::removeUser(bool msgRcv) {
    std::shared_ptr<user> u(new user());
    this->setLoggedUser(*u);
    this->userColors.clear();
    #ifdef DISPATCHER_ON
    this->dispatcher->successAction();
    #endif
}

Color SymClient::colorOfUser(uint_positive_cnt::type resId, uint_positive_cnt::type siteId) {
    std::map<std::pair<uint_positive_cnt::type, uint_positive_cnt::type>, std::pair<user, Color>>::iterator mp;
    mp = this->userColors.find(std::make_pair(siteId,resId));
    if(mp != this->userColors.end())
        return mp->second.second;
    else
        throw SymClientException(SymClientException::nocolorOfUser, UnpackFileLineFunction());
}

const std::map<std::pair<uint_positive_cnt::type, uint_positive_cnt::type>, std::pair<user, Color>> &
SymClient::getUserColors() const {
    return userColors;
}

