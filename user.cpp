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

#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

using namespace Symposium;

BOOST_CLASS_EXPORT(Symposium::user)

template<class Archive>
void user::serialize(Archive &ar, const unsigned int version){
    //TODO: save home
    ar & username & pwdHash & hashSalt & siteId & nickname & iconPath & home;
};

user::user(const std::string &username, const std::string &pwd, const std::string &nickname,
           const std::string &iconPath,
           uint_positive_cnt::type siteId, std::shared_ptr<directory> home) : username(username), siteId(siteId),
                                                                   nickname(nickname), iconPath(iconPath), home(home)
{
    if(pwd.length()<=5)
        throw userException(userException::shortPwd, UnpackFileLineFunction());

    else if(pwd.length()>=22)
        throw userException(userException::longPwd, UnpackFileLineFunction());

    else if(noCharPwd(pwd))
        throw userException(userException::noCharPwd, UnpackFileLineFunction());

    else if(noNumPwd(pwd))
        throw userException(userException::noNumPwd, UnpackFileLineFunction());

    else if(noSpecialCharPwd(pwd))
        throw userException(userException::noSpecialCharPwd, UnpackFileLineFunction());

    else if(nickname.empty())
        throw userException(userException::nickname, UnpackFileLineFunction());

    //generating random salt for user
    hashSalt=saltGenerate();
    //saving the password with the use of hash algorithm sha256
    pwdHash=sha256(pwd+hashSalt);
}

const std::string &user::getUsername() const {
    return username;
}

uint_positive_cnt::type user::getSiteId() const {
    return siteId;
}

const std::string &user::getNickname() const {
    return nickname;
}

const std::string &user::getIconPath() const {
    return iconPath;
}

void user::setPwd(const std::string &pwd) {
    hashSalt=saltGenerate();
    pwdHash=sha256(pwd+hashSalt);
}

void user::setNickname(const std::string &nickname) {
    user::nickname = nickname;
}

void user::setSiteId(uint_positive_cnt::type siteId) {
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

std::shared_ptr<file> user::newFile(const std::string &fileName, const std::string &pathFromHome, uint_positive_cnt::type idToAssign) const {

    std::shared_ptr<file> newF= home->addFile(pathFromHome, fileName);
    newF->setUserPrivilege(this->getUsername(), privilege::owner);
    return newF;
}


std::shared_ptr<directory>
user::newDirectory(const std::string &dirName, const std::string &pathFromHome, uint_positive_cnt::type idToAssign) const{
    std::shared_ptr<directory> newDir;
    std::string pathToDir;
    std::string idDir;
    std::shared_ptr<directory> dir=home;
    if(!(pathFromHome.empty()) && pathFromHome!="./" && pathFromHome!=".")
    {
        tie(pathToDir, idDir)= filesystem::separate(pathFromHome);
        dir=home->getDir(pathToDir, idDir);
    }
    newDir=dir->addDirectory(dirName, idToAssign);
    return newDir;
}



std::pair<int, std::shared_ptr<file>> user::accessFile(const std::string &resId, const std::string &path,  const std::string &fileName ) const {    std::string pathAdd;
    std::string idAdd;

    std::shared_ptr<directory> root1=this->home->getRoot();

    tie(pathAdd, idAdd)= filesystem::separate(resId); //separate the path and the id of file which the user want

    std::shared_ptr<file> fi=root1->getFile(pathAdd, idAdd);

    privilege priv=fi->getUserPrivilege(this->getUsername());
    if(priv==privilege::none)
        throw userException(userException::noPriv, UnpackFileLineFunction());

    privilege priv2=fi->getSharingPolicy().getShare(priv);
    if(priv2==privilege::none)
        throw userException(userException::noPriv, UnpackFileLineFunction());

    std::shared_ptr<symlink> sym= home->addLink(path, fileName, pathAdd, idAdd);
    return std::make_pair(sym->getId(), fi);
}

//FIXME: al chiamante serve avere indietro il file e il documento aperto, potremmo tornare un pair
std::shared_ptr<file> user::openFile(const std::string &path, const std::string &fileName, privilege accessMode) const {
    auto f=home->getFile(path, fileName);
    //FIXME: perchè abbiamo commentato questa linea?
    // In origine volevo solo avere indietro il file al posto del documento
    //document& newD= home->access(*this, path, fileName, accessMode);
    return f;
}

privilege user::editPrivilege(const std::string &otherUser, const std::string &resPath, const std::string &resName,
                              privilege newPrivilege) const {
    std::shared_ptr<file> newF=home->getFile(resPath, resName);
    privilege newP;
    if((this->username!=otherUser && newF->getUserPrivilege(this->username)==privilege::owner) ||
        (this->username==otherUser && newF->validateAction(this->username, newPrivilege)))
        newP=newF->setUserPrivilege(otherUser, newPrivilege);
    else
        throw userException(userException::noPermissionToChange, UnpackFileLineFunction());
    return newP;
}

/*privilege user::changePrivilege(const std::string &resPath, const std::string &resName, privilege newPrivilege) const {
    std::shared_ptr<file> newF=home->getFile(resPath, resName);
    privilege newP;
    newP=newF->setUserPrivilege(username, newPrivilege);
    return newP;
}*/

std::shared_ptr<filesystem> user::shareResource(const std::string &resPath, const std::string &resName, const uri &newPrefs) const {
    std::shared_ptr<file> newF=home->getFile(resPath, resName);
    uri u;
    u=newF->setSharingPolicy(username, newPrefs);
    return newF;
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
    if(!newData.iconPath.empty())
        iconPath=newData.iconPath;
    if(!newData.pwdHash.empty()) {
        pwdHash = newData.pwdHash;
        hashSalt=newData.hashSalt;
    }
    if(!newData.nickname.empty())
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
    std::size_t found = pass.find_first_not_of(noChar);
    return found == std::string::npos;
}

bool user::noNumPwd(const std::string &pass)
{
    std::size_t found = pass.find_first_not_of(noNum);
    return found == std::string::npos;
}

bool user::noSpecialCharPwd(const std::string &pass)
{
    std::size_t found = pass.find_first_not_of(noSpecialChar);
    return found == std::string::npos;
}

user user::makeCopyNoPwd() const {
    user copy(*this);
    copy.pwdHash.clear();
    copy.hashSalt.clear();
    return copy;
}