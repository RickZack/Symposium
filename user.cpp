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
#include <boost/random/random_device.hpp>
#include "user.h"
#include "filesystem.h"
#include "SymposiumException.h"

using namespace Symposium;

user::user(const std::string &username, const std::string &pwd, const std::string &nickname,
           const std::string &iconPath,
           int siteId, std::shared_ptr<directory> home) : username(username), siteId(siteId),
                                                          nickname(nickname), iconPath(iconPath), home(home)
{
    if(pwd.length()<=5)
        throw userException(userException::shortPwd, UnpackFileLineFunction());

    if(pwd.length()>=22)
        throw userException(userException::longPwd, UnpackFileLineFunction());

    if(noCharPwd(pwd))
        throw userException(userException::noCharPwd, UnpackFileLineFunction());

    if(noNumPwd(pwd))
        throw userException(userException::noNumPwd, UnpackFileLineFunction());

    if(noSpecialCharPwd(pwd))
        throw userException(userException::noSpecialCharPwd, UnpackFileLineFunction());

    if(nickname=="")
        throw userException(userException::nickname, UnpackFileLineFunction());

    //generating random salt for user
    hashSalt=saltGenerate();
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
    pwdHash=sha256(pwd+hashSalt);
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

    return home->print(username, recursive);

}

std::shared_ptr<file> user::newFile(const std::string &fileName, const std::string &pathFromHome) const {

    std::shared_ptr<file> newF=home->addFile(pathFromHome, fileName);
    privilege priv=newF->setUserPrivilege(this->getUsername(), privilege::owner);
    return newF;
}

std::shared_ptr<directory>
user::newDirectory(const std::string &dirName, const std::string &pathFromHome, int idToAssign) const{
    std::shared_ptr<directory> newD;
    if(idToAssign==-1)
        newD=home->addDirectory(dirName);
    else
        newD=home->addDirectory(dirName, idToAssign);
    return newD;
}

//FIXME: accessFile should check what privilege the resource pointed by [resId] can give to other users, using
// the uri object of file pointed by [resId]
// following the call stack for opening a file already inserted with a symlink:
//      user::openFile -> called when a user will want to access the file pointed by the symlink
//      directory::access -> called on user's home directory, searches the file and call access on it
//      file::access -> called on the file retrieved
//      RMOAccess::getUserPrivilege -> called by file::access, verify that the user obtained a privilege
//      --> Ops, [file1] has never granted a privilege for the user, exception thrown
//      --> Ops, [file1] has never granted a privilege for the user, exception thrown
std::shared_ptr<file> user::accessFile(const std::string &resId, const std::string &path,  const std::string &fileName ) const {
    std::string pathCurrent;
    std::string idCurrent;
    std::string pathAdd;
    std::string idAdd;

    tie(pathCurrent, idCurrent)= separate(path); //separate the current path and the id of directory where user want to create a symlink

    std::shared_ptr<directory> dir=this->home->getDir(pathCurrent, idCurrent);

    std::shared_ptr<directory> root1=dir->getRoot();

    tie(pathAdd, idAdd)= separate(resId); //separate the path and the id of file which the user want

    std::shared_ptr<symlink> sym= home->addLink(idCurrent, fileName, pathAdd, idAdd);

    std::shared_ptr<file> fi=root1->getFile(pathAdd, idAdd);

    privilege priv=fi->getUserPrivilege(this->getUsername());
    if(priv==privilege::none)
        throw userException(userException::noPriv, UnpackFileLineFunction());
    return fi;
}

std::shared_ptr<file> user::openFile(const std::string &path, const std::string &fileName, privilege accessMode) const {
    auto f=home->getFile(path, fileName);
    //document& newD= home->access(*this, path, fileName, accessMode);
    return f;
}

privilege user::editPrivilege(const std::string &otherUser, const std::string &resPath, const std::string &resName,
                              privilege newPrivilege) const {
    std::shared_ptr<file> newF=home->getFile(resPath, resName);
    privilege newP;
    newP=newF->setUserPrivilege(otherUser, newPrivilege);
    return newP;
}

privilege user::changePrivilege(const std::string &resPath, const std::string &resName, privilege newPrivilege) {
    std::shared_ptr<file> newF=home->getFile(resPath, resName);
    privilege newP;
    newP=newF->setUserPrivilege(username, newPrivilege);
    return newP;
}

uri user::shareResource(const std::string &resPath, const std::string &resName, uri &newPrefs) const {
    std::shared_ptr<file> newF=home->getFile(resPath, resName);
    uri u;
    u=newF->setSharingPolicy(username, newPrefs);
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
    std::shared_ptr<filesystem> object=home->get(resPath, resName);
    object->setName(newName);
    return object;
}

std::shared_ptr<filesystem> user::removeResource(const std::string &path, const std::string &name) const {
    std::shared_ptr<filesystem> object=home->remove(*this, path, name);
    return object;
}



std::string user::saltGenerate()
{
    std::string sale;
    boost::random::random_device device;
    std::mt19937 generator(device());
    std::srand(time(NULL));
    std::uniform_int_distribution<> dis(20, 30);
    std::poisson_distribution<int> distribution(rand());

    for (size_t i = 0; i < dis(generator); i++)//salt have random lenght between 20 and 30
    {
        sale+=static_cast<char>(distribution(generator));//the sequence of random characters used as salt
    }
    return sale;
}

bool user::noCharPwd(const std::string &pass)
{
    std::size_t found = pass.find_first_not_of("1234567890?!$+-/.,@ˆ_ ");
    return found == std::string::npos;
}

bool user::noNumPwd(const std::string &pass)
{
    std::size_t found = pass.find_first_not_of("abcdefghijklmnopqrstuvwxyz?!$+-/.,@ˆ_ ");
    return found == std::string::npos;
}

bool user::noSpecialCharPwd(const std::string &pass)
{
    std::size_t found = pass.find_first_not_of("abcdefghijklmnopqrstuvwxyz1234567890 ");
    return found == std::string::npos;
}

std::tuple<std::string, std::string>  user::separate(const std::string &path)
{
    std::string path2;
    std::string id;
    std::size_t found = path.find_last_of("/\\");
    path2.append(path,0, found); //path to the directory of the current user
    id.append(path.begin()+found+1,path.end()); //the id of directory where the current user want to insert the file
    return  std::make_tuple(path2, id);
}
