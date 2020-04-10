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
 * File:   SymServer.cpp
 * Project: Symposium
 * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Alice Morano <s259158 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 20 Giugno 2019, 21.30
 */
#include "SymServer.h"
#include "message.h"
#include "filesystem.h"
#include <regex>
#include "SymposiumException.h"

#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>

using namespace Symposium;

uint_positive_cnt SymServer::idCounter;
const user SymServer::unknownUser("unknown", "@dummY!Pwd", "unknown", ":resources/avatar/unknown.png", -1, nullptr);

const user & SymServer::addUser(user &newUser) {
    if(userIsRegistered(newUser.getUsername()))
        throw SymServerException(SymServerException::userAlreadyExist, UnpackFileLineFunction());
    if(!userIsValid(newUser)){
        throw SymServerException(SymServerException::userWrongParam, UnpackFileLineFunction());
    }
    auto userDir=rootDir->addDirectory(newUser.getUsername());
    newUser.setHome(userDir);
    newUser.setSiteId(idCounter++);
    auto& target=registerUser(newUser);

    //response to client
    auto response=std::make_shared<loginMessage>(msgType::registration, msgOutcome::success, target);
    insertMessageForSiteIds({target.getSiteId()}, response);

    return target;
}

const user SymServer::login(const std::string &username, const std::string &pwd) {
    SymServerException::SymServerExceptionCodes exCode=SymServerException::userNotLogged;
    if(!userIsRegistered(username))
        throw SymServerException(SymServerException::userNotRegistered, UnpackFileLineFunction());

    user& target=getRegistered(username);
    if(!target.hasPwd(pwd))
        exCode=SymServerException::userWrongPwd;
    else if(userIsActive(username))
        exCode=SymServerException::userAlreadyActive;

    if(exCode!=SymServerException::userNotLogged)
        throw SymServerException(exCode, UnpackFileLineFunction());

    auto response=std::make_shared<loginMessage>(msgType::login, msgOutcome::success, target);
    insertMessageForSiteIds({target.getSiteId()}, response);

    active[username]=&target;
    return target;
}

std::shared_ptr<file>
SymServer::openSource(const std::string &opener, const std::string &path, const std::string &name, privilege reqPriv) {
    if(!userIsActive(opener))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    const user& target=getRegistered(opener);
    std::shared_ptr<file> fileReq= target.openFile(path, name, reqPriv);
    handleUserState(opener, fileReq->getDoc().getId());
    document& docReq= fileReq->access(target, reqPriv);
    workingDoc[opener].push_front(&docReq);
    resIdToSiteId[docReq.getId()].push_front(target.getSiteId());

    //Propagation to other clients
    auto toSend=std::make_shared<updateActiveMessage>(msgType::addActiveUser, msgOutcome::success, target.makeCopyNoPwd(), docReq.getId(), privilege::owner);
    auto siteIdToSend= siteIdsFor(docReq.getId(), target.getSiteId());
    insertMessageForSiteIds(siteIdToSend, toSend);

    //response to client
    auto response=std::make_shared<sendResMessage>(msgType::openRes, msgOutcome::success, fileReq);
    insertMessageForSiteIds({target.getSiteId()}, response);
    return fileReq;
}

std::shared_ptr<file>
SymServer::openNewSource(const std::string &opener, const std::string &resourceId, const std::string &destPath,
                         const std::string &destName, privilege reqPriv) {
    if(!userIsActive(opener))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    const user& target=getRegistered(opener);

    std::pair<int, std::shared_ptr<file>> fileReq=target.accessFile(resourceId, destPath, destName);
    handleUserState(opener, fileReq.second->getDoc().getId());
    document& docReq=fileReq.second->access(target, reqPriv);
    workingDoc[opener].push_front(&docReq);
    resIdToSiteId[docReq.getId()].push_front(target.getSiteId());

    //Propagation to other clients
    auto toSend=std::make_shared<updateActiveMessage>(msgType::addActiveUser, msgOutcome::success, target.makeCopyNoPwd(), docReq.getId(), privilege::owner);
    auto siteIdToSend= siteIdsFor(docReq.getId(), target.getSiteId());
    insertMessageForSiteIds(siteIdToSend, toSend);

    //response to client
    auto response=std::make_shared<sendResMessage>(msgType::openNewRes, msgOutcome::success, fileReq.second, fileReq.first);
    insertMessageForSiteIds({target.getSiteId()}, response);
    return fileReq.second;
}

const document & SymServer::createNewSource(const std::string &opener, const std::string &path, const std::string &name) {
    if(!userIsActive(opener))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    user& target=getRegistered(opener);
    std::shared_ptr<file> fileCreated= target.newFile(name, path, 0);
    document& docReq=fileCreated->access(target, privilege::owner);
    workingDoc[opener].push_front(&docReq);
    resIdToSiteId[docReq.getId()].push_front(target.getSiteId());
    return docReq;
}

std::shared_ptr<directory> SymServer::createNewDir(const std::string &opener, const std::string &path, const std::string &name) {
    if(!userIsActive(opener))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    user& target=getRegistered(opener);
    return target.newDirectory(name, path);
}

void SymServer::remoteInsert(const std::string &inserter, uint_positive_cnt::type resourceId, symbolMessage &symMsg) {
    if(!userIsActive(inserter))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    std::pair<bool, document*> docRetrieved=userIsWorkingOnDocument(inserter, resourceId);
    if(!docRetrieved.first)
        throw SymServerException(SymServerException::userNotWorkingOnDoc, UnpackFileLineFunction());
    symMsg.clearAuthParam();
    docRetrieved.second->remoteInsert(symMsg.getSiteId(), symMsg.verifySym().getSym());
    insertMessageForSiteIds(siteIdsFor(resourceId, getRegistered(inserter).getSiteId()), std::shared_ptr<serverMessage>(new symbolMessage(symMsg)));
    generateSimpleResponse(getRegistered(inserter).getSiteId(), msgType::insertSymbol);
}

void SymServer::remoteRemove(const std::string &remover, uint_positive_cnt::type resourceId, symbolMessage &rmMsg) {
    if(!userIsActive(remover))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    std::pair<bool, document*> docRetrieved=userIsWorkingOnDocument(remover, resourceId);
    user& actionU=getRegistered(remover);
    if(!docRetrieved.first)
        throw SymServerException(SymServerException::userNotWorkingOnDoc, UnpackFileLineFunction());
    rmMsg.clearAuthParam();
    docRetrieved.second->remoteRemove(0, rmMsg.verifySym().getSym());
    insertMessageForSiteIds(siteIdsFor(resourceId, actionU.getSiteId()), std::shared_ptr<serverMessage>(new symbolMessage(rmMsg)));

    generateSimpleResponse(actionU.getSiteId(), msgType::removeSymbol);
}

void SymServer::updateCursorPos(const std::string &targetUser, uint_positive_cnt::type resourceId, cursorMessage &crMsg) {
    if(!userIsActive(targetUser))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    std::pair<bool, document*> docRetrieved=userIsWorkingOnDocument(targetUser, resourceId);
    user& actionU=getRegistered(targetUser);
    if(!docRetrieved.first)
        throw SymServerException(SymServerException::userNotWorkingOnDoc, UnpackFileLineFunction());
    crMsg.clearAuthParam();
    docRetrieved.second->updateCursorPos(actionU.getSiteId(), crMsg.getRow(), crMsg.getCol());
    insertMessageForSiteIds(siteIdsFor(resourceId, actionU.getSiteId()), std::shared_ptr<serverMessage>(new cursorMessage(crMsg)));

    generateSimpleResponse(actionU.getSiteId(), msgType::updateCursor);
}

privilege SymServer::editPrivilege(const std::string &actionUser, const std::string &targetUser, const std::string &resPath,
                                   const std::string &resName, privilege newPrivilege) {
    if(!userIsActive(actionUser) || !userIsRegistered(targetUser))
        throw SymServerException(SymServerException::actionUserNotLoggedOrTargetUserNotRegistered, UnpackFileLineFunction());
    std::string pathFromUserHome="./"+resPath.substr(strlen("./")+strlen(actionUser.c_str())+strlen("/"));
    const user& actionU=getRegistered(actionUser);
    int docId= handleAccessToDoc(actionUser, resName, pathFromUserHome, actionU);
    handleUserState(targetUser, docId);
    privilege oldPriv=actionU.editPrivilege(targetUser, pathFromUserHome, resName, newPrivilege);

    std::forward_list<uint_positive_cnt::type> setSiteIds= siteIdOfUserOfDoc(resIdOfDocOfUser(actionUser), actionU.getSiteId());

    auto toSend=std::make_shared<privMessage>(msgType::changePrivileges, std::make_pair(actionUser,""), msgOutcome::success, std::to_string(docId), targetUser, privilege::readOnly);
    insertMessageForSiteIds(setSiteIds, toSend);

    generateSimpleResponse(actionU.getSiteId(), msgType::changePrivileges);
    return oldPriv;
}

int
SymServer::handleAccessToDoc(const std::string &actionUser, const std::string &resName,
                             const std::string &pathFromUserHome, const user &actionU) {
    std::shared_ptr<file> fileReq= actionU.openFile(pathFromUserHome, resName, privilege::owner);
    document& docReq=fileReq->access(actionU, privilege::owner);
    int docId=docReq.getId();
    if(!userIsWorkingOnDocument(actionUser, docId).first)
        docReq.close(actionU);
    return docId;
}

void SymServer::handleUserState(const std::string &targetUser, int docId, bool working) {
    if(userIsWorkingOnDocument(targetUser, docId).first==working){
        SymServerException::SymServerExceptionCodes code;
        if(working)
            code=SymServerException::userWorkingOnDoc;
        else
            code=SymServerException::userNotWorkingOnDoc;
        throw SymServerException(code, UnpackFileLineFunction());
    }

}

std::shared_ptr<filesystem> SymServer::shareResource(const std::string &actionUser, const std::string &resPath, const std::string &resName,
                                                     const uri &newPrefs) {
    if(!userIsActive(actionUser))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    const user& actionU=getRegistered(actionUser);
    auto res= actionU.shareResource(resPath, resName, newPrefs);

    auto toSend=std::make_shared<uriMessage>(msgType::shareRes, make_pair(actionUser, ""), msgOutcome ::success, resPath, resName, newPrefs);
    int docId= std::dynamic_pointer_cast<file>(res)->getDoc().getId();

    std::forward_list<uint_positive_cnt::type> setSiteIds= siteIdsFor(docId, actionU.getSiteId());
    insertMessageForSiteIds(setSiteIds, toSend);

    generateSimpleResponse(actionU.getSiteId(), msgType::shareRes);
    return res;
}

std::shared_ptr<filesystem>
SymServer::renameResource(const std::string &renamer, const std::string &resPath, const std::string &resName,
                          const std::string &newName) {
    if(!userIsActive(renamer))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    user& actionU=getRegistered(renamer);
    auto res=actionU.renameResource(resPath, resName, newName);

    generateSimpleResponse(actionU.getSiteId(), msgType::changeResName);
    return res;
}

std::shared_ptr<filesystem>
SymServer::removeResource(const std::string &remover, const std::string &resPath, const std::string &resName) {
    if(!userIsActive(remover))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    user& actionU=getRegistered(remover);
    auto res=actionU.removeResource(resPath, resName);

    generateSimpleResponse(actionU.getSiteId(), msgType::removeRes);
    return res;
}

void SymServer::closeSource(const std::string &actionUser, uint_positive_cnt::type resIdtoClose) {
    std::pair<bool, document*> toClose=userIsWorkingOnDocument(actionUser, resIdtoClose);
    if(!toClose.first)
        throw SymServerException(SymServerException::userNotWorkingOnDoc, UnpackFileLineFunction());
    user& actionU=getRegistered(actionUser);
    toClose.second->close(actionU);
    workingDoc[actionUser].remove_if([resIdtoClose](document* doc){return resIdtoClose== doc->getId();});
    resIdToSiteId[resIdtoClose].remove(actionU.getSiteId());

    auto toSend= std::make_shared<updateActiveMessage>(msgType::removeActiveUser, msgOutcome::success, getRegistered(actionUser).makeCopyNoPwd(), resIdtoClose);
    insertMessageForSiteIds(siteIdsFor(resIdtoClose, actionU.getSiteId()), toSend);

    generateSimpleResponse(actionU.getSiteId(), msgType::closeRes);
}

const user & SymServer::editUser(const std::string &username, user &newUserData) {
    if(!userIsActive(username))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    user& toEdit=getRegistered(username);
    toEdit.setNewData(newUserData);

    auto toSend= std::make_shared<userDataMessage>(msgType::changeUserData, std::make_pair(username, ""), msgOutcome::success, toEdit.makeCopyNoPwd());

    //notification to other clients interested in changes on this user
    std::forward_list<uint_positive_cnt::type> setSiteIds= siteIdOfUserOfDoc(resIdOfDocOfUser(username), toEdit.getSiteId());
    insertMessageForSiteIds(setSiteIds, toSend);

    generateSimpleResponse(toEdit.getSiteId(), msgType::changeUserData);
    return toEdit;
}

void SymServer::removeUser(const std::string &username, const std::string &pwd) {
    if (!userIsRegistered(username))
        throw SymServerException(SymServerException::userNotRegistered, UnpackFileLineFunction());
    user& toRemove=getRegistered(username);
    if(!toRemove.hasPwd(pwd))
        throw SymServerException(SymServerException::userWrongPwd, UnpackFileLineFunction());
    closeAllDocsAndPropagateMex(toRemove, workingDoc[username]);
    active.erase(username);
    removeRegistered(username);

    generateSimpleResponse(toRemove.getSiteId(), msgType::removeUser);
}

void SymServer::logout(const std::string &username) {
    if(!userIsActive(username))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    user& toLogOut=getRegistered(username);
    closeAllDocsAndPropagateMex(toLogOut, workingDoc[username]);
    active.erase(username);

    generateSimpleResponse(toLogOut.getSiteId(), msgType::logout);
}

std::map<uint_positive_cnt::type, user> SymServer::mapSiteIdToUser(const std::string& actionUser, uint_positive_cnt::type resourceId) {
    std::pair<bool, document*> retrieved=userIsWorkingOnDocument(actionUser, resourceId);
    if(!retrieved.first)
        throw SymServerException(SymServerException::userNotWorkingOnDoc, UnpackFileLineFunction());
    std::set<uint_positive_cnt::type> siteIds=retrieved.second->retrieveSiteIds();
    std::map<uint_positive_cnt::type, user> result;
    //FIXME: così potrebbe essere lenta, complessità theta(N*size(siteIds))
    // con N pari alla dimensione di registered.
    // Idea: findUserBySiteId potrebbe ricevere un elenco di siteId e tornare
    // un elenco di user. In questo modo la complessità diventa O(N)
    for(int siteId:siteIds){
        user founded= findUserBySiteId(siteId);
        result.emplace(siteId, founded);
    }

    //response to client
    auto response=std::make_shared<mapMessage>(msgType::mapChangesToUser, msgOutcome::success, result);
    insertMessageForSiteIds({getRegistered(actionUser).getSiteId()}, response);

    return result;
}

bool SymServer::userIsRegistered(const std::string &toCheck) const noexcept {
    return registered.count(toCheck)==1;
}

bool SymServer::userIsValid(const user &toCheck) noexcept {
    std::regex pathPattern{R"(\:\/resources/avatar\/([a-zA-Z 0-9]*)*([a-zA-Z 0-9]*\.((jpg|png|ico|bmp))))"};
    return !toCheck.getUsername().empty() && !toCheck.getNickname().empty()
            && std::regex_match(toCheck.getIconPath(), pathPattern);
}

SymServer::SymServer() {
rootDir=directory::getRoot();
}

template<class Archive>
void SymServer::serialize(Archive &ar, const unsigned int version)
{
    ar & registered & idCounter  & rootDir;
}

bool SymServer::userIsActive(const std::string &username) const{
    return active.count(username)==1;
}

std::pair<bool, document*> SymServer::userIsWorkingOnDocument(const std::string &username, int resourceId) const {
    std::pair<bool, document*> result(false, nullptr);
    if(workingDoc.count(username)==0) return result;
    for(auto doc:workingDoc.at(username))
        if (doc->getId()==resourceId){
            result.first=true; result.second=doc;
            break;
        }
    return result;
}

user SymServer::findUserBySiteId(int id) const noexcept{
    for(const auto& elem:registered)
        if(elem.second.getSiteId()==id)
            return elem.second;
    return unknownUser;
}

user &SymServer::registerUser(const user &toInsert) {
    return registered[toInsert.getUsername()]=toInsert;
}

user &SymServer::getRegistered(const std::string &username) {
    return registered.at(username);
}

void SymServer::removeRegistered(const std::string &username) {
    registered.erase(username);
}

std::forward_list<uint_positive_cnt::type> SymServer::siteIdsFor(uint_positive_cnt::type resId, uint_positive_cnt::type siteIdToExclude) const {
    auto it=resIdToSiteId.find(resId);
    auto siteIds=it->second;
    if(it == resIdToSiteId.end())
        throw SymServerException(SymServerException::noUserWorkingOnRes, UnpackFileLineFunction());
    if(siteIdToExclude>=0)
        siteIds.remove(siteIdToExclude);
    return siteIds;
}

void SymServer::insertMessageForSiteIds(const std::forward_list<uint_positive_cnt::type> &siteIds, std::shared_ptr<serverMessage> toSend) {
    for(uint_positive_cnt::type id:siteIds){
        siteIdToMex[id].push(toSend);
    }
}

std::forward_list<uint_positive_cnt::type> SymServer::resIdOfDocOfUser(const std::string &username) const {
    std::forward_list<uint_positive_cnt::type> resIds;
    if(workingDoc.count(username)==0) return resIds;
    for(const auto* d:workingDoc.find(username)->second){
        resIds.push_front(d->getId());
    }
    return resIds;
}

std::forward_list<uint_positive_cnt::type>
SymServer::siteIdOfUserOfDoc(const std::forward_list<uint_positive_cnt::type> &resIds, unsigned int siteIdToExclude) const {
    std::set<int> siteIds;
    for(int resId:resIds){
        for(int siteId:siteIdsFor(resId, siteIdToExclude)){
            siteIds.insert(siteId);
        }
    }
    return std::forward_list<uint_positive_cnt::type>(siteIds.begin(), siteIds.end());
}

void SymServer::closeAllDocsAndPropagateMex(const user &loggedOut, const std::forward_list<document*>& listOfDocs) {
    for(document* doc: listOfDocs) {
        doc->close(loggedOut);
        auto toSend=std::make_shared<updateActiveMessage>(msgType::removeActiveUser, msgOutcome::success, loggedOut.makeCopyNoPwd(), doc->getId());
        insertMessageForSiteIds(siteIdsFor(doc->getId(), loggedOut.getSiteId()), toSend);
    }
    handleLeavingUser(loggedOut);
}

void SymServer::handleLeavingUser(const user &loggedOut) {
    auto l=resIdOfDocOfUser(loggedOut.getUsername());
    for (int resId:l){
        resIdToSiteId[resId].remove(loggedOut.getSiteId());
    }
    workingDoc.erase(loggedOut.getUsername());
    siteIdToMex.erase(loggedOut.getSiteId());
}

std::pair<const int, std::shared_ptr<serverMessage>> SymServer::extractNextMessage() {
    std::pair<int, std::shared_ptr<serverMessage>> result(-1, nullptr);
    if(siteIdToMex.empty())
        return result;
    for(std::pair<const int, std::queue<std::shared_ptr<serverMessage>>> mexForSiteId:siteIdToMex)
        if(!mexForSiteId.second.empty()){
            result.first=mexForSiteId.first;
            result.second=mexForSiteId.second.front();
            mexForSiteId.second.pop();
            break;
        }
    return result;
}

void SymServer::generateSimpleResponse(unsigned int recvSiteId, msgType action) {
    auto response=std::make_shared<serverMessage>(action, msgOutcome::success);
    insertMessageForSiteIds({recvSiteId}, response);
}

bool SymServer::operator==(const SymServer &rhs) const {
    return registered == rhs.registered && idCounter == rhs.idCounter;
}

bool SymServer::operator!=(const SymServer &rhs) const {
    return !(rhs == *this);
}

uint_positive_cnt::type SymServer::getSiteIdOfUser(const std::string &username) const {
    return registered.at(username).getSiteId();
}


BOOST_CLASS_EXPORT(Symposium::SymServer)



