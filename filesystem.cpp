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

#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/weak_ptr.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <fstream>

#include "filesystem.h"
#include "symbol.h"
#include "user.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Symposium::file)
BOOST_CLASS_EXPORT_IMPLEMENT(Symposium::symlink)
BOOST_CLASS_EXPORT_IMPLEMENT(Symposium::directory)

using namespace Symposium;

uint_positive_cnt filesystem::idCounter;
std::shared_ptr<directory> directory::root;


template<class Archive>
void filesystem::serialize(Archive &ar, const unsigned int){
    ar & id & name & sharingPolicy & strategy;
};

template<class Archive>
void file::serialize(Archive &ar, const unsigned int){
    ar & boost::serialization::base_object<Symposium::filesystem>(*this);
    ar & doc;
};

template<class Archive>
void symlink::serialize(Archive &ar, const unsigned int){
    ar & boost::serialization::base_object<Symposium::filesystem>(*this);
    ar & absPathWithoutId & resId;
};


template<class Archive>
void directory::serialize(Archive &ar, const unsigned int){
    ar & boost::serialization::base_object<Symposium::filesystem>(*this);
    ar & root & contained & parent & self;
}


filesystem::filesystem(const std::string &name, const uint_positive_cnt::type idToAssign) : name(name){
    if(idToAssign==0){
        id=idCounter;
        idCounter++;
    }
    else
        id=idToAssign;
}

uint_positive_cnt::type filesystem::getId() const {
    return id;
}

const std::string &filesystem::getName() const {
    return name;
}

privilege filesystem::getUserPrivilege(const std::string &) const {
    throw filesystemException(filesystemException::objSha, UnpackFileLineFunction());
}

uri &filesystem::getSharingPolicy() {
    return sharingPolicy;
}

privilege filesystem::setUserPrivilege(const std::string &, privilege) {
    throw filesystemException(filesystemException::objSha, UnpackFileLineFunction());
}

uri filesystem::setSharingPolicy(const std::string &, const uri &) {
    throw filesystemException(filesystemException::objSha, UnpackFileLineFunction());
}


std::tuple<std::string, std::string>  filesystem::separate(const std::string &path)
{
    if(path.empty())
        throw filesystemException(filesystemException::pathEmpty, UnpackFileLineFunction());
    std::string path2;
    std::string path3;
    path3.append(path);
    std::string id2;
    if(path3.back()=='/') //if path terminate with "/", we need to remove it for separate correctly the id
        path3.pop_back();
    if(path3.at(0)=='.'||path3.at(0)=='/')
    {
        path3.erase(path3.begin()+0);
        return separate(path3);
    }
    std::size_t found = path3.find_last_of("/\\");//find the last number which represent the id
    if(found==std::string::npos)//if there isn't any "/" it means that I', alredy in the correct directory and the path represent only id
    {
        id2.append(path3);
        path3.clear();
        return std::make_tuple(path3, id2); //return clear path and the id
    }
    path2.append(path3,0, found); //path to the directory of the current user
    auto it=path3.begin();
    std::advance(it, found+1);
    id2.append(it,path3.end()); //the id of directory where the current user want to insert the file

    return  std::make_tuple(path2, id2);
}

bool filesystem::pathIsValid2(const std::string &toCheck) {
    std::regex pathPattern{R"(\.(\/[a-zA-Z0-9]+)+)"};
    return !toCheck.empty() && std::regex_match(toCheck, pathPattern);
}

bool filesystem::moreOwner(const std::string &username)
{
    return strategy->moreOwner(username);
}

bool filesystem::operator==(const filesystem &rhs) const {
    return id == rhs.id;
}

bool filesystem::operator!=(const filesystem &rhs) const {
    return !(rhs == *this);
}

void filesystem::setName(const std::string &name) {
    filesystem::name = name;
}


file::file(const std::string &name, uint_positive_cnt::type idToAssign) : filesystem(name, idToAssign), doc(){
    strategy=std::make_unique<RMOAccess>();
}

resourceType file::resType() const {
    return resourceType::file;
}

const std::unordered_map<std::string, privilege> file::getUsers()
{
    return strategy->getPermission();
}


privilege file::getUserPrivilege(const std::string &targetUser) const {

    privilege userPriv= strategy->getPrivilege(targetUser);
    return userPriv;
}


privilege file::setUserPrivilege(const std::string &targetUser, privilege newPrivilege) {
    privilege oldPrivilege=getUserPrivilege(targetUser);
    if(oldPrivilege==privilege::owner)
        throw filesystemException(filesystemException::changePriv, UnpackFileLineFunction());
    strategy->setPrivilege(targetUser,newPrivilege);
    return newPrivilege;
}

uri file::setSharingPolicy(const std::string &actionUser, const uri &newSharingPrefs) {

    if(strategy->validateAction(actionUser, privilege::owner)) {
        this->sharingPolicy=newSharingPrefs;
    }
    else
        throw filesystemException(filesystemException::notOwn, UnpackFileLineFunction());
    return newSharingPrefs;
}

document & file::access(const user &targetUser, privilege accessMode) {
    bool controllo=this->strategy->validateAction(targetUser.getUsername(), accessMode);
    if(!controllo)
        throw filesystemException(filesystemException::noPermission, UnpackFileLineFunction());
    return doc.access(targetUser, accessMode);
}

void file::storeContent() const {
    doc.store();
}


void file::deleteFromStrategy(const std::string &userName)
{
   strategy->setPrivilege(userName, privilege::none);
}

std::string file::print(const std::string &targetUser, bool, unsigned int indent) const {
    std::string ritorno;
    if (indent>0)
        ritorno.insert(0, indent, ' '); //first need to insert indent
    std::ostringstream typeres;
    typeres<<resType(); //then the typer of the resource
    std::ostringstream priv;
    if(getUserPrivilege(targetUser)==privilege::none)
        return name+" You no longer have the possibility to access the file in any mode";
    priv<<getUserPrivilege(targetUser); //the privilege
    return typeres.str()+" "+std::to_string(getId())+" "+ritorno+name + " " + priv.str();
}



const document &file::getDoc() const {
    return doc;
}

bool file::validateAction(const std::string &userName, privilege priv) {
    return strategy->validateAction(userName, priv);
}


void file::replacement(const std::shared_ptr<file> replace){
    this->doc=replace->doc;
    this->sharingPolicy=replace->sharingPolicy;
    this->strategy=std::move(replace->strategy);
}

bool file::operator==(const file &rhs) const {
    return static_cast<const Symposium::filesystem &>(*this) == static_cast<const Symposium::filesystem &>(rhs) &&
           doc == rhs.doc;
}

bool file::operator!=(const file &rhs) const {
    return !(rhs == *this);
}

directory::directory(const std::string &name, const uint_positive_cnt::type &idToAssign) : filesystem(name, idToAssign) {

    strategy=std::make_unique<TrivialAccess>();
}

std::shared_ptr<directory> directory::emptyDir() {
    //this way I should not interfere with ids generation
    return std::shared_ptr<directory>(new directory("EmptyDir", -1));
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


std::tuple<std::string, std::string> directory::separateFirst(std::string path)
{
    if(path.empty())
        throw filesystemException(filesystemException::pathEmpty, UnpackFileLineFunction());
    std::string path2;
    std::string id;
    if(path.at(0) == '.' || path.at(0) == '/')
    {
        path.erase(path.begin());
        return separateFirst(path);
    }
    std::size_t found = path.find_first_of("/\\"); //find first character "/" in order to separate the first directory of the path
    if(found==std::string::npos)//if there is none of "/" it means that the path is composed by only one directory
    {
        id.append(path);
        path.clear();
        return std::make_tuple(path, id);// return clear path and the id
    }
    path2.append(path,found, path.size());
    id.append(path, 0, found);
    return  std::make_tuple(path2, id);
}



std::shared_ptr<filesystem> directory::get(const std::string &resPath, const std::string &resId) {
    if(resPath == "" || resPath == "./")//if path is "here" so I only need to get the object with the id represented by name
    {
        auto it=std::find_if(contained.begin(), contained.end(),
                             [resId](std::shared_ptr<filesystem> i){return std::to_string(i->getId()) == resId;});
        if(it==contained.end())
            throw filesystemException(filesystemException::noGet, UnpackFileLineFunction());
        return *it;
    }
    std::string idRes;
    std::string pathRes;
    if(resPath.back() == '/')
        pathRes.append(resPath.begin(), resPath.end() - 1);
    else
        pathRes.append(resPath);
    tie(pathRes, idRes)= separateFirst(pathRes);//otherwise need to separate the first directory and move into it

    auto it=std::find_if(contained.begin(), contained.end(),
                         [idRes](std::shared_ptr<filesystem> i){return std::to_string(i->getId())==idRes;});

    if(it==contained.end())
        throw filesystemException(filesystemException::noGet, UnpackFileLineFunction());
    if((*it)->resType()!=resourceType::directory)//if the element is not a directory, there is some error in path
        throw filesystemException(filesystemException::noGet, UnpackFileLineFunction());
    std::shared_ptr <directory> dir=std::dynamic_pointer_cast<directory>(*it);
    return dir->get(pathRes, resId);
}


std::shared_ptr<directory> directory::getDir(const std::string &resPath, const std::string &resId) {
    std::shared_ptr<filesystem> res=this->get(resPath, resId);
    if(res->resType()!=resourceType::directory)
        throw filesystemException(filesystemException::noGetDir, UnpackFileLineFunction());
    return std::dynamic_pointer_cast<directory>(res);

}

std::shared_ptr<file> directory::getFile(const std::string &resPath, const std::string &resId) {
    std::shared_ptr<filesystem> res=this->get(resPath, resId);
    if(res->resType()==resourceType::file)
        return std::dynamic_pointer_cast<file>(res);
    if(res->resType()==resourceType::symlink) //is used also for symlink because symlink is the reference to the file object
    {
        std::shared_ptr<symlink> sym=std::dynamic_pointer_cast<symlink>(res);
        std::string pathSym=sym->getPath();
        std::string pathRes;
        std::string idRes;
        tie(pathRes, idRes)= separate(pathSym);
        return this->root->getFile(pathRes, idRes);
    }
    throw filesystemException(filesystemException::noGetFile, UnpackFileLineFunction());
}

std::string directory::setName(const std::string &resPath, const std::string &resId, const std::string& newName) {
    std::string pathRename;
    std::string idRename;
    std::shared_ptr<directory> save;
    if(resPath!="./"){
        tie(pathRename, idRename)= separate(resPath);
        save=getDir(pathRename, idRename);
    }
    else
        save=this->self.lock();
    if(std::any_of(save->contained.begin(), save->contained.end(), [newName](const std::shared_ptr<filesystem> i){return i->getName()==newName;}))
        throw filesystemException(filesystemException::sameName, UnpackFileLineFunction());
    std::shared_ptr<filesystem> res=this->get(resPath, resId);
    std::string old=res->getName();
    res->setName(newName);
    return old;
}



std::shared_ptr<directory> directory::addDirectory(const std::string &resName, uint_positive_cnt::type idToAssign) {
    if(std::any_of(contained.begin(), contained.end(), [resName](const std::shared_ptr<filesystem> &i){return i->getName() == resName;}))
        throw filesystemException(filesystemException::sameName, UnpackFileLineFunction());
    std::shared_ptr<directory> newDir(new directory(resName, idToAssign));
    newDir->parent=this->self;
    newDir->self=newDir;
    contained.push_back(newDir);
    return newDir;
}


std::shared_ptr<file> directory::addFile(const std::string &resPath, const std::string &resName, uint_positive_cnt::type idToAssign) {
    std::string pathAdd;
    std::string idAdd;
    std::shared_ptr<directory> save;
    if(resPath != "./")
        {
        tie(pathAdd, idAdd)= separate(resPath);
        save=getDir(pathAdd, idAdd);
        }
    else
        save=self.lock();
    if(std::any_of(save->contained.begin(), save->contained.end(), [resName](const std::shared_ptr<filesystem> i){return i->getName() == resName;}))
        throw filesystemException(filesystemException::sameName, UnpackFileLineFunction());
    std::shared_ptr<file> newFile(new file(resName, idToAssign));
    save->contained.push_back(newFile);
    return newFile;
}

std::shared_ptr<class symlink>
directory::addLink(const std::string &symPath, const std::string &symName, const std::string &absPathWithoutId,
                   const std::string &resId, uint_positive_cnt::type idToAssign)
{
    std::string pathAdd;
    std::string idAdd;
    std::shared_ptr<directory> save;
    if(symPath != "./")
    {
        tie(pathAdd, idAdd)= separate(symPath);
        save=getDir(pathAdd, idAdd);
    }
    else
        save=self.lock();
    if(std::any_of(save->contained.begin(), save->contained.end(), [symName](const std::shared_ptr<filesystem> i){return i->getName() == symName;}))
        throw filesystemException(filesystemException::sameName, UnpackFileLineFunction());
    std::shared_ptr<symlink> newSym(new symlink(symName, absPathWithoutId, resId, idToAssign));
    save->contained.push_back(newSym);
    return newSym;
}

resourceType directory::resType() const {
    return resourceType::directory;
}


document &
directory::access(const user &targetUser, const std::string &resPath, const std::string &resId, privilege accessMode) {
   std::shared_ptr<file> f=getFile(resPath, resId);
    return f->access(targetUser, accessMode);
}



std::shared_ptr<filesystem> directory::remove(const user &targetUser, const std::string &resPath, const std::string &resId) {
    std::shared_ptr<filesystem> obj=this->get(resPath, resId); //take the object to remove
    std::string idRem;
    if(resPath != "./" && !resPath.empty())
    {
        std::string pathRem;
        tie(pathRem, idRem)= separate(resPath);
        std::shared_ptr<directory> dir=this->getDir(pathRem, idRem);
        return dir->remove(targetUser, "", resId); //call remove recursivly if the path is not "here" in order to take the right directory of the object to remove
    }
    idRem=resId;//otherwise the path is "here"
    if(obj->resType()==resourceType::file)//if the object to remove is the file
    {
        std::shared_ptr<file> f=std::dynamic_pointer_cast<file>(obj);
        if(f->moreOwner(targetUser.getUsername()))//check if targetUser is the only user, only in this case the file can be deleted
            throw filesystemException(filesystemException::notOnlyOwn, UnpackFileLineFunction());
        auto it=std::find_if(contained.begin(), contained.end(),
                             [idRem, f](const std::shared_ptr<filesystem>& i){return i->getId()==f->getId();});
        contained.erase(it);//cancel from the container the object
        return obj;
    }

    else if(obj->resType()==resourceType::symlink)//if the object is symlink
    {
        std::shared_ptr<symlink> s=std::dynamic_pointer_cast<symlink>(obj);

        std::string pathFile;
        std::string idFile;
        tie(pathFile, idFile)= separate(s->getPath());//need to have a pointer for the file and not a symlink in order to operate on file strategy
        std::shared_ptr<file> f=getRoot()->getFile(pathFile, idFile);
        f->deleteFromStrategy(targetUser.getUsername());
        auto it=std::find_if(contained.begin(), contained.end(),
                             [idRem, s](std::shared_ptr<filesystem> i){return i->getId()==s->getId();});

        contained.erase(it);//delete the object symlink in the right directory of the user
        return obj;
    }
    std::shared_ptr<directory> d=std::dynamic_pointer_cast<directory>(obj);//otherwise the object is a directory
    for(auto iterator: d->contained)
    {
        std::shared_ptr<filesystem> rem=d->remove(targetUser, "", std::to_string(iterator->getId()));//call remove recursively in order to remove any object in the directory
    }
    auto it=std::find_if(contained.begin(), contained.end(),
                         [idRem, d](std::shared_ptr<filesystem> i){return i->getId()==d->getId();});

    contained.erase(it);//in the end remove the directory
    return obj;
}


void directory::storeContent() const {
    for(const auto& el:contained)
        el->storeContent();
}

std::string directory::print(const std::string &targetUser, bool recursive, unsigned int indent) const {
    std::string result;
    if(indent==0)
        {result.append(targetUser); result+=" ";}
    //otherwise need to invoke recursively the method prints for all subdirectory
        for(const auto & it : contained)
        {
            resourceType type=it->resType();
            if(type==resourceType::directory)
            {
                std::shared_ptr<directory> dir=std::dynamic_pointer_cast<directory>(it);
                std::ostringstream typeres;
                typeres<<dir->resType();
                std::string spaces = "";
                if(result.size()>=indent)
                    spaces.insert(result.end(), indent, ' ');
                else
                    spaces.push_back(' ');
                result=result+typeres.str()+" "+std::to_string(it->getId())+" "+spaces+dir->name+"\n";
                if(recursive)
                    result+=dir->print(targetUser, recursive, indent+1);
            }
            else
                result+=" "+it->print(targetUser, recursive, indent)+"\n";
        }

    return result;
}


Symposium::symlink::symlink(const std::string &symName, const std::string &absPathWithoutId, const std::string &resId,
                            uint_positive_cnt::type idToAssign) : filesystem(symName, idToAssign), absPathWithoutId(absPathWithoutId), resId(resId) {

    if(!pathIsValid2(absPathWithoutId))
        throw filesystemException(filesystemException::pathNvalid, UnpackFileLineFunction());
    strategy=std::make_unique<TrivialAccess>();

}

resourceType Symposium::symlink::resType() const {
    return resourceType::symlink;
}

document& Symposium::symlink::access(const user &targetUser, privilege accessMode) {
    std::shared_ptr<file> f=directory::getRoot()->getFile(absPathWithoutId, resId);
    return f->access(targetUser, accessMode);
}

std::string Symposium::symlink::getPath() {
    return absPathWithoutId + "/" + resId;
}

std::string Symposium::symlink::print(const std::string &targetUser, bool, unsigned int indent) const {
    std::shared_ptr<file> file=directory::getRoot()->getFile(absPathWithoutId, resId);
    std::ostringstream priv;
    std::ostringstream typeres;
    typeres<<resType();
    std::string ritorno;
    priv<<file->getUserPrivilege(targetUser);
    if(file->getUserPrivilege(targetUser)==privilege::none)
        return name+" You no longer have the possibility to access the file in any mode";
    if (indent>0)
    {
        ritorno.insert(0, indent, ' ');
    }
    return typeres.str()+" "+std::to_string(getId())+" "+ritorno+name + " " + priv.str();
}

