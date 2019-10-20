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
#include <regex>

int SymServer::idCounter=0;

const user SymServer::addUser(const user &newUser) {
    if(userIsRegistered(newUser.getUsername()))
        throw SymServerException("SymServer::addUser: the user already exists");
    if(!userIsValid(newUser)){
        throw SymServerException("SymServer::addUser: the user has wrong parameters");
    }
    auto userDir=rootDir->addDirectory(newUser.getUsername());
    user inserted=registered[newUser.getUsername()]=std::move(newUser);
    inserted.setHome(userDir);
    inserted.setSiteId(idCounter++);
    return inserted;
}

const user SymServer::login(const std::string &username, const std::string &pwd) {
    if(!userIsRegistered(username))
        throw SymServerException("SymServer::login: the user is not registered");
    user& target=registered[username];
    if(!target.hasPwd(pwd))
        throw SymServerException("SymServer::login: wrong password");
    if(userIsActive(username))
        throw SymServerException("SymServer::login: user already active");
    active[username]=&target;
    return target;
}

document
SymServer::openSource(const user &opener, const std::string &path, const std::string &name, privilege reqPriv) {
    if(!userIsActive(opener.getUsername()))
        throw SymServerException("SymServer::openSource: the user is not logged in");
    document docReq= opener.openFile(path, name, reqPriv);
    workingDoc[opener.getUsername()].push_front(&docReq);
    return docReq;
}

document
SymServer::openNewSource(const user &opener, const std::string &path, const std::string &name, privilege reqPriv,
                         const std::string &destPath) {
    if(!userIsActive(opener.getUsername()))
        throw SymServerException("SymServer::openNewSource: the user is not logged in");
    std::shared_ptr<file> fileReq=opener.accessFile(path+"/"+name, destPath);
    document docReq=fileReq->access(opener, uri::getDefaultPrivilege());
    workingDoc[opener.getUsername()].push_front(&docReq);
    return docReq;
}

document SymServer::createNewSource(const user &opener, const std::string &path, const std::string &name) {
    if(!userIsActive(opener.getUsername()))
        throw SymServerException("SymServer::createNewSource: the user is not logged in");
    std::shared_ptr<file> fileCreated=opener.newFile(name, path);
    document docReq=fileCreated->access(opener, privilege::owner);
    workingDoc[opener.getUsername()].push_front(&docReq);
    return docReq;
}

std::shared_ptr<directory> SymServer::createNewDir(const user &opener, const std::string &path, const std::string &name) {
    if(!userIsActive(opener.getUsername()))
        throw SymServerException("SymServer::createNewDir: the user is not logged in");
    return opener.newDirectory(name, path);
}

void SymServer::remoteInsert(const std::string &inserter, int resourceId, const symbol &newSym) {
    //TODO: to implement
}

void SymServer::remoteRemove(const std::string &remover, int resourceId, const symbol &rmSym) {
    //TODO: to implement
}

privilege SymServer::editPrivilege(const user &actionUser, const user &targetUser, const std::string &resPath,
                                   const std::string &resName, privilege newPrivilege) {
    //TODO: to implement
    return privilege::none;
}

uri SymServer::shareResource(const user &actionUser, const std::string &resPath, const std::string &resName,
                             uri &newPrefs) {
    return uri();
}

std::shared_ptr<filesystem>
SymServer::renameResource(const user &renamer, const std::string &resPath, const std::string &resName,
                          const std::string &newName) {
    //TODO: to implement
    return std::shared_ptr<filesystem>();
}

std::shared_ptr<filesystem>
SymServer::removeResource(const user &remover, const std::string &resPath, const std::string &resName) {
    //TODO: to implement
    return std::shared_ptr<filesystem>();
}

void SymServer::closeSource(const user &actionUser, document &toClose) {
    //TODO: to implement
}

const user SymServer::editUser(const std::string &username, const std::string &pwd, user &newUserData) {
    //TODO: to implement
    return user("", "", "", "", 0,
                std::shared_ptr<directory>());
}

void SymServer::removeUser(const std::string &username, const std::string &pwd) {
    if (!userIsRegistered(username))
        throw SymServerException("SymServer::removeUser: the user is not registered");
    auto listOfDocs=workingDoc[username];
    for(document* doc: listOfDocs)
        doc->close(registered[username]);
    workingDoc.erase(username);
    registered.erase(username);
}

void SymServer::logout(const std::string &username, const std::string &pwd) {
    if(!userIsActive(username))
        throw SymServerException("SymServer::logout: the user is not logged in");
    auto listOfDocs=workingDoc[username];
    for(document* doc: listOfDocs)
        doc->close(registered[username]);
    workingDoc.erase(username);
    active.erase(username);
}

std::map<int, user> SymServer::mapSiteIdToUser(const std::string& actionUser, int resourceId) {
    //TODO: to implement
    return std::map<int, user>();
}

bool SymServer::userIsRegistered(const std::string &toCheck) {
    return registered.find(toCheck)!=registered.end();
}

bool SymServer::userIsValid(const user &toCheck) {
    std::regex pathPattern{"\.?(/[a-zA-Z 0-9]*)+[a-zA-Z]*\.*"};
    return toCheck.getUsername()!="" && toCheck.getNickname()!=""
            && std::regex_match(toCheck.getIconPath(), pathPattern);
}

SymServer::SymServer() {
rootDir=directory::getRoot();
}

bool SymServer::userIsActive(const std::string &username) {
    return active.find(username)!=active.end();
}


