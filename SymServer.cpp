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

using namespace Symposium;


int SymServer::idCounter=0;
const user SymServer::unknownUser("unknown", "@dummY!Pwd", "unknown", "./icons/1.png", -1, nullptr);

const user & SymServer::addUser(user &newUser) {
    if(userIsRegistered(newUser.getUsername()))
        throw SymServerException(SymServerException::userAlreadyExist, UnpackFileLineFunction());
    if(!userIsValid(newUser)){
        throw SymServerException(SymServerException::userWrongParam, UnpackFileLineFunction());
    }
    auto userDir=rootDir->addDirectory(newUser.getUsername());
    newUser.setHome(userDir);
    newUser.setSiteId(idCounter++);
    user& inserted=registerUser(&newUser);
    return inserted;
}

const user SymServer::login(const std::string &username, const std::string &pwd) {
    if(!userIsRegistered(username))
        throw SymServerException(SymServerException::userNotRegistered, UnpackFileLineFunction());
    user& target=getRegistered(username);
    if(!target.hasPwd(pwd))
        throw SymServerException(SymServerException::userWrongPwd, UnpackFileLineFunction());
    if(userIsActive(username))
        throw SymServerException(SymServerException::userAlreadyActive, UnpackFileLineFunction());
    active[username]=&target;
    return target;
}

std::shared_ptr<file>
SymServer::openSource(const std::string &opener, const std::string &path, const std::string &name, privilege reqPriv) {
    if(!userIsActive(opener))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    const user& target=getRegistered(opener);
    std::shared_ptr<file> fileReq= target.openFile(path, name, reqPriv);
    document& docReq= fileReq->access(target, reqPriv);
    workingDoc[opener].push_front(&docReq);
    return fileReq;
}

std::shared_ptr<file>
SymServer::openNewSource(const std::string &opener, const std::string &resourceId, const std::string &destPath,
                         const std::string &destName, privilege reqPriv) {
    if(!userIsActive(opener))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    const user& target=getRegistered(opener);
    std::shared_ptr<file> fileReq=target.accessFile(resourceId, destPath, destName);
    document& docReq=fileReq->access(target, reqPriv);
    workingDoc[opener].push_front(&docReq);
    return fileReq;
}

const document & SymServer::createNewSource(const std::string &opener, const std::string &path, const std::string &name) {
    if(!userIsActive(opener))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    user& target=getRegistered(opener);
    std::shared_ptr<file> fileCreated=target.newFile(name, path);
    document& docReq=fileCreated->access(target, privilege::owner);
    workingDoc[opener].push_front(&docReq);
    return docReq;
}

std::shared_ptr<directory> SymServer::createNewDir(const std::string &opener, const std::string &path, const std::string &name) {
    if(!userIsActive(opener))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    user& target=getRegistered(opener);
    return target.newDirectory(name, path);
}

void SymServer::remoteInsert(const std::string &inserter, int resourceId, symbolMessage &symMsg) {
    if(!userIsActive(inserter))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    std::pair<bool, document*> docRetrieved=userIsWorkingOnDocument(inserter, resourceId);
    if(!docRetrieved.first)
        throw SymServerException(SymServerException::userNotWorkingOnDoc, UnpackFileLineFunction());
    docRetrieved.second->remoteInsert(symMsg.verifySym().getSym());
    workingQueue[resourceId].push(std::shared_ptr<message>(new symbolMessage(symMsg)));
}

void SymServer::remoteRemove(const std::string &remover, int resourceId, const symbolMessage &rmMsg) {
    if(!userIsActive(remover))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    std::pair<bool, document*> docRetrieved=userIsWorkingOnDocument(remover, resourceId);
    if(!docRetrieved.first)
        throw SymServerException(SymServerException::userNotWorkingOnDoc, UnpackFileLineFunction());
    docRetrieved.second->remoteRemove(rmMsg.getSym());
    //workingQueue[resourceId].push(rmMsg);
}

privilege SymServer::editPrivilege(const std::string &actionUser, const std::string &targetUser, const std::string &resPath,
                                   const std::string &resName, privilege newPrivilege) {
    if(!userIsActive(actionUser) || !userIsRegistered(targetUser))
        throw SymServerException(SymServerException::actionUserNotLoggedOrTargetUserNotRegistered, UnpackFileLineFunction());
    std::string pathFromUserHome="./"+resPath.substr(strlen("./")+strlen(actionUser.c_str())+strlen("/"));
    const user& actionU=getRegistered(actionUser);
    std::shared_ptr<file> fileReq= actionU.openFile(pathFromUserHome, resName, privilege::owner);
    document& docReq=fileReq->access(actionU, privilege::owner);
    if(userIsWorkingOnDocument(targetUser, docReq.getId()).first)
        throw SymServerException(SymServerException::userWorkingOnDoc, UnpackFileLineFunction());
    return actionU.editPrivilege(targetUser, pathFromUserHome, resName, newPrivilege);
}

uri SymServer::shareResource(const user &actionUser, const std::string &resPath, const std::string &resName,
                             uri &newPrefs) {
    if(!userIsActive(actionUser.getUsername()))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    return actionUser.shareResource(resPath, resName, newPrefs);
}

std::shared_ptr<filesystem>
SymServer::renameResource(const user &renamer, const std::string &resPath, const std::string &resName,
                          const std::string &newName) {
    if(!userIsActive(renamer.getUsername()))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    return renamer.renameResource(resPath, resName, newName);
}

std::shared_ptr<filesystem>
SymServer::removeResource(const user &remover, const std::string &resPath, const std::string &resName) {
    if(!userIsActive(remover.getUsername()))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    return remover.removeResource(resPath, resName);
}

void SymServer::closeSource(const std::string &actionUser, document &toClose) {
    if(!userIsWorkingOnDocument(actionUser,toClose.getId()).first)
        throw SymServerException(SymServerException::userNotWorkingOnDoc, UnpackFileLineFunction());
    toClose.close(getRegistered(actionUser));
    workingDoc[actionUser].remove_if([&toClose](document* doc){return toClose.getId()==doc->getId();});
}

const user & SymServer::editUser(const std::string &username, const std::string &pwd, user &newUserData) {
    if(!userIsActive(username))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    user& target=getRegistered(username);
    //user& target=registered[username];
    target.setNewData(newUserData);
    return target;
}

void SymServer::removeUser(const std::string &username, const std::string &pwd) {
    if (!userIsRegistered(username))
        throw SymServerException(SymServerException::userNotRegistered, UnpackFileLineFunction());
    auto listOfDocs=workingDoc[username];
    for(document* doc: listOfDocs)
        doc->close(getRegistered(username));
    workingDoc.erase(username);
    removeRegistered(username);
    //registered.erase(username);
}

void SymServer::logout(const std::string &username, const std::string &pwd) {
    if(!userIsActive(username))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    auto listOfDocs=workingDoc[username];
    for(document* doc: listOfDocs)
        doc->close(registered[username]);
    workingDoc.erase(username);
    active.erase(username);
}

std::map<int, user> SymServer::mapSiteIdToUser(const std::string& actionUser, int resourceId) {
    std::pair<bool, document*> retrieved=userIsWorkingOnDocument(actionUser, resourceId);
    if(!retrieved.first)
        throw SymServerException(SymServerException::userNotWorkingOnDoc, UnpackFileLineFunction());
    std::set<int> siteIds=retrieved.second->retrieveSiteIds();
    std::map<int, user> result;
    for(int siteId:siteIds){
        user founded= findUserBySiteId(siteId);
        result.emplace(siteId, founded);
    }
    return result;
}

bool SymServer::userIsRegistered(const std::string &toCheck) {
    return registered.find(toCheck)!=registered.end();
}

bool SymServer::userIsValid(const user &toCheck) {
    //OPTIMIZE: correctness check of user data should be available within user class,
    // here we should call that method(s), adding only the constrain on the icon path
    std::regex pathPattern{R"(\.(\/[a-zA-Z 0-9]*)*([a-zA-Z 0-9]*\.((jpg|png|ico|bmp))))"};
    return !toCheck.getUsername().empty() && !toCheck.getNickname().empty()
            && std::regex_match(toCheck.getIconPath(), pathPattern);
}

SymServer::SymServer() {
rootDir=directory::getRoot();
}

bool SymServer::userIsActive(const std::string &username) {
    return active.find(username)!=active.end();
}

std::pair<bool, document*> SymServer::userIsWorkingOnDocument(const std::string &username, int resourceId) {
    std::pair<bool, document*> result(false, nullptr);
    if(workingDoc.find(username)==workingDoc.end()) return result;
    for(auto doc:workingDoc[username])
        if (doc->getId()==resourceId){
            result.first=true; result.second=doc;
            break;
        }
    return result;
}

user SymServer::findUserBySiteId(int id) {
    for(const auto& elem:registered)
        if(elem.second.getSiteId()==id)
            return elem.second;
    return unknownUser;
}

user &SymServer::registerUser(user *toInsert) {
    return registered[toInsert->getUsername()]=*toInsert;
}

user &SymServer::getRegistered(const std::string &username) {
    return registered[username];
}

void SymServer::removeRegistered(const std::string &username) {
    registered.erase(username);
}


