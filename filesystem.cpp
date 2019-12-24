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

filesystem::filesystem(const std::string &name, const int idToAssign) : name(name){
    if(idToAssign==0){
        id=idCounter;
        idCounter++;
    }
    else
        id=idToAssign;
}

int filesystem::getId() const {
    return id;
}

const std::string &filesystem::getName() const {
    return name;
}

privilege filesystem::getUserPrivilege(const std::string &targetUser) const {
    throw filesystemException(filesystemException::objSha, UnpackFileLineFunction());
}

uri &filesystem::getSharingPolicy() {
    return sharingPolicy;
}

privilege filesystem::setUserPrivilege(const std::string &targetUser, privilege newPrivilege) {
    throw filesystemException(filesystemException::objSha, UnpackFileLineFunction());
}

std::string filesystem::setName(const std::string &newName) {
    filesystem::name=newName;
    return name;
}

uri filesystem::setSharingPolicy(const std::string &actionUser, const uri &newSharingPrefs) {
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
    id2.append(path3.begin()+found+1,path3.end()); //the id of directory where the current user want to insert the file
    return  std::make_tuple(path2, id2);
}

bool filesystem::pathIsValid2(const std::string &toCheck) {
    std::regex pathPattern{R"(\.(\/[a-zA-Z0-9]+)+)"};
    return !toCheck.empty() && std::regex_match(toCheck, pathPattern);
}

file::file(const std::string &name, const std::string &realPath, int idToAssign) : filesystem(name, idToAssign), realPath(realPath), doc(0){
    if(!(pathIsValid2(realPath)))
        throw filesystemException(filesystemException::pathNvalid, UnpackFileLineFunction());
    strategy=std::make_unique<RMOAccess>();
}

resourceType file::resType() const {
    return resourceType::file;
}

privilege file::getUserPrivilege(const std::string &targetUser) const {

    privilege userPriv= strategy->getPrivilege(targetUser);
    return userPriv;
}

//FIXME: questa funzione viene chiamata anche quando un utente vuole cambiare il proprio privilegio,
// ma in tal caso lancerebbe sempre eccezione.
// Ad esempio: Riccardo ha privilegio privilege::modify e vuole avere privilege::readOnly
// il metodo controlla che Riccardo abbia privilege::owner, trova che non è così e lancia
// Anche il test è da rivedere.
privilege file::setUserPrivilege(const std::string &targetUser, privilege newPrivilege) {
    //FIXME: use stategy->validateAction() as guard to action
    privilege oldPrivilege=getUserPrivilege(targetUser);
    if(oldPrivilege==privilege::owner)
        throw filesystemException(filesystemException::changePriv, UnpackFileLineFunction());
    strategy->setPrivilege(targetUser,newPrivilege);
    return newPrivilege;
}

uri file::setSharingPolicy(const std::string &actionUser, const uri &newSharingPrefs) {
    //FIXME: use stategy->validateAction() as guard to action
    privilege userPriv=strategy->getPrivilege(actionUser);
    if(userPriv==privilege::owner) {
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

void file::store(const std::string &storePath) const {
    //TODO: implement
}

void file::load(const std::string &loadPath) {
    //TODO: implement
}

void file::send() const {
    //TODO: implement
}

//FIXME: è più corretto implementare questa operazione per oggetti di tipo
// filesystem in generale. Non cambierebbe nulla, ma si potrebbe chiamare anche sulle directory.
// Anche TrivialAccess avrà il metodo moreOwner implementato, quindi è ok. Questa scelta sarebbe in
// linea con quella di avere setUserPrivilege, setSharingPolicy, ... implementati anche per filesystem
bool file::moreOwner(const std::string &username)
{
   return strategy->moreOwner(username);
}

bool file::deleteFromStrategy(const std::string &userName)
{
    //FIXME: leggere prima commento a directory::remove
    // Se non serve più il bool si può rendere questa funzione void
    // e chiamare setUserPrivilege(username, privilege::none) per ottenere
    // lo stesso risultato
   return strategy->deleteUser(userName);
}

std::string file::print(const std::string &targetUser, bool recursive, int indent) const {
    std::string ritorno;
    if (indent>0)
        ritorno.insert(0, indent, ' '); //first need to insert indent
    std::ostringstream typeres;
    typeres<<resType(); //then the typer of the resource
    std::ostringstream priv;
    if(getUserPrivilege(targetUser)==privilege::none)
        return name+" You no longer have the possibility to access the file in any mode";
    priv<<getUserPrivilege(targetUser); //the privilege
    return ritorno+typeres.str()+" "+name + " " + priv.str();
}



const document &file::getDoc() const {
    return doc;
}

directory::directory(const std::string &name, const int &idToAssign) : filesystem(name, idToAssign) {

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
        path.erase(path.begin()+0); // TODO: +0 superfluo
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



std::shared_ptr<filesystem> directory::get(const std::string &path, const std::string &name) {
    if(path==""||path=="./")//if path is "here" so I only need to get the object with the id represented by name
    {
        auto it=std::find_if(contained.begin(), contained.end(),
                             [name](std::shared_ptr<filesystem> i){return std::to_string(i->getId())==name;});
        if(it==contained.end())
            throw filesystemException(filesystemException::noGet, UnpackFileLineFunction());
        return *it;
    }
    std::string idRes;
    std::string pathRes;
    if(path.back()=='/')
        pathRes.append(path.begin(),path.end()-1);
    else
        pathRes.append(path);
    tie(pathRes, idRes)= separateFirst(pathRes);//otherwise need to separate the first directory and move into it

    auto it=std::find_if(contained.begin(), contained.end(),
                         [idRes](std::shared_ptr<filesystem> i){return std::to_string(i->getId())==idRes;});

    if(it==contained.end())
        throw filesystemException(filesystemException::noGet, UnpackFileLineFunction());
    if((*it)->resType()!=resourceType::directory)//if the element is not a directory, there is some error in path
        throw filesystemException(filesystemException::noGet, UnpackFileLineFunction());
    std::shared_ptr <directory> dir=std::dynamic_pointer_cast<directory>(*it);
    return dir->get(pathRes, name);
}


std::shared_ptr<directory> directory::getDir(const std::string &path, const std::string &name) {
    std::shared_ptr<filesystem> res=this->get(path, name);
    if(res->resType()!=resourceType::directory)
        throw filesystemException(filesystemException::noGetDir, UnpackFileLineFunction());
    return std::dynamic_pointer_cast<directory>(res);

}

std::shared_ptr<file> directory::getFile(const std::string &path, const std::string &name) {
    std::shared_ptr<filesystem> res=this->get(path, name);
    if(res->resType()==resourceType::file)
        return std::dynamic_pointer_cast<file>(res);
    if(res->resType()==resourceType::symlink) //is used also for symlink because symlink is the reference to the file object
    {
        std::shared_ptr<symlink> sym=std::dynamic_pointer_cast<symlink>(res);
        std::string pathSym=sym->getPath();
        std::string pathRes;
        std::string idRes;
        tie(pathRes, idRes)= separate(pathSym);
        return getFile(pathRes, idRes);
    }
    throw filesystemException(filesystemException::noGetFile, UnpackFileLineFunction());
}

std::string directory::setName(const std::string &path, const std::string &fileName, const std::string& newName) {
    std::shared_ptr<filesystem> res=this->get(path, fileName);
    std::string old=res->getName();
    std::string newN=res->setName(newName);
    return old;
}



std::shared_ptr<directory> directory::addDirectory(const std::string &name, int idToAssign) {
    if(std::any_of(contained.begin(), contained.end(), [name](const std::shared_ptr<filesystem> &i){return i->getName()==name;}))
        throw filesystemException(filesystemException::sameName, UnpackFileLineFunction());
    std::shared_ptr<directory> newDir(new directory(name, idToAssign));
    newDir->parent=this->self;
    newDir->self=newDir;
    contained.push_back(newDir);
    return newDir;
}


std::shared_ptr<file> directory::addFile(const std::string &path, const std::string &name, int idToAssign) {
    std::string pathAdd;
    std::string idAdd;
    tie(pathAdd, idAdd)= separate(path);
    std::shared_ptr<directory> save=getDir(pathAdd, idAdd);
    if(std::any_of(save->contained.begin(), save->contained.end(), [name](const std::shared_ptr<filesystem> i){return i->getName()==name;}))
        throw filesystemException(filesystemException::sameName, UnpackFileLineFunction());
    std::shared_ptr<file> newFile(new file(name, path, idToAssign));
    save->contained.push_back(newFile);
    return newFile;
}

std::shared_ptr<class symlink>
directory::addLink(const std::string &path, const std::string &name, const std::string &filePath,
                   const std::string &fileName, int idToAssign)
{
    std::string pathAdd;
    std::string idAdd;
    tie(pathAdd, idAdd)= separate(path);
    std::shared_ptr<directory> save=getDir(pathAdd, idAdd);
    if(std::any_of(save->contained.begin(), save->contained.end(), [name](const std::shared_ptr<filesystem> i){return i->getName()==name;}))
        throw filesystemException(filesystemException::sameName, UnpackFileLineFunction());
    std::shared_ptr<symlink> newSym(new symlink(name, filePath, fileName, idToAssign));
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



std::shared_ptr<filesystem> directory::remove(const user &targetUser, const std::string &path, const std::string &resName) {
    std::shared_ptr<filesystem> obj=this->get(path, resName); //take the object to remove
    std::string idRem;
    if(path!="./" && !path.empty())
    {
        std::string pathRem;
        tie(pathRem, idRem)= separate(path);
        std::shared_ptr<directory> dir=this->getDir(pathRem, idRem);
        return dir->remove(targetUser, "", resName); //call remove recursivly if the path is not "here" in order to take the right directory of the object to remove
    }
    idRem=resName;//otherwise the path is "here"
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
    //FIXME: ci vuole un else if: se è un file non è un symlink
    if(obj->resType()==resourceType::symlink)//if the object is symlink
    {
        std::shared_ptr<symlink> s=std::dynamic_pointer_cast<symlink>(obj);

        std::string pathFile;
        std::string idFile;
        tie(pathFile, idFile)= separate(s->getPath());//need to have a pointer for the file and not a symlink in order to operate on file strategy
        std::shared_ptr<file> f=getRoot()->getFile(pathFile, idFile);
        //FIXME: mi viene in mente almeno un caso in cui lanciare è sbagliato e crea problemi.
        // Supponiamo che Riccardo abbia condiviso un file con un privilegio priv!=owner, e che Martina abbia
        // usato l'uri (es. /Riccardo/dir/somefile) per crearsi un symlink. Adesso supponiamo che Riccardo
        // (che è owner) voglia togliere il privilegio a Martina. Adesso Martina ha ancora il symlink, ma al
        // prossimo accesso incontrerà problemi perchè non ha più privilegi (e questo è giusto che lanci eccezione).
        // Quello che non va bene è che Martina (ammesso che non abbia ancora cercato di aprire il file puntato
        // dal symlink) non può eliminare il symlink, perchè si andrà sempre nel ramo del throw.
        // Possiamo prevedere che l'utente sia avvisato di questo problema quando cerca di aprire un file per cui
        // non ha più privilegi, e che gli venga chiesto se vuole eliminare il symlink. Quindi questa operazione non
        // dovrebbe lanciare.
        // A questo punto deleteUser (chiamata da deleteFromStrategy) può avere ritorno void ed essere semplificata
        if(!(f->deleteFromStrategy(targetUser.getUsername())))//delete from strategy target user
            throw filesystemException(filesystemException::someError, UnpackFileLineFunction());
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
    std::string result;
    if(indent==0)
        result.append(targetUser);
    if(!recursive) //if recursive is false, need only the elements of the directory, not a subdirectory elements
    {
        //FIXME: a cosa serve printElement? print è una funzione di cui si fa l'override nelle sottoclassi
        // quindi automaticamente viene chiamata quella del tipo dell'elemento, senza ulteriori if o cast
        for(const auto & it : contained)
            result+=" "+printElement(it, targetUser, 0);

    }
    else { //otherwise need to invoke recursively the method prints for all subdirectory
        for(const auto & it : contained)
        {
            resourceType type=it->resType();
            if(type==resourceType::directory)
            {
                result+=" "+printElement(it, targetUser, indent);
                indent=indent+1;//need to add the indent if it is a subdirectory
                std::shared_ptr<directory> dir=std::dynamic_pointer_cast<directory>(it);
                //FIXME: qui giustamente passi indent+1 (rispetto al valore di indent nella prima nota)
                // Se passi indent+1 direttamente qui ti eviti le due istruzioni in cui incrementi e decrementi
                result+=dir->print(targetUser, true, indent);
                indent=indent-1;//when finished the recursion need to restore the original indent
            }
            else
                result+=" "+printElement(it, targetUser, indent);
        }
    }
    return result;
}

//FIXME: a cosa server? perchè abbiamo print e printElement? Vedi nota in directory::print
std::string directory::printElement(const std::shared_ptr<filesystem> &it, const std::string &targetUser, int indent)
{
    resourceType type=it->resType();
    std::string result;
    result.insert(0, indent, ' ');
    if(type==resourceType::file)
        return result+std::dynamic_pointer_cast<file>(it)->print(targetUser)+"\r\n";
    if(type==resourceType::symlink)
        return result+std::dynamic_pointer_cast<symlink>(it)->print(targetUser)+"\r\n";
    std::shared_ptr<directory> dir=std::dynamic_pointer_cast<directory>(it);
    std::ostringstream typeres;
    typeres<<dir->resType();
    return result+typeres.str()+" "+dir->name+"\r\n";
}

Symposium::symlink::symlink(const std::string &name, const std::string &pathToFile, const std::string &fileName,
                            int idToAssign) : filesystem(name, idToAssign), pathToFile(pathToFile), fileName(fileName) {

    if(!pathIsValid2(pathToFile))
        throw filesystemException(filesystemException::pathNvalid, UnpackFileLineFunction());
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

//TODO: indent dovrebbe essere sempre positivo, quindi porlo come unsigned int. Come garantisco che sia
// sempre positivo?
std::string Symposium::symlink::print(const std::string &targetUser, bool recursive, int indent) const {
    std::shared_ptr<file> file=directory::getRoot()->getFile(pathToFile, fileName);
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
    return ritorno+typeres.str()+" " +name + " " + priv.str();
}

const std::string &Symposium::symlink::getFileName() const {
    return fileName;
}


