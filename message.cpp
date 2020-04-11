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
#include "filesystem.h"
#include "message.h"
#include "SymServer.h"
#include "SymClient.h"
#include "resourceType.h"


#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>

using namespace Symposium;



//FIXME: put unsigned, siteIds must be positive, because a negative id is assumed as "no user present"
uint_positive_cnt message::msgCounter;

message::message(uint_positive_cnt::type msgId){
    if(msgId==0)
    {
        this->msgId=msgCounter;
        msgCounter++;
    }
    else
        this->msgId=msgId;
}

template<class Archive>
void message::serialize(Archive &ar, const unsigned int version)
{
    ar & msgId & action;
}

uint_positive_cnt::type message::getMsgId() const {
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

clientMessage::clientMessage(msgType action, const std::pair<std::string, std::string> &actionOwner, uint_positive_cnt::type msgId)
        : message(msgId), actionOwner(actionOwner) {
    if(action!=msgType::login && action!=msgType::removeUser && action!=msgType::logout)
        throw messageException(messageException::action, UnpackFileLineFunction());
    this->action=action;
}

clientMessage::clientMessage(const std::pair<std::string, std::string> &actionOwner, uint_positive_cnt::type msgId)
        : message(msgId), actionOwner(actionOwner) {
}

template<class Archive>
void clientMessage::serialize(Archive &ar, const unsigned int version)
{
    // save/load base class information
    ar & boost::serialization::base_object<Symposium::message>(*this);
    ar & actionOwner;
}

const std::pair<std::string, std::string> &clientMessage::getActionOwner() const {
    return actionOwner;
}

void clientMessage::invokeMethod(SymServer &server) {
    switch(action)
    {
        case msgType::login:{
            user us= server.login(actionOwner.first, actionOwner.second, msgId);
            break;
        }
        case msgType::removeUser:{
            server.removeUser(actionOwner.first, actionOwner.second, msgId);
            break;
        }
        case msgType::logout:{
            server.logout(actionOwner.first, msgId);
            break;
        }
        default:
            break;
    }

}

void clientMessage::completeAction(SymClient &client, msgOutcome serverResult) {
    if(serverResult==msgOutcome::failure)
        throw messageException(messageException::notSucc, UnpackFileLineFunction());
    if(action==msgType::logout)
    {
        client.logout(true);
    }
    if(action==msgType::removeUser)
    {
        client.removeUser(true);
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
BOOST_CLASS_EXPORT(Symposium::clientMessage)

askResMessage::askResMessage(msgType action, const std::pair<std::string, std::string> &actionOwner,
                             const std::string &path,
                             const std::string &name, const std::string &resourceId, privilege accessMode, uint_positive_cnt::type msgId)
        : message(msgId), clientMessage( actionOwner, msgId), path(path), name(name),
          resourceId(resourceId), accessMode(accessMode) {
    if(action!=msgType::createRes && action!= msgType::openRes && action!=msgType::openNewRes&& action!=msgType::changeResName
    && action!=msgType::createNewDir&& action!=msgType::removeRes)
        throw messageException(messageException::action, UnpackFileLineFunction());

    this->action=action;
}

template<class Archive>
void askResMessage::serialize(Archive &ar, const unsigned int version)
{
    // save/load base class information
    ar & boost::serialization::base_object<Symposium::clientMessage>(*this);
    ar & path & name & resourceId & accessMode;
}

void askResMessage::invokeMethod(SymServer &server) {
    switch(action)
    {
        case msgType::createRes:{
            server.createNewSource(getActionOwner().first, path, name, msgId);
          break;
        }
        case msgType::openRes:{
            server.openSource(getActionOwner().first, path, name, accessMode, msgId);
            break;
        }
        case msgType::openNewRes:{
            server.openNewSource(getActionOwner().first, resourceId, path, name, accessMode, msgId);
            break;
        }
        case msgType::changeResName:{
            server.renameResource(getActionOwner().first, path, name, resourceId, msgId);
            break;
        }
        case msgType::createNewDir:{
            server.createNewDir(getActionOwner().first, path, name, msgId);
            break;
        }
        case msgType::removeRes:{
            server.removeResource(getActionOwner().first, path, name, msgId);
            break;
        }
        default:
            break;
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
    if(serverResult==msgOutcome::failure)
        throw messageException(messageException::notSucc, UnpackFileLineFunction());
    switch(action)
    {
        case msgType::changeResName:{
            client.renameResource(path, name, resourceId, true);
            break;
        }
        case msgType::removeRes:{
            client.removeResource(path, name, true);
            break;
        }
        default:
            break;
    }
}
BOOST_CLASS_EXPORT(Symposium::askResMessage)

signUpMessage::signUpMessage(msgType action, const std::pair<std::string, std::string> &actionOwner,
                            const user &newUser,
                             uint_positive_cnt::type msgId)
                             : message(msgId), clientMessage(actionOwner, msgId), newUser(newUser) {
    if(action!=msgType::registration)
        throw messageException(messageException::action, UnpackFileLineFunction());
    this->action=action;
}

template<class Archive>
void signUpMessage::serialize(Archive &ar, const unsigned int version)
{
    // save/load base class information
    ar & boost::serialization::base_object<Symposium::clientMessage>(*this);
    ar & newUser;
}

void signUpMessage::invokeMethod(SymServer &ss) {
    ss.addUser(newUser, msgId);
}

const user &signUpMessage::getNewUser() const {
    return newUser;
}

bool signUpMessage::operator==(const signUpMessage &rhs) const {
    return static_cast<const Symposium::clientMessage &>(*this) == static_cast<const Symposium::clientMessage &>(rhs) &&
           newUser == rhs.newUser;
}

bool signUpMessage::operator!=(const signUpMessage &rhs) const {
    return !(rhs == *this);
}

BOOST_CLASS_EXPORT(Symposium::signUpMessage)

serverMessage::serverMessage(msgType action, msgOutcome result, uint_positive_cnt::type msgId) : message(msgId), result(result) {
    this->action=action;
}

serverMessage::serverMessage(msgOutcome result, uint_positive_cnt::type msgId) : message(msgId), result(result) {
}

template<class Archive>
void serverMessage::serialize(Archive &ar, const unsigned int version)
{
    // save/load base class information
    ar & boost::serialization::base_object<Symposium::message>(*this);
    ar & result & errDescr;
}

void serverMessage::invokeMethod(SymClient &client) {
    auto mex=client.retrieveRelatedMessage(*this);
    mex->completeAction(client, result);
}

bool serverMessage::isRelatedTo(const clientMessage &other) const {
    return other.getMsgId() == this->msgId && other.getAction() == this->action;
}

msgOutcome serverMessage::getResult() const {
    return result;
}

bool serverMessage::operator==(const serverMessage &rhs) const {
    return static_cast<const Symposium::message &>(*this) == static_cast<const Symposium::message &>(rhs) &&
           result == rhs.result;
}

bool serverMessage::operator!=(const serverMessage &rhs) const {
    return !(rhs == *this);
}

const std::string &serverMessage::getErrDescr() const {
    return errDescr;
}

void serverMessage::setErrDescr(const std::string &errDescr) {
    serverMessage::errDescr = errDescr;
}

BOOST_CLASS_EXPORT(Symposium::serverMessage)

loginMessage::loginMessage(msgType action, msgOutcome result, const user &loggedUser, uint_positive_cnt::type msgId)
                           : message(msgId), serverMessage(result, msgId), loggedUser(loggedUser) {
    if(action!=msgType::login && action!=msgType::registration)
        throw messageException(messageException::action, UnpackFileLineFunction());
    this->action=action;
}

template<class Archive>
void loginMessage::serialize(Archive &ar, const unsigned int version)
{
    // save/load base class information
    ar & boost::serialization::base_object<Symposium::serverMessage>(*this);
    ar & loggedUser;
}

const user &loginMessage::getLoggedUser() const {
    return loggedUser;
}

void loginMessage::invokeMethod(SymClient &client) {
    auto msg= client.retrieveRelatedMessage(*this);
    client.logIn(loggedUser);
}

bool loginMessage::operator==(const loginMessage &rhs) const {
    return static_cast<const Symposium::serverMessage &>(*this) == static_cast<const Symposium::serverMessage &>(rhs) &&
           loggedUser == rhs.loggedUser;
}

bool loginMessage::operator!=(const loginMessage &rhs) const {
    return !(rhs == *this);
}

BOOST_CLASS_EXPORT(Symposium::loginMessage)

mapMessage::mapMessage(msgType action, msgOutcome result, const std::map<uint_positive_cnt::type, user> &siteIdToUser, uint_positive_cnt::type msgId)
        : message(msgId), serverMessage(result, msgId), siteIdToUser(siteIdToUser) {
    if(action!=msgType::mapChangesToUser)
        throw messageException(messageException::action, UnpackFileLineFunction());
    this->action=action;
}

template<class Archive>
void mapMessage::serialize(Archive &ar, const unsigned int version)
{
    // save/load base class information
    ar & boost::serialization::base_object<Symposium::serverMessage>(*this);
    ar & siteIdToUser;
}

const std::map<uint_positive_cnt::type, user> & mapMessage::getSiteIdToUser() const {
    return siteIdToUser;
}

void mapMessage::invokeMethod(SymClient &client) {
    auto msg= client.retrieveRelatedMessage(*this);
    std::shared_ptr<updateDocMessage> mex2=std::dynamic_pointer_cast<updateDocMessage>(msg);
    client.setUserColors(mex2->getResourceId(), siteIdToUser);
}

bool mapMessage::operator==(const mapMessage &rhs) const {
    return static_cast<const Symposium::serverMessage &>(*this) == static_cast<const Symposium::serverMessage &>(rhs) &&
           siteIdToUser == rhs.siteIdToUser;
}

bool mapMessage::operator!=(const mapMessage &rhs) const {
    return !(rhs == *this);
}

BOOST_CLASS_EXPORT(Symposium::mapMessage)

sendResMessage::sendResMessage(msgType action, msgOutcome result, std::shared_ptr<filesystem> resource, uint_positive_cnt::type symId, uint_positive_cnt::type msgId)
        : message(msgId), serverMessage(result, msgId), resource{resource} {
    if(action!=msgType::createRes && action!=msgType::createNewDir && action!=msgType::openNewRes
    && action!=msgType::openRes)
        throw messageException(messageException::action, UnpackFileLineFunction());
    this->action=action;
    if(symId!=0)
        this->symId=symId;
}

template<class Archive>
void sendResMessage::serialize(Archive &ar, const unsigned int version)
{
    // save/load base class information
    ar & boost::serialization::base_object<Symposium::serverMessage>(*this);
    ar & symId & resource;
}

std::shared_ptr<filesystem> sendResMessage::getResource() const {
    return resource;
}

void sendResMessage::invokeMethod(SymClient &client) {
    auto mex=client.retrieveRelatedMessage(*this);
    std::shared_ptr <askResMessage> mex2=std::dynamic_pointer_cast<askResMessage>(mex);
    switch(mex->getAction())
    {
        case msgType::createRes:{
            client.createNewSource(mex2->getPath(), mex2->getName(), resource->getId());
            break;
        }
        case msgType::openRes:{
            resourceType type=resource->resType();
            if(type==resourceType::file)
            {
                std::shared_ptr <file> f= std::dynamic_pointer_cast<file>(resource);
                client.openSource(mex2->getPath(), mex2->getName(), f,
                                  resource->getUserPrivilege(mex2->getActionOwner().first));
            }
            break;
        }
        case msgType::openNewRes:{
            resourceType type=resource->resType();
            if(type==resourceType::file)
            {
                std::shared_ptr <file> f= std::dynamic_pointer_cast<file>(resource);
                client.openNewSource(mex2->getResourceId(), resource->getUserPrivilege(mex2->getActionOwner().first),
                        mex2->getPath(), mex2->getName(), resource->getId(), f);
            }
           break;
        }
        case msgType::createNewDir:{
            client.createNewDir(mex2->getPath(), mex2->getName(), resource->getId());
            break;
        }
        default:
            break;
    }
}
BOOST_CLASS_EXPORT(Symposium::sendResMessage)

privMessage::privMessage(msgType action, const std::pair<std::string, std::string> &actionOwner, msgOutcome result,
                         const std::string &resourceId, const std::string &targetUser, privilege newPrivilege,
                         uint_positive_cnt::type msgId)
                         : message(msgId), clientMessage(actionOwner, msgId),
                           serverMessage(result, msgId), resourceId(resourceId), targetUser(targetUser), newPrivilege(newPrivilege) {
    if(action!=msgType::changePrivileges)
        throw messageException(messageException::action, UnpackFileLineFunction());
    this->action=action;
}

template<class Archive>
void privMessage::serialize(Archive &ar, const unsigned int version)
{
    // save/load base class information
    ar & boost::serialization::base_object<Symposium::clientMessage>(*this);
    ar & boost::serialization::base_object<Symposium::serverMessage>(*this);
    ar & resourceId & targetUser & newPrivilege;
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
    std::string path1;
    std::string name1;
    tie(path1, name1) = directory::separateFirst(resourceId);

    std::string path2;
    std::string nameRes;
    tie(path2, nameRes) = filesystem::separate(path1);

    std::string pathRes="./";
    pathRes.append(path2);

    server.editPrivilege(getActionOwner().first, targetUser, pathRes, nameRes, newPrivilege, msgId);


}
void privMessage::invokeMethod(SymClient &client) {

    std::string path1;
    std::string name1;
    tie(path1, name1) = directory::separateFirst(resourceId);

    std::string path2;
    std::string nameRes;
    tie(path2, nameRes) = filesystem::separate(path1);

    std::string pathRes="./";
    pathRes.append(path2);

    client.editPrivilege(targetUser,pathRes,nameRes,newPrivilege,false);


}

void privMessage::completeAction(SymClient &client, msgOutcome serverResult) {
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
        throw messageException(messageException::notSucc, UnpackFileLineFunction());

}

bool privMessage::operator==(const privMessage &rhs) const {
    return static_cast<const Symposium::clientMessage &>(*this) == static_cast<const Symposium::clientMessage &>(rhs) &&
           static_cast<const Symposium::serverMessage &>(*this) == static_cast<const Symposium::serverMessage &>(rhs) &&
           resourceId == rhs.resourceId &&
           targetUser == rhs.targetUser &&
           newPrivilege == rhs.newPrivilege;
}

bool privMessage::operator!=(const privMessage &rhs) const {
    return !(rhs == *this);
}

BOOST_CLASS_EXPORT(Symposium::privMessage)

symbolMessage::symbolMessage(msgType action, const std::pair<std::string, std::string> &actionOwner, msgOutcome result,
                             uint_positive_cnt::type siteId,
                             uint_positive_cnt::type resourceId, const symbol &sym, uint_positive_cnt::type msgId)
                             : message(msgId), clientMessage(actionOwner, msgId),
                               serverMessage(result, msgId), siteId(siteId), resourceId(resourceId), sym(sym) {
    if(action!=msgType::insertSymbol && action!=msgType::removeSymbol)
        throw messageException(messageException::action, UnpackFileLineFunction());
    this->action=action;
}

template<class Archive>
void symbolMessage::serialize(Archive &ar, const unsigned int version)
{
    // save/load base class information
    ar & boost::serialization::base_object<Symposium::clientMessage>(*this);
    ar & boost::serialization::base_object<Symposium::serverMessage>(*this);
    ar & siteId & resourceId & sym;
}

uint_positive_cnt::type symbolMessage::getSiteId() const {
    return siteId;
}

uint_positive_cnt::type symbolMessage::getResourceId() const {
    return resourceId;
}

const symbol &symbolMessage::getSym() const {
    return sym;
}

void symbolMessage::invokeMethod(SymServer &server) {
    switch(action)
    {
        case msgType::insertSymbol:{
           server.remoteInsert(getActionOwner().first,resourceId,*this);
            break;
        }
        case msgType::removeSymbol:{
            server.remoteRemove(getActionOwner().first,resourceId,*this);
            break;
        }
        default:
            break;
    }

}

void symbolMessage::invokeMethod(SymClient &client) {
    switch(action)
    {
        case msgType::insertSymbol:{
            client.remoteInsert(siteId, resourceId, sym);
            break;
        }
        case msgType::removeSymbol:{
            client.remoteRemove(siteId, resourceId, sym);
            break;
        }
        default:
            break;
    }

}

symbolMessage & symbolMessage::verifySym() {
    sym.setVerified();
    return *this;
}

void symbolMessage::completeAction(SymClient &client, msgOutcome serverResult) {
    if(serverResult==msgOutcome::success){
        if(action==msgType::insertSymbol)
        client.verifySymbol(resourceId,sym);
    }
   else{
        if(action==msgType::insertSymbol)
            client.remoteRemove(siteId, resourceId, sym);
        else if(action==msgType::removeSymbol)
            client.remoteInsert(siteId, resourceId, sym);
        else
            throw messageException(messageException::notSucc, UnpackFileLineFunction());
    }

}

bool symbolMessage::operator==(const symbolMessage &rhs) const {
    return static_cast<const Symposium::clientMessage &>(*this) == static_cast<const Symposium::clientMessage &>(rhs) &&
           static_cast<const Symposium::serverMessage &>(*this) == static_cast<const Symposium::serverMessage &>(rhs) &&
           siteId == rhs.siteId &&
           resourceId == rhs.resourceId &&
           sym == rhs.sym;
}

bool symbolMessage::operator!=(const symbolMessage &rhs) const {
    return !(rhs == *this);
}
BOOST_CLASS_EXPORT(Symposium::symbolMessage)

uriMessage::uriMessage(msgType action, const std::pair<std::string, std::string> &actionOwner, msgOutcome result,
                       const std::string &path, const std::string &name, const uri &sharingPrefs, uint_positive_cnt::type msgId)
                       : message(msgId), clientMessage(actionOwner, msgId),
                         serverMessage(result, msgId), sharingPrefs(sharingPrefs) {
    if(action!=msgType::shareRes)
        throw messageException(messageException::action, UnpackFileLineFunction());

    this->action=action;
    this->path=path;
    this->name=name;
}

template<class Archive>
void uriMessage::serialize(Archive &ar, const unsigned int version)
{
    // save/load base class information
    ar & boost::serialization::base_object<Symposium::clientMessage>(*this);
    ar & boost::serialization::base_object<Symposium::serverMessage>(*this);
    ar & path & name & sharingPrefs;
}

const uri &uriMessage::getSharingPrefs() const {
    return sharingPrefs;
}

void uriMessage::invokeMethod(SymServer &server) {
    server.shareResource(getActionOwner().first, path, name, sharingPrefs, msgId);

}

void uriMessage::invokeMethod(SymClient &client) {
    client.shareResource(path,name,sharingPrefs,false);
}


void uriMessage::completeAction(SymClient &client, msgOutcome serverResult) {
    if(serverResult==msgOutcome::success)
        client.shareResource(path,name,sharingPrefs,true);
    else
        throw messageException(messageException::notSucc, UnpackFileLineFunction());
}

bool uriMessage::operator==(const uriMessage &rhs) const {
    return static_cast<const Symposium::clientMessage &>(*this) == static_cast<const Symposium::clientMessage &>(rhs) &&
           static_cast<const Symposium::serverMessage &>(*this) == static_cast<const Symposium::serverMessage &>(rhs) &&
           path == rhs.path &&
           name == rhs.name &&
           sharingPrefs == rhs.sharingPrefs;
}

bool uriMessage::operator!=(const uriMessage &rhs) const {
    return !(rhs == *this);
}
BOOST_CLASS_EXPORT(Symposium::uriMessage)

updateActiveMessage::updateActiveMessage(msgType action, msgOutcome result, const user &newUser, uint_positive_cnt::type resourceId,
                                         privilege priv,
                                         uint_positive_cnt::type msgId)
                                         : message(msgId), serverMessage(result, msgId),
                                           newUser(newUser), resourceId(resourceId), userPrivilege(priv) {
    this->action=action;
    if(action!=msgType::addActiveUser && action!=msgType::removeActiveUser)
        throw messageException(messageException::action, UnpackFileLineFunction());
}

template<class Archive>
void updateActiveMessage::serialize(Archive &ar, const unsigned int version)
{
    // save/load base class information
    ar & boost::serialization::base_object<Symposium::serverMessage>(*this);
    ar & newUser & resourceId & userPrivilege;
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
            client.addActiveUser(resourceId,newUser,userPrivilege);
            break;
        }
        case msgType::removeActiveUser:{
            client.removeActiveUser(resourceId,newUser);
            break;
        }
        default:
            break;
    }

}

bool updateActiveMessage::operator==(const updateActiveMessage &rhs) const {
    return static_cast<const Symposium::serverMessage &>(*this) == static_cast<const Symposium::serverMessage &>(rhs) &&
           newUser == rhs.newUser &&
           resourceId == rhs.resourceId &&
           userPrivilege == rhs.userPrivilege;
}

bool updateActiveMessage::operator!=(const updateActiveMessage &rhs) const {
    return !(rhs == *this);
}

BOOST_CLASS_EXPORT(Symposium::updateActiveMessage)

updateDocMessage::updateDocMessage(msgType action, const std::pair<std::string, std::string> &actionOwner,
                                   uint_positive_cnt::type resourceId, uint_positive_cnt::type msgId)
                                 : message(msgId), clientMessage(actionOwner, msgId),
                                   resourceId(resourceId) {
    if(action!=msgType::mapChangesToUser && action!=msgType::closeRes)
        throw messageException(messageException::action, UnpackFileLineFunction());
    this->action=action;
}

template<class Archive>
void updateDocMessage::serialize(Archive &ar, const unsigned int version)
{
    // save/load base class information
    ar & boost::serialization::base_object<Symposium::clientMessage>(*this);
    ar & resourceId;
}

uint_positive_cnt::type updateDocMessage::getResourceId() const {
    return resourceId;
}

void updateDocMessage::invokeMethod(SymServer &server) {

    switch(action)
    {
        case msgType::closeRes:{
            server.closeSource(getActionOwner().first, resourceId, msgId);
            break;
        }
        case msgType::mapChangesToUser:{
            server.mapSiteIdToUser(getActionOwner().first, resourceId, msgId);
            break;
        }
        default:
            break;
    }
}

bool updateDocMessage::operator==(const updateDocMessage &rhs) const {
    return static_cast<const Symposium::clientMessage &>(*this) == static_cast<const Symposium::clientMessage &>(rhs) &&
           resourceId == rhs.resourceId;
}

bool updateDocMessage::operator!=(const updateDocMessage &rhs) const {
    return !(rhs == *this);
}

BOOST_CLASS_EXPORT(Symposium::updateDocMessage)

userDataMessage::userDataMessage(msgType action, const std::pair<std::string, std::string> &actionOwner,
                                 msgOutcome result,
                                 const user &newUserData, uint_positive_cnt::type msgId)
                                 : message(msgId), clientMessage(actionOwner, msgId),
                                   serverMessage(action, result, msgId), newUserData(newUserData) {
   if(action!=msgType::changeUserData && action !=msgType::changeUserPwd)
       throw messageException(messageException::action, UnpackFileLineFunction());
}

template<class Archive>
void userDataMessage::serialize(Archive &ar, const unsigned int version)
{
    // save/load base class information
    ar & boost::serialization::base_object<Symposium::clientMessage>(*this);
    ar & boost::serialization::base_object<Symposium::serverMessage>(*this);
    ar & newUserData;
}

void userDataMessage::invokeMethod(SymServer &server) {
    server.editUser(getActionOwner().first, newUserData, msgId);
}

void userDataMessage::invokeMethod(SymClient &client) {
        client.editUser(newUserData, false);
}

void userDataMessage::completeAction(SymClient &client, msgOutcome serverResult) {
    if(serverResult==msgOutcome::failure)
        throw messageException(messageException::notSucc, UnpackFileLineFunction());
    client.editUser(newUserData, true);
}

bool userDataMessage::operator==(const userDataMessage &rhs) const {
    return static_cast<const Symposium::clientMessage &>(*this) == static_cast<const Symposium::clientMessage &>(rhs) &&
           static_cast<const Symposium::serverMessage &>(*this) == static_cast<const Symposium::serverMessage &>(rhs) &&
           newUserData == rhs.newUserData;
}

bool userDataMessage::operator!=(const userDataMessage &rhs) const {
    return !(rhs == *this);
}
BOOST_CLASS_EXPORT(Symposium::userDataMessage)

cursorMessage::cursorMessage(msgType action, const std::pair<std::string, std::string> &actionOwner, msgOutcome result,
                             uint_positive_cnt::type siteId,
                             uint_positive_cnt::type resourceId, int row, int col, uint_positive_cnt::type msgId) : message(msgId), clientMessage(actionOwner, msgId),
                                                                            serverMessage(result, msgId), siteId(siteId),
                                                                            resourceId(resourceId), row(row), col(col){
    if(action!=msgType::updateCursor)
        throw messageException(messageException::action, UnpackFileLineFunction());
    this->action=action;
}

template<class Archive>
void cursorMessage::serialize(Archive &ar, const unsigned int version)
{
    // save/load base class information
    ar & boost::serialization::base_object<Symposium::clientMessage>(*this);
    ar & boost::serialization::base_object<Symposium::serverMessage>(*this);
    ar & siteId & resourceId & row & col;
}

void cursorMessage::invokeMethod(SymServer &server) {

    server.updateCursorPos(getActionOwner().first, resourceId, *this);
}

void cursorMessage::invokeMethod(SymClient &client) {
    client.updateCursorPos(siteId, resourceId, row, col);
}

bool cursorMessage::operator==(const cursorMessage &rhs) const {
    return static_cast<const Symposium::clientMessage &>(*this) == static_cast<const Symposium::clientMessage &>(rhs) &&
           static_cast<const Symposium::serverMessage &>(*this) == static_cast<const Symposium::serverMessage &>(rhs) &&
           siteId == rhs.siteId &&
           resourceId == rhs.resourceId &&
           row == rhs.row &&
           col == rhs.col;
}

bool cursorMessage::operator!=(const cursorMessage &rhs) const {
    return !(rhs == *this);
}

uint_positive_cnt::type cursorMessage::getResourceId() const {
    return resourceId;
}

int cursorMessage::getRow() const {
    return row;
}

int cursorMessage::getCol() const {
    return col;
}

BOOST_CLASS_EXPORT(Symposium::cursorMessage)