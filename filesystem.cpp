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
std::shared_ptr<directory> directory::root;
uri filesystem::u;

filesystem::filesystem(const std::string &name) : name(name), sharingPolicy(u){
    id=idCounter;
    idCounter++;
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

std::tuple<std::string, std::string>  filesystem::separate(const std::string &path)
{
    std::string path2;
    std::string id2;
    std::size_t found = path.find_last_of("/\\");
    path2.append(path,0, found); //path to the directory of the current user
    id2.append(path.begin()+found+1,path.end()); //the id of directory where the current user want to insert the file
    return  std::make_tuple(path2, id2);
}

file::file(const std::string &name, const std::string &realPath) : filesystem(name), realPath(realPath), doc(0){
    if(!(pathIsValid(realPath)))
        throw filesystemException("Path is not valid!");
    std::unique_ptr<AccessStrategy> newStrategy(new RMOAccess());
    strategy=std::move(newStrategy);

}

resourceType file::resType() const {
    return resourceType::file;
}

privilege file::getUserPrivilege(const std::string &targetUser) const {

    privilege userPriv= strategy->getPrivilege(targetUser);
    return userPriv;
}

privilege file::setUserPrivilege(const std::string &targetUser, privilege newPrivilege) {
    strategy->setPrivilege(targetUser,newPrivilege);
    return newPrivilege;
}

uri file::setSharingPolicy(const std::string &actionUser, uri &newSharingPrefs) {
    privilege userPriv=strategy->getPrivilege(actionUser);
    if(userPriv==privilege::owner) {
        this->sharingPolicy=newSharingPrefs;
    }
    else
        throw filesystemException("You are not an owner to share this file");
    return newSharingPrefs;
}

document & file::access(const user &targetUser, privilege accessMode) {
    bool controllo=this->strategy->validateAction(targetUser.getUsername(), accessMode);
    if(!controllo)
        throw filesystemException("You have not permission to this file in this mode");
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
    std::string ritorno="";
    if (indent>0)
    {
        for(int i=0; i<indent; i++)
            ritorno.append(" ");
    }
    std::ostringstream priv;
    if(getUserPrivilege(targetUser)==privilege::none)
        return name+" You no longer have the possibility to access the file in any mode";
    priv<<getUserPrivilege(targetUser);
    return ritorno+"1 "+name + " " + priv.str();
}

bool file::pathIsValid(const std::string &toCheck) {
    //OPTIMIZE: correctness check of user data should be available within user class,
    // here we should call that method(s), adding only the constrain on the icon path
    std::regex pathPattern{R"(\.(\/[a-zA-Z0-9]+)+)"};
    return !toCheck.empty() && std::regex_match(toCheck, pathPattern);
}

const document &file::getDoc() const {
    return doc;
}

directory::directory(const std::string &name) : filesystem(name) {

    //TODO: implement
}

std::shared_ptr<directory> directory::nullDir() {
    return std::shared_ptr<directory>();
}


std::shared_ptr<directory> directory::getRoot() {

    if(root){
        return root;
    }
    std::string name="/";
    std::shared_ptr<directory> new_root(new directory(name));
    new_root->self=new_root;
    root=new_root;
    return root;


}
//FIXME: must search inside contained directories using path
std::shared_ptr<filesystem> directory::get(const std::string &path, const std::string &name) {
    //FIXME: use std::find_if to search in a container, see cppreference or code in SymServer
    for (unsigned i = 0; i < contained.size(); i++) {
        std::shared_ptr<filesystem> f = contained.at(i);
        std::string name_f = f->getName();
        if (name_f == name)
            return f;
        }
    throw filesystemException("FileSystem not found");
}

//FIXME: must search inside contained directories using path
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
    //FIXME: use std::find_if to search in a container, see cppreference or code in SymServer
    for(unsigned i=0;i<contained.size();i++){
       std::shared_ptr<filesystem> f= contained.at(i);
       std::string name_f= f->getName();
       if(name_f==name)
           return std::dynamic_pointer_cast<file>(f);
    }
    throw filesystemException("File are you searching for, is not present");

}

std::string& directory::setName(const std::string &path, const std::string &fileName, const std::string& newName) {
    //FIXME: use std::find_if to search in a container, see cppreference or code in SymServer
    // also this function must use path to retrieve the object whose name has to be changed
    // You can also use other directory's functions as helper (for example directory::get())
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
    int idBackup=idCounter;
    idCounter=idToAssign;
    if(std::any_of(contained.begin(), contained.end(), [name](const std::shared_ptr<filesystem> i){return i->getName()==name;}))
        throw filesystemException("You already have an element with the same name");
    std::shared_ptr<directory> newDir(new directory(name));//directory deve essere protetto
    idCounter=idBackup;
    newDir->parent=this->self;
    newDir->self=newDir;
    contained.push_back(newDir);
    return newDir;
}

//FIXME: must use path to insert in the correct location
std::shared_ptr<file> directory::addFile(const std::string &path, const std::string &name) {
    std::string pathAdd;
    std::string idAdd;
    tie(pathAdd, idAdd)= separate(path);
    std::shared_ptr<directory> save=getDir(pathAdd, idAdd);
    if(std::any_of(save->contained.begin(), save->contained.end(), [name](const std::shared_ptr<filesystem> i){return i->getName()==name;}))
        throw filesystemException("You already have an element with the same name");
    std::shared_ptr<file> newFile(new file(name, path));
    save->contained.push_back(newFile);
    return newFile;
}

//FIXME: must use path to insert in the correct location
std::shared_ptr<class symlink>
directory::addLink(const std::string &path, const std::string &name, const std::string &filePath,
                   const std::string &fileName)
{
    //std::shared_ptr<directory> daInserire=getDir()
    if(std::any_of(contained.begin(), contained.end(), [name](const std::shared_ptr<filesystem> i){return i->getName()==name;}))
        throw filesystemException("You already have an element with the same name");
    std::shared_ptr<symlink> newSym(new symlink(name, filePath, fileName));
    contained.push_back(newSym);
    return newSym;
}

resourceType directory::resType() const {
    return resourceType::directory;
}


document &
directory::access(const user &targetUser, const std::string &path, const std::string &resName, privilege accessMode) {
    //FIXME: don't declare a static variable inside a method, it's not reentrant
    static document doc;
    //FIXME: this line will always throw because you're calling getUserPrivilege against a directory
    // this should be called against the file we actually want to access
    privilege user_priv=this->getUserPrivilege(targetUser.getUsername());
    if(user_priv==privilege::none)
        throw filesystemException("You no longer have the possibility to access the file in any mode");
    if(user_priv>accessMode)
        throw filesystemException("You have a lower privilege than you ask");
    std::shared_ptr<file> newF= this->getFile(path,resName);
    //FIXME: why not:
    // return newF->access(targetUser, accessMode);
    doc= newF->access(targetUser,accessMode);
    return doc;
}
//FIXME: for directory::access why not calling access against the retrieved file? This method would be two lines long

//FIXME: pay attention when deleting a directory or a file: we should ensure that [targetUser] is the only owner of each file
std::shared_ptr<filesystem> directory::remove(const user &targetUser, const std::string &path, const std::string &resName) {
    privilege user_priv=this->getUserPrivilege(targetUser.getUsername());
    if(user_priv==privilege::owner){
        //FIXME: use std::find_if to search in a container, see cppreference or code in SymServer
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
//FIXME: changing specification 18/11/2019
// print must print the type of filesystem object and its name
std::string directory::print(const std::string &targetUser, bool recursive, int indent) const {
    //FIXME: recursive and indent not used. What if we want to print the complete tree of user's filesystem?
    /*
     * For example:
     * -/ (root directory)
     *   -someUser (user's directory)
     *     -dir1
     *       -file1
     *     -dir2
     *       -file2
     */
    std::string new_string= "root\r\n";
    //FIXME: use std::find_if to search in a container, see cppreference or code in SymServer
    for (unsigned i = 0; i < contained.size(); i++){
        std::shared_ptr<filesystem> f = contained.at(i);
        std::string name= f->getName();
        new_string=new_string+"-"+name+"\r\n";
    }
    return new_string;
}

Symposium::symlink::symlink(const std::string &name, const std::string &pathToFile, const std::string &fileName) : filesystem(name), pathToFile(pathToFile), fileName{fileName} {
    //TODO: implement
    //TODO: check pathToFile to mach a valid path format, IT'S IMPORTANT!
    // use a regex, you can look at SymServer.cpp, line 212

}

resourceType Symposium::symlink::resType() const {
    return resourceType::symlink;
}

document& Symposium::symlink::access(const user &targetUser, privilege accessMode) {
    //TODO: implement
    throw std::exception();
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
    std::shared_ptr<file> file=directory::getRoot()->getFile(pathToFile, fileName);
    std::ostringstream priv;
    std::string ritorno="";
    priv<<file->getUserPrivilege(targetUser);
    if (indent>0)
    {
        for(int i=0; i<indent; i++)
            ritorno+="  ";
    }
    return ritorno+"2 " +name + " " + priv.str();
}
