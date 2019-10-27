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
#include <iostream>
#include "user.h"

user::user(const std::string &username, const std::string &pwd, const std::string &nickname,
           const std::string &iconPath,
           int siteId, std::shared_ptr<directory> home) : username(username), siteId(siteId),
                                                          nickname(nickname), iconPath(iconPath), home(home)
{
    //generating random salt for user
    std::random_device device;
    std::mt19937 generator(device());
    std::srand(time(NULL));
    std::uniform_int_distribution<> dis(20, 30);
    std::poisson_distribution<int> distribution(rand());

    for (size_t i = 0; i < dis(generator); i++)//salt have random lenght between 20 and 30
    {
        hashSalt+=static_cast<char>(distribution(generator));//the sequence of random characters used as salt
    }
    //saving the password with the use of hash algorithm sha256
    pwdHash=sha256(pwd+hashSalt);
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

void user::setPwd(const std::string &pwd) {
    //TODO: implement
    user::pwdHash = pwd;
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

    return home->print(*this, recursive);

}

std::shared_ptr<file> user::newFile(const std::string &fileName, const std::string &pathFromHome) const {

    std::shared_ptr<file> newF=home->addFile(pathFromHome, fileName);
    if(newF!= nullptr)
        return newF;
    throw userException("An error occurred while creating a new file");
}

std::shared_ptr<directory> user::newDirectory(const std::string &dirName, const std::string &pathFromHome) const{
    std::shared_ptr<directory> newD=home->addDirectory(dirName);
    if(newD!= nullptr)
        return newD;
    throw userException("An error occurred while creating a new directory");
}

std::shared_ptr<file> user::accessFile(const std::string &resId, const std::string &path,  const std::string &fileName ) const {
    std::string str1;
    std::string str2;
    std::string str3;
    std::string str4;
    std::size_t found = path.find_last_of("/\\");
    str1.append(path,0, found); //path to the directory of the current user
    str2.append(path.begin()+found+1,path.end()); //the id of directory where the current user want to insert the file
    std::shared_ptr<directory> dir=this->home->getDir(str1, str2);
    if(dir== nullptr)
        throw userException("An error occurred while trying to access the directory where you want save the link");

    std::shared_ptr<directory> root1=dir->getRoot();
    if(root1== nullptr)
        throw userException("A system error");

    std::shared_ptr<symlink> sym=home->addLink(path, fileName);
    if(sym ==nullptr)
        throw userException("An error occurred while trying to add link");

    found = resId.find_last_of("/\\");
    str3.append(resId,0, found); //absolute path to the directory of the file to add
    str4.append(resId.begin()+found+1,resId.end()); //the id of file to add
    std::shared_ptr<file> fi=root1->getFile(str3, str4);;
    if(fi == nullptr)
        throw userException("An error occurred while trying to add link");
    return fi;
}

document & user::openFile(const std::string &path, const std::string &fileName, privilege accessMode) const {
    document& newD= home->access(*this, path, fileName, accessMode);
    return newD;
}

privilege user::editPrivilege(const user &otherUser, const std::string &resPath, const std::string &resName,
                              privilege newPrivilege) const {
    std::shared_ptr<file> newF=home->getFile(resPath, resName);
    if(newF==nullptr)
        throw userException("An error occurred while trying to edit privilege");
    privilege newP;
    newP=newF->setUserPrivilege(otherUser, newPrivilege);
    return newP;
}

privilege user::changePrivilege(const std::string &resPath, const std::string &resName, privilege newPrivilege) {
    std::shared_ptr<file> newF=home->getFile(resPath, resName);
    if(newF==nullptr)
        throw userException("An error occurred while trying to change privilege");
    privilege newP;
    newP=newF->setUserPrivilege(*this, newPrivilege);
    return newP;
}

uri user::shareResource(const std::string &resPath, const std::string &resName, uri &newPrefs) const {
    std::shared_ptr<file> newF=home->getFile(resPath, resName);
    if(newF==nullptr)
        throw userException("An error occurred while trying to share resource");
    uri u;
    u=newF->setSharingPolicy(*this, newPrefs);
    return u;
}



bool user::operator==(const user &rhs) const {
    return this->username == rhs.username;
}

bool user::operator!=(const user &rhs) const {
    return !(rhs == *this);
}

const std::shared_ptr<directory> &user::getHome() const {
    return home;
}

bool user::hasPwd(const std::string &pwd) const {
    return sha256(pwd+hashSalt)==pwdHash;
}

void user::setNewData(const user &newData) {
    username=newData.username;
    pwdHash=newData.pwdHash;
    nickname=newData.nickname;
}

const std::string &user::getPwdHash() const {
    return pwdHash;
}

std::shared_ptr<filesystem>
user::renameResource(const std::string &resPath, const std::string &resName, const std::string &newName) const {
    //TODO: implement
    return std::shared_ptr<filesystem>();
}
