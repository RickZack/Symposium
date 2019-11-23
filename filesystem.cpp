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

filesystem::filesystem(const std::string &name) : name(name){
    id=idCounter;
    idCounter++;
}

filesystem::filesystem(const std::string &name, const int &idToAssign) : name(name), id(idToAssign){
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

//FIXME: again, I must quickly understand what this method does. It needs to be much shorter
// max 7-10 lines. Make sure that doesn't throw, the call at() can throw ad we will never catch
// that type of exception -> the program crashes
std::tuple<std::string, std::string>  filesystem::separate(const std::string &path)
{
    std::string path2;
    std::string path3;
    path3.append(path);
    std::string id2;
    if(path3.back()=='/')
        path3.pop_back();
    if(path3.at(0)=='.'||path3.at(0)=='/')
    {
        path3.erase(path3.begin()+0);
        return separate(path3);
    }
    std::size_t found = path3.find_last_of("/\\");
    if(found==std::string::npos)
    {
        id2.append(path3);
        path3.clear();
        return std::make_tuple(path3, id2);
    }
    path2.append(path3,0, found); //path to the directory of the current user
    id2.append(path3.begin()+found+1,path3.end()); //the id of directory where the current user want to insert the file
    return  std::make_tuple(path2, id2);
}

//FIXME: choose a better name, here I don't understand what is checked,
// realPath is an attribute of file, so it's better to write this method to file class
//  - make it static;
// Anyway, it should be private to keep the interface clean
bool filesystem::pathIsValid2(const std::string &toCheck) {
    //OPTIMIZE: correctness check of user data should be available within user class,
    // here we should call that method(s), adding only the constrain on the icon path
    std::regex pathPattern{R"(\.(\/[a-zA-Z0-9]+)+)"};
    return !toCheck.empty() && std::regex_match(toCheck, pathPattern);
}

file::file(const std::string &name, const std::string &realPath) : filesystem(name), realPath(realPath), doc(0){
    if(!(pathIsValid2(realPath)))
        throw filesystemException("Path is not valid!");
    strategy=std::make_unique<RMOAccess>();
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
    return doc.access(targetUser, accessMode);
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

bool file::moreOwner(std::string username)
{
   return strategy->moreOwner(username);
}

bool file::deleteFromStrategy(const std::string &userName)
{
   return strategy->deleteUser(userName);
}

//FIXME: try to clean a little, too many lines for what it does
std::string file::print(const std::string &targetUser, bool recursive, int indent) const {
    std::string ritorno;
    //FIXME: try always to use std::string methods before coding a solution by hand
    // for example in tests I wrote something like this loop with:
    // ritorno.insert(0, indent, ' ');
    // INDENT DOESN'T NEED TO BE >0, THE CALL IS CORRECT ANYWAY
    if (indent>0)
    {
        for(int i=0; i<indent; i++)
            ritorno.append(" ");
    }
    //FIXME: you can put a string in a ostringstream with <<, so you can try to put all the contents
    // in a ostringstream and only at the end call obj.str()
    std::ostringstream typeres;
    typeres<<resType();
    std::ostringstream priv;
    if(getUserPrivilege(targetUser)==privilege::none)
        return name+" You no longer have the possibility to access the file in any mode";
    priv<<getUserPrivilege(targetUser);
    return ritorno+typeres.str()+" "+name + " " + priv.str();
}



const document &file::getDoc() const {
    return doc;
}

directory::directory(const std::string &name) : filesystem(name) {
    strategy=std::make_unique<TrivialAccess>();
}

directory::directory(const std::string &name, const int &idToAssign) : filesystem(name, idToAssign) {

    strategy=std::make_unique<TrivialAccess>();
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

//FIXME: difficult to understand and long. Advice: more explanatory names, avoid strange operations
// as possible and, if something remains obscure, write some COMMENTS
// More important: be sure that this does not throw any exception that is not SymposiumException
// (it would be difficult to catch the specific type at the handler)
// for example at() can throw, but we will not catch this type of exception and the program crashes
std::tuple<std::string, std::string> directory::separateFirst(std::string path)
{
    std::string path2;
    std::string id;
    if(path.at(0) == '.' || path.at(0) == '/')
    {
        path.erase(path.begin()+0);
        return separateFirst(path);
    }
    std::size_t found = path.find_first_of("/\\");
    if(found==std::string::npos)
    {
        id.append(path);
        path.clear();
        return std::make_tuple(path, id);
    }
    path2.append(path,found, path.size());
    id.append(path, 0, found);
    return  std::make_tuple(path2, id);
}


//FIXME: way too long and difficult to understand. Also WBT testing would be very difficult.
// Advices: isolate the condition on "here" path, put inside a private helper function
// lines that are common in this method (es. 229-233, 242-248).
// make a clear distinction between the "get something that is here" and "I need to call again recursively"
std::shared_ptr<filesystem> directory::get(const std::string &path, const std::string &name) {
    if(path==""||path=="./")
    {
        auto it=std::find_if(contained.begin(), contained.end(),
                [name](std::shared_ptr<filesystem> i){return std::to_string(i->getId())==name;});
        if(it==contained.end())
            throw filesystemException("The element has not been found with get");
        return *it;
    }
    std::string idRes;
    std::string pathRes;
    if(path.back()=='/')
        pathRes.append(path.begin(),path.end()-1);
    else
        pathRes.append(path);
    tie(pathRes, idRes)= separateFirst(pathRes);

    auto it=std::find_if(contained.begin(), contained.end(),
                         [idRes](std::shared_ptr<filesystem> i){return std::to_string(i->getId())==idRes;});

    if(it==contained.end())
        throw filesystemException("The element has not been found with get");
    if((*it)->resType()!=resourceType::directory)
        throw filesystemException("The element has not been found with get");
    std::shared_ptr <directory> dir=std::dynamic_pointer_cast<directory>(*it);
    return dir->get(pathRes, name);
}


std::shared_ptr<directory> directory::getDir(const std::string &path, const std::string &name) {
    std::shared_ptr<filesystem> res=this->get(path, name);
    if(res->resType()!=resourceType::directory)
        filesystemException("File are you searching for, is not present");
    return std::dynamic_pointer_cast<directory>(res);

}

std::shared_ptr<file> directory::getFile(const std::string &path, const std::string &name) {
    std::shared_ptr<filesystem> res=this->get(path, name);
    if(res->resType()==resourceType::file)
        return std::dynamic_pointer_cast<file>(res);
    if(res->resType()==resourceType::symlink)
    {
        std::shared_ptr<symlink> sym=std::dynamic_pointer_cast<symlink>(res);
        std::string pathSym=sym->getPath();
        std::string pathRes;
        std::string idRes;
        tie(pathRes, idRes)= separate(pathSym);
        return getFile(pathRes, idRes);
    }
   throw filesystemException("File are you searching for, is not present");
}

std::string directory::setName(const std::string &path, const std::string &fileName, const std::string& newName) {
    std::shared_ptr<filesystem> res=this->get(path, fileName);
    std::string old=res->getName();
    std::string newN=res->setName(newName);
    return old;
}



std::shared_ptr<directory> directory::addDirectory(const std::string &name, int idToAssign) {
    if(std::any_of(contained.begin(), contained.end(), [name](const std::shared_ptr<filesystem> &i){return i->getName()==name;}))
        throw filesystemException("You already have an element with the same name");
    std::shared_ptr<directory> newDir(new directory(name, idToAssign));//directory deve essere protetto
    newDir->parent=this->self;
    newDir->self=newDir;
    contained.push_back(newDir);
    return newDir;
}


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

std::shared_ptr<class symlink>
directory::addLink(const std::string &path, const std::string &name, const std::string &filePath,
                   const std::string &fileName)
{
    std::string pathAdd;
    std::string idAdd;
    tie(pathAdd, idAdd)= separate(path);
    std::shared_ptr<directory> save=getDir(pathAdd, idAdd);
    if(std::any_of(save->contained.begin(), save->contained.end(), [name](const std::shared_ptr<filesystem> i){return i->getName()==name;}))
        throw filesystemException("You already have an element with the same name");
    std::shared_ptr<symlink> newSym(new symlink(name, filePath, fileName));
    save->contained.push_back(newSym);
    return newSym;
}

resourceType directory::resType() const {
    return resourceType::directory;
}


document &
directory::access(const user &targetUser, const std::string &path, const std::string &resName, privilege accessMode) {
   std::shared_ptr<file> f=getFile(path, resName);
    return f->access(targetUser, accessMode);
}


//FIXME: way too too long! Methods should be at maximum 7-10 lines long
// Simplify, extract blocks in private methods and make all the thing understandable
// identify what a single block must do, this may help to simplify and reduce the code
// Please note that someday (soon) we will have to achieve some parallelization, it's impossible with a method this long
std::shared_ptr<filesystem> directory::remove(const user &targetUser, const std::string &path, const std::string &resName) {
    std::shared_ptr<filesystem> obj=this->get(path, resName);
    std::string idRem;
    if(path!="./" && !path.empty())
    {
        std::string pathRem;
        tie(pathRem, idRem)= separate(path);
        std::shared_ptr<directory> dir=this->getDir(pathRem, idRem);
        return dir->remove(targetUser, "", resName);
    }
    idRem=resName;
    if(obj->resType()==resourceType::file)
    {
        std::shared_ptr<file> f=std::dynamic_pointer_cast<file>(obj);
        if(f->moreOwner(targetUser.getUsername()))
            throw filesystemException("You are not the only owner of this file, so you cannot delete it");
        auto it=std::find_if(contained.begin(), contained.end(),
                             [idRem, f](const std::shared_ptr<filesystem>& i){return i->getId()==f->getId();});
        contained.erase(it);
        return obj;
    }

    if(obj->resType()==resourceType::symlink)
    {
        std::shared_ptr<symlink> s=std::dynamic_pointer_cast<symlink>(obj);

        std::string pathFile;
        std::string idFile;
        tie(pathFile, idFile)= separate(s->getPath());
        std::shared_ptr<file> f=getRoot()->getFile(pathFile, idFile);
        if(f->moreOwner(targetUser.getUsername()))
            throw filesystemException("You are not the only owner of this file, so you cannot delete it");
        if(!(f->deleteFromStrategy(targetUser.getUsername())))
            throw filesystemException("You have some error while you try deleting a symlink");
        auto it=std::find_if(contained.begin(), contained.end(),
                             [idRem, s](std::shared_ptr<filesystem> i){return i->getId()==s->getId();});

        contained.erase(it);
        return obj;
    }
    std::shared_ptr<directory> d=std::dynamic_pointer_cast<directory>(obj);
    for(auto iterator: d->contained)
    {
        std::shared_ptr<filesystem> rem=d->remove(targetUser, "", std::to_string(iterator->getId()));
    }
    auto it=std::find_if(contained.begin(), contained.end(),
                         [idRem, d](std::shared_ptr<filesystem> i){return i->getId()==d->getId();});

    contained.erase(it);
    return obj;
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

//FIXME: clean what is unuseful.
Symposium::symlink::symlink(const std::string &name, const std::string &pathToFile, const std::string &fileName) : filesystem(name), pathToFile(pathToFile), fileName(fileName) {

    //if(!pathIsValid2(pathToFile))
        //throw filesystemException("Invalid path to symlink");
    //std::regex pathPattern{R"(\.(\/[a-zA-Z0-9]+)+)"};
    //if(std::regex_match(pathToFile, pathPattern))
            //throw filesystemException("Invalid path to symlink");
    strategy=std::make_unique<TrivialAccess>();

}

resourceType Symposium::symlink::resType() const {
    return resourceType::symlink;
}

document& Symposium::symlink::access(const user &targetUser, privilege accessMode) {
    std::shared_ptr<file> f=directory::getRoot()->getFile(pathToFile, fileName);
    return f->access(targetUser, accessMode);
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

std::string Symposium::symlink::getPath() {
    return pathToFile+"/"+fileName;
}

//FIXME: same thing of file::print
std::string Symposium::symlink::print(const std::string &targetUser, bool recursive, int indent) const {
    std::shared_ptr<file> file=directory::getRoot()->getFile(pathToFile, fileName);
    std::ostringstream priv;
    std::ostringstream typeres;
    typeres<<resType();
    std::string ritorno="";
    priv<<file->getUserPrivilege(targetUser);
    if(file->getUserPrivilege(targetUser)==privilege::none)
        return name+" You no longer have the possibility to access the file in any mode";
    //FIXME: no need for this if, you can call it even with indent=0
    if (indent>0)
    {
        ritorno.insert(0, indent, ' ');
    }
    return ritorno+typeres.str()+" " +name + " " + priv.str();
}


