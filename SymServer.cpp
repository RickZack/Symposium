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
 * File:   SymServer.cpp
 * Project: Symposium
 * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Alice Morano <s259158 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 20 Giugno 2019, 21.30
 */
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/export.hpp>
#include <fstream>

#include "SymServer.h"
#include "message.h"
#include "filesystem.h"
#include <regex>
#include "SymposiumException.h"



using namespace Symposium;

uint_positive_cnt SymServer::idCounter; // NOLINT
const user SymServer::unknownUser("unknown", "@dummY!Pwd", "unknown", ":/resources/avatar/unknown.png", 0, nullptr); // NOLINT

const user & SymServer::addUser(user &newUser, uint_positive_cnt::type respMsgId) {
    if(userIsRegistered(newUser.getUsername()))
        throw SymServerException(SymServerException::userAlreadyExist, UnpackFileLineFunction());
    if(!userIsValid(newUser)){
        throw SymServerException(SymServerException::userWrongParam, UnpackFileLineFunction());
    }
    auto userDir=rootDir->addDirectory(newUser.getUsername());
    newUser.setHome(userDir);
    newUser.setSiteId(idCounter++);
    auto& target=registerUser(newUser);

    //response to client
    auto response=std::make_shared<loginMessage>(msgType::registration, msgOutcome::success, target, respMsgId);
    insertMessageForSiteIds({target.getSiteId()}, response);

    return target;
}

const user& SymServer::login(const std::string &username, const std::string &pwd, uint_positive_cnt::type respMsgId) {
    SymServerException::SymServerExceptionCodes exCode=SymServerException::userNotLogged;
    if(!userIsRegistered(username))
        throw SymServerException(SymServerException::userNotRegistered, UnpackFileLineFunction());

    user& target=getRegistered(username);
    if(!target.hasPwd(pwd))
        exCode=SymServerException::userWrongPwd;
    else if(userIsActive(username))
        exCode=SymServerException::userAlreadyActive;

    if(exCode!=SymServerException::userNotLogged)
        throw SymServerException(exCode, UnpackFileLineFunction());

    auto response=std::make_shared<loginMessage>(msgType::login, msgOutcome::success, target, respMsgId);
    insertMessageForSiteIds({target.getSiteId()}, response);

    active[username]=&target;
    return target;
}

std::shared_ptr<file>
SymServer::openSource(const std::string &opener, const std::string &resPath, const std::string &resId, privilege reqPriv,
                      uint_positive_cnt::type respMsgId) {
    if(!userIsActive(opener))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    const user& target=getRegistered(opener);
    std::shared_ptr<file> fileReq= target.openFile(resPath, resId, reqPriv);
    handleUserState(opener, fileReq->getDoc().getId());
    document& docReq= fileReq->access(target, reqPriv);
    workingDoc[opener].push_front(&docReq);
    resIdToSiteId[docReq.getId()].push_front(target.getSiteId());

    //Propagation to other clients
    auto toSend=std::make_shared<updateActiveMessage>(msgType::addActiveUser, msgOutcome::success, target.makeCopyNoPwd(), docReq.getId(), reqPriv, respMsgId);
    auto siteIdToSend= siteIdsFor(docReq.getId(), target.getSiteId());
    insertMessageForSiteIds(siteIdToSend, toSend);

    //response to client
    auto response=std::make_shared<sendResMessage>(msgType::openRes, msgOutcome::success, fileReq, 0, respMsgId);
    insertMessageForSiteIds({target.getSiteId()}, response);
    return fileReq;
}

std::shared_ptr<file>
SymServer::openNewSource(const std::string &opener, const std::string &absolutePath, const std::string &destPath,
                         const std::string &destName, privilege reqPriv, uint_positive_cnt::type respMsgId) {
    if(!userIsActive(opener))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    const user& target=getRegistered(opener);

    std::pair<int, std::shared_ptr<file>> fileReq= target.accessFile(absolutePath, destPath, destName,
                                                                     reqPriv);
    handleUserState(opener, fileReq.second->getDoc().getId());
    privilege granted=fileReq.second->getUserPrivilege(opener);
    document& docReq=fileReq.second->access(target, granted);
    workingDoc[opener].push_front(&docReq);
    resIdToSiteId[docReq.getId()].push_front(target.getSiteId());

    //Propagation to other clients
    auto toSend=std::make_shared<updateActiveMessage>(msgType::addActiveUser, msgOutcome::success, target.makeCopyNoPwd(), docReq.getId(), granted, respMsgId);
    auto siteIdToSend= siteIdsFor(docReq.getId(), target.getSiteId());
    insertMessageForSiteIds(siteIdToSend, toSend);

    //response to client
    auto response=std::make_shared<sendResMessage>(msgType::openNewRes, msgOutcome::success, fileReq.second, fileReq.first, respMsgId);
    insertMessageForSiteIds({target.getSiteId()}, response);
    return fileReq.second;
}

const document & SymServer::createNewSource(const std::string &opener, const std::string &resPath, const std::string &resName,
                                            uint_positive_cnt::type respMsgId) {
    if(!userIsActive(opener))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    user& target=getRegistered(opener);
    std::shared_ptr<file> fileCreated= target.newFile(resName, resPath, 0);
    document& docReq=fileCreated->access(target, privilege::owner);
    workingDoc[opener].push_front(&docReq);
    resIdToSiteId[docReq.getId()].push_front(target.getSiteId());

    //response to client
    auto response=std::make_shared<sendResMessage>(msgType::createRes, msgOutcome::success, fileCreated, 0, respMsgId);
    insertMessageForSiteIds({target.getSiteId()}, response);
    return docReq;
}

std::shared_ptr<directory>
SymServer::createNewDir(const std::string &opener, const std::string &resPath, const std::string &resName,
                        uint_positive_cnt::type respMsgId) {
    if(!userIsActive(opener))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    user& target=getRegistered(opener);
    auto d=target.newDirectory(resName, resPath);

    //response to client
    auto response=std::make_shared<sendResMessage>(msgType::createNewDir, msgOutcome::success, d, 0, respMsgId);
    insertMessageForSiteIds({target.getSiteId()}, response);
    return d;
}

void SymServer::remoteInsert(const std::string &inserter, uint_positive_cnt::type docId, const symbolMessage &symMsg) {
    if(!userIsActive(inserter))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    std::pair<bool, document*> docRetrieved=userIsWorkingOnDocument(inserter, docId);
    if(!docRetrieved.first)
        throw SymServerException(SymServerException::userNotWorkingOnDoc, UnpackFileLineFunction());
    auto toSend=new symbolMessage(symMsg);
    toSend->verifySym().clearAuthParam();
    docRetrieved.second->remoteInsert(symMsg.getSiteId(), toSend->getSym());


    insertMessageForSiteIds(siteIdsFor(docId, getRegistered(inserter).getSiteId()), std::shared_ptr<serverMessage>(toSend));
    generateSimpleResponse(getRegistered(inserter).getSiteId(), msgType::insertSymbol, symMsg.getMsgId());
}

void SymServer::remoteRemove(const std::string &remover, uint_positive_cnt::type docId, const symbolMessage &rmMsg) {
    if(!userIsActive(remover))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    std::pair<bool, document*> docRetrieved=userIsWorkingOnDocument(remover, docId);
    user& actionU=getRegistered(remover);
    if(!docRetrieved.first)
        throw SymServerException(SymServerException::userNotWorkingOnDoc, UnpackFileLineFunction());
    auto toSend=new symbolMessage(rmMsg);
    toSend->verifySym().clearAuthParam();

    docRetrieved.second->remoteRemove(toSend->getSiteId(), toSend->getSym());

    insertMessageForSiteIds(siteIdsFor(docId, actionU.getSiteId()), std::shared_ptr<serverMessage>(toSend));
    generateSimpleResponse(actionU.getSiteId(), msgType::removeSymbol, rmMsg.getMsgId());
}

void SymServer::updateCursorPos(const std::string &targetUser, uint_positive_cnt::type docId, const cursorMessage &crMsg) {
    if(!userIsActive(targetUser))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    std::pair<bool, document*> docRetrieved=userIsWorkingOnDocument(targetUser, docId);
    user& actionU=getRegistered(targetUser);
    if(!docRetrieved.first)
        throw SymServerException(SymServerException::userNotWorkingOnDoc, UnpackFileLineFunction());
    docRetrieved.second->updateCursorPos(actionU.getSiteId(), crMsg.getRow(), crMsg.getCol());

    auto toSend=new cursorMessage(crMsg);
    toSend->clearAuthParam();
    insertMessageForSiteIds(siteIdsFor(docId, actionU.getSiteId()), std::shared_ptr<serverMessage>(toSend));
    generateSimpleResponse(actionU.getSiteId(), msgType::updateCursor, crMsg.getMsgId());
}

privilege
SymServer::editPrivilege(const std::string &actionUser, const std::string &targetUser, const std::string &resPath,
                         const std::string &resId, privilege newPrivilege, uint_positive_cnt::type respMsgId) {
    if(!userIsActive(actionUser) || !userIsRegistered(targetUser))
        throw SymServerException(SymServerException::actionUserNotLoggedOrTargetUserNotRegistered, UnpackFileLineFunction());
    const user& actionU=getRegistered(actionUser);
    uint_positive_cnt::type docId= handleAccessToDoc(actionUser, resId, resPath, actionU);
    handleUserState(targetUser, docId);
    privilege oldPriv=actionU.editPrivilege(targetUser, resPath, resId, newPrivilege);

    std::forward_list<uint_positive_cnt::type> setSiteIds= siteIdOfUserOfDoc(resIdOfDocOfUser(actionUser), actionU.getSiteId());
    std::string absPath, realId;
    std::tie(absPath, realId)= fromLocalPathToGlobal(actionU, resPath, resId);
    auto toSend=std::make_shared<privMessage>(msgType::changePrivileges, std::make_pair(actionUser,""), msgOutcome::success, absPath+"/"+realId, targetUser, newPrivilege, respMsgId);
    insertMessageForSiteIds(setSiteIds, toSend);

    generateSimpleResponse(actionU.getSiteId(), msgType::changePrivileges, respMsgId);
    return oldPriv;
}

uint_positive_cnt::type
SymServer::handleAccessToDoc(const std::string &actionUser, const std::string &resName,
                             const std::string &pathFromUserHome, const user &actionU) {
    std::shared_ptr<file> fileReq= actionU.openFile(pathFromUserHome, resName, privilege::owner);
    document& docReq=fileReq->access(actionU, privilege::owner);
    int docId=docReq.getId();
    if(!userIsWorkingOnDocument(actionUser, docId).first)
        docReq.close(actionU);
    return docId;
}

void SymServer::handleUserState(const std::string &targetUser, uint_positive_cnt::type docId, bool working) {
    if(userIsWorkingOnDocument(targetUser, docId).first==working){
        SymServerException::SymServerExceptionCodes code;
        if(working)
            code=SymServerException::userWorkingOnDoc;
        else
            code=SymServerException::userNotWorkingOnDoc;
        throw SymServerException(code, UnpackFileLineFunction());
    }

}

std::shared_ptr<filesystem>
SymServer::shareResource(const std::string &actionUser, const std::string &resPath, const std::string &resId,
                         const uri &newPrefs, uint_positive_cnt::type respMsgId) {
    if(!userIsActive(actionUser))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    const user& actionU=getRegistered(actionUser);
    auto res= actionU.shareResource(resPath, resId, newPrefs);
    std::string absPath, realId;
    std::tie(absPath, realId)= fromLocalPathToGlobal(actionU, resPath, resId);

    auto toSend=std::make_shared<uriMessage>(msgType::shareRes, make_pair(actionUser, ""), msgOutcome ::success, absPath, realId, newPrefs, respMsgId);
    int docId= std::dynamic_pointer_cast<file>(res)->getDoc().getId();

    std::forward_list<uint_positive_cnt::type> setSiteIds= siteIdsFor(docId, actionU.getSiteId());
    insertMessageForSiteIds(setSiteIds, toSend);

    generateSimpleResponse(actionU.getSiteId(), msgType::shareRes, respMsgId);
    return res;
}

std::shared_ptr<filesystem>
SymServer::renameResource(const std::string &renamer, const std::string &resPath, const std::string &resId,
                          const std::string &newName, uint_positive_cnt::type respMsgId) {
    if(!userIsActive(renamer))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    user& actionU=getRegistered(renamer);
    auto res=actionU.renameResource(resPath, resId, newName);

    generateSimpleResponse(actionU.getSiteId(), msgType::changeResName, respMsgId);
    return res;
}

std::shared_ptr<filesystem>
SymServer::removeResource(const std::string &remover, const std::string &resPath, const std::string &resId,
                          uint_positive_cnt::type respMsgId) {
    if(!userIsActive(remover))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    user& actionU=getRegistered(remover);
    auto res=actionU.removeResource(resPath, resId);

    generateSimpleResponse(actionU.getSiteId(), msgType::removeRes, respMsgId);
    return res;
}

void SymServer::closeSource(const std::string &actionUser, uint_positive_cnt::type docId,
                            uint_positive_cnt::type respMsgId) {
    std::pair<bool, document*> toClose=userIsWorkingOnDocument(actionUser, docId);
    if(!toClose.first)
        throw SymServerException(SymServerException::userNotWorkingOnDoc, UnpackFileLineFunction());
    user& actionU=getRegistered(actionUser);
    toClose.second->close(actionU);
    workingDoc[actionUser].remove_if([docId](document* doc){return docId == doc->getId();});
    resIdToSiteId[docId].remove(actionU.getSiteId());

    auto toSend= std::make_shared<updateActiveMessage>(msgType::removeActiveUser, msgOutcome::success, getRegistered(actionUser).makeCopyNoPwd(), docId, privilege::readOnly, respMsgId);
    insertMessageForSiteIds(siteIdsFor(docId, actionU.getSiteId()), toSend);

    generateSimpleResponse(actionU.getSiteId(), msgType::closeRes, respMsgId);
}

const user & SymServer::editUser(const std::string &username, user &newUserData, uint_positive_cnt::type respMsgId) {
    if(!userIsActive(username))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    user& toEdit=getRegistered(username);
    toEdit.setNewData(newUserData);

    auto toSend= std::make_shared<userDataMessage>(msgType::changeUserData, std::make_pair(username, ""), msgOutcome::success, toEdit.makeCopyNoPwd(), respMsgId);

    //notification to other clients interested in changes on this user
    std::forward_list<uint_positive_cnt::type> setSiteIds= siteIdOfUserOfDoc(resIdOfDocOfUser(username), toEdit.getSiteId());
    insertMessageForSiteIds(setSiteIds, toSend);

    generateSimpleResponse(toEdit.getSiteId(), msgType::changeUserData, respMsgId);
    return toEdit;
}

void SymServer::removeUser(const std::string &username, const std::string &pwd, uint_positive_cnt::type respMsgId) {
    if (!userIsRegistered(username))
        throw SymServerException(SymServerException::userNotRegistered, UnpackFileLineFunction());
    user& toRemove=getRegistered(username);
    uint_positive_cnt::type userSiteId=toRemove.getSiteId();
    if(!toRemove.hasPwd(pwd))
        throw SymServerException(SymServerException::userWrongPwd, UnpackFileLineFunction());
    closeAllDocsAndPropagateMex(toRemove, workingDoc[username], respMsgId);
    active.erase(username);
    rootDir->remove(toRemove, "./", std::to_string(toRemove.getHome()->getId()));
    removeRegistered(username);

    generateSimpleResponse(userSiteId, msgType::removeUser, respMsgId);
}

void SymServer::logout(const std::string &username, uint_positive_cnt::type respMsgId) {
    if(!userIsActive(username))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    user& toLogOut=getRegistered(username);
    closeAllDocsAndPropagateMex(toLogOut, workingDoc[username], respMsgId);
    active.erase(username);

    generateSimpleResponse(toLogOut.getSiteId(), msgType::logout, respMsgId);
}

std::map<uint_positive_cnt::type, user>
SymServer::mapSiteIdToUser(const std::string &actionUser, uint_positive_cnt::type docId,
                           uint_positive_cnt::type respMsgId) {
    std::pair<bool, document*> retrieved=userIsWorkingOnDocument(actionUser, docId);
    if(!retrieved.first)
        throw SymServerException(SymServerException::userNotWorkingOnDoc, UnpackFileLineFunction());
    std::set<uint_positive_cnt::type> siteIds=retrieved.second->retrieveSiteIds();
    std::map<uint_positive_cnt::type, user> result;
    for(uint_positive_cnt::type siteId:siteIds){
        const user& founded= findUserBySiteId(siteId);
        result.emplace(siteId, founded);
    }

    //response to client
    auto response=std::make_shared<mapMessage>(msgType::mapChangesToUser, msgOutcome::success, result, respMsgId);
    insertMessageForSiteIds({getRegistered(actionUser).getSiteId()}, response);

    return result;
}

bool SymServer::userIsRegistered(const std::string &toCheck) const noexcept {
    return registered.count(toCheck)==1;
}

bool SymServer::userIsValid(const user &toCheck) noexcept {
    std::regex pathPattern{R"(\:\/resources\/avatar\/([a-zA-Z 0-9]*)*([a-zA-Z 0-9]*\.((jpg|png|ico|bmp))))"};
    return !toCheck.getUsername().empty() && !toCheck.getNickname().empty()
            && std::regex_match(toCheck.getIconPath(), pathPattern);
}

SymServer::SymServer(bool loading, bool storing) : loadData(loading), storeData(storing){
    bool loaded=false;
    if(loadData)
        document::doLightSerializing([&](){
            loaded=load();
        });
    if(!loaded)
        rootDir=directory::getRoot();
}

template<class Archive>
void SymServer::serialize(Archive &ar, const unsigned int)
{
    ar & registered & idCounter  & rootDir;
}

bool SymServer::userIsActive(const std::string &username) const{
    return active.count(username)==1;
}

std::pair<bool, document*> SymServer::userIsWorkingOnDocument(const std::string &username, uint_positive_cnt::type resourceId) const {
    std::pair<bool, document*> result(false, nullptr);
    if(workingDoc.count(username)==0) return result;
    for(const auto& doc:workingDoc.at(username))
        if (doc->getId()==resourceId){
            result.first=true; result.second=doc;
            break;
        }
    return result;
}

const user& SymServer::findUserBySiteId(int id) const noexcept{
    for(const auto& elem:registered)
        if(elem.second.getSiteId()==id)
            return elem.second;
    return unknownUser;
}

user &SymServer::registerUser(const user &toInsert) {
    return registered[toInsert.getUsername()]=toInsert;
}

user &SymServer::getRegistered(const std::string &username) {
    return registered.at(username);
}

void SymServer::removeRegistered(const std::string &username) {
    registered.erase(username);
}

std::forward_list<uint_positive_cnt::type> SymServer::siteIdsFor(uint_positive_cnt::type resId, uint_positive_cnt::type siteIdToExclude) const {
    auto it=resIdToSiteId.find(resId);
    auto siteIds=it->second;
    if(it == resIdToSiteId.end())
        throw SymServerException(SymServerException::noUserWorkingOnRes, UnpackFileLineFunction());
    if(siteIdToExclude>0)
        siteIds.remove(siteIdToExclude);
    return siteIds;
}

void SymServer::insertMessageForSiteIds(const std::forward_list<uint_positive_cnt::type> &siteIds, const std::shared_ptr<serverMessage>& toSend) {
    for(uint_positive_cnt::type id:siteIds){
        siteIdToMex[id].push(toSend);
    }
}

std::forward_list<uint_positive_cnt::type> SymServer::resIdOfDocOfUser(const std::string &username) const {
    std::forward_list<uint_positive_cnt::type> resIds;
    if(workingDoc.count(username)==0) return resIds;
    for(const auto* d:workingDoc.find(username)->second){
        resIds.push_front(d->getId());
    }
    return resIds;
}

std::forward_list<uint_positive_cnt::type>
SymServer::siteIdOfUserOfDoc(const std::forward_list<uint_positive_cnt::type> &resIds, unsigned int siteIdToExclude) const {
    std::set<int> siteIds;
    for(int resId:resIds){
        for(int siteId:siteIdsFor(resId, siteIdToExclude)){
            siteIds.insert(siteId);
        }
    }
    return std::forward_list<uint_positive_cnt::type>(siteIds.begin(), siteIds.end());
}

void SymServer::closeAllDocsAndPropagateMex(const user &loggedOut, const std::forward_list<document *> &listOfDocs,
                                            uint_positive_cnt::type respMsgId) {
    for(document* doc: listOfDocs) {
        doc->close(loggedOut);
        auto toSend=std::make_shared<updateActiveMessage>(msgType::removeActiveUser, msgOutcome::success, loggedOut.makeCopyNoPwd(), doc->getId(), privilege::readOnly, respMsgId);
        insertMessageForSiteIds(siteIdsFor(doc->getId(), loggedOut.getSiteId()), toSend);
    }
    handleLeavingUser(loggedOut);
}

void SymServer::handleLeavingUser(const user &loggedOut) {
    auto l=resIdOfDocOfUser(loggedOut.getUsername());
    for (int resId:l){
        resIdToSiteId[resId].remove(loggedOut.getSiteId());
    }
    workingDoc.erase(loggedOut.getUsername());
}

std::pair<const uint_positive_cnt::type, std::shared_ptr<serverMessage>> SymServer::extractNextMessage() {
    std::pair<uint_positive_cnt::type, std::shared_ptr<serverMessage>> result(0, nullptr);
    for(std::pair<const uint_positive_cnt::type, std::queue<std::shared_ptr<serverMessage>>>& mexForSiteId:siteIdToMex)
        if(!mexForSiteId.second.empty()){
            result.first=mexForSiteId.first;
            result.second=mexForSiteId.second.front();
            mexForSiteId.second.pop();
            if(result.second->isFinalMex())
                siteIdToMex.erase(result.first);
            break;
        }
    return result;
}

void SymServer::generateSimpleResponse(unsigned int recvSiteId, msgType action, uint_positive_cnt::type respMsgId) {
    auto response=std::make_shared<serverMessage>(action, msgOutcome::success, respMsgId);
    insertMessageForSiteIds({recvSiteId}, response);
}

bool SymServer::operator==(const SymServer &rhs) const {
    return registered == rhs.registered;
}

bool SymServer::operator!=(const SymServer &rhs) const {
    return !(rhs == *this);
}

uint_positive_cnt::type SymServer::getSiteIdOfUser(const std::string &username) const {
    return registered.at(username).getSiteId();
}

void SymServer::store() const {
    std::ofstream out{storeFile};
    if(out.good()) {
        try {
            boost::archive::text_oarchive oa(out);
            oa << *this;
        }
        catch(std::exception& e) {
            std::cerr << "Unable to store data on disk: " << e.what() << std::endl;
            remove(storeFile);
        }

    } else
        std::cerr<<"Error opening output file"<<std::endl;
}

bool SymServer::load() {
    std::ifstream input{storeFile, std::ios::in};
    if(input.good()){
        try {
            SymServer temp(false, false);
            boost::archive::text_iarchive ia(input);
            ia>>temp;
            *this=std::move(temp);
            storeData=true; loadData=true; //they were overwritten by moving temp into *this
            return true;
        }
        catch(std::exception& e) {
            std::cerr << "Unable to load data from disk: " << e.what() << std::endl;
        }
    }
    return false;
}

SymServer::~SymServer() {
    if(storeData) {
        document::doLightSerializing([this]() {
            this->store();
        });
        rootDir->storeContent();
    }
}

void SymServer::hardLogout(uint_positive_cnt::type siteId) {
    const auto& userInActive=std::find_if(active.cbegin(), active.cend(), [siteId](const auto& el){return el.second->getSiteId()==siteId;});
    if(userInActive!=active.end()){
        const std::string& username=userInActive->second->getUsername();
        user& toLogOut=getRegistered(username);
        closeAllDocsAndPropagateMex(toLogOut, workingDoc[username], 1);
        active.erase(username);
    }
}

void SymServer::editLineStyle(const std::string &actionUser, uint_positive_cnt::type docId,
                              const std::pair<alignType, unsigned> &newLineStyle, unsigned row,
                              const editLineStyleMessage &editMsg) {
    if(!userIsActive(actionUser))
        throw SymServerException(SymServerException::userNotLogged, UnpackFileLineFunction());
    std::pair<bool, document*> docRetrieved=userIsWorkingOnDocument(actionUser, docId);
    user& actionU=getRegistered(actionUser);
    if(!docRetrieved.first)
        throw SymServerException(SymServerException::userNotWorkingOnDoc, UnpackFileLineFunction());
    docRetrieved.second->editLineStyle(newLineStyle, row);

    auto toSend=new editLineStyleMessage(editMsg);
    toSend->clearAuthParam();

    insertMessageForSiteIds(siteIdsFor(docId, actionU.getSiteId()), std::shared_ptr<serverMessage>(toSend));
    generateSimpleResponse(actionU.getSiteId(), msgType::editLineStyle, editMsg.getMsgId());
}

std::pair<std::string, std::string>
SymServer::fromLocalPathToGlobal(const user &actionU, const std::string &resPath, const std::string &resId) {
    auto res=actionU.getHome()->get(resPath, resId);
    std::string absPath;
    std::string id;
    if(res->resType()==resourceType::symlink){
        auto sym=std::dynamic_pointer_cast<symlink>(res);
        absPath=sym->getPath();
        absPath.erase(absPath.find_last_of('/'), absPath.size());
        id=sym->getResId();
    }
    else{
        absPath="./"+std::to_string(actionU.getHome()->getId());
        absPath.insert(absPath.end(), resPath.begin()+2, resPath.end()); //do not copy the initial "./" from resPath
        id=resId;
    }
    return {absPath, id};
}



