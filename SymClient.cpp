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

using namespace Symposium;

SymClient::SymClient() :loggedUser("username", "P@assW0rd!!", "noempty", "", 0, nullptr){
    //TODO: implement
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
}

clientMessage SymClient::logIn(const std::string &username, const std::string &pwd) {
    std::shared_ptr<clientMessage> mess(new clientMessage(msgType::login, {username, pwd}));
    unanswered.push_front(mess);
    return *mess;
}

void SymClient::logIn(const user &logged) {
    setLoggedUser(logged);
}

askResMessage SymClient::openSource(const std::string &path, const std::string &name, privilege reqPriv) {
    std::shared_ptr<askResMessage> mess(new askResMessage(msgType::openRes, {SymClient::getLoggedUser().getUsername(),""}, path, name, "", reqPriv, 0));
    unanswered.push_front(mess);
    return *mess;
}
//TEST FALLISCE
void SymClient::openSource(const std::shared_ptr<file> fileAsked) {
    std::string nome_file = fileAsked->getName();
    activeFile.push_front(fileAsked);
    activeDoc.push_front(const_cast<document *>(&(fileAsked->getDoc())));
}

askResMessage
SymClient::openNewSource(const std::string &resourceId, privilege reqPriv, const std::string &destPath, const std::string& destName) {
    std::shared_ptr<askResMessage> mess(new askResMessage(msgType::openNewRes, {SymClient::getLoggedUser().getUsername(), ""}, destPath, destName, resourceId, reqPriv, 0));
    unanswered.push_front(mess);
    return *mess;
}
//TEST FALLISCE
void SymClient::openNewSource(const std::string &resId, privilege reqPriv, const std::string &destPath,
                              const std::string &destName,
                              int idToAssign, const std::shared_ptr<file> fileAsked) {
    activeFile.push_front(fileAsked);
    activeDoc.push_front(const_cast<document *>(&(fileAsked->getDoc())));
    std::shared_ptr<directory> ut = SymClient::getLoggedUser().getHome();   //TEST FALLISCE: il metodo gethome() mi restituisce puntatore a null
    this->getLoggedUser().getHome()->addLink(destPath, destName, resId, (*fileAsked).getName(), idToAssign);
}

askResMessage SymClient::createNewSource(const std::string &path, const std::string &name) {
    std::shared_ptr<askResMessage> mess(new askResMessage(msgType::createRes, {SymClient::getLoggedUser().getUsername(), ""}, path, name, "", uri::getDefaultPrivilege(), 0));
    unanswered.push_front(mess);
    return *mess;
}

void SymClient::createNewSource(const std::string &path, const std::string &name, int idToAssign) {
    std::shared_ptr<file> nfile = getLoggedUser().newFile(name, path, idToAssign);
    activeFile.push_front(nfile);
    document d = nfile->getDoc();
    activeDoc.push_front(&d);
}

askResMessage SymClient::createNewDir(const std::string &path, const std::string &name) {
    std::shared_ptr<askResMessage> mess(new askResMessage(msgType::createNewDir, {SymClient::getLoggedUser().getUsername(), ""}, path, name, "", uri::getDefaultPrivilege(), 0));
    unanswered.push_front(mess);
    return *mess;
}

void SymClient::createNewDir(const std::string &path, const std::string &name, int idToAssign) {
    getLoggedUser().newDirectory(name,path,idToAssign);
}


symbolMessage SymClient::localInsert(int resourceId, const symbol &newSym, const std::pair<int, int> &index) {
    document d = this->getActiveDocumentbyID(resourceId);
    int pos[2] = {index.first,index.second};
    d.localInsert(pos, const_cast<symbol &>(newSym));
    return symbolMessage(msgType::insertSymbol, {SymClient::getLoggedUser().getUsername(), ""}, msgOutcome::success, SymClient::getLoggedUser().getSiteId(), resourceId, newSym);
}
//RIVEDERE: non abbiamo il simbolo da eliminare, ma nel messaggio dobbiamo inserire perforza il simbolo, quindi ne ho creato uno a caso ma impostato con la posizione in cui dobbiamo eliminare, va bene?
symbolMessage SymClient::localRemove(int resourceId, int indexes[2]) {
    std::vector<int> pos = {indexes[0], indexes[1]};
    return symbolMessage(msgType::removeSymbol, {SymClient::getLoggedUser().getUsername(), ""}, msgOutcome::success, SymClient::getLoggedUser().getSiteId(), resourceId, symbol('a', 0, 0, pos, false));
}

void SymClient::remoteInsert(int resourceId, const symbol &newSym) {
    document d = this->getActiveDocumentbyID(resourceId);
    d.remoteInsert(newSym);
}

void SymClient::remoteRemove(int resourceId, const symbol &rmSym) {
    document d = this->getActiveDocumentbyID(resourceId);
    d.remoteRemove(rmSym);
}

privMessage SymClient::editPrivilege(const std::string &targetUser, const std::string &resPath, const std::string &resName,
                                     privilege newPrivilege) {
    std::string idres = std::to_string(getLoggedUser().getHome()->getFile(resPath, resName)->getDoc().getId());
    privMessage *mess = new privMessage(msgType::changePrivileges, {SymClient::getLoggedUser().getUsername(),""}, msgOutcome::success, idres, targetUser, newPrivilege);
    unanswered.push_front(std::shared_ptr<privMessage>(mess));
    return *mess;
}

privilege SymClient::editPrivilege(const std::string &targetUser, const std::string &resPath, const std::string &resName,
                                   privilege newPrivilege, bool msgRcv) {
    return this->getLoggedUser().editPrivilege(targetUser, resPath, resName, newPrivilege);
}

uriMessage SymClient::shareResource(const std::string &resPath, const std::string &resName, uri &newPrefs) {
    uriMessage *mess = new uriMessage(msgType::shareRes, {SymClient::getLoggedUser().getUsername(), ""}, msgOutcome::success, resPath, resName, newPrefs, 0);
    unanswered.push_front(std::shared_ptr<uriMessage>(mess));
    return *mess;
}
//FIXME: Metodo non funzionante
uri SymClient::shareResource(const std::string &resPath, const std::string &resName, uri &newPrefs, bool msgRcv) {
    //TODO: modified this method, return the file
    std::shared_ptr<filesystem> fil = this->getLoggedUser().shareResource(resPath, resName, newPrefs);
   // return fil->getSharingPolicy();
    return newPrefs;
}

askResMessage
SymClient::renameResource(const std::string &resPath, const std::string &resName, const std::string &newName) {
    //FIXME: mess is not in dynamic memory, assign its pointer to a shared_ptr is wrong
    askResMessage *mess = new askResMessage(msgType::changeResName, {SymClient::getLoggedUser().getUsername(), ""}, resPath, resName, newName, uri::getDefaultPrivilege(), 0);
    unanswered.push_front(std::shared_ptr<askResMessage>(mess));
    return *mess;
}

std::shared_ptr<filesystem>
SymClient::renameResource(const std::string &resPath, const std::string &resName, const std::string &newName,
                          bool msgRcv) {
    return SymClient::getLoggedUser().renameResource(resPath, resName, newName);
}

askResMessage SymClient::removeResource(const std::string &resPath, const std::string &resName) {
    std::string idres = std::to_string(getLoggedUser().getHome()->getFile(resPath, resName)->getDoc().getId());
    askResMessage *mess = new askResMessage(msgType::removeRes, {SymClient::getLoggedUser().getUsername(), ""}, resPath, resName, idres, uri::getDefaultPrivilege(), 0);
    unanswered.push_front(std::shared_ptr<askResMessage>(mess));
    return *mess;
}

std::shared_ptr<filesystem>
SymClient::removeResource(const std::string &resPath, const std::string &resName, bool msgRcv) {
    return getLoggedUser().getHome()->remove(getLoggedUser(),resPath,resName);
}
//RIVEDERE: Cosa deve fare questo metodo?
//la funzione stampa l'albero filesyste dell'utente
std::string SymClient::showDir(bool recursive) const {
    return "";
}

updateDocMessage SymClient::closeSource(int resourceId) {
    document d = getActiveDocumentbyID(resourceId);
    activeFile.remove_if([resourceId](std::shared_ptr<file> it){return (it->getDoc().getId() == resourceId);});
    activeDoc.remove(&d);
    return updateDocMessage(msgType::closeRes, {SymClient::getLoggedUser().getUsername(), ""}, resourceId);
}
//FIXME: add and review tests for editUser
userDataMessage SymClient::editUser(user &newUserData) {
    userDataMessage *mess = new userDataMessage(msgType::changeUserData, std::pair(SymClient::getLoggedUser().getUsername(), ""), msgOutcome::success, newUserData);
    unanswered.push_front(std::shared_ptr<userDataMessage>(mess));
    return *mess;
}

const user SymClient::editUser(user &newUserData, bool msgRcv) {
    user old = SymClient::getLoggedUser();
    this->setLoggedUser(newUserData);
    return old;
}

clientMessage SymClient::removeUser() {
    return clientMessage(msgType::removeUser, {SymClient::getLoggedUser().getUsername(), ""});
}

clientMessage SymClient::logout() {
    std::string user = SymClient::getLoggedUser().getUsername();
    return clientMessage(msgType::logout, {user, ""});
}

updateDocMessage SymClient::mapSiteIdToUser(const document &currentDoc) {
    return updateDocMessage(msgType::mapChangesToUser, {SymClient::getLoggedUser().getUsername(), ""}, currentDoc.getId());
}
//con questo metodo associamo agli utenti i colori (che sono diversi per ogni client), da fare con qcolor
void SymClient::setUserColors(const std::map<int, user> &siteIdToUser) {
    //TODO:implement
}
//RIVEDERE: per aggiungere il nuovo utente alla lista di utenti attivi sul documento uso la funzione access di document?
//Se si, non dovrei avere il livello di privilegio dell'utente da aggiungere? SI, il parametro è da aggiungere
void SymClient::addActiveUser(int resourceId, user &targetUser) {
    document d = getActiveDocumentbyID(resourceId);
}

void SymClient::removeActiveUser(int resourceId, user &targetUser) {
    document d = getActiveDocumentbyID(resourceId);
    d.close(targetUser);
}

user &SymClient::getLoggedUser() {
    return loggedUser;
}

clientMessage SymClient::retrieveRelatedMessage(const serverMessage& smex) {
    serverMessage mess = smex;
    for (std::shared_ptr<clientMessage> it:SymClient::unanswered){
        if(mess.isRelatedTo(*it)){
            clientMessage *cmess = new clientMessage(*it); //costruttore di copia
            unanswered.remove(it);
            return *cmess;
        }
    }
    throw SymClientException(SymClientException::noRelatedMessage, UnpackFileLineFunction());
}

void SymClient::verifySymbol(int resourceId, const symbol &sym) {
    //TODO: to implement
}

filterShared::filterShared(const user &currentUser): currentUser{currentUser} {
    //TODO: to implement
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

document SymClient::getActiveDocumentbyID(int id){
    for (document *it:this->activeDoc){
        if((*it).getId() == id)
            return (*it);
    }
    throw SymClientException(SymClientException::noActiveDocument, UnpackFileLineFunction());
}
