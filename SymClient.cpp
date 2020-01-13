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

void SymClient::openSource(const std::shared_ptr<file> fileAsked, privilege reqPriv) {
    document& doc = fileAsked->access(this->getLoggedUser(), reqPriv);
    this->getLoggedUser().getHome()->getFile(fileAsked->getRealPath(),fileAsked->getName())->replacement(fileAsked);
    activeFile.push_front(fileAsked);
    activeDoc.push_front(&doc);
}

askResMessage
SymClient::openNewSource(const std::string &resourceId, privilege reqPriv, const std::string &destPath, const std::string& destName) {
    std::shared_ptr<askResMessage> mess(new askResMessage(msgType::openNewRes, {SymClient::getLoggedUser().getUsername(), ""}, destPath, destName, resourceId, reqPriv, 0));
    unanswered.push_front(mess);
    return *mess;
}

void SymClient::openNewSource(const std::string &resId, privilege reqPriv, const std::string &destPath,
                              const std::string &destName,
                              int idToAssign, const std::shared_ptr<file> fileAsked) {
    this->getLoggedUser().getHome()->addLink(destPath, destName, resId, (*fileAsked).getName(), idToAssign);
    activeFile.push_front(fileAsked);
    document& doc = fileAsked->access(this->getLoggedUser(), reqPriv);
    activeDoc.push_front(&doc);
}

askResMessage SymClient::createNewSource(const std::string &path, const std::string &name) {
    std::shared_ptr<askResMessage> mess(new askResMessage(msgType::createRes, {SymClient::getLoggedUser().getUsername(), ""}, path, name, "", uri::getDefaultPrivilege(), 0));
    unanswered.push_front(mess);
    return *mess;
}

void SymClient::createNewSource(const std::string &path, const std::string &name, int idToAssign) {
    std::shared_ptr<file> file = this->getLoggedUser().newFile(name, path, idToAssign);
    document& docReq = file->access(this->getLoggedUser(), privilege::owner);
    activeFile.push_front(file);
    activeDoc.push_front(&docReq);
}

askResMessage SymClient::createNewDir(const std::string &path, const std::string &name) {
    std::shared_ptr<askResMessage> mess(new askResMessage(msgType::createNewDir, {SymClient::getLoggedUser().getUsername(), ""}, path, name, "", uri::getDefaultPrivilege(), 0));
    unanswered.push_front(mess);
    return *mess;
}

void SymClient::createNewDir(const std::string &path, const std::string &name, int idToAssign) {
    this->getLoggedUser().newDirectory(name,path,idToAssign);
}


symbolMessage SymClient::localInsert(int resourceId, const symbol &newSym, const std::pair<int, int> &index) {
    document* d = this->getActiveDocumentbyID(resourceId);
    d->localInsert(index, const_cast<symbol &>(newSym));
    return symbolMessage(msgType::insertSymbol, {SymClient::getLoggedUser().getUsername(), ""}, msgOutcome::success, SymClient::getLoggedUser().getSiteId(), resourceId, newSym);
}

symbolMessage SymClient::localRemove(int resourceId, const std::pair<int, int> indexes) {
    document* d = this->getActiveDocumentbyID(resourceId);
    return symbolMessage(msgType::removeSymbol, {SymClient::getLoggedUser().getUsername(), ""}, msgOutcome::success, SymClient::getLoggedUser().getSiteId(), resourceId, d->localRemove(indexes));
}

void SymClient::remoteInsert(int resourceId, const symbol &newSym) {
    document* d = this->getActiveDocumentbyID(resourceId);
    d->remoteInsert(newSym);
}

void SymClient::remoteRemove(int resourceId, const symbol &rmSym) {
    document* d = this->getActiveDocumentbyID(resourceId);
    d->remoteRemove(rmSym);
}

privMessage SymClient::editPrivilege(const std::string &targetUser, const std::string &resPath, const std::string &resName,
                                     privilege newPrivilege) {
    std::string idres = std::to_string(getLoggedUser().getHome()->getFile(resPath, resName)->getDoc().getId());
    std::shared_ptr<privMessage> mess (new privMessage(msgType::changePrivileges, {SymClient::getLoggedUser().getUsername(),""}, msgOutcome::success, idres, targetUser, newPrivilege));
    unanswered.push_front(mess);
    return *mess;
}

privilege SymClient::editPrivilege(const std::string &targetUser, const std::string &resPath, const std::string &resName,
                                   privilege newPrivilege, bool msgRcv) {
    return this->getLoggedUser().editPrivilege(targetUser, resPath, resName, newPrivilege);
}

uriMessage SymClient::shareResource(const std::string &resPath, const std::string &resName, const uri &newPrefs) {    std::shared_ptr<uriMessage> mess (new uriMessage(msgType::shareRes, {SymClient::getLoggedUser().getUsername(), ""}, msgOutcome::success, resPath, resName, newPrefs, 0));
    unanswered.push_front(mess);
    return *mess;
}

std::shared_ptr<filesystem> SymClient::shareResource(const std::string &resPath, const std::string &resName, const uri &newPrefs, bool msgRcv) {    //TODO: modified this method, return the file: FATTO
    std::shared_ptr<filesystem> fil = this->getLoggedUser().shareResource(resPath, resName, newPrefs);
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
    return SymClient::getLoggedUser().renameResource(resPath, resName, newName);
}

askResMessage SymClient::removeResource(const std::string &resPath, const std::string &resName) {
    std::string idres = std::to_string(getLoggedUser().getHome()->getFile(resPath, resName)->getDoc().getId());
    std::shared_ptr<askResMessage> mess(new askResMessage(msgType::removeRes, {SymClient::getLoggedUser().getUsername(), ""}, resPath, resName, idres, uri::getDefaultPrivilege(), 0));
    unanswered.push_front(mess);
    return *mess;
}

std::shared_ptr<filesystem>
SymClient::removeResource(const std::string &resPath, const std::string &resName, bool msgRcv) {
    return this->getLoggedUser().removeResource(resPath,resName);
}

std::string SymClient::showDir(bool recursive) const {
    return getLoggedUser().showDir(recursive);
}

updateDocMessage SymClient::closeSource(int resourceId) {
    document* d = getActiveDocumentbyID(resourceId);
    activeFile.remove_if([resourceId](std::shared_ptr<file> it){return (it->getDoc().getId() == resourceId);});
    activeDoc.remove(d);
    return updateDocMessage(msgType::closeRes, {SymClient::getLoggedUser().getUsername(), ""}, resourceId);
}
//FIXME: add and review tests for editUser
userDataMessage SymClient::editUser(user &newUserData) {
    std::shared_ptr<userDataMessage> mess (new userDataMessage(msgType::changeUserData, std::pair(SymClient::getLoggedUser().getUsername(), ""), msgOutcome::success, newUserData));
    unanswered.push_front(mess);
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
    return clientMessage(msgType::logout, {this->getLoggedUser().getUsername(), ""});
}

void SymClient::logout(bool msgRcv){
    //TODO: Boh, conferma azione. Fatto solo per uniformità. Magari ci sarà da dire qualcosa alla GUI.
}

updateDocMessage SymClient::mapSiteIdToUser(const document &currentDoc) {
    std::shared_ptr<updateDocMessage> mess (new updateDocMessage(msgType::mapChangesToUser, {SymClient::getLoggedUser().getUsername(), ""}, currentDoc.getId()));
    unanswered.push_front(mess);
    return *mess;
}
//con questo metodo associamo agli utenti i colori (che sono diversi per ogni client), da fare con qcolor
void SymClient::setUserColors(const std::map<int, user> &siteIdToUser) {
    //TODO:implement
}

void SymClient::addActiveUser(int resourceId, user &targetUser, privilege Priv) {
    getActiveDocumentbyID(resourceId)->access(targetUser, Priv);
}

void SymClient::removeActiveUser(int resourceId, user &targetUser) {
    getActiveDocumentbyID(resourceId)->close(targetUser);
}

const user& SymClient::getLoggedUser() const{
    return loggedUser;
}

std::shared_ptr<clientMessage> SymClient::retrieveRelatedMessage(const serverMessage& smex) {
    for (std::shared_ptr<clientMessage> it:unanswered){
        if(smex.isRelatedTo(*it)){
            unanswered.remove(it);
            return it;
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

document* SymClient::getActiveDocumentbyID(int id){
    for (document *it:this->activeDoc){
        if((*it).getId() == id)
            return (it);
    }
    throw SymClientException(SymClientException::noActiveDocument, UnpackFileLineFunction());
}
