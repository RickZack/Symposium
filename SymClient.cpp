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

SymClient::SymClient() :loggedUser("", "", "", "", 0, std::shared_ptr<directory>()){
    //TODO: implement
}

void SymClient::setLoggedUser(const user &loggedUser) {
    //TODO:implement
}

void SymClient::setUserColors(const std::map<int, user> &siteIdToUser) {
    //TODO:implement
}

signUpMessage SymClient::signUp(const std::string &username, const std::string &pwd, const std::string &nickname) {
    return signUpMessage(msgType::login, {"", ""}, user("", "", "", "", -1, nullptr));
}

void SymClient::signUp(const user &logged) {
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
    return askResMessage(msgType::openRes, {"", ""}, "", "");
}

void SymClient::openSource(const std::shared_ptr<file> fileAsked) {
    //TODO:implement
}

askResMessage
SymClient::openNewSource(const std::string &resourceId, privilege reqPriv, const std::string &destPath, const std::string& destName) {
    //TODO:implement
    return askResMessage(msgType::openRes, {"", ""}, "", "");
}

void SymClient::openNewSource(const std::shared_ptr<file> fileAsked) {
    //TODO:implement
}

askResMessage SymClient::createNewSource(const std::string &path, const std::string &name) {
    //TODO:implement
    return askResMessage(msgType::openRes, {"", ""}, "", "");
}

void SymClient::createNewSource(const std::shared_ptr<file> fileCreated) {
    //TODO:implement
}

askResMessage SymClient::createNewDir(const std::string &path, const std::string &name) {
    //TODO:implement
    return askResMessage(msgType::openRes, {"", ""}, "", "");
}

void SymClient::createNewDir(const std::shared_ptr<directory> dirCreated) {
    //TODO:implement
}

void SymClient::remoteInsert(int resourceId, const symbol &newSym) {
    //TODO:implement
}

void SymClient::remoteRemove(int resourceId, const symbol &rmSym) {
    //TODO:implement
}

privilege SymClient::editPrivilege(const user &targetUser, const std::string &resPath, const std::string &resName,
                                   privilege newPrivilege, bool msgRcv) {
    //TODO:implement
    return privilege::none;
}

uri SymClient::shareResource(const std::string &resPath, const std::string &resName, uri &newPrefs) {
    //TODO:implement
    return uri();
}

std::shared_ptr<filesystem>
SymClient::renameResource(const std::string &resPath, const std::string &resName, const std::string &newName,
                          bool msgRcv) {
    //TODO:implement
    return std::shared_ptr<filesystem>();
}

std::shared_ptr<filesystem>
SymClient::removeResource(const std::string &resPath, const std::string &resName, bool msgRcv) {
    //TODO:implement
    return std::shared_ptr<filesystem>();
}

std::string SymClient::printDir() {
    return "";
}

template<typename C>
std::string SymClient::show(C condition) {
    //TODO:implement
    return "";
}

template<typename C>
void SymClient::showChanges(C condition) {
    //TODO:implement
}

void SymClient::closeSource(int resourceId) {
    //TODO:implement
}

const user SymClient::editUser(user &newUserData, bool msgRcv) {
    //TODO: to implement
    return user("", "", "", "", 0,
                std::shared_ptr<directory>());
}

void SymClient::removeUser() {
    //TODO: to implement
}

void SymClient::logout() {
    //TODO: to implement
}

std::map<int, std::pair<user, MyColor>> SymClient::mapSiteIdToUser(const document &currentDoc, bool msgRcv) {
    //TODO: to implement
    return std::map<int, std::pair<user, MyColor>>();
}

void SymClient::exportPDF(const std::string &resPath, const std::string &resName) {
    //TODO: to implement
}

void SymClient::addActiveUser(int resourceId, user &targetUser) {
    //TODO: to implement
}

void SymClient::removeActiveUser(int resourceId, user &targetUser) {
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
