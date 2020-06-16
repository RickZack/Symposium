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
#include "onoff_networkinteraction.h"
#ifdef DISPATCHER_ON
#include "GUI/SymposiumGui/Dispatcher/clientdispatcher.h"
#endif

using namespace Symposium;

SymClient::SymClient(): loggedUser("username", "P@assW0rd!!", "noempty", "", 0, nullptr), usersOnDocuments(),
                        activeFile(), activeDoc(), userColors(), unanswered(){
    document::doLoadAndStore=false;
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
    #endif
}

askResMessage SymClient::openSource(const std::string &resPath, const std::string &resId, privilege reqPriv) {
    std::shared_ptr<askResMessage> mess(new askResMessage(msgType::openRes, {SymClient::getLoggedUser().getUsername(),""}, resPath, resId, "", reqPriv, 0));
    unanswered.push_front(mess);
    return *mess;
}

void SymClient::openSource(const std::string &resPath, const std::string &resId, const std::shared_ptr<file> fileAsked,
                           privilege reqPriv) {
    auto p=getLoggedUser().openFile(resPath, resId, reqPriv);
    p->replacement(fileAsked);
    document& doc = p->access(this->getLoggedUser(), reqPriv);
    activeFile.push_front(p);
    colorGen c;
    //inseriamo nella mappa userColor gli utenti che sono già online sul documento
    assignUsersColor(c,doc);
    this->userColors.insert(std::pair<std::pair<uint_positive_cnt::type, uint_positive_cnt::type>, std::pair<user, Color>>
    (std::make_pair(this->getLoggedUser().getSiteId(),doc.getId()),std::make_pair(this->getLoggedUser(),c())));
    activeDoc.push_front({&doc, c});
    //notifichiamo alla gui il successo
    #ifdef DISPATCHER_ON
    this->dispatcher->updateRequestDocandSuccess(doc.getId());
    #endif
}

askResMessage
SymClient::openNewSource(const std::string &absolutePath, privilege reqPriv, const std::string &destPath, const std::string& destName) {
    std::shared_ptr<askResMessage> mess(new askResMessage(msgType::openNewRes, {SymClient::getLoggedUser().getUsername(), ""}, destPath, destName, absolutePath, reqPriv, 0));
    unanswered.push_front(mess);
    return *mess;
}

void SymClient::openNewSource(const std::string &absolutePath, privilege reqPriv, const std::string &destPath,
                              const std::string &destName,
                              uint_positive_cnt::type idToAssign, const std::shared_ptr<file> fileAsked) {
    std::string filePath = absolutePath.substr(0, absolutePath.find_last_of('/'));
    std::string fileName = absolutePath.substr(absolutePath.find_last_of('/') + 1);
    (this->getLoggedUser().getHome())->addLink(destPath, destName, filePath, fileName, idToAssign);
    activeFile.push_front(fileAsked);
    document& doc = fileAsked->access(this->getLoggedUser(), reqPriv);
    colorGen c;
    //inseriamo nella mappa userColor gli utenti che sono già online sul documento
    assignUsersColor(c,doc);
    this->userColors.insert(std::pair<std::pair<uint_positive_cnt::type, uint_positive_cnt::type>, std::pair<user, Color>>
                                    (std::make_pair(this->getLoggedUser().getSiteId(),doc.getId()),std::make_pair(this->getLoggedUser(),c())));
    activeDoc.push_front({&doc,c});
    //notifichiamo alla gui il successo
    #ifdef DISPATCHER_ON
    qDebug() << "ID Symlink: " << idToAssign;
    this->dispatcher->setSymlinkID(idToAssign, fileAsked->getId());
    this->dispatcher->updateRequestDocandSuccess(doc.getId());
    #endif
}

askResMessage SymClient::createNewSource(const std::string &resPath, const std::string &resName) {
    std::shared_ptr<askResMessage> mess(new askResMessage(msgType::createRes, {SymClient::getLoggedUser().getUsername(), ""}, resPath, resName, "", uri::getDefaultPrivilege(), 0));
    unanswered.push_front(mess);
    return *mess;
}

void SymClient::createNewSource(const std::string &resPath, const std::string &resName, uint_positive_cnt::type idToAssign,
                                const std::shared_ptr<file> fileCreated) {
    std::shared_ptr<file> file = this->getLoggedUser().newFile(resName, resPath, idToAssign);
    file->replacement(fileCreated);
    document& docReq = file->access(this->getLoggedUser(), privilege::owner);
    activeFile.push_front(file);
    colorGen c;
    this->userColors.insert(std::pair<std::pair<uint_positive_cnt::type, uint_positive_cnt::type>, std::pair<user, Color>>
                                    (std::make_pair(this->getLoggedUser().getSiteId(),docReq.getId()),std::make_pair(this->getLoggedUser(),c())));
    activeDoc.push_front({&docReq,c});
    //notifichiamo alla gui il successo
    #ifdef DISPATCHER_ON
    this->dispatcher->setNewFileID(file->getId());
    this->dispatcher->updateRequestDocandSuccess(docReq.getId());
    #endif
}

askResMessage SymClient::createNewDir(const std::string &resPath, const std::string &resName) {
    std::shared_ptr<askResMessage> mess(new askResMessage(msgType::createNewDir, {SymClient::getLoggedUser().getUsername(), ""}, resPath, resName, "", uri::getDefaultPrivilege(), 0));
    unanswered.push_front(mess);
    return *mess;
}

void SymClient::createNewDir(const std::string &resPath, const std::string &resName, uint_positive_cnt::type idToAssign) {
    this->getLoggedUser().newDirectory(resName, resPath, idToAssign);
    //notifichiamo alla gui il successo
    #ifdef DISPATCHER_ON
    this->dispatcher->successAction();
    #endif
}


symbolMessage SymClient::localInsert(uint_positive_cnt::type docId, const symbol &newSym, const std::pair<unsigned int, unsigned int> &index) {
    document* d = this->getActiveDocumentbyID(docId);
    symbol sym=document::emptySymbol;
    bool ok=handleDocException([&](){
        sym=d->localInsert(index, const_cast<symbol &>(newSym));
    });
    if(ok){
        std::shared_ptr<symbolMessage> mess (new symbolMessage(msgType::insertSymbol, {SymClient::getLoggedUser().getUsername(), ""}, msgOutcome::success, SymClient::getLoggedUser().getSiteId(), docId, sym));
        this->unanswered.push_front(mess);
        return *mess;
    }else{
        #ifdef DISPATCHER_ON
        this->dispatcher->errorOnDocument(docId);
        #endif
    }
}

symbolMessage SymClient::localRemove(uint_positive_cnt::type docId, const std::pair<unsigned int, unsigned int> &indexes) {
    document* d = this->getActiveDocumentbyID(docId);
    symbol s=document::emptySymbol;
    bool ok=handleDocException([&](){
        s = d->localRemove(indexes, getLoggedUser().getSiteId());
    });
    if(ok){
        std::shared_ptr<symbolMessage> mess (new symbolMessage(msgType::removeSymbol, {SymClient::getLoggedUser().getUsername(), ""}, msgOutcome::success, SymClient::getLoggedUser().getSiteId(), docId, s));
        this->unanswered.push_front(mess);
        return *mess;
    }else{
        #ifdef DISPATCHER_ON
        this->dispatcher->errorOnDocument(docId);
        #endif
    }
}

void SymClient::remoteInsert(uint_positive_cnt::type siteId, uint_positive_cnt::type docId, const symbol &newSym) {
    document* d = this->getActiveDocumentbyID(docId);
    if(d==nullptr)
        return;
    std::pair<unsigned int, unsigned int> p;
    bool ok=handleDocException([&](){
        p = d->remoteInsert(siteId, newSym);
    });
    if(ok){
        //notifica alla gui
        #ifdef DISPATCHER_ON
        this->dispatcher->remoteInsert(docId,newSym, siteId, p);
        #endif
    }else{
        #ifdef DISPATCHER_ON
        this->dispatcher->closeSource(docId);
        this->dispatcher->errorOnDocument(docId);
        #endif
    }
}

void SymClient::remoteRemove(uint_positive_cnt::type siteId, uint_positive_cnt::type docId, const symbol &rmSym) {
    document* d = this->getActiveDocumentbyID(docId);
    if(d==nullptr)
        return;
    std::pair<unsigned int, unsigned int> p;
    bool ok=handleDocException([&](){
        p = d->remoteRemove(siteId, rmSym);
    });
    if(ok){
        //notifica alla gui
        #ifdef DISPATCHER_ON
        this->dispatcher->remoteRemove(docId, siteId, p);
        #endif
    }else{
        #ifdef DISPATCHER_ON
        this->dispatcher->closeSource(docId);
        this->dispatcher->errorOnDocument(docId);
        #endif
    }
}

privMessage SymClient::editPrivilege(const std::string &targetUser, const std::string &resPath, const std::string &resId,
                                     privilege newPrivilege) {
    std::string path;
    if(resPath=="./")
        path = resPath + resId;
    else
        path = resPath + "/" + resId;

    std::shared_ptr<privMessage> mess (new privMessage(msgType::changePrivileges, {this->getLoggedUser().getUsername(),""}, msgOutcome::success, path, targetUser, newPrivilege));
    unanswered.push_front(mess);
    return *mess;
}

privilege SymClient::editPrivilege(const std::string &targetUser, const std::string &resPath, const std::string &resId,
                                   privilege newPrivilege, bool msgRcv) {
    privilege p;
    if(msgRcv){
        p=getLoggedUser().editPrivilege(targetUser, resPath, resId, newPrivilege);
        //notifichiamo alla gui il successo
        #ifdef DISPATCHER_ON
        this->dispatcher->successAction();
        #endif
    }
    else{
        const_cast<file&>(getActiveFiletoOpenbyID(std::stoul(resId))).setUserPrivilege(targetUser, newPrivilege);
    }
    return p;
}

uriMessage SymClient::shareResource(const std::string &resPath, const std::string &resId, const uri &newPrefs) {
    std::shared_ptr<uriMessage> mess (new uriMessage(msgType::shareRes, {SymClient::getLoggedUser().getUsername(), ""}, msgOutcome::success, resPath, resId, newPrefs, 0));
    unanswered.push_front(mess);
    return *mess;
}

std::shared_ptr<filesystem>
SymClient::shareResource(const std::string &actionUser, const std::string &resPath, const std::string &resId,
                         const uri &newPrefs,
                         bool msgRcv) {
    std::shared_ptr<filesystem> res;
    if(msgRcv){
        res=getLoggedUser().shareResource(resPath, resId, newPrefs);
        //notifichiamo alla gui il successo
        #ifdef DISPATCHER_ON
        this->dispatcher->successAction();
        #endif
    }
    else{
        const_cast<file&>(getActiveFiletoOpenbyID(std::stoul(resId))).setSharingPolicy(actionUser, newPrefs);
    }

    return res;
}

askResMessage
SymClient::renameResource(const std::string &resPath, const std::string &resId, const std::string &newName) {
    std::shared_ptr<askResMessage> mess(new askResMessage(msgType::changeResName, {SymClient::getLoggedUser().getUsername(), ""}, resPath, resId, newName, uri::getDefaultPrivilege(), 0));
    unanswered.push_front(mess);
    return *mess;
}

std::shared_ptr<filesystem>
SymClient::renameResource(const std::string &resPath, const std::string &resId, const std::string &newName,
                          bool) {
    std::shared_ptr<filesystem> f (getLoggedUser().renameResource(resPath, resId, newName));
    //notifichiamo alla gui il successo
    #ifdef DISPATCHER_ON
    this->dispatcher->successAction();
    #endif
    return f;
}

askResMessage SymClient::removeResource(const std::string &resPath, const std::string &resId) {
    std::shared_ptr<askResMessage> mess(new askResMessage(msgType::removeRes, {SymClient::getLoggedUser().getUsername(), ""}, resPath, resId, "", uri::getDefaultPrivilege(), 0));
    unanswered.push_front(mess);
    return *mess;
}

std::shared_ptr<filesystem>
SymClient::removeResource(const std::string &resPath, const std::string &resId, bool) {
    std::shared_ptr<filesystem> f = this->getLoggedUser().removeResource(resPath, resId);
    //notifichiamo alla gui il successo
    #ifdef DISPATCHER_ON
    this->dispatcher->successAction();
    #endif
    return f;
}

std::string SymClient::showDir(bool recursive) const {
    return getLoggedUser().showDir(recursive);
}

updateDocMessage SymClient::closeSource(uint_positive_cnt::type docId) {
    document* d = getActiveDocumentbyID(docId);
    d->close(this->getLoggedUser());
    activeFile.remove_if([docId](const std::shared_ptr<file>& it){return (it->getDoc().getId() == docId);});
    activeDoc.remove_if([&](auto that){return that.first==d;});
    std::shared_ptr<updateDocMessage> mess (new updateDocMessage(msgType::closeRes, {SymClient::getLoggedUser().getUsername(), ""}, docId));
    unanswered.push_front(mess);
    return *mess;
}

userDataMessage SymClient::editUser(user &newUserData) {
    std::shared_ptr<userDataMessage> mess (new userDataMessage(msgType::changeUserData, std::pair(SymClient::getLoggedUser().getUsername(), ""), msgOutcome::success, newUserData));
    unanswered.push_front(mess);
    return *mess;
}

const user SymClient::editUser(user &newUserData, bool) {
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

void SymClient::logout(bool){
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

void SymClient::setUserColors(uint_positive_cnt::type docId, const std::map<uint_positive_cnt::type, user> &siteIdToUser) {
    //prendiamo il generatore di colore associato al documento
    colorGen& gen = this->getColorGeneratorbyDocumentiID(docId);
    //iteratore alla mappa ricevuta
    std::map<uint_positive_cnt::type, user>::const_iterator it = siteIdToUser.begin();
    //iteratore alla mappa {siteId, documentId}->{user, color}
    //std::map<std::pair<uint_positive_cnt::type, uint_positive_cnt::type>, std::pair<user, Color>>::iterator mp;
    //scorriamo la mappa
    while(it != siteIdToUser.end()){
        //controlliamo se c'è già il pair, altrimenti inseriamo
        if(this->userColors.find(std::make_pair(it->first, docId)) == this->userColors.end()){
            //non è stato trovato, quindi inseriamo
            this->userColors.insert(std::pair<std::pair<uint_positive_cnt::type, uint_positive_cnt::type>, std::pair<user, Color>>
                                            (std::make_pair(it->first, docId), std::make_pair(it->second, gen())));
        }
        it++;
    }
    //notifichiamo il successo alla GUI
    #ifdef DISPATCHER_ON
    this->dispatcher->successSetUserColors(docId);
    #endif
}

void SymClient::addActiveUser(uint_positive_cnt::type docId, user &targetUser, privilege Priv) {
    //salviamo l'utente ricevuto
    auto& target = addUsersOnDocument(targetUser);
    getActiveDocumentbyID(docId)->access(target, Priv);
    auto f=getFilebyDocumentID(docId);
    if(f->getUserPrivilege(target.getUsername())==privilege::none)
        f->setUserPrivilege(target.getUsername(), Priv);
    //recuperiamo il generatore di colore associato al documento
    colorGen& c = getColorGeneratorbyDocumentiID(docId);
    //generiamo un colore per il nuovo utente
    //inseriamolo nella mappa
    this->userColors.insert(std::pair<std::pair<uint_positive_cnt::type, uint_positive_cnt::type>, std::pair<user, Color>>
                                    (std::make_pair(target.getSiteId(), docId), std::make_pair(target, c())));
    //dobbiamo aggiungere il cursore alla GUI, se necessario
    #ifdef DISPATCHER_ON
    if(Priv!=privilege::readOnly){
        this->dispatcher->addUserCursor(target.getSiteId(),target.getUsername(),docId);
    }else{
        this->dispatcher->addUserReadOnly(docId);
    }
    #endif
}

void SymClient::removeActiveUser(uint_positive_cnt::type docId, user &targetUser) {
    //recuperiamo l'utente dalla lista
    auto& target = getUsersOnDocument(targetUser.getUsername());
    document* d = getActiveDocumentbyID(docId);
    if(d==nullptr)
        return;
    d->close(target);
    //dobbiamo rimuovere il cursore dalla GUI, se il cursore non era presente, la GUI non fa niente
    #ifdef DISPATCHER_ON
    this->dispatcher->removeUserCursor(target.getSiteId(),docId);
    #endif
    //rimuoviamo l'utente dalla mappa userColors
    this->userColors.erase(std::make_pair(target.getSiteId(), docId));
    removeUsersOnDocument(target.getUsername());
}

const user& SymClient::getLoggedUser() const{
    return loggedUser;
}

const user& SymClient::addUsersOnDocument(const user &toInsert){
    if(this->usersOnDocuments.count(toInsert.getUsername())==0){
        this->usersOnDocuments.insert({toInsert.getUsername(),std::make_pair(toInsert,1)});
    }else{
        int c = ++(this->usersOnDocuments.at(toInsert.getUsername()).second);
        this->usersOnDocuments[toInsert.getUsername()].second = c;
    }
    return usersOnDocuments.at(toInsert.getUsername()).first;
}

const user& SymClient::getUsersOnDocument(const std::string &username){
    return this->usersOnDocuments[username].first;
}

void SymClient::removeUsersOnDocument(const std::string &username){
    int c = this->usersOnDocuments[username].second;
    if(c == 1)
        this->usersOnDocuments.erase(username);
    else
        this->usersOnDocuments[username].second = --c;
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
#ifdef DISPATCHER_ON
void SymClient::setClientDispatcher(clientdispatcher *cl){
    this->dispatcher = cl;
}
#endif

void SymClient::verifySymbol(uint_positive_cnt::type docId, const symbol &sym) {
    document* d = this->getActiveDocumentbyID(docId);
    if(d==nullptr)
        return;
    std::pair<unsigned int, unsigned int> p;
    bool ok=handleDocException([&](){
        p = d->verifySymbol(sym);
    });
    if(ok){
        //notifichiamo alla GUI
        #ifdef DISPATCHER_ON
        this->dispatcher->verifySymbol(docId, sym, p);
        #endif
        return;
    }else{
        #ifdef DISPATCHER_ON
        this->dispatcher->closeSource(docId);
        this->dispatcher->errorOnDocument(docId);
        #endif
    }
}

const std::forward_list<std::pair<const user *, sessionData>> SymClient::onlineUsersonDocument(uint_positive_cnt::type documentID){
    return ((this->getActiveDocumentbyID(documentID))->getActiveUsers());
}

const std::unordered_map<std::string, privilege> SymClient::allUsersonDocument(uint_positive_cnt::type documentID){
    return ((this->getFilebyDocumentID(documentID))->getUsers());
}

const std::shared_ptr<file> SymClient::getFilebyDocumentID(uint_positive_cnt::type id){
    for (std::shared_ptr<file> it:this->activeFile){
        if((it->getDoc()).getId() == id)
            return (it);
    }
    return std::shared_ptr<file>();
}


document* SymClient::getActiveDocumentbyID(uint_positive_cnt::type id){
    for (std::pair<document*, colorGen> it:this->activeDoc){
        if((it.first->getId() == id))
            return it.first;
    }
    return nullptr;
}

bool SymClient::controlFileIsActive(uint_positive_cnt::type id){
    for (std::shared_ptr<file> it:this->activeFile){
        if((it->getId() == id))
            return true;
    }
    return false;
}

bool SymClient::controlDocumentIsActive(uint_positive_cnt::type id){
    document* d = this->getActiveDocumentbyID(id);
    if(d == nullptr)
        return false;
    else
        return true;
}

const document& SymClient::getActiveDocumenttoOpenbyID(uint_positive_cnt::type id){
    for (std::pair<document*, colorGen> it:this->activeDoc){
        if((it.first->getId() == id))
            return *(it.first);
    }
    throw SymClientException(SymClientException::noActiveDocument, UnpackFileLineFunction());
}

const file& SymClient::getActiveFiletoOpenbyID(uint_positive_cnt::type id){
    for (std::shared_ptr<file> it:this->activeFile){
        if((it->getId() == id))
            return *it;
    }
    throw SymClientException(SymClientException::noActiveFile, UnpackFileLineFunction());
}

colorGen& SymClient::getColorGeneratorbyDocumentiID(uint_positive_cnt::type id){
    for (auto& it:this->activeDoc){
        if((it.first->getId() == id))
            return (it.second);
    }
}

void SymClient::assignUsersColor(colorGen& c, document& d){
    std::forward_list<std::pair<const user *, sessionData>> l = d.getActiveUsers();
    std::forward_list<std::pair<const user *, sessionData>>::iterator it = l.begin();
    while(it!=l.end()){
        if(it->first->getSiteId() != this->getLoggedUser().getSiteId()){
            this->userColors.insert(std::pair<std::pair<uint_positive_cnt::type, uint_positive_cnt::type>, std::pair<user, Color>>
            (std::make_pair(it->first->getSiteId(),d.getId()),std::make_pair(*it->first,c())));
            addUsersOnDocument(*it->first);
        }
        it++;
    }
}

const user& SymClient::userData(){
    return this->getLoggedUser();
}

cursorMessage SymClient::updateCursorPos(uint_positive_cnt::type docId, unsigned int row, unsigned int col) {
    document* d = getActiveDocumentbyID(docId);
    d->updateCursorPos(getLoggedUser().getSiteId(),row,col);
    std::shared_ptr<cursorMessage> mess (new cursorMessage(msgType::updateCursor, {this->getLoggedUser().getUsername(),""}, msgOutcome::success, this->getLoggedUser().getSiteId(), docId, row, col));
    this->unanswered.push_front(mess);
    return *mess;
}

void SymClient::updateCursorPos(uint_positive_cnt::type userSiteId, uint_positive_cnt::type docId, unsigned int row, unsigned int col){
    document* d = getActiveDocumentbyID(docId);
    d->updateCursorPos(userSiteId,row,col);
    #ifdef DISPATCHER_ON
    this->dispatcher->moveUserCursor(docId,row,col,userSiteId);
    #endif
}

std::string SymClient::directoryContent(std::string &ID_Cartella, std::string &path){
    return this->getLoggedUser().getHome()->getDir(path,ID_Cartella)->print(this->getLoggedUser().getUsername(),false);
}

void SymClient::removeUser(bool) {
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
        return Color(82,82,82);
}

Color SymClient::colorOfUserbyUsername(uint_positive_cnt::type resId, const std::string& username) {
    std::map<std::pair<uint_positive_cnt::type, uint_positive_cnt::type>, std::pair<user, Color>>::iterator mp = this->userColors.begin();
    while (mp != this->userColors.end()){
        if(mp->first.second == resId){
            if(mp->second.first.getUsername()==username){
                return mp->second.second;
            }
        }
        mp++;
    }
    return Color(82,82,82);
}

editLineStyleMessage
SymClient::localEditLineStyle(uint_positive_cnt::type docId, const std::pair<alignType, unsigned int> &oldLineStyle,
                              const std::pair<alignType, unsigned int> &newLineStyle, unsigned int row) {
    document* d = getActiveDocumentbyID(docId);
    bool ok=handleDocException([&](){
        d->editLineStyle(newLineStyle, row);
    });
    if(ok){
        std::shared_ptr<editLineStyleMessage> mess (new editLineStyleMessage(msgType::editLineStyle, {this->getLoggedUser().getUsername(),""}, msgOutcome::success, oldLineStyle, newLineStyle, docId, row));
        this->unanswered.push_front(mess);
        return *mess;
    }else{
        #ifdef DISPATCHER_ON
        this->dispatcher->errorOnDocument(docId);
        #endif
    }
}

void
SymClient::remoteEditLineStyle(uint_positive_cnt::type docId, const std::pair<alignType, unsigned int> &newLineStyle,
                               unsigned int row) {
    document* d = this->getActiveDocumentbyID(docId);
    if(d==nullptr)
        return;
    bool ok=handleDocException([&](){
        d->editLineStyle(newLineStyle, row);
    });
    if(ok){
        //notifica alla gui
        #ifdef DISPATCHER_ON
        this->dispatcher->remoteEditLineStyle(docId, newLineStyle, row);
        #endif
        return;
    }else{
        #ifdef DISPATCHER_ON
        this->dispatcher->closeSource(docId);
        this->dispatcher->errorOnDocument(docId);
        #endif
    }
}

bool SymClient::handleDocException(const std::function<void(void)> &op) {
    try{
        op();
    }
    catch(documentException& e){
        return false;
    }
    return true;
}

