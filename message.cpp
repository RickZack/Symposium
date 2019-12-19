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
 * File:   message.cpp
 * Project: Symposium
 * Authors:
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Alice Morano <s259158 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 16 Giugno 2019, 12.39
 */
#include <string>
#include <iostream>
#include "message.h"
#include "SymServer.h"
#include "SymClient.h"
#include "resourceType.h"

using namespace Symposium;
int message::msgCounter=1;

message::message(int msgId){
    if(msgId==0)
    {
        this->msgId=msgCounter;
        msgCounter++;
    }
    else
        this->msgId=msgId;
}

int message::getMsgId() const {
    return msgId;
}

msgType message::getAction() const {
    return action;
}

bool message::operator==(const message &rhs) const {
    return msgId == rhs.msgId &&
           action == rhs.action;
}

bool message::operator!=(const message &rhs) const {
    return !(rhs == *this);
}

clientMessage::clientMessage(msgType action, const std::pair<std::string, std::string> &actionOwner, int msgId)
        : message(msgId), actionOwner(actionOwner) {
    if(action!=msgType::login && action!=msgType::removeUser && action!=msgType::logout)
        throw messageException("Action is not consistent with the message type");
    this->action=action;
}

clientMessage::clientMessage(const std::pair<std::string, std::string> &actionOwner, int msgId)
        : message(msgId), actionOwner(actionOwner) {
}

const std::pair<std::string, std::string> &clientMessage::getActionOwner() const {
    return actionOwner;
}

void clientMessage::invokeMethod(SymServer &server) {
    switch(action)
    {
        case msgType::login:{
            user us=server.login(actionOwner.first, actionOwner.second);
            break;
        }
        case msgType::removeUser:{
            server.removeUser(actionOwner.first, actionOwner.second);
            break;
        }
        case msgType::logout:{
            server.logout(actionOwner.first);
            break;
        }
        default:
            throw messageException("This is not a client message");
    }

}

void clientMessage::completeAction(SymClient &client, msgOutcome serverResult) {
    if(action==msgType::logout)
    {
        client.logout(true);
    }
}

bool clientMessage::operator==(const clientMessage &rhs) const {
    return static_cast<const message &>(*this) == static_cast<const message &>(rhs) &&
           actionOwner == rhs.actionOwner;
}

bool clientMessage::operator!=(const clientMessage &rhs) const {
    return !(rhs == *this);
}

clientMessage & clientMessage::clearAuthParam() {
    actionOwner.first.clear();
    actionOwner.second.clear();
    return *this;
}

askResMessage::askResMessage(msgType action, const std::pair<std::string, std::string> &actionOwner,
                             const std::string &path,
                             const std::string &name, const std::string &resourceId, privilege accessMode, int msgId)
        : message(msgId), clientMessage( actionOwner, msgId), path(path), name(name),
          resourceId(resourceId), accessMode(accessMode) {
    if(action!=msgType::createRes && action!= msgType::openRes && action!=msgType::openNewRes&& action!=msgType::changeResName
    && action!=msgType::createNewDir&& action!=msgType::removeRes)
        throw messageException("Action is not consistent with the message type");

    this->action=action;
}

void askResMessage::invokeMethod(SymServer &server) {
    switch(action)
    {
        case msgType::createRes:{
          server.createNewSource(getActionOwner().first,path,name);
          break;
        }
        case msgType::openRes:{
            server.openSource(getActionOwner().first,path,name,accessMode);
            break;
        }
        case msgType::openNewRes:{
            server.openNewSource(getActionOwner().first,resourceId,path,name,accessMode);
            break;
        }
        case msgType::changeResName:{
            server.renameResource(getActionOwner().first,path, name, resourceId);
            break;
        }
        case msgType::createNewDir:{
            server.createNewDir(getActionOwner().first,path,name);
            break;
        }
        case msgType::removeRes:{
            server.removeResource(getActionOwner().first,path,name);
            break;
        }
        default:
            throw messageException("This is not a valid message");
    }

}

bool askResMessage::operator==(const askResMessage &rhs) const {
    return static_cast<const clientMessage &>(*this) == static_cast<const clientMessage &>(rhs) &&
           path == rhs.path &&
           name == rhs.name &&
           resourceId == rhs.resourceId;
}

bool askResMessage::operator!=(const askResMessage &rhs) const {
    return !(rhs == *this);
}

const std::string &askResMessage::getPath() const {
    return path;
}

const std::string &askResMessage::getName() const {
    return name;
}

const std::string &askResMessage::getResourceId() const {
    return resourceId;
}

void askResMessage::completeAction(SymClient &client, msgOutcome serverResult) {
    if(action==msgType::changeResName)
    {
        client.renameResource(path, name, resourceId, true);
    }
    else if(action==msgType::removeRes)
    {
        client.removeResource(path, name, true);
    }
    else
        throw messageException("Action is not consistent with the message type");
}

signUpMessage::signUpMessage(msgType action, const std::pair<std::string, std::string> &actionOwner,
                            const user &newUser,
                             int msgId)
                             : message(msgId), clientMessage(actionOwner, msgId), newUser(newUser) {
    if(action!=msgType::registration)
        throw messageException("Action is not consistent with the message type");
    this->action=action;
}

void signUpMessage::invokeMethod(SymServer &ss) {
    ss.addUser(newUser);
}

const user &signUpMessage::getNewUser() const {
    return newUser;
}

serverMessage::serverMessage(msgType action, msgOutcome result, int msgId) : message(msgId), result(result) {
    this->action=action;
}

serverMessage::serverMessage(msgOutcome result, int msgId) : message(msgId), result(result) {
}

void serverMessage::invokeMethod(SymClient &client) {
    auto mex=client.retrieveRelatedMessage(*this);
    if(result==msgOutcome::failure)
        throw messageException("The action had not succeded");
    mex->completeAction(client, result);
}

bool serverMessage::isRelatedTo(const clientMessage &other) const {
    return other.getMsgId() == this->msgId && other.getAction() == this->action;
}

msgOutcome serverMessage::getResult() const {
    return result;
}

loginMessage::loginMessage(msgType action, msgOutcome result, const user &loggedUser, int msgId)
                           : message(msgId), serverMessage(result, msgId), loggedUser(loggedUser) {
    if(action!=msgType::login && action!=msgType::registration)
        throw messageException("The action is not consistent with the message type");
    this->action=action;
}

const user &loginMessage::getLoggedUser() const {
    return loggedUser;
}

void loginMessage::invokeMethod(SymClient &client) {
    auto msg= client.retrieveRelatedMessage(*this);
    client.setLoggedUser(loggedUser);
}

mapMessage::mapMessage(msgType action, msgOutcome result, const std::map<int, user> &siteIdToUser, int msgId)
        : message(msgId), serverMessage(result, msgId), siteIdToUser(siteIdToUser) {
    if(action!=msgType::mapChangesToUser)
        throw messageException("The action is not consistent with the message type");
    this->action=action;
}

const std::map<int, user> & mapMessage::getSiteIdToUser() const {
    return siteIdToUser;
}

void mapMessage::invokeMethod(SymClient &client) {
    auto msg= client.retrieveRelatedMessage(*this);
    client.setUserColors(siteIdToUser);
}

sendResMessage::sendResMessage(msgType action, msgOutcome result, std::shared_ptr<filesystem> resource, int symId, int msgId)
        : message(msgId), serverMessage(result, msgId), resource{resource} {
    if(action!=msgType::createRes && action!=msgType::createNewDir && action!=msgType::openNewRes
    && action!=msgType::openRes)
        throw messageException("The action is not consistent with the message type");
    this->action=action;
    if(resource->resType()==resourceType::symlink)
        this->symId=resource->getId();
    else
        this->symId=symId;
}

std::shared_ptr<filesystem> sendResMessage::getResource() const {
    return resource;
}

void sendResMessage::invokeMethod(SymClient &client) {
    auto mex=client.retrieveRelatedMessage(*this);
    switch(mex->getAction())
    {
        case msgType::createRes:{
            std::shared_ptr <askResMessage> mex2=std::dynamic_pointer_cast<askResMessage>(mex);
            client.createNewSource(mex2->getPath(), mex2->getName(), resource->getId());
            break;
        }
        case msgType::openRes:{
            std::shared_ptr <askResMessage> mex2=std::dynamic_pointer_cast<askResMessage>(mex);
            resourceType type=resource->resType();
            if(type==resourceType::file)
            {
                std::shared_ptr <file> f= std::dynamic_pointer_cast<file>(resource);
                client.openSource(f, resource->getUserPrivilege(mex2->getActionOwner().first));
            }
            if(type==resourceType::symlink)
            {
                std::shared_ptr <symlink> s= std::dynamic_pointer_cast<symlink>(resource);
                std::shared_ptr<file> f=directory::getRoot()->getFile(s->getPath(), s->getFileName());
                client.openSource(f, resource->getUserPrivilege(mex2->getActionOwner().first));
            }
            break;
        }
        case msgType::openNewRes:{
            std::shared_ptr <askResMessage> mex2=std::dynamic_pointer_cast<askResMessage>(mex);
            resourceType type=resource->resType();
            if(type==resourceType::file)
            {
                std::shared_ptr <file> f= std::dynamic_pointer_cast<file>(resource);
                client.openNewSource(mex2->getResourceId(), resource->getUserPrivilege(mex2->getActionOwner().first),
                        mex2->getPath(), mex2->getName(), resource->getId(), f);
            }

            if(type==resourceType::symlink)
            {
                std::shared_ptr <symlink> s= std::dynamic_pointer_cast<symlink>(resource);
                std::shared_ptr<file> f=directory::getRoot()->getFile(s->getPath(), s->getFileName());
                client.openNewSource(mex2->getResourceId(), resource->getUserPrivilege(mex2->getActionOwner().first),
                                     mex2->getPath(), mex2->getName(), resource->getId(), f);
            }
           break;
        }
        case msgType::createNewDir:{
            std::shared_ptr <askResMessage> mex2=std::dynamic_pointer_cast<askResMessage>(mex);
            client.createNewDir(mex2->getPath(), mex2->getName(), resource->getId());
            break;
        }
        default:
            throw messageException("This is not a valid message");
    }
}

privMessage::privMessage(msgType action, const std::pair<std::string, std::string> &actionOwner, msgOutcome result,
                         const std::string &resourceId, const std::string &targetUser, privilege newPrivilege,
                         int msgId)
                         : message(msgId), clientMessage(actionOwner, msgId),
                           serverMessage(result, msgId), resourceId(resourceId), targetUser(targetUser), newPrivilege(newPrivilege) {
    if(action!=msgType::changePrivileges)
        throw messageException("The action is not consistent with the message type");
    this->action=action;
}

const std::string &privMessage::getResourceId() const {
    return resourceId;
}

const std::string &privMessage::getTargetUser() const {
    return targetUser;
}

privilege privMessage::getNewPrivilege() const {
    return newPrivilege;
}

void privMessage::invokeMethod(SymServer &server) {
    //TODO: implement
    //
    std::string path1;
    std::string name1;
    tie(path1, name1) = directory::separateFirst(resourceId);

    std::string path2;
    std::string nameRes;
    tie(path2, nameRes) = filesystem::separate(path1);

    std::string pathRes="./";
    pathRes.append(path2);

    server.editPrivilege(getActionOwner().first,targetUser,pathRes,nameRes,newPrivilege);


}
void privMessage::invokeMethod(SymClient &client) {
    //TODO: implement
    std::string path1;
    std::string name1;
    tie(path1, name1) = directory::separateFirst(resourceId);

    std::string path2;
    std::string nameRes;
    tie(path2, nameRes) = filesystem::separate(path1);

    std::string pathRes="./";
    pathRes.append(path2);

    //FIXME: ATTENZIONE, GUARDA GLI SCHEMI, LE DUE LINEE COMMENTATE NON CI DEVONO ESSERE
    //auto msg= client.retrieveRelatedMessage(*this);
    client.editPrivilege(targetUser,pathRes,nameRes,newPrivilege,false);

    //msg->completeAction(client, msgOutcome::success);

}

void privMessage::completeAction(SymClient &client, msgOutcome serverResult) {
    //TODO: implement
    std::string path1;
    std::string name1;
    tie(path1, name1) = directory::separateFirst(resourceId);

    std::string path2;
    std::string nameRes;
    tie(path2, nameRes) = filesystem::separate(path1);

    std::string pathRes="./";
    pathRes.append(path2);

    if(serverResult==msgOutcome::success){
        client.editPrivilege(targetUser,pathRes,nameRes,newPrivilege, true);
    }
    else
        throw messageException("This is not a valid message");
        //FIXME: metti un messaggio di fallimento più adeguato, è fallita l'azione
        // ma il messaggio è valido

}

symbolMessage::symbolMessage(msgType action, const std::pair<std::string, std::string> &actionOwner, msgOutcome result,
                             int siteId,
                             int resourceId, const symbol &sym, int msgId)
                             : message(msgId), clientMessage(actionOwner, msgId),
                               serverMessage(result, msgId), siteId(siteId), resourceId(resourceId), sym(sym) {
    if(action!=msgType::insertSymbol && action!=msgType::removeSymbol)
        throw messageException("The action is not consistent with the message type");
    this->action=action;
}

int symbolMessage::getSiteId() const {
    return siteId;
}

int symbolMessage::getResourceId() const {
    return resourceId;
}

const symbol &symbolMessage::getSym() const {
    return sym;
}

void symbolMessage::invokeMethod(SymServer &server) {
    //TODO: implement
}

void symbolMessage::invokeMethod(SymClient &client) {
    //TODO: implement
}

symbolMessage & symbolMessage::verifySym() {
    sym.setVerified();
    return *this;
}

void symbolMessage::completeAction(SymClient &client, msgOutcome serverResult) {
    //TODO: implement
    clientMessage::completeAction(client, msgOutcome::success);
}

uriMessage::uriMessage(msgType action, const std::pair<std::string, std::string> &actionOwner, msgOutcome result,
                       const std::string &path, const std::string &name, const uri &sharingPrefs, int msgId)
                       : message(msgId), clientMessage(actionOwner, msgId),
                         serverMessage(result, msgId), sharingPrefs(sharingPrefs) {
    if(action!=msgType::shareRes)
        throw messageException("The action is not consistent with the message type");

    this->action=action;
    this->path=path;
    this->name=name;
}

const uri &uriMessage::getSharingPrefs() const {
    return sharingPrefs;
}

void uriMessage::invokeMethod(SymServer &server) {
    //TODO: implement
    server.shareResource(getActionOwner().first,path,name,sharingPrefs);

}

void uriMessage::invokeMethod(SymClient &client) {
    client.shareResource(path,name,sharingPrefs);
    auto msg= client.retrieveRelatedMessage(*this);
}

void uriMessage::completeAction(SymClient &client, msgOutcome serverResult) {
    //TODO: implement
}

updateActiveMessage::updateActiveMessage(msgType action, msgOutcome result, const user &newUser, int resourceId,
                                         privilege priv,
                                         int msgId)
                                         : message(msgId), serverMessage(result, msgId),
                                           newUser(newUser), resourceId(resourceId), userPrivilege(priv) {
    this->action=action;
    if(action!=msgType::addActiveUser && action!=msgType::removeActiveUser)
        throw messageException("The action is not consistent with the message type");
}

const user &updateActiveMessage::getNewUser() const {
    return newUser;
}

int updateActiveMessage::getResourceId() const {
    return resourceId;
}

privilege updateActiveMessage::getUserPrivilege() const {
    return userPrivilege;
}

void updateActiveMessage::invokeMethod(SymClient &client) {
    switch(action)
    {
        case msgType::addActiveUser:{
            client.addActiveUser(resourceId,newUser);
            break;
        }
        case msgType::removeActiveUser:{
            client.removeActiveUser(resourceId,newUser);
            break;
        }
        default:
            throw messageException("This is not a valid message");
    }

}

updateDocMessage::updateDocMessage(msgType action, const std::pair<std::string, std::string> &actionOwner,
                                   int resourceId, int msgId)
                                 : message(msgId), clientMessage(actionOwner, msgId),
                                   resourceId(resourceId) {
    if(action!=msgType::mapChangesToUser && action!=msgType::closeRes)
        throw messageException("The action is not consistent with the message type");
    this->action=action;
}

int updateDocMessage::getResourceId() const {
    return resourceId;
}

void updateDocMessage::invokeMethod(SymServer &server) {
    if(action==msgType::closeRes){
        server.closeSource(getActionOwner().first,resourceId);
    }
    else if(action==msgType::mapChangesToUser){
        server.mapSiteIdToUser(getActionOwner().first,resourceId);
    }
    else
        throw messageException("This is not a valid message");
}

userDataMessage::userDataMessage(msgType action, const std::pair<std::string, std::string> &actionOwner,
                                 msgOutcome result,
                                 const user &newUserData, int msgId)
                                 : message(msgId), clientMessage(actionOwner, msgId),
                                   serverMessage(action, result, msgId), newUserData(newUserData) {
   if(action!=msgType::changeUserData && action !=msgType::changeUserPwd)
       throw messageException("The action is not consistent with the message type");
}

void userDataMessage::invokeMethod(SymServer &server) {
    if(action==msgType::changeUserData){
        server.editUser(getActionOwner().first,newUserData);
    }
    else
        throw messageException("This is not a valid message");
}

void userDataMessage::invokeMethod(SymClient &client) {
    if(action==msgType::changeUserData){
        client.editUser(newUserData, false);
    }
    else
        throw messageException("This is not a valid message");
}

void userDataMessage::completeAction(SymClient &client, msgOutcome serverResult) {
    if(action==msgType::changeUserData)
        client.editUser(newUserData, true);
    //FIXME: caso failure?
}

