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
 * File:   filesystem.cpp
 * Project: Symposium
 * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Alice Morano <s259158 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 18 Giugno 2019, 22.02
 */
#include "filesystem.h"
#include "symbol.h"
#include "user.h"
using namespace Symposium;

int filesystem::idCounter=0;

filesystem::filesystem(const std::string &name) : name(name) {
    //TODO: implement
}

int filesystem::getId() const {
    return id;
}

const std::string &filesystem::getName() const {
    return name;
}

privilege filesystem::getUserPrivilege(const std::string &targetUser) const {
    throw filesystemException("Object"+name+"is not shareable");
}

const uri &filesystem::getSharingPolicy() const {
    return sharingPolicy;
}

privilege filesystem::setUserPrivilege(const std::string &targetUser, privilege newPrivilege) {
    throw filesystemException("Object"+name+"is not shareable");
}

std::string filesystem::setName(const std::string &newName) {
    filesystem::name=newName;
    return name;
}

uri filesystem::setSharingPolicy(const std::string &actionUser, uri &newSharingPrefs) {
    throw filesystemException("Object"+name+"is not shareable");
}

file::file(const std::string &name, const std::string &realPath) : filesystem(name), realPath(realPath), doc(0) {
    //TODO: implement
    //settare id
}

resourceType file::resType() const {
    return resourceType::file;
}

privilege file::getUserPrivilege(const std::string &targetUser) const {

    privilege userPriv= strategy->getPrivilege(targetUser);
    return userPriv;
}

privilege file::setUserPrivilege(const std::string &targetUser, privilege newPrivilege) {
    //privilege oldUserPriv= strategy->getPrivilege(targetUser);
    strategy->setPrivilege(targetUser,newPrivilege);
    return newPrivilege;
}

uri file::setSharingPolicy(const std::string &actionUser, uri &newSharingPrefs) {
    privilege userPriv=strategy->getPrivilege(actionUser);
    if(userPriv==privilege::owner) {
        file::sharingPolicy = newSharingPrefs;
    }
    return newSharingPrefs;
}

document & file::access(const user &targetUser, privilege accessMode) {
    privilege priv=this->getUserPrivilege(targetUser.getUsername());
    if(priv==privilege::none)
        throw filesystemException("You no longer have the possibility to access the file in any mode");
    if(priv>accessMode)
        throw filesystemException("You have a lower privilege than you ask");
    return doc.access(targetUser, accessMode);;
}

void file::store(const std::string &storePath) const {
    //TODO: implement
}

void file::load(const std::string &loadPath) {
    //TODO: implement
}

void file::send() const {
    //TODO: implement
}

std::string file::print(const std::string &targetUser, bool recursive, int indent) const {

    std::ostringstream priv;
    if(getUserPrivilege(targetUser)==privilege::none)
        return name+" You no longer have the possibility to access the file in any mode";
    priv<<getUserPrivilege(targetUser);
    return name + " " + priv.str();
}

const document &file::getDoc() const {
    return doc;
}

directory::directory(const std::string &name) : filesystem(name) {
    //guardare lab 2
    //settare id qui
    //TODO: implement
}

std::shared_ptr<directory> directory::nullDir() {
    return std::shared_ptr<directory>();
}

std::shared_ptr<directory> directory::getRoot() {
    return std::shared_ptr<directory>();
}

std::shared_ptr<filesystem> directory::get(const std::string &path, const std::string &name) {
    for (unsigned i = 0; i < contained.size(); i++) {
        std::shared_ptr<filesystem> f = contained.at(i);
        std::string name_f = f->getName();
        if (name_f == name)
            return f;
        }
    throw filesystemException("FileSystem not found");
}

std::shared_ptr<directory> directory::getDir(const std::string &path, const std::string &name) {
   if(name=="." && !self.expired())
       return self.lock();
   if(name==".." &&!parent.expired())
       return parent.lock();
    for(unsigned i=0;i<contained.size();i++){
        std::shared_ptr<filesystem> f= contained.at(i);
        std::string name_f= f->getName();
        if(name_f==name)
            return std::dynamic_pointer_cast<directory>(f);
    }
    throw filesystemException("Directory are you searching for, is not present");

}

std::shared_ptr<file> directory::getFile(const std::string &path, const std::string &name) {
    for(unsigned i=0;i<contained.size();i++){
       std::shared_ptr<filesystem> f= contained.at(i);
       std::string name_f= f->getName();
       if(name_f==name)
           return std::dynamic_pointer_cast<file>(f);
    }
    throw filesystemException("File are you searching for, is not present");

}

std::string& directory::setName(const std::string &path, const std::string &fileName, const std::string& newName) {
    for(unsigned i=0;i<contained.size();i++) {
        std::shared_ptr<filesystem> f = contained.at(i);
        std::string name_f = f->getName();
        if (name_f == name) {
            f->setName(newName);
            return name;
        }
    }
   throw filesystemException("File are you searching for, is not present");
   //this->addFile(path,newName);
    //return name;
}

std::shared_ptr<directory> directory::addDirectory(const std::string &name, int idToAssign) {

    for (unsigned i = 0; i < contained.size(); i++)
    {
    if (contained.at(i)->getName() == name)
        throw filesystemException("You already have an element with the same name");
    }
    std::shared_ptr<directory> newDir(new directory(name));//directory deve essere protetto
    newDir->parent=this->self;
    newDir->self=newDir;
    contained.push_back(newDir);
    return newDir;
}

std::shared_ptr<file> directory::addFile(const std::string &path, const std::string &name) {
    for (unsigned i = 0; i < contained.size(); i++)
    {
        if (contained.at(i)->getName() == name)
            throw filesystemException("You already have an element with the same name");
    }
    std::shared_ptr<file> newFile(new file(name, path));
    contained.push_back(newFile);
    idCounter++;
    return newFile;
}

std::shared_ptr<Symposium::symlink> directory::addLink(const std::string &path, const std::string &name)
{
    for (unsigned i = 0; i < contained.size(); i++)
    {
        if (contained.at(i)->getName() == name)
            throw filesystemException("You already have an element with the same name");
    }
    std::shared_ptr<symlink> newSym(new symlink(name, path, name));
    idCounter++;
    contained.push_back(newSym);
    return newSym;
}

resourceType directory::resType() const {
    return resourceType::directory;
}

document &
directory::access(const user &targetUser, const std::string &path, const std::string &resName, privilege accessMode) {
    static document doc;
    privilege user_priv=this->getUserPrivilege(targetUser.getUsername());
    if(user_priv==privilege::none)
        throw filesystemException("You no longer have the possibility to access the file in any mode");
    if(user_priv>accessMode)
        throw filesystemException("You have a lower privilege than you ask");
    std::shared_ptr<file> newF= this->getFile(path,resName);
    doc= newF->access(targetUser,accessMode);
    return doc;
}

std::shared_ptr<filesystem> directory::remove(const user &targetUser, const std::string &path, const std::string &resName) {
    privilege user_priv=this->getUserPrivilege(targetUser.getUsername());
    if(user_priv==privilege::owner){
        for (unsigned i = 0; i < contained.size(); i++)
        {
            if (contained.at(i)->getName() == resName) {
                std::shared_ptr<filesystem> f = contained.at(i);
                contained.erase(contained.begin()+i);
                return f;
            }

        }
        throw filesystemException("There is no file to remove");
    }
    throw filesystemException("You no longer have the possibility to remove the file in any mode");
}

void directory::store(const std::string &storePath) const {
    //TODO: implement
}

void directory::load(const std::string &loadPath) {
    //TODO: implement
}

void directory::send() const {
    //TODO: implement
}

std::string directory::print(const std::string &targetUser, bool recursive, int indent) const {

    std::ostringstream user_priv;
    if(getUserPrivilege(targetUser)==privilege::none)
        return name+" You no longer have the possibility to access the file in any mode";
    user_priv<<getUserPrivilege(targetUser);
    return name + " " + user_priv.str();
}

Symposium::symlink::symlink(const std::string &name, const std::string &pathToFile, const std::string &fileName) : filesystem(name), pathToFile(pathToFile), fileName{fileName} {
    //TODO: implement
    //settare id
}

resourceType Symposium::symlink::resType() const {
    return resourceType::symlink;
}

document Symposium::symlink::access() {
    //TODO: implement
    return document(0);
}

void Symposium::symlink::store(const std::string &storePath) const {
    //TODO: implement
}

void Symposium::symlink::load(const std::string &loadPath) {
    //TODO: implement
}

void Symposium::symlink::send() const {
    //TODO: implement
}

std::string Symposium::symlink::print(const std::string &targetUser, bool recursive, int indent) const {
    //TODO: implement
    return "";
}
