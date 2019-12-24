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
    //FIXME: dovrebbero essere tutti degli else if, evitiamo le costanti scritte nel codice, preferisci
    // delle espressioni costanti definite nel .h, vedi ad esempio document.h linea 51
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

std::shared_ptr<file> user::newFile(const std::string &fileName, const std::string &pathFromHome, int idToAssign) const {

    std::shared_ptr<file> newF= home->addFile(pathFromHome, fileName);
    newF->setUserPrivilege(this->getUsername(), privilege::owner);
    return newF;
}


std::shared_ptr<directory>
user::newDirectory(const std::string &dirName, const std::string &pathFromHome, int idToAssign) const{
    std::shared_ptr<directory> newDir;
    std::string pathToDir;
    std::string idDir;
    std::shared_ptr<directory> dir=home;
    if(!(pathFromHome.empty()) && pathFromHome!="./" && pathFromHome!=".")
    {
        //FIXME: esiste una funzione con lo stesso nome in filesystem e sembra faccia la stessa cosa.
        // Rendila pubblica e statica se non lo è ed usa quella
        tie(pathToDir, idDir)= separate(pathFromHome);
        dir=home->getDir(pathToDir, idDir);
    }
    //FIXME: dovremmo poter eliminare questo if, bisogna decidere in un solo punto quale deve un id invalido
    // Potrebbe essere lo 0, cioè dire che è riservato. Filesystem usa questa convenzione
    if(idToAssign==-1)
        newDir=dir->addDirectory(dirName);
    else
        newDir=dir->addDirectory(dirName, idToAssign);
    return newDir;
}



std::pair<int, std::shared_ptr<file>> user::accessFile(const std::string &resId, const std::string &path,  const std::string &fileName ) const {    std::string pathAdd;
    std::string idAdd;

    std::shared_ptr<directory> root1=this->home->getRoot();

    tie(pathAdd, idAdd)= separate(resId); //separate the path and the id of file which the user want

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
    newP=newF->setUserPrivilege(otherUser, newPrivilege);
    return newP;
}

//FIXME: queste due metodi sono uguali ma non dovrebbero esserlo. Sospetto che abbiamo inserito la distinzione
// al livello sbagliato. Idea per risolvere: eliminiamo questa funzione, usiamo sempre editPrivilege e cambiamo
// setUserPrivilege in modo che se actionUser==targetUser cambia sempre se newPrivige<oldPrivilege
privilege user::changePrivilege(const std::string &resPath, const std::string &resName, privilege newPrivilege) const {
    std::shared_ptr<file> newF=home->getFile(resPath, resName);
    privilege newP;
    newP=newF->setUserPrivilege(username, newPrivilege);
    return newP;
}

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

//FIXME: esiste una funzione analoga in filesystem, dalla documentazione sembra faccia la stessa cosa
// Le duplicazioni di codice sono da evitare: se puoi usare quella rendila pubblica se non lo è e richiamala dove serve
// Se sono leggermente diverse cerca di generalizzare e avere una sola funzione che si occupa della separazione.
// Se ti accorgi che ci sono più moduli diversi del programma dove serve, valuta la possibilità di scrivere una classe
// a parte con soli metodi statici che raccoglie le funzioni per manipolare le stringhe che contengono percorsi.
// Poi fammi sapere cosa scegli perchè è opportuno testare ognuna di queste funzioni (in ogni caso, sia che scelga di
// farla in una classe o meno)
std::tuple<std::string, std::string>  user::separate(const std::string &path)
{
    std::string id;
    std::string path2;
    std::size_t found = path.find_last_of("/\\");
    if(found==0)
        path2="";
    else
        path2.append(path,0, found); //path to the directory of the current user
    id.append(path.begin()+found+1,path.end()); //the id of directory where the current user want to insert the file
    return  std::make_tuple(path2, id);
}

user user::makeCopyNoPwd() const {
    user copy(*this);
    copy.pwdHash.clear();
    copy.hashSalt.clear();
    return copy;
}