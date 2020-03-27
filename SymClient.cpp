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

#include "SymClient.h"
#include "uri.h"
#include "message.h"
#include "filesystem.h"
#include "SymposiumException.h"
//#include "GUI/SymposiumGui/Dispatcher/clientdispatcher.h"

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
    //notifichiamo alla gui il successo
//    this->dispatcher->successSignUp();
}

clientMessage SymClient::logIn(const std::string &username, const std::string &pwd) {
    std::shared_ptr<clientMessage> mess(new clientMessage(msgType::login, {username, pwd}));
    unanswered.push_front(mess);
    return *mess;
}

void SymClient::logIn(const user &logged) {
    setLoggedUser(logged);
    //notifichiamo alla gui il successo
//    this->dispatcher->successLogin();
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
    colorGen* c = new colorGen();
    activeDoc.push_front({&doc, *c});
    Color f = (*c)();
    this->userColors.insert(std::pair<std::pair<uint_positive_cnt::type, uint_positive_cnt::type>, std::pair<user, Color>>
    (std::make_pair(this->getLoggedUser().getSiteId(),doc.getId()),std::make_pair(this->getLoggedUser(),f)));
    //notifichiamo alla gui il successo
 //   this->dispatcher->successOpenSource(doc);
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
    colorGen* c = new colorGen();
    std::shared_ptr<Color> col (new Color((*c)()));
    activeDoc.push_front({&doc,*c});
    this->userColors.insert(std::pair<std::pair<uint_positive_cnt::type, uint_positive_cnt::type>, std::pair<user, Color>>
                                    (std::make_pair(this->getLoggedUser().getSiteId(),doc.getId()),std::make_pair(this->getLoggedUser(),*col)));
	//notifichiamo alla gui il successo
//    this->dispatcher->successInsertUri();
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
    colorGen* c = new colorGen();
    Color f = (*c)();
    activeDoc.push_front({&docReq,*c});
    this->userColors.insert(std::pair<std::pair<uint_positive_cnt::type, uint_positive_cnt::type>, std::pair<user, Color>>
                                    (std::make_pair(this->getLoggedUser().getSiteId(),docReq.getId()),std::make_pair(this->getLoggedUser(),f)));
    //notifichiamo alla gui il successo
//    this->dispatcher->successCreateNewSource(std::to_string(idToAssign));
}

askResMessage SymClient::createNewDir(const std::string &path, const std::string &name) {
    std::shared_ptr<askResMessage> mess(new askResMessage(msgType::createNewDir, {SymClient::getLoggedUser().getUsername(), ""}, path, name, "", uri::getDefaultPrivilege(), 0));
    unanswered.push_front(mess);
    return *mess;
}

void SymClient::createNewDir(const std::string &path, const std::string &name, uint_positive_cnt::type idToAssign) {
    this->getLoggedUser().newDirectory(name,path,idToAssign);
    //notifichiamo alla gui il successo
//    this->dispatcher->successCreateNewDir(std::to_string(idToAssign));
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
    std::shared_ptr<symbolMessage> mess (new symbolMessage(msgType::removeSymbol, {SymClient::getLoggedUser().getUsername(), ""}, msgOutcome::success, SymClient::getLoggedUser().getSiteId(), resourceId, d->localRemove(indexes)));
    this->unanswered.push_front(mess);
    return *mess;
}

void SymClient::remoteInsert(uint_positive_cnt::type siteId, uint_positive_cnt::type resourceId, const symbol &newSym) {
    document* d = this->getActiveDocumentbyID(resourceId);
    d->remoteInsert(siteId, newSym);
    //notifica alla gui
//    this->dispatcher->remoteInsert(resourceId,newSym);
}

void SymClient::remoteRemove(uint_positive_cnt::type siteId, uint_positive_cnt::type resourceId, const symbol &rmSym) {
    document* d = this->getActiveDocumentbyID(resourceId);
    d->remoteRemove(siteId, rmSym);
    //notifica alla gui

    //DOVE PRENDO IL PAIR DELLE COORDINATE DEL SYMBOL?

    //this->dispatcher->remoteRemove(resourceId, ...);
}

privMessage SymClient::editPrivilege(const std::string &targetUser, const std::string &resPath, const std::string &resName,
                                     privilege newPrivilege) {
    //FIXME: non è questo che devi passare al messaggio, vedi documentazione
    //std::string idres = std::to_string(getLoggedUser().getHome()->getFile(resPath, resName)->getDoc().getId());
    std::shared_ptr<privMessage> mess (new privMessage(msgType::changePrivileges, {this->getLoggedUser().getUsername(),""}, msgOutcome::success, resPath + "/" + resName, targetUser, newPrivilege));
    unanswered.push_front(mess);
    return *mess;
}

privilege SymClient::editPrivilege(const std::string &targetUser, const std::string &resPath, const std::string &resName,
                                   privilege newPrivilege, bool msgRcv) {
    privilege* p = new privilege(this->getLoggedUser().editPrivilege(targetUser, resPath, resName, newPrivilege));
    //notifichiamo alla gui il successo
//    this->dispatcher->successEditPrivilege();
    return *p;
}

uriMessage SymClient::shareResource(const std::string &resPath, const std::string &resName, const uri &newPrefs) {    std::shared_ptr<uriMessage> mess (new uriMessage(msgType::shareRes, {SymClient::getLoggedUser().getUsername(), ""}, msgOutcome::success, resPath, resName, newPrefs, 0));
    unanswered.push_front(mess);
    return *mess;
}

std::shared_ptr<filesystem> SymClient::shareResource(const std::string &resPath, const std::string &resName, const uri &newPrefs, bool msgRcv) {    //TODO: modified this method, return the file: FATTO
    std::shared_ptr<filesystem> fil (this->getLoggedUser().shareResource(resPath, resName, newPrefs));
    //notifichiamo alla gui il successo
//    this->dispatcher->successShareResource("/" + resPath);
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
//    this->dispatcher->successRenameResource();
    return f;
}

askResMessage SymClient::removeResource(const std::string &resPath, const std::string &resName) {
    //FIXME: non è questo che devi passare al messaggio, è il percorso completo
    //std::string idres = std::to_string(getLoggedUser().getHome()->getFile(resPath, resName)->getDoc().getId());
    std::shared_ptr<askResMessage> mess(new askResMessage(msgType::removeRes, {SymClient::getLoggedUser().getUsername(), ""}, resPath, resName, "", uri::getDefaultPrivilege(), 0));
    unanswered.push_front(mess);
    return *mess;
}

std::shared_ptr<filesystem>
SymClient::removeResource(const std::string &resPath, const std::string &resName, bool msgRcv) {
    std::shared_ptr<filesystem> f = this->getLoggedUser().removeResource(resPath,resName);
    //notifichiamo alla gui il successo
//    this->dispatcher->successRemoveResource();
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
//FIXME: add and review tests for editUser
userDataMessage SymClient::editUser(user &newUserData) {
    std::shared_ptr<userDataMessage> mess (new userDataMessage(msgType::changeUserData, std::pair(SymClient::getLoggedUser().getUsername(), ""), msgOutcome::success, newUserData));
    unanswered.push_front(mess);
    return *mess;
}

const user SymClient::editUser(user &newUserData, bool msgRcv) {
    user old = this->getLoggedUser();
    //this->getLoggedUser().setNewData(newUserData);
    this->loggedUser.setNewData(newUserData);
    //notifichiamo alla gui il successo
//    this->dispatcher->successEditUser();
    return old;
}

clientMessage SymClient::removeUser() {
    std::shared_ptr<clientMessage> mess (new clientMessage(msgType::removeUser, {SymClient::getLoggedUser().getUsername(), ""}));
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
}

updateDocMessage SymClient::mapSiteIdToUser(const document &currentDoc) {
    std::shared_ptr<updateDocMessage> mess (new updateDocMessage(msgType::mapChangesToUser, {SymClient::getLoggedUser().getUsername(), ""}, currentDoc.getId()));
    unanswered.push_front(mess);
    return *mess;
}
//con questo metodo associamo agli utenti i colori (che sono diversi per ogni client), da fare con qcolor
void
SymClient::setUserColors(uint_positive_cnt::type resId, const std::map<uint_positive_cnt::type, user> &siteIdToUser) {
    //TODO:implement
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
    if(Priv!=privilege::readOnly){
//        this->dispatcher->addUserCursor(targetUser.getSiteId(),targetUser.getUsername(),resourceId);
    }
}

void SymClient::removeActiveUser(uint_positive_cnt::type resourceId, user &targetUser) {
    getActiveDocumentbyID(resourceId)->close(targetUser);
    //dobbiamo rimuovere il cursore dalla GUI, se il cursore non era presente, non la GUI non fa niente
//    this->dispatcher->removeUserCursor(targetUser.getSiteId(),resourceId);
}

const user& SymClient::getLoggedUser() const{
    return loggedUser;
}

std::shared_ptr<clientMessage> SymClient::retrieveRelatedMessage(const serverMessage& smex) {
    for (std::shared_ptr<clientMessage> it:unanswered){
        if(smex.isRelatedTo(*it)){
            unanswered.remove(it);
			//fermiamo il timer di attesa nel dispatcher
//            this->dispatcher->stopTimer();
            return it;
        }
    }
    throw SymClientException(SymClientException::noRelatedMessage, UnpackFileLineFunction());
}

void SymClient::setClientDispatcher(clientdispatcher *cl){
//    this->dispatcher = cl;
}

void SymClient::verifySymbol(uint_positive_cnt::type resourceId, const symbol &sym) {
    //TODO: to implement
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
    return this->loggedUser;
}

cursorMessage SymClient::updateCursorPos(uint_positive_cnt::type resourceId, unsigned int row, unsigned int col) {
    std::shared_ptr<cursorMessage> mess (new cursorMessage(msgType::updateCursor, {this->getLoggedUser().getUsername(),""}, msgOutcome::success, this->getLoggedUser().getSiteId(), resourceId, row, col));
    this->unanswered.push_front(mess);
    return *mess;
}

void SymClient::updateCursorPos(uint_positive_cnt::type userSiteId, uint_positive_cnt::type resourceId, unsigned int row, unsigned int col){
    document* d = getActiveDocumentbyID(resourceId);
    d->updateCursorPos(userSiteId,row,col);
//    this->dispatcher->moveUserCursor(resourceId,row,col,userSiteId);
}

std::string SymClient::directoryContent(std::string &ID_Cartella, std::string &path){
    return this->getLoggedUser().getHome()->getDir(path,ID_Cartella)->print(std::to_string(this->getLoggedUser().getSiteId()),false);
}

void SymClient::removeUser(bool msgRcv) {
    std::shared_ptr<user> u(new user());
    this->setLoggedUser(*u);
    this->userColors.clear();
//	this->dispatcher->successDeleteAccount();
}

Color SymClient::colorOfUser(uint_positive_cnt::type resId, uint_positive_cnt::type siteId) {
    //TODO: implement
    return Color();
}

const std::map<std::pair<uint_positive_cnt::type, uint_positive_cnt::type>, std::pair<user, Color>> &
SymClient::getUserColors() const {
    return userColors;
}

