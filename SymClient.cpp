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

using namespace Symposium;

SymClient::SymClient() :loggedUser("SomeUsername", "P@assW0rd!!", "noempty", "", 0, nullptr){
    //TODO: implement
}

void SymClient::setLoggedUser(const user &loggedUser) {
    //TODO:implement
}

signUpMessage SymClient::signUp(const std::string &username, const std::string &pwd, const std::string &nickname,
                                const std::string &iconPath) {
    return signUpMessage(msgType::login, {"", ""}, user("SomeUsername", "P@assW0rd!!", "noempty", "./icons/a.jpg", -1, nullptr));
}

void SymClient::signUp(const user &logged) {
    setLoggedUser(logged);
    //TODO:implement
}

clientMessage SymClient::logIn(const std::string &username, const std::string &pwd) {
    return clientMessage(msgType::login, {"", ""});
}

void SymClient::logIn(const user &logged) {
    //TODO: implement
}

askResMessage SymClient::openSource(const std::string &path, const std::string &name, privilege reqPriv) {
    //TODO:implement
    return askResMessage(msgType::openRes, {"", ""}, "", "", "", uri::getDefaultPrivilege(), 0);
}

void SymClient::openSource(const std::shared_ptr<file> fileAsked) {
    //TODO:implement
}

askResMessage
SymClient::openNewSource(const std::string &resourceId, privilege reqPriv, const std::string &destPath, const std::string& destName) {
    //TODO:implement
    return askResMessage(msgType::openRes, {"", ""}, "", "", "", uri::getDefaultPrivilege(), 0);
}

void SymClient::openNewSource(const std::string &resId, privilege reqPriv, const std::string &destPath,
                              const std::string &destName,
                              int idToAssign, const std::shared_ptr<file> fileAsked) {
    //TODO:implement
}

askResMessage SymClient::createNewSource(const std::string &path, const std::string &name) {
    //TODO:implement
    return askResMessage(msgType::openRes, {"", ""}, "", "", "", uri::getDefaultPrivilege(), 0);
}

void SymClient::createNewSource(const std::string &path, const std::string &name, int idToAssign) {
    //TODO:implement
}

askResMessage SymClient::createNewDir(const std::string &path, const std::string &name) {
    //TODO:implement
    return askResMessage(msgType::openRes, {"", ""}, "", "", "", uri::getDefaultPrivilege(), 0);
}

void SymClient::createNewDir(const std::string &path, const std::string &name, int idToAssign) {
    //TODO:implement
}

symbolMessage SymClient::localInsert(int resourceId, const symbol &newSym, const std::pair<int, int> &index) {
    return symbolMessage(msgType::insertSymbol, {"", ""}, msgOutcome::success, 0, 0, symbol('a', 0, 0, {}, false));
}

symbolMessage SymClient::localRemove(int resourceId, int indexes[2]) {
    return symbolMessage(msgType::insertSymbol, {"", ""}, msgOutcome::success, 0, 0, symbol('a', 0, 0, {}, false));
}

void SymClient::remoteInsert(int resourceId, const symbol &newSym) {
    //TODO:implement
}

void SymClient::remoteRemove(int resourceId, const symbol &rmSym) {
    //TODO:implement
}

privMessage SymClient::editPrivilege(const std::string &targetUser, const std::string &resPath, const std::string &resName,
                                     privilege newPrivilege) {
    return privMessage(msgType::openRes, {"",""}, msgOutcome::success, "", "", privilege::none);
}

privilege SymClient::editPrivilege(const std::string &targetUser, const std::string &resPath, const std::string &resName,
                                   privilege newPrivilege, bool msgRcv) {
    //TODO:implement
    return privilege::none;
}

uriMessage SymClient::shareResource(const std::string &resPath, const std::string &resName, uri &newPrefs) {
    //TODO:implement
    return uriMessage(msgType::shareRes, {"", ""}, msgOutcome::success, "", "", uri(), 0);
}

uri SymClient::shareResource(const std::string &resPath, const std::string &resName, uri &newPrefs, bool msgRcv) {
    //TODO:implement
    return uri();
}

askResMessage
SymClient::renameResource(const std::string &resPath, const std::string &resName, const std::string &newName) {
    //TODO:implement
    return askResMessage(msgType::openRes, {"", ""}, "", "", "", uri::getDefaultPrivilege(), 0);
}

std::shared_ptr<filesystem>
SymClient::renameResource(const std::string &resPath, const std::string &resName, const std::string &newName,
                          bool msgRcv) {
    //TODO:implement
    return std::shared_ptr<filesystem>();
}

askResMessage SymClient::removeResource(const std::string &resPath, const std::string &resName) {
    //TODO:implement
    return askResMessage(msgType::openRes, {"", ""}, "", "", "", uri::getDefaultPrivilege(), 0);
}

std::shared_ptr<filesystem>
SymClient::removeResource(const std::string &resPath, const std::string &resName, bool msgRcv) {
    //TODO:implement
    return std::shared_ptr<filesystem>();
}

std::string SymClient::showDir(bool recursive) const {
    return "";
}

updateDocMessage SymClient::closeSource(int resourceId) {
    //TODO:implement
    return updateDocMessage(msgType::closeRes, {"",""}, 0);
}
//FIXME: add and review tests for editUser
userDataMessage SymClient::editUser(user &newUserData) {
    //TODO: to implement
    return userDataMessage(msgType::changeUserNick, std::pair("", ""), msgOutcome::success, Symposium::user());
}

const user SymClient::editUser(user &newUserData, bool msgRcv) {
    //TODO: to implement
    return user("", "", "", "", 0,
                std::shared_ptr<directory>());
}

clientMessage SymClient::removeUser() {
    //TODO: to implement
    return clientMessage(msgType::removeUser, {"", ""});
}

clientMessage SymClient::logout() {
    //TODO: to implement
    return clientMessage(msgType::logout, {"", ""});
}

updateDocMessage SymClient::mapSiteIdToUser(const document &currentDoc) {
    //TODO: to implement
    return updateDocMessage(msgType::mapChangesToUser, {"", ""}, 0);
}

void SymClient::setUserColors(const std::map<int, user> &siteIdToUser) {
    //TODO:implement
}

void SymClient::addActiveUser(int resourceId, user &targetUser) {
    //TODO: to implement
}

void SymClient::removeActiveUser(int resourceId, user &targetUser) {
    //TODO: to implement
}

user &SymClient::getLoggedUser() {
    return loggedUser;
}

clientMessage& SymClient::retrieveRelatedMessage(const serverMessage& smex) {
    //TODO: to implement
    throw std::exception();
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
