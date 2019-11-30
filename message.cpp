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
message::message(msgType action, int msgId): action{action}, msgId{msgId} {
    //TODO: implement
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
        : message(action, msgId), actionOwner(actionOwner) {
    //TODO: implement
}

const std::pair<std::string, std::string> &clientMessage::getActionOwner() const {
    return actionOwner;
}

void clientMessage::invokeMethod(SymServer &server) {
    //TODO: implement

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

askResMessage::askResMessage(msgType action, const std::pair<std::string, std::string> &actionOwner,
                             const std::string &path,
                             const std::string &name, const std::string &resourceId, privilege accessMode, int msgId)
        : message(action, msgId), clientMessage(action, actionOwner, msgId), path(path), name(name),
          resourceId(resourceId) {
    //TODO: implement
}

void askResMessage::invokeMethod(SymServer &server) {
    //TODO: implement
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
                             : message(action, msgId), clientMessage(action, actionOwner, msgId), newUser(newUser) {
    //TODO:implement
}

void signUpMessage::invokeMethod(SymServer &ss) {
    //TODO: implement
}

const user &signUpMessage::getNewUser() const {
    return newUser;
}

serverMessage::serverMessage(msgType action, msgOutcome result, int msgId) : message(action, msgId), result(result) {
    //TODO: implement
}

msgOutcome serverMessage::getResult() const {
    return result;
}

void serverMessage::invokeMethod(SymClient &client) {
    //TODO: implement
}

bool serverMessage::isRelatedTo(const clientMessage &other) {
    //TODO: implement
    return false;
}

loginMessage::loginMessage(msgType action, msgOutcome result, const user &loggedUser, int msgId)
                           : message(action, msgId), serverMessage(action, result, msgId), loggedUser(loggedUser) {
    //TODO: implement
}

const user &loginMessage::getLoggedUser() const {
    return loggedUser;
}

void loginMessage::invokeMethod(SymClient &client) {
    //TODO: implement
}

mapMessage::mapMessage(msgType action, msgOutcome result, const std::map<int, user> &siteIdToUser, int msgId)
        : message(action, msgId), serverMessage(action, result, msgId), siteIdToUser(siteIdToUser) {
    //TODO: implement
}

const std::map<int, user> & mapMessage::getSiteIdToUser() const {
    return siteIdToUser;
}

void mapMessage::invokeMethod(SymClient &client) {
    //TODO: implement
}

sendResMessage::sendResMessage(msgType action, msgOutcome result, filesystem &resource, int msgId)
        : message(action, msgId), serverMessage(action, result, msgId), resource{resource} {
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
                         : message(action, msgId), clientMessage(action, actionOwner, msgId),
                           serverMessage(action, result, msgId), resourceId(resourceId), targetUser(targetUser), newPrivilege(newPrivilege) {
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
                             : message(action, msgId), clientMessage(action, actionOwner, msgId),
                               serverMessage(action, result, msgId), siteId(siteId), resourceId(resourceId), sym(sym) {
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
                       const uri &sharingPrefs, int msgId)
                       : message(action, msgId), clientMessage(action, actionOwner, msgId),
                         serverMessage(action, result, msgId), sharingPrefs(sharingPrefs) {
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
                                         : message(action, msgId), serverMessage(action, result, msgId),
                                           newUser(newUser), resourceId(resourceId), userPrivilege(priv) {
    //TODO: implement
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
    //TODO: implement
    serverMessage::invokeMethod(client);
}

updateDocMessage::updateDocMessage(msgType action, const std::pair<std::string, std::string> &actionOwner,
                                   int resourceId, int msgId)
                                 : message(action, msgId), clientMessage(action, actionOwner, msgId),
                                   resourceId(resourceId) {
    //TODO: implement
}

int updateDocMessage::getResourceId() const {
    //TODO: implement
    return resourceId;
}

void updateDocMessage::invokeMethod(SymServer &server) {
    //TODO: implement
    clientMessage::invokeMethod(server);
}

userDataMessage::userDataMessage(msgType action, const std::pair<std::string, std::string> &actionOwner,
                                 msgOutcome result,
                                 const user &newUserData, int msgId)
                                 : message(action, msgId), clientMessage(action, actionOwner, msgId),
                                   serverMessage(action, result, msgId), newUserData(newUserData) {
    //TODO: implement
}

void userDataMessage::invokeMethod(SymServer &server) {
    clientMessage::invokeMethod(server);
}

void userDataMessage::invokeMethod(SymClient &client) {
    serverMessage::invokeMethod(client);
}
