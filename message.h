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
 * File:   message.h
 * Project: Symposium
 * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Alice Morano <s259158 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 16 Giugno 2019, 12.39
 */
#ifndef SYMPOSIUM_MESSAGE_H
#define SYMPOSIUM_MESSAGE_H

#include <string>
#include <map>
#include "Symposium.h"
#include "privilege.h"
#include "user.h"
#include "symbol.h"
#include "uri.h"
#include "messageData.h"
#include "resourceType.h"
#include "SymposiumException.h"


/**
 * @interface message message.h message
 * @brief class used as interface for a message
 */
 namespace Symposium {
     class message {
         friend class boost::serialization::access;
         template<class Archive>
         void serialize(Archive &ar, unsigned int);
     protected:
         static uint_positive_cnt msgCounter;
         uint_positive_cnt::type msgId;                 /**< random identifier for the message, used when a message is followed by an answer*/
         msgType action;                                /**< Defines the action for the current message */

         explicit message(uint_positive_cnt::type msgId= 0);
     public:
         uint_positive_cnt::type getMsgId() const;

         msgType getAction() const;

         bool isFinalMex() const;

         bool operator==(const message &rhs) const;

         bool operator!=(const message &rhs) const;

         virtual ~message() = default;
     };

    /**
     * @brief class used to model a message sent by a client
     *
     * A message sent by a client performs an action on the server that has received it. An object of this class is used to
     * log in the user indicated in @e actionOwner and to identify the user who asks for the @e action
     */
     class clientMessage : public virtual message {
         std::pair<std::string, std::string> actionOwner; /**< Defines the user (username, password) that has just performed the action */

         friend class boost::serialization::access;
         template<class Archive>
         void serialize(Archive &ar, const unsigned int);



     protected:
         //Needed by boost::serialization
         clientMessage():message(1){}
         explicit clientMessage(const std::pair<std::string, std::string> &actionOwner, uint_positive_cnt::type msgId = 0);
     public:

         /**
          * @throws messageException if @e action is not consistent with the message type
          */
         clientMessage(msgType action, const std::pair<std::string, std::string> &actionOwner, uint_positive_cnt::type msgId = 0);


         const std::pair<std::string, std::string> &getActionOwner() const;

         /**
          * @brief clear the authentication parameters of the the user who sent this message
          * @return the message itself
          *
          * This method is needed when the server has to forward the message to other clients, that
          * mustn't know the authentication parameters of the user who sent this message in the first place
          */
         clientMessage & clearAuthParam();

         /**
          * @brief make @e newUser an active user of the server the message is sent to
          * @param server the server to login to
          *
          * Depending on the value of @e action, the @e invokeMethod ask for different actions on the server:
          * @li <em> action=msgType::login </em>: calls @ref SymServer::login on @e server. A message of type
          * @ref loginMessage is sent back to the client, containing the @ref user object retrieved by the server.
          *
          * @li <em> action=msgType::removeUser </em>: calls @ref SymServer::removeUser on @e server. A message of type
          * @ref serverMessage is sent back to the client, containing the outcome of the action.
          *
          * @li <em> action=msgType::logout </em>: calls @ref SymServer::logout on @e server. A message of type
          * @ref serverMessage is sent back to the client, containing the outcome of the action.
          */
         virtual void invokeMethod(SymServer &server);

         /**
          * @brief completes an action for which the client asked to the server
          * @param client the same client that had originated the clientMessage
          *
          * Some actions on SymClient require to ask to the server to assure that the action is valid
          * and to propagate the change on the server. For such actions, only if the outcome from the
          * server is positive the action can be actually done.
          */
         virtual void completeAction(SymClient &client, msgOutcome serverResult);

         bool operator==(const clientMessage &rhs) const;

         bool operator!=(const clientMessage &rhs) const;

         ~clientMessage() override = default;
     };


    /**
     * @brief class used to model a message sent by a client asking for a resource
     *
     * An instance of this type is use to ask to server to create a resource in @e path with name @e name
     * or to remove a resource named @e name from @e path or to open a resource already known.
     * It is used also to rename an existing resource.
     * This depends on the @e action. If @e action is "openNewRes" then @e resourceId contains the path to the resource (the uri).
     * If @e action is "changeResName" then @e resourceId is the new file name.
     */
     class askResMessage : public clientMessage {

         friend class boost::serialization::access;
         template<class Archive>
         void serialize(Archive &ar, unsigned int);
         //Needed by boost::serialization
         askResMessage()=default;

         std::string path;          /**< path where to put the resource */
         std::string name;          /**< name to assign to the resource */
         std::string resourceId;    /**< when a sharing request is made, contains the path to the resource (the uri). */
         privilege accessMode;      /**< the privilege requested opening the resource */
     public:

         /**
          * @throws messageException if @e action is not consistent with the message type
          */
         askResMessage(msgType action, const std::pair<std::string, std::string> &actionOwner, const std::string &path,
                       const std::string &name, const std::string &resourceId = "", privilege accessMode=uri::getDefaultPrivilege(),
                       uint_positive_cnt::type msgId = 0);

         /**
          * @brief perform an action regarding a resource for the user @e actionOwner
          * @param server the server to whom the user is already registered and logged in
          *
          * Depending on the value of @e action, the @e invokeMethod ask for different actions on the server:
          * @li <em> action=msgType::createRes </em>: calls @ref SymServer::createNewSource on @e server. A message of type
          * @ref sendResMessage is sent back to the client, containing the @ref file object created by the server.
          *
          * @li <em> action=msgType::openRes </em>: calls @ref SymServer::openSource on @e server. A message of type
          * @ref sendResMessage is sent back to the client, containing the @ref file object stored by the server.
          *
          * @li <em> action=msgType::openNewRes </em>: calls @ref SymServer::openNewSource on @e server. A message of type
          * @ref sendResMessage is sent back to the client, containing the @ref symlink object created by the server.
          *
          * @li <em> action=msgType::changeResName </em>: calls @ref SymServer::renameResource on @e server. A message of type
          * @ref serveMessage is sent back to the client, to indicate whether the action succeeded or not.
          *
          * @li <em> action=msgType::createNewDir </em>: calls @ref SymServer::createNewDir on @e server. A message of type
          * @ref sendResMessage is sent back to the client, containing the @ref directory object created by the server.
          *
          * @li <em> action=msgType::removeRes </em>: calls @ref SymServer::removeResource on @e server. A message of type
          * @ref serveMessage is sent back to the client, to indicate whether the action succeeded or not.
          */
         void invokeMethod(SymServer &server) override;

         void completeAction(SymClient &client, msgOutcome serverResult) override;


         bool operator==(const askResMessage &rhs) const;
         bool operator!=(const askResMessage &rhs) const;

         const std::string &getPath() const;

         const std::string &getName() const;

         const std::string &getResourceId() const;

         ~askResMessage() override = default;
     };

    /**
     * @brief class used to model a sign up message sent by a client
     */
     class signUpMessage : public clientMessage {

         friend class boost::serialization::access;
         template<class Archive>
         void serialize(Archive &ar, unsigned int);
         signUpMessage()=default;

         user newUser;          /**< new SympUser data inserted by the user */
     public:

         /**
          * @throws messageException if @e action is not consistent with the message type
          */
         signUpMessage(msgType action, const std::pair<std::string, std::string> &actionOwner, const user &newUser,
                       uint_positive_cnt::type msgId = 0);

         const user &getNewUser() const;

         /**
          * @brief make @e newUser a registered user of the server the message is sent to
          * @param server the server to sign up to
          *
          * When this message is received by the server, the @e invokeMethod calls @ref SymServer::addUser on the @e server
          * passed as parameter.
          */
         void invokeMethod(SymServer &server) override;

         bool operator==(const signUpMessage &rhs) const;

         bool operator!=(const signUpMessage &rhs) const;

         ~signUpMessage() override{}
     };

    /**
     * @brief class used to model a message sent by a client to close a resource
     */
     class updateDocMessage : public clientMessage {
         friend class boost::serialization::access;
         template<class Archive>
         void serialize(Archive &ar, const unsigned int);
         //Needed by boost::serialization
         updateDocMessage()=default;

         uint_positive_cnt::type resourceId;            /**< identifier of the resource on which perform the action */
     public:

         /**
          * @throws messageException if @e action is not consistent with the message type
          */
         updateDocMessage(msgType action, const std::pair<std::string, std::string> &actionOwner, uint_positive_cnt::type resourceId,
                          uint_positive_cnt::type msgId = 0);

         uint_positive_cnt::type getResourceId() const;

         /**
          * @brief asks the server for an action on the document indicated by @e resourceId the @e actionUser is working on
          * @param server the server the user is active on
          *
          * Depending on the value of @e action, the @e invokeMethod ask for different actions on the server:
          * @li <em> action=msgType::closeRes </em>: calls @ref SymServer::closeSource on @e server.
          * @li <em> action=msgType::mapChangesToUser </em>: calls @ref SymServer::mapSiteIdToUser on @e server. A message of
          * type @ref mapMessage is sent back to the client
          */
         void invokeMethod(SymServer &server) override;

         bool operator==(const updateDocMessage &rhs) const;

         bool operator!=(const updateDocMessage &rhs) const;

         ~updateDocMessage() override = default;

     };

    /**
     * @brief class used to model a message sent by the server
     */
     class serverMessage : public virtual message {
         friend class boost::serialization::access;
         template<class Archive>
         void serialize(Archive &ar, unsigned int);
         serverMessage()=default;
     protected:
         //Needed by boost::serialization
         explicit serverMessage(msgOutcome result, uint_positive_cnt::type msgId = 0);

         msgOutcome result;         /**< result of an operation asked to the server */
         std::string errDescr;
     public:

         /**
          * @throws messageException if @e action is not consistent with the message type
          */
         serverMessage(msgType action, msgOutcome result, uint_positive_cnt::type msgId = 0);

         const std::string &getErrDescr() const;

         void setErrDescr(const std::string &errDescr);

         msgOutcome getResult() const;

         /**
          * @brief invoke an action on the SymClient given as parameter
          * @param client the client instance on which perform the action
          */
         virtual void invokeMethod(SymClient &client);

         /**
          * @brief indicates of a message from a @ref SymServer is related to a @ref SymClient 's one
          * @param other the message this message could be in response to
          * @return true if the current message is in response to the one passed as argument; false otherwise
          */
         bool isRelatedTo(const clientMessage &other) const;

         bool operator==(const serverMessage &rhs) const;

         bool operator!=(const serverMessage &rhs) const;

         ~serverMessage() override = default;
     };

    /**
     * @brief class used to model a message sent by a server as answer to a login message
     *
     * The client sends a @link clientMessage clientMessage @endlink object with the authentication parameters and receives
     * a @link loginMessage @endlink object with all the data of the logged user.
     * In this message <em> action=msgType::registration </em> or <em> action=msgType::login </em>
     */
     class loginMessage : public serverMessage {

         friend class boost::serialization::access;
         template<class Archive>
         void serialize(Archive &ar, unsigned int);
         //Needed by boost::serialization
         loginMessage():serverMessage(msgOutcome::success, 1){}

         user loggedUser;
     public:

         /**
          * @throws messageException if @e action is not consistent with the message type
          */
         loginMessage(msgType action, msgOutcome result, const user &loggedUser, uint_positive_cnt::type msgId = 0);

         /**
          * @brief enable the client have the same representation for an user after a login
          * @param client the client instance to fill with user's data
          *
          * When this message is received by the client, the @e invokeMethod calls @ref SymClient::setLoggedUser on the @e client
          * passed as parameter.
          */
         void invokeMethod(SymClient &client) override;

         bool operator==(const loginMessage &rhs) const;

         bool operator!=(const loginMessage &rhs) const;

         ~loginMessage() override = default;
     };

    /**
     * @brief class used to model an answer message sent by a server for a @link clientMessage clientMessage @endlink
     *
     * The server answers with the map siteId->user for the document the current user is working on.
     * The client ask for this answer with a @link updateDocMessage updateDocMessage @endlink with <em> action=msgType::mapChangesToUser </em>
     * In this message <em> action=msgType::mapChangesToUser </em>: calls
     */
     class mapMessage : public serverMessage {

         friend class boost::serialization::access;
         template<class Archive>
         void serialize(Archive &ar, unsigned int);
         //Needed by boost::serialization
         mapMessage(): serverMessage(msgOutcome::success, 1){}

         std::map<uint_positive_cnt::type, user> siteIdToUser;
     public:

         /**
          * @throws messageException if @e action is not consistent with the message type
          */
         mapMessage(msgType action, msgOutcome result, const std::map<uint_positive_cnt::type, user> &siteIdToUser, uint_positive_cnt::type msgId = 0);

         /**
          * @brief update the mapping siteId->pair<user, color> with the information sent by the server
          * @param client the client instance to update
          *
          * When this message is received by the client, the @e invokeMethod calls @ref SymClient::setUserColors on the
          * @e client passed as parameter.
          */
         void invokeMethod(SymClient &client) override;

         bool operator==(const mapMessage &rhs) const;

         bool operator!=(const mapMessage &rhs) const;

         ~mapMessage() override = default;
     };

    /**
     * @brief class used to model an answer message sent by a server for a @link askResMessage askResMessage @endlink
     */
     class sendResMessage : public serverMessage {

         friend class boost::serialization::access;
         template<class Archive>
         void serialize(Archive &ar, unsigned int);
         //Needed by boost::serialization
         sendResMessage():serverMessage(msgOutcome::success, 1){}

         uint_positive_cnt::type symId{};             /**< in case of <em> action=msgType::openNewRes </em>, the id assigned to the symlink */
         std::shared_ptr<filesystem> resource;
     public:

         /**
          * @throws messageException if @e action is not consistent with the message type
          */
         sendResMessage(msgType action, msgOutcome result, std::shared_ptr<filesystem> resource, uint_positive_cnt::type symId=0, uint_positive_cnt::type msgId = 0);

         /**
          * @brief make a client receive a new resource after a request
          * @param client the client instance to update
          *
          * Depending on the value of @e action, the @e invokeMethod ask for different actions on the client:
          * @li <em> action=msgType::createRes </em>: calls @ref SymClient::createNewSource
          * @li <em> action=msgType::createNewDir </em>: calls @ref SymClient::createNewDir
          * @li <em> action=msgType::openNewRes </em>: calls @ref SymClient::openNewSource
          * @li <em> action=msgType::changeResName </em>: calls @ref SymClient::renameResource
          * @li <em> action=msgType::removeRes </em>: calls @ref SymClient::removeResource
          */
         void invokeMethod(SymClient &client) override;

         ~sendResMessage() override = default;
     };

    /**
     * @brief class used to model the joining of an user to a document
     *
     * The server sends this message when an user access or closes a document. If a user opens a document,
     * then becomes @e active on it and the other clients must be able to see who is active on the document:
     * in this case <em> action=msgType::addActiveUser </em>. Similarly, when a user closes a document, since
     * it is not longer active on it, the server sends a @e updateActiveMessage with <em> action=msgType::removeActiveUser </em>.
     * Client side, if @e newUser is not in any set (readers, writers, owners), then add the user to the proper
     * set basing on @e userPrivilege
     */
     class updateActiveMessage : public serverMessage {

         friend class boost::serialization::access;
         template<class Archive>
         void serialize(Archive &ar, unsigned int);

         updateActiveMessage():serverMessage(msgOutcome::success, 1){}
         user newUser;              /**< user who joined the document */
         uint_positive_cnt::type resourceId;            /**< identifier of the opened resource */
         privilege userPrivilege;   /**< the privilege @e newUser has on @e resourceId */
     public:

         /**
          * @throws messageException if @e action is not consistent with the message type
          */
         updateActiveMessage(msgType action, msgOutcome result, const user &newUser, uint_positive_cnt::type resourceId,
                             privilege priv = privilege::readOnly, uint_positive_cnt::type msgId = 0);

         int getResourceId() const;

         privilege getUserPrivilege() const;

         /**
          * @brief enable all the clients active on document know what other users are active on the document
          * @param client the client instance to update
          *
          * Depending on the value of @e action, the @e invokeMethod ask for different actions on the client:
          * @li <em> action=msgType::addActiveUser </em>: calls @ref SymClient::addActiveUser
          * @li <em> action=msgType::removeActiveUser </em>: calls @ref SymClient::removeActiveUser
          */
         void invokeMethod(SymClient &client) override;

         bool operator==(const updateActiveMessage &rhs) const;

         bool operator!=(const updateActiveMessage &rhs) const;

         ~updateActiveMessage() override = default;
     };

    /**
     * @brief class used to model a privilege change message sent by a client. It is also used to propagate such a change on other clients
     *
     * The client sends the resource identifier for the target resource, the user whose privilege has to be changed and the new privilege.
     * The server forward this message to other clients that share a privilege on that resource to inform them about what changed.
     * Object of this class have <em> action=msgType::changePrivileges </em>
     */
     class privMessage : public clientMessage, public serverMessage {

         friend class boost::serialization::access;
         template<class Archive>
         void serialize(Archive &ar, unsigned int);

         privMessage():serverMessage(msgOutcome::success, 1){}

         std::string resourceId;    /**< path to the resource (the uri) */
         std::string targetUser;    /**< username of the user whose privilege on @e resourceId has to be changed */
         privilege newPrivilege;    /**< new privilege to assign to @e targetUser for @e resourceId */
     public:

         /**
          * @throws messageException if @e action is not consistent with the message type
          */
         privMessage(msgType action, const std::pair<std::string, std::string> &actionOwner, msgOutcome result,
                     const std::string &resourceId, const std::string &targetUser, privilege newPrivilege,
                     uint_positive_cnt::type msgId = 0);

         /**
          * @brief asks the server to modify a file privilege
          * @param server the server the user is active on
          *
          * When this message is received by the server, the @e invokeMethod calls @ref SymServer::editPrivilege on the
          * @e server  passed as parameter.
          */
         void invokeMethod(SymServer &server) override;

         /**
          * @brief propagate the changes made by a client over a file privilege to other clients
          * @param client the client on which propagate the change
          *
          * When this message is received by the client, the @e invokeMethod calls @ref SymClient::editPrivilege on the
          * @e client passed as parameter.
          */
         void invokeMethod(SymClient &client) override;

         void completeAction(SymClient &client, msgOutcome serverResult) override;

         bool operator==(const privMessage &rhs) const;

         bool operator!=(const privMessage &rhs) const;

         ~privMessage() override = default;
     };

    /**
     * @brief class used to model a message regarding a symbol
     *
     * The client sends the @link symbol symbol @endlink to be inserted or removed.
     * The server forwards this message to other clients that are working on the same document.
     * Object of this class have <em> action=msgType::insertSymbol </em> or <em> action=msgType::removeSymbol </em>
     */
     class symbolMessage : public clientMessage, public serverMessage {

         friend class boost::serialization::access;
         template<class Archive>
         void serialize(Archive &ar, unsigned int);

         //Needed for boost::serialization
         symbolMessage(): serverMessage(msgOutcome::success, 1), sym('a', 0, 0, {}){}

         uint_positive_cnt::type siteId;                /**< siteId of the client that send the message */
         uint_positive_cnt::type resourceId;            /**< id of the resource the client is working on*/
         symbol sym;                /**< symbol to be inserted or deleted*/
     public:

         /**
          * @throws messageException if @e action is not consistent with the message type
          */
         symbolMessage(msgType action, const std::pair<std::string, std::string> &actionOwner, msgOutcome result,
                       uint_positive_cnt::type siteId,
                       uint_positive_cnt::type resourceId, const symbol &sym, uint_positive_cnt::type msgId = 0);

         uint_positive_cnt::type getSiteId() const;

         /**
          * @brief set the contained symbol as "verified" calling @ref symbol::setVerified on @e sym
          * @return a reference to the symbolMessage itself
          */
        symbolMessage & verifySym();
         /**
          * @brief normal getter for member @e sym, normally used to inspect the content
          * @return a const reference to the @ref symbol hold by the message
          */
         const symbol &getSym() const;

         /**
          * @brief propagate the local change on a document content on the server
          * @param server the server the user is active on
          *
          * Depending on the value of @e action, the @e invokeMethod ask for different actions on the server:
          * @li <em> action=msgType::insertSymbol </em>: calls @ref SymServer::remoteInsert
          * @li <em> action=msgType::removeSymbol </em>: calls @ref SymServer::remoteRemove
          */
         void invokeMethod(SymServer &server) override;

         /**
          * @brief propagate the changes made by a client over a document content to other clients
          * @param client the client on which propagate the change
          *
          * Depending on the value of @e action, the @e invokeMethod ask for different actions on the client:
          * @li <em> action=msgType::insertSymbol </em>: calls @ref SymClient::remoteInsert
          * @li <em> action=msgType::removeSymbol </em>: calls @ref SymClient::remoteRemove
          */
         void invokeMethod(SymClient &client) override;

         /**
          * @brief confirm the insertion or deletion of a symbol made by a client or abort it
          * @param client the client which sent the message
          *
          * Depending on the value of @e action and @e result, the @e invokeMethod ask for different actions on the client:
          * <ul>
          * <li> <em> result=msgOutcome::success </em>:
          *  <ul>
          *   <li> <em> action=msgType::insertSymbol </em>: calls @ref SymClient::verifySymbol
          *   <li> <em> action=msgType::removeSymbol </em>: calls nothing
          *  </ul>
          * <li> <em> result=msgOutcome::failure </em>:
          *  <ul>
          *   <li> <em> action=msgType::insertSymbol </em>: calls @ref SymClient::remoteRemove
          *   <li> <em> action=msgType::removeSymbol </em>: calls @ref SymClient::remoteInsert
          *  </ul>
          * </ul>
          */
          void completeAction(SymClient &client, msgOutcome serverResult) override;

         bool operator==(const symbolMessage &rhs) const;

         bool operator!=(const symbolMessage &rhs) const;

         ~symbolMessage() override = default;
     };

    /**
     * @brief class used to model a message for sharing a document
     *
     * The client sends an object of type @link uri uri @endlink with the sharing preferences changed by the user.
     * The server forwards this message to other clients that are enabled to see the sharing preferences of a document.
     */
     class uriMessage : public clientMessage, public serverMessage {

         friend class boost::serialization::access;
         template<class Archive>
         void serialize(Archive &ar, unsigned int);

         //Needed for boost::serialization
         uriMessage():serverMessage(msgOutcome::success, 1){}

         std::string path;
         std::string name;
         uri sharingPrefs;
     public:

         /**
          * @throws messageException if @e action is not consistent with the message type
          */
         uriMessage(msgType action, const std::pair<std::string, std::string> &actionOwner, msgOutcome result,
                    const std::string &path, const std::string &name, const uri &sharingPrefs, uint_positive_cnt::type msgId = 0);

         const uri &getSharingPrefs() const;

         /**
          * @brief asks the server to modify a file sharing preferences'
          * @param server the server the user is active on
          *
          * When this message is received by the server, the @e invokeMethod calls @ref SymServer::shareResource on the
          * @e server passed as parameter.
          */
         void invokeMethod(SymServer &server) override;

         /**
          * @brief propagate the changes made by a client over a file sharing preferences to other clients
          * @param client the client on which propagate the change
          *
          * When this message is received by the client, the @e invokeMethod calls @ref SymClient::shareResource on the
          * @e client passed as parameter.
          */
         void invokeMethod(SymClient &client) override;

         void completeAction(SymClient &client, msgOutcome serverResult) override;

         bool operator==(const uriMessage &rhs) const;

         bool operator!=(const uriMessage &rhs) const;

         ~uriMessage() override = default;
     };


    /**
     * @brief class used to model a message to change the parameters of a user
     */
     class userDataMessage : public clientMessage, public serverMessage {

         friend class boost::serialization::access;
         template<class Archive>
         void serialize(Archive &ar, unsigned int);

         //Needed for boost::serialization
         userDataMessage():serverMessage(msgOutcome::success, 1){}

         user newUserData;
     public:

         /**
          * @throws messageException if @e action is not consistent with the message type
          */
         userDataMessage(msgType action, const std::pair<std::string, std::string> &actionOwner, msgOutcome result,
                         const user &newUserData, uint_positive_cnt::type msgId = 0);

         /**
          * @brief asks the server to modify a user's data
          * @param server the server the user is active on
          *
          * When this message is received by the server, the @e invokeMethod calls @ref SymServer::editUser on the
          * @e server passed as parameter.
          */
         void invokeMethod(SymServer &server) override;

         /**
          * @brief propagate the changes made by a client over a user to other clients
          * @param client the client on which propagate the change
          *
          * When this message is received by the client, the @e invokeMethod calls @ref SymClient::editUser on the
          * @e client passed as parameter.
          * A message of this type is sent from the server to clients only if:
          * @li <em> action==msgType::changeUserData </em>;
          */
         void invokeMethod(SymClient &client) override;

         void completeAction(SymClient &client, msgOutcome serverResult) override;

         bool operator==(const userDataMessage &rhs) const;

         bool operator!=(const userDataMessage &rhs) const;

         ~userDataMessage() override = default;

     };

     /**
      * @brief class used to model a message to update the position of a user's cursor
      */
     class cursorMessage: public clientMessage, public serverMessage{
         friend class boost::serialization::access;
         template<class Archive>
         void serialize(Archive &ar, unsigned int);

         //Needed for boost::serialization
         cursorMessage():serverMessage(msgOutcome::success, 1){}

         uint_positive_cnt::type siteId;                /**< siteId of the client that send the message */
         uint_positive_cnt::type resourceId;            /**< resourceId of the resource on which the user cursor's position has changed */
         unsigned row;                                  /**< new row of the cursor */
         unsigned col;                                  /**< new column of the cursor */

     public:
         cursorMessage(msgType action, const std::pair<std::string, std::string> &actionOwner, msgOutcome result,
                       uint_positive_cnt::type siteId,
                       uint_positive_cnt::type resourceId, unsigned int row, unsigned int col, uint_positive_cnt::type msgId = 0);

         /**
          * @brief notify the server that the position of the user's cursor has changed
          * @param server the server the user is active on
          */
         void invokeMethod(SymServer &server) override;

         /**
          * @brief propagate the update of user cursor's position to other clients working on the same resource
          * @param client the client on which propagate the change
          */
         void invokeMethod(SymClient &client) override;

         unsigned int getRow() const;

         unsigned int getCol() const;

         bool operator==(const cursorMessage &rhs) const;

         bool operator!=(const cursorMessage &rhs) const;

         ~cursorMessage() override = default;

     };

     class editLineStyleMessage: public clientMessage, public serverMessage{
         friend class boost::serialization::access;
         template<class Archive>
         void serialize(Archive &ar, unsigned int);
         //Needed for boost::serialization
         editLineStyleMessage():serverMessage(msgOutcome::success, 1){}

         std::pair<alignType, unsigned> oldLineStyle;
         std::pair<alignType, unsigned> newLineStyle;
         uint_positive_cnt::type docId;
         unsigned row;
     public:
        editLineStyleMessage(msgType action, const std::pair<std::string, std::string> &actionOwner, msgOutcome result,
                             const std::pair<alignType, unsigned>& oldLineStyle, const std::pair<alignType, unsigned>& newLineStyle, uint_positive_cnt::type docId,
                             unsigned row, uint_positive_cnt::type msgId=0);

        /**
         * @brief Notify the server that the alignment and/or the index style of a paragraph has been changed
         * @param server the server the user is active on
         */
         void invokeMethod(SymServer &server) override;

         /**
          * @brief propagate the update alignment and/or the index style to other clients working on the same resource
          * @param client the client on which propagate the change
          */
         void invokeMethod(SymClient &client) override;

         /**
          * @brief confirm the changing of alignment and/or the index style made by a client or abort it
          * @param client the client which sent the message
          *
          * Depending on the value of @e result, the @e invokeMethod ask for different actions on the client:
          * <ul>
          * <li> <em> result=msgOutcome::success </em>: calls nothing
          * <li> <em> result=msgOutcome::failure </em>: calls @ref SymClient::changeLineStyle with @e oldLineStyle as parameter
          * </ul>
          */

         void completeAction(SymClient &client, msgOutcome serverResult) override;

         bool operator==(const editLineStyleMessage &rhs) const;

         bool operator!=(const editLineStyleMessage &rhs) const;
     };
 }

#endif //SYMPOSIUM_MESSAGE_H
