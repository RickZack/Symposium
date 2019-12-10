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
            server.logout(actionOwner.first, actionOwner.second);
            break;
        }
        default:
            throw messageException("This is not a client message");
    }

}

void clientMessage::completeAction(SymClient &client) {
    //TODO: implement
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
            //server.renameResource()
            break;
        }
        case msgType::createNewDir:{
            server.createNewDir(getActionOwner().first,path,name);
            break;
        }
        case msgType::removeRes:{
            //server.removeResource()
            break;
        }
        default:
            throw messageException("This is not a valid message");
    }

}

void askResMessage::completeAction(SymClient &client) {
    clientMessage::completeAction(client);
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
    //TODO: implement
}

serverMessage::serverMessage(msgOutcome result, int msgId) : message(msgId), result(result) {
    //TODO: implement
}

void serverMessage::setResult(msgOutcome outcome) {
    result=outcome;
}

void serverMessage::invokeMethod(SymClient &client) {
    //TODO: implement
}

bool serverMessage::isRelatedTo(const clientMessage &other) {
    //TODO: implement
    return false;
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
    client.setUserColors(siteIdToUser);
}

sendResMessage::sendResMessage(msgType action, msgOutcome result, filesystem &resource, int symId, int msgId)
        : message(msgId), serverMessage(result, msgId), resource{resource} {
    //TODO: implement
}

const filesystem &sendResMessage::getResource() const {
    return resource;
}

void sendResMessage::invokeMethod(SymClient &client) {
    //TODO: implement
}

privMessage::privMessage(msgType action, const std::pair<std::string, std::string> &actionOwner, msgOutcome result,
                         const std::string &resourceId, const std::string &targetUser, privilege newPrivilege,
                         int msgId)
                         : message(msgId), clientMessage(actionOwner, msgId),
                           serverMessage(result, msgId), resourceId(resourceId), targetUser(targetUser), newPrivilege(newPrivilege) {
    //TODO:implement
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
}

void privMessage::invokeMethod(SymClient &client) {
    //TODO: implement
}

void privMessage::completeAction(SymClient &client) {
    //TODO: implement
}

symbolMessage::symbolMessage(msgType action, const std::pair<std::string, std::string> &actionOwner, msgOutcome result,
                             int siteId,
                             int resourceId, const symbol &sym, int msgId)
                             : message(msgId), clientMessage(actionOwner, msgId),
                               serverMessage(result, msgId), siteId(siteId), resourceId(resourceId), sym(sym) {
    //TODO:implement
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

void symbolMessage::completeAction(SymClient &client) {
    //TODO: implement
    clientMessage::completeAction(client);
}

uriMessage::uriMessage(msgType action, const std::pair<std::string, std::string> &actionOwner, msgOutcome result,
                       const std::string &path, const std::string &name, const uri &sharingPrefs, int msgId)
                       : message(msgId), clientMessage(actionOwner, msgId),
                         serverMessage(result, msgId), sharingPrefs(sharingPrefs) {
    //TODO:implement
}

const uri &uriMessage::getSharingPrefs() const {
    return sharingPrefs;
}

void uriMessage::invokeMethod(SymServer &server) {
    //TODO: implement
}

void uriMessage::invokeMethod(SymClient &client) {
    //TODO: implement
}

void uriMessage::completeAction(SymClient &client) {
    //TODO: implement
}

updateActiveMessage::updateActiveMessage(msgType action, msgOutcome result, const user &newUser, int resourceId,
                                         privilege priv,
                                         int msgId)
                                         : message(msgId), serverMessage(result, msgId),
                                           newUser(newUser), resourceId(resourceId), userPrivilege(priv) {
    if(action!=msgType::openRes|| action!=msgType::openNewRes|| action!=msgType::closeRes)
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
    serverMessage::invokeMethod(client);
}

updateDocMessage::updateDocMessage(msgType action, const std::pair<std::string, std::string> &actionOwner,
                                   int resourceId, int msgId)
                                 : message(msgId), clientMessage(actionOwner, msgId),
                                   resourceId(resourceId) {
    if(action!=msgType::closeRes)
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
    if(action==msgType::mapChangesToUser){
        server.mapSiteIdToUser(getActionOwner().first,resourceId);
    }
    clientMessage::invokeMethod(server);
}

userDataMessage::userDataMessage(msgType action, const std::pair<std::string, std::string> &actionOwner,
                                 msgOutcome result,
                                 const user &newUserData, int msgId)
                                 : message(msgId), clientMessage(actionOwner, msgId),
                                   serverMessage(action, result, msgId), newUserData(newUserData) {
    //TODO: implement
}

void userDataMessage::invokeMethod(SymServer &server) {
    clientMessage::invokeMethod(server);
}

void userDataMessage::invokeMethod(SymClient &client) {
    serverMessage::invokeMethod(client);
}
