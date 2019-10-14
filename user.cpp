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
 * File:   user.cpp
 * Project: Symposium
 * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Alice Morano <s259158 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 17 Giugno 2019, 22.41
 */
#include "user.h"

user::user(const std::string &username, const std::string &pwd, const std::string &nickname,
           const std::string &iconPath,
           int siteId, std::shared_ptr<directory> home) : username(username), pwdHash(pwd), siteId(siteId),
                                                          nickname(nickname), iconPath(iconPath), home(home) {
    //TODO: implement
}

const std::string &user::getUsername() const {
    return username;
}

int user::getSiteId() const {
    return siteId;
}

const std::string &user::getNickname() const {
    return nickname;
}

const std::string &user::getIconPath() const {
    return iconPath;
}

void user::setPwdHash(const std::string &pwdHash) {
    user::pwdHash = pwdHash;
}

void user::setNickname(const std::string &nickname) {
    user::nickname = nickname;
}

void user::setSiteId(int siteId) {
    user::siteId = siteId;
}

void user::setIconPath(const std::string &iconPath) {
    user::iconPath = iconPath;
}

void user::setHome(std::shared_ptr<directory> home) {
    user::home = home;
}

std::string user::showDir(bool recursive) const {
    //TODO: implement
    return "";
}

void user::newFile(const std::string &fileName, const std::string &pathFromHome) {
<<<<<<< HEAD

    std::shared_ptr<file> newF(nullptr);
    newF=home->addFile(pathFromHome, fileName);

}

void user::newDirectory(const std::string &dirName, const std::string &pathFromHome) {
    std::shared_ptr<directory> newD(nullptr);
    newD=home->addDirectory(dirName);

}

std::shared_ptr<symlink> user::accessFile(const user &otherUser, const std::string &resId, const std::string &path,  const std::string &fileName ){
    std::shared_ptr<file> newF(nullptr);
    std::shared_ptr<symlink> newS(nullptr);
    newF=otherUser.home->getFile(resId, fileName);
    newS=this->home->addLink(newF, path, fileName);
    return newS;
=======
    //TODO: implement
}

void user::newDirectory(const std::string &dirName, const std::string &pathFromHome) {
    //TODO: implement
}

std::shared_ptr<file> accessFile(const std::string &resId, const std::string &path,  const std::string &fileName ){
    //TODO: implement
    return std::shared_ptr<file>();
>>>>>>> parent of 1bbe741... user
}

document user::openFile(const std::string &path, const std::string &fileName) {
    return document(0);
}

privilege user::editPrivilege(const user &otherUser, const std::string &resPath, const std::string &resName,
                              privilege newPrivilege) {
    //TODO: implement
    return privilege::none;
}

privilege user::changePrivilege(const std::string &resPath, const std::string &resName, privilege newPrivilege) {
    //TODO: implement
    return privilege::none;
}

uri user::shareResource(const std::string &resPath, const std::string &resName, uri &newPrefs) {
    return uri();
}


bool user::operator==(const user &rhs) const {
    return this->username == rhs.username;
}

bool user::operator!=(const user &rhs) const {
    return !(rhs == *this);
}

<<<<<<< HEAD
template <typename C>

std::shared_ptr<filesystem> user::deleteDirectory(const std::string &path, const std::string &name, C condition)
{
    std::shared_ptr<filesystem> newD(nullptr);
    newD=home->remove(*this, path, name);
    return newD;

}

template <typename C>
std::shared_ptr<filesystem> user::deleteFile(const std::string &path, const std::string &name, C condition)
{
    std::shared_ptr<filesystem> newF(nullptr);
    newF=home->remove(*this, path, name);
    return newF;

=======
std::shared_ptr<file> user::accessFile(const std::string &resId, const std::string &path, const std::string &fileName) {
    return std::shared_ptr<file>();
>>>>>>> parent of 1bbe741... user
}



