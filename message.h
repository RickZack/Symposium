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

/**
 * @interface message message.h message
 * @brief class used as interface for a message
 */
 namespace Symposium {
     class message {
     protected:
         static int msgCounter;
         int msgId;                 /**< random identifier for the message, used when a message is followed by an answer*/
         msgType action;            /**< Defines the action for the current message */

         message(int msgId);

     public:
         int getMsgId() const;

         msgType getAction() const;

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
     protected:
         clientMessage(const std::pair<std::string, std::string> &actionOwner, int msgId = 0);
     public:

         /**
          * @throws messageException if @e action is not consistent with the message type
          */
         clientMessage(msgType action, const std::pair<std::string, std::string> &actionOwner, int msgId = 0);


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
         virtual void completeAction(SymClient &client);

         bool operator==(const clientMessage &rhs) const;

         bool operator!=(const clientMessage &rhs) const;

         virtual ~clientMessage() override = default;
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
                       int msgId = 0);

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

         /**
          * @brief completes an action for which the client asked to the server
          * @param client the same client that had originated the clientMessage
          *
          * When an a client sends an askResMessage with <em> action=msgType::changeResName </em>, the server answers
          * with a serverMessage containing the outcome: in case it is positive, completeAction() performs the actual name change.
          */
         void completeAction(SymClient &client) override;

         bool operator==(const askResMessage &rhs) const;
         bool operator!=(const askResMessage &rhs) const;

         ~askResMessage() override = default;
     };

/**
 * @brief class used to model a sign up message sent by a client
 */
     class signUpMessage : public clientMessage {
         user newUser;          /**< new SympUser data inserted by the user */
     public:

         /**
          * @throws messageException if @e action is not consistent with the message type
          */
         signUpMessage(msgType action, const std::pair<std::string, std::string> &actionOwner, const user &newUser,
                       int msgId = 0);

         const user &getNewUser() const;

         /**
          * @brief make @e newUser a registered user of the server the message is sent to
          * @param server the server to sign up to
          *
          * When this message is received by the server, the @e invokeMethod calls @ref SymServer::addUser on the @e server
          * passed as parameter.
          */
         void invokeMethod(SymServer &server) override;

         virtual ~signUpMessage() override{};
     };

    /**
     * @brief class used to model a message sent by a client to close a resource
     */
     class updateDocMessage : public clientMessage {
         int resourceId;            /**< identifier of the resource on which perform the action */
     public:

         /**
          * @throws messageException if @e action is not consistent with the message type
          */
         updateDocMessage(msgType action, const std::pair<std::string, std::string> &actionOwner, int resourceId,
                          int msgId = 0);

         int getResourceId() const;

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

         ~updateDocMessage() override = default;

     };

/**
 * @brief class used to model a message sent by the server
 */
     class serverMessage : public virtual message {
         msgOutcome result;         /**< result of an operation asked to the server */
     protected:
         serverMessage(msgOutcome result, int msgId = 0);
     public:

         /**
          * @throws messageException if @e action is not consistent with the message type
          */
         serverMessage(msgType action, msgOutcome result, int msgId = 0);



         void setResult(msgOutcome outcome);

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
         bool isRelatedTo(const clientMessage &other);

         virtual ~serverMessage() = default;
     };

/**
 * @brief class used to model a message sent by a server as answer to a login message
 *
 * The client sends a @link clientMessage clientMessage @endlink object with the authentication parameters and receives
 * a @link loginMessage @endlink object with all the data of the logged user.
 * In this message <em> action=msgType::registration </em> or <em> action=msgType::login </em>
 */
     class loginMessage : public serverMessage {
         user loggedUser;
     public:

         /**
          * @throws messageException if @e action is not consistent with the message type
          */
         loginMessage(msgType action, msgOutcome result, const user &loggedUser, int msgId = 0);

         const user &getLoggedUser() const;

         /**
          * @brief enable the client have the same representation for an user after a login
          * @param client the client instance to fill with user's data
          *
          * When this message is received by the client, the @e invokeMethod calls @ref SymClient::setLoggedUser on the @e client
          * passed as parameter.
          */
         void invokeMethod(SymClient &client) override;

         virtual ~loginMessage() = default;
     };

/**
 * @brief class used to model an answer message sent by a server for a @link clientMessage clientMessage @endlink
 *
 * The server answers with the map siteId->user for the document the current user is working on.
 * The client ask for this answer with a @link updateDocMessage updateDocMessage @endlink with <em> action=msgType::mapChangesToUser </em>
 * In this message <em> action=msgType::mapChangesToUser </em>: calls
 */
     class mapMessage : public serverMessage {
         std::map<int, user> siteIdToUser;
     public:

         /**
          * @throws messageException if @e action is not consistent with the message type
          */
         mapMessage(msgType action, msgOutcome result, const std::map<int, user> &siteIdToUser, int msgId = 0);

         const std::map<int, user> &getSiteIdToUser() const;

         /**
          * @brief update the mapping siteId->pair<user, color> with the information sent by the server
          * @param client the client instance to update
          *
          * When this message is received by the client, the @e invokeMethod calls @ref SymClient::setUserColors on the
          * @e client passed as parameter.
          */
         void invokeMethod(SymClient &client) override;

         virtual ~mapMessage() = default;
     };

/**
 * @brief class used to model an answer message sent by a server for a @link askResMessage askResMessage @endlink
 */
     class sendResMessage : public serverMessage {
         int symId;             /**< in case of <em> action=msgType::openNewRes </em>, the id assigned to the symlink */
         filesystem &resource;
     public:

         /**
          * @throws messageException if @e action is not consistent with the message type
          */
         sendResMessage(msgType action, msgOutcome result, filesystem &resource, int symId=0, int msgId = 0);

         const filesystem &getResource() const;

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

         virtual ~sendResMessage() = default;
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
         user newUser;              /**< user who joined the document */
         int resourceId;            /**< identifier of the opened resource */
         privilege userPrivilege;   /**< the privilege @e newUser has on @e resourceId */
     public:

         /**
          * @throws messageException if @e action is not consistent with the message type
          */
         updateActiveMessage(msgType action, msgOutcome result, const user &newUser, int resourceId,
                             privilege priv = privilege::readOnly, int msgId = 0);

         const user &getNewUser() const;

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
         std::string resourceId;    /**< path to the resource (the uri) */
         std::string targetUser;    /**< username of the user whose privilege on @e resourceId has to be changed */
         privilege newPrivilege;    /**< new privilege to assign to @e targetUser for @e resourceId */
     public:

         /**
          * @throws messageException if @e action is not consistent with the message type
          */
         privMessage(msgType action, const std::pair<std::string, std::string> &actionOwner, msgOutcome result,
                     const std::string &resourceId, const std::string &targetUser, privilege newPrivilege,
                     int msgId = 0);

         const std::string &getResourceId() const;

         const std::string &getTargetUser() const;

         privilege getNewPrivilege() const;

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

         void completeAction(SymClient &client) override;

         virtual ~privMessage() = default;
     };

/**
 * @brief class used to model a message regarding a symbol
 *
 * The client sends the @link symbol symbol @endlink to be inserted or removed.
 * The server forwards this message to other clients that are working on the same document.
 * Object of this class have <em> action=msgType::insertSymbol </em> or <em> action=msgType::removeSymbol </em>
 */
     class symbolMessage : public clientMessage, public serverMessage {
         int siteId;                /**< siteId of the client that send the message */
         int resourceId;            /**< id of the resource the client is working on*/
         symbol sym;                /**< symbol to be inserted or deleted*/
     public:

         /**
          * @throws messageException if @e action is not consistent with the message type
          */
         symbolMessage(msgType action, const std::pair<std::string, std::string> &actionOwner, msgOutcome result,
                       int siteId,
                       int resourceId, const symbol &sym, int msgId = 0);

         int getSiteId() const;

         int getResourceId() const;

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
          //TODO: complete description
         void completeAction(SymClient &client) override;

         virtual ~symbolMessage() = default;
     };

/**
 * @brief class used to model a message for sharing a document
 *
 * The client sends an object of type @link uri uri @endlink with the sharing preferences changed by the user.
 * The server forwards this message to other clients that are enabled to see the sharing preferences of a document.
 */
     class uriMessage : public clientMessage, public serverMessage {
         std::string path;
         std::string name;
         uri sharingPrefs;
     public:

         /**
          * @throws messageException if @e action is not consistent with the message type
          */
         uriMessage(msgType action, const std::pair<std::string, std::string> &actionOwner, msgOutcome result,
                    const std::string &path, const std::string &name, const uri &sharingPrefs, int msgId = 0);

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

         void completeAction(SymClient &client) override;

         virtual ~uriMessage() = default;
     };


/**
 * @brief class used to model a message to change the parameters of a user
 */
     class userDataMessage : public clientMessage, public serverMessage {
         user newUserData;
     public:

         /**
          * @throws messageException if @e action is not consistent with the message type
          */
         userDataMessage(msgType action, const std::pair<std::string, std::string> &actionOwner, msgOutcome result,
                         const user &newUserData, int msgId = 0);

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

         ~userDataMessage() override = default;

     };

/**
 * @brief models an exception thrown for error in messages
 */
     class messageException : public std::exception {
         std::string msg;
     public:
         messageException(const std::string &message) : msg(message) {};

         const char *what() const noexcept override {
             return msg.c_str();
         }

         ~messageException() = default;
     };
 }

#endif //SYMPOSIUM_MESSAGE_H
