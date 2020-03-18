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
 * File:   SymServer.h
 * Project: Symposium
 * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Alice Morano <s259158 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 20 Giugno 2019, 21.30
 */
#ifndef SYMPOSIUM_SYMSERVER_H
#define SYMPOSIUM_SYMSERVER_H

#include <string>
#include <memory>
#include <unordered_map>
#include <forward_list>
#include <queue>
#include <map>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>


#include "Symposium.h"
#include "user.h"
#include "message.h"


/**
 * @brief class used to model a server of Symposium system
 *
 * Handles all the data and actions that are required to a Symposium server.
 * For all of the received client's requestes, the server answer sending an object
 * of class @ref serverMessage (or derivates) with the field @e result set to
 * @ref msgOutcome::success or @ref msgOutcome::failure, indicating the action's outcome.
 * The client should confirm the action only if it has received a positive outcome.
 */
 namespace Symposium {
     class SymServer {

         friend class boost::serialization::access;
         template<class Archive>
         void serialize(Archive &ar, const unsigned int version);

     protected:
         std::unordered_map<std::string, user> registered;                                                 /**< registered users, indexed by username */
         std::unordered_map<std::string, const user *> active;                                             /**< active users, indexed by username */
         std::unordered_map<std::string, std::forward_list<document *>> workingDoc;                        /**< list of document each user is working on */
         std::unordered_map<int, std::queue<std::shared_ptr<serverMessage>>> siteIdToMex;                  /**< messages queues associated with every user by means of @e siteId */
         std::unordered_map<int, std::forward_list<int>> resIdToSiteId;                                    /**< list of users involved in a document, by means of @e siteIds and @e resIds */
         static uint_positive_cnt idCounter;                                                               /**< siteId to be assigned to the next registered user */
         std::shared_ptr<directory> rootDir;                                                               /**< virtual filesystem of the Symposium server */

     public:
         static const user unknownUser;

         //Some methods are virtual in order to use the mocks in tests
         SymServer();

         bool operator==(const SymServer &rhs) const;

         bool operator!=(const SymServer &rhs) const;


         /**
          * @brief adds a new user to the set of users registered to the system
          * @param newUser the user to be added to the system
          * @return the user just added
          * @throws SymServerException thrown if @ref newUser is already registered
          * @throws SymServerException thrown if @ref newUser data is not correct
          *
          * When a client asks for the registration of a new user via a @ref signUpMessage, the server validates the user data
          * received (a @ref signUpMessage contains a whole new user), assignes to it a subdirectory inside @e rootDir,
          * adds the user to @e registered and send back to the client the user structure just filled inside a @ref loginMessage.
          * Note that the @e siteId of the user is assigned by the server, as the user's @e home directory structure and the
          * @e hashSalt (that mustn't be sent to the client inside the @ref loginMessage), so the user filled client side is always incomplete.
          */
         virtual const user &addUser(user &newUser);

         /**
          * @brief log in an already registered user, adding it to @e active
          * @param username the username of the user who is performing the log in
          * @param pwd the user's password
          * @return the logged in user
          * @throws SymServerException thrown if the user with @ref username is not registered
          * @throws SymServerException thrown if the @ref pwd is wrong
          * @throws SymServerException thrown if the user with @ref username is already logged in
          *
          * When a client asks for the login via a @ref clientMessage, the server checks that the @e username and
          * @e password corresponds to a user contained in @e registered, then sends the retrieved user object to
          * the client, after having darken the @e password and the @e hashSalt, via a @ref loginMessage
          */
         virtual const user login(const std::string &username, const std::string &pwd);

         /**
          * @brief access a document that is already in the user's filesystem
          * @param opener the user who wants to access the document
          * @param path the path of the document, relative to user's home directory
          * @param name the document's name
          * @param reqPriv the privilege requested opening the document
          * @return the document just retrieved
          * @throws SymServerException thrown if the user @ref opener is not logged in
          * @throws filesystemException rethrown if there are problems regarding the asked resource
          *
          * When a client asks for a document, the server checks that the file named @e name in @e path is
          * available and then that @e opener has a privilege less or equal to @e reqPriv on it. If these requirements are met,
          * then the server sends the document inside a @ref sendResMessage
          * If the operation succeed, the server sends a @ref updateActiveMessage to the clients working on the document
          */
         virtual std::shared_ptr<file>
         openSource(const std::string &opener, const std::string &path, const std::string &name, privilege reqPriv);

         /**
          * @brief access a user's document via uri to the filesystem of the another user
          * @param opener the user who made the request
          * @param resourceId the absolute path of the requested document
          * @param destPath the path where to put the @ref symlink to @e name, inside @e opener 's home directory
          * @param destName the name to assign to the symlink
          * @param reqPriv the privilege requested opening the document
          * @return the document just retrieved
          * @throws SymServerException thrown if the user @ref opener is not logged in
          * @throws filesystemException rethrown if there are problems regarding the asked resource
          *
          * When a client asks for a document for which the user has no privilege with @ref askResMessage, the server
          * checks that the file in @e resourceId is available and then that the file is shareable.
          * Under these conditions, the server adds the @e opener to the subset of users who have @e reqPriv privilege,
          * adds a @ref symlink to the file in @e destPath named @e destName and send the document inside a @ref sendResMessage
          * If the operation succeed, the server sends a @ref updateActiveMessage to the clients working on the document
          * and send back to the client a @ref sendResMessage with the symlink just created
          */
         virtual std::shared_ptr<file>
         openNewSource(const std::string &opener, const std::string &resourceId, const std::string &destPath,
                       const std::string &destName, privilege reqPriv);

         /**
          * @brief creates a new file with an empty document inside
          * @param opener the user who made the request
          * @param path the relative path to the @e opener 's @e home directory where to create the file
          * @param name the name of the new file
          * @return the document just created
          * @throws SymServerException thrown if the user @ref opener is not logged in
          * @throws filesystemException rethrown if there are problems creating the resource
          *
          * When a client asks for a new directory, the server tries to do it and send back to the client a @ref sendResMessage
          * containing the resource just added to @e path.
          */
         virtual const document &createNewSource(const std::string &opener, const std::string &path, const std::string &name);

         /**
          * @brief creates a new directory in the user's filesystem
          * @param opener the user who made the request
          * @param path the relative path to the @e opener 's @e home directory where to create the directory
          * @param name the name of the new directory
          * @return the directory just created
          * @throws SymServerException thrown if the user @ref opener is not logged in
          * @throws filesystemException rethrown if there are problems creating the resource
          *
          * When a client asks for a new directory, the server tries to do it and send back to the client a @ref sendResMessage
          * containing the directory just added to @e path.
          */
         virtual std::shared_ptr<directory>
         createNewDir(const std::string &opener, const std::string &path, const std::string &name);

         /**
          * @brief update a document with a new symbol from a client
          * @param inserter the user who is working on the document
          * @param resourceId the id of the document inside @e workingDoc
          * @param symMsg message received containing the symbol to insert
          *
          * When a user client side inserts a new symbol, the client sends to the server a @ref symbolMessage containing
          * the symbol, the @e resourceId and the @e siteId. The server validates the message checking that @e inserter
          * is a valid user and that is a user currently working on the document indicated by @e resourceId, then
          * update its own copy of the document and propagate the update to the other clients putting the message in
          * @e workingQueue
          */
         virtual void remoteInsert(const std::string &inserter, int resourceId, symbolMessage &symMsg);

         /**
          * @brief update a document removing a symbol
          * @param remover the user who is working on the document
          * @param resourceId the id of the document inside @e workingDoc
          * @param rmMsg message received containing the symbol to remove
          *
          * When a user client side removes a symbol, the client sends to the server a @ref symbolMessage containing
          * the symbol, the @e resourceId and the @e siteId. The server validates the message checking that @e remover
          * is a valid user and that is a user currently working on the document indicated by @e resourceId, then
          * update its own copy of the document and propagate the update to the other clients putting the message in
          * @e workingQueue
          */
         virtual void remoteRemove(const std::string &remover, int resourceId, symbolMessage &rmMsg);
         //dispatchMessages ->function to be active in background to send messages from server to client
         //updateActiveUsers(); ->useful or just done inside other functions?

         /**
          * @brief update the current position of user's cursor inside one of the documents he's working on
          * @param targetUser the user whose cursor position has changed
          * @param resourceId the id of the document in which the user's cursor has been moved
          * @param crMsg the message received by the client
          */
         virtual void updateCursorPos(const std::string &targetUser, int resourceId, cursorMessage& crMsg);

         /**
          * @brief edit the privilege of @e targetUser user for the resource @e resName in @e resPath to @e newPrivilege
          * @param actionUser the user who made the request
          * @param targetUser the user whose privilege has to be modified
          * @param resPath the absolute path of the resource
          * @param resName the name of the resource
          * @param newPrivilege the new privilege to be granted to @e targetUser
          * @return the old privilege of @e targetUser had on the resource
          *
          * When a user client side want to edit the privilege of another user on a resource, it sends a
          * @ref PrivMessage and waits for an answer from the server. The server checks that @e actionUser is
          * in @e registered and in @e active, and that @e targetUser is in @e registered and is not working
          * on the resource in @e resPath named @e resName. Calls @ref user::openFile to get the document and
          * then calls @ref user::editPrivilege on @e actionUser
          * At the end send a @ref serverMessage with the action outcome
          */
         virtual privilege editPrivilege(const std::string &actionUser, const std::string &targetUser, const std::string &resPath,
                                         const std::string &resName, privilege newPrivilege);

         /**
          * @brief set new sharing preferences for a resource
          * @param actionUser the user who made the request
          * @param resPath the absolute path of the resource
          * @param resName the name of the resource
          * @param newPrefs new sharing preferences for the resource
          * @return the old @e sharingPolicy
          *
          * When a user client side wants to set a new sharing policy for a resource, it sends a @ref uriMessage
          * and waits for an answer from the server. The server checks that @e actionUser is in @e registered and
          * in @e active, then calls @ref user::shareResource on @e actionUser
          * At the end send a @ref serverMessage with the action outcome
          */
         virtual std::shared_ptr<filesystem>
         shareResource(const std::string &actionUser, const std::string &resPath, const std::string &resName, const uri &newPrefs);

         /**
          * @brief renames a resource from @e remover 's @e home directory
          * @param renamer the user who is asking to rename its resource
          * @param resPath the relative path to the @e renamer 's @e home directory where to find the resource
          * @param resName the resource's name (meaning the its unique id)
          * @param newName the new resource's name (not the id)
          * @return the resource just renamed
          *
          * When a user client side wants to set a new name for a resource, it sends a @ref askResMessage and waits
          * for an answer from the server. The server checks that @e actionUser is in @e registered and in @e active,
          * then calls @ref user::renameResource on @e renamer. Please note that, in case the filesystem object is a symlink,
          * this method renames the symlink, not the resource pointed.
          * Sends back a @ref serveMessage, to indicate whether the action succeeded or not.
          */
         virtual std::shared_ptr<filesystem>
         renameResource(const std::string &renamer, const std::string &resPath, const std::string &resName,
                        const std::string &newName);

         /**
          * @brief removes a resource from @e remover 's @e home directory
          * @param remover the user who is asking to remove its resource
          * @param resPath the relative path to the @e remover 's @e home directory where to create the file
          * @param resName the resource's name
          * @return the resource just removed
          *
          * When a user client side wants remove a resource, it sends a @ref askResMessage and waits for an answer from the server.
          * The server checks that @e actionUser is in @e registered and in @e active, then calls @ref user::renameResource on @e remover.
          */
         virtual std::shared_ptr<filesystem>
         removeResource(const std::string &remover, const std::string &resPath, const std::string &resName);

         /**
         * @brief close a @ref document for a user
         * @param actionUser the user who wants to close the document
         * @param resIdtoClose document to be closed
         *
         * This method is invoked by receiving a @ref updateDocMessage and has the effect of calling
         * @ref document::close and the removal of @e actionUser from @e workingDoc for @e toClose
         */
         virtual void closeSource(const std::string &actionUser, int resIdtoClose);

         /**
          * @brief changes user's data
          * @param username the username of the user whose data are to be changed
          * @param newUserData a user object filled with new data
          * @return the new user data
          *
          * The server validates the new data and changes the user in @e registered and in @e active and sends back to the client
          * a @ref serverMessage with the outcome.
          * The server must send a @ref userDataMessage to the users in @e active that share some files with the user
          * identified by @e username if the user changed the nickname or the icon
          */
         virtual const user &editUser(const std::string &username, user &newUserData);

         /**
         * @brief removes an user to the set of users registered to the system
         * @param username the name of the user to remove
         * @param pwd the user's password
         * @return the user just removed
          */
         virtual void removeUser(const std::string &username, const std::string &pwd);

         /**
          * @brief performs a log out, removing the user from @e active
          * @param username the username of the user who is performing the log out
          * @return the logged out user
          */
         virtual void logout(const std::string &username);

         /**
          * @brief maps siteIds to users to allow a client to identify the owner of each change in a document
          * @param actionUser the user who is asking for the mapping
          * @param resourceId the id of the  document for which the client asked for the mapping
          * @return the mapping siteId->user
          *
          * This method id invoked by receiving a @ref updateDocMessage with @action <em> action=msgType::mapChangesToUser </em>.
          * It verifies that @e actionUser is registered and active and that it is currently working on the document that has the
          * same @e resourceId. Calls @ref document::retrieveSiteIds to know which users has modified the document, then produce
          * the map with users in @e registered.
          * Sends to the client a @ref mapMessage
          */
         virtual std::map<int, user> mapSiteIdToUser(const std::string &actionUser, int resourceId);
         //OPTIMIZE: this operation seems expensive, other ways to make it lighter? Only thing is minimize these requests client side

         /**
          * @brief extract a message from the message queue associated with an user
          * @return a pair containing the siteId of the user the message is to send to and the message itself
          */
         std::pair<const int, std::shared_ptr<serverMessage>> extractNextMessage();
         virtual ~SymServer() = default;

     protected:
         /*
          * These methods have been created for test suites to allow the mock class to access elements.
          * They must be used in code to allow tests to verify expectations
          */
         /**
          * @brief insert a new user in the table of registered users
          * @param toInsert the user to be inserted
          * @return a reference to the inserted user
          */
         virtual user &registerUser(const user &toInsert);

         /**
          * @brief return the registered user associated with @e username
          * @param username the username of the registered user
          * @return a reference to the user
          * @warning before a call to this method, the user with @e username must exist in server memory, otherwise an exception is thrown
          * @throw std::out_of_range can be thrown if the method is called without previously verifying that the user is registered
          */
         virtual user &getRegistered(const std::string &username);

         virtual void removeRegistered(const std::string &username);

         /*
          * These methods are for internal use, to simplify the body of public functions.
          * Some of them are also virtual to be overridden in test suites (read ./tests/SymServerTest.cpp)
          */
         virtual bool userIsRegistered(const std::string &toCheck) const noexcept ;
         virtual user findUserBySiteId(int id) const noexcept;
         virtual bool userIsActive(const std::string &username) const;

         /*
          * These are utility methods for internal use that don't need override
          */
     private:
         static bool userIsValid(const user &toCheck)  noexcept ;
         /**
          * @brief searches in the @e workingDoc map for the document that has the given @e resourceId
          * @param username the username of the user whose working state on a document is to be checked
          * @param resourceId the id of the resource we want to know if the user is working on
          * @return a pair that containt {false, nulltpr} if the user is not working on the resource that has the given
          * @e resourceId, or {true, pointer to document} if the user is working on the resource
          */
         std::pair<bool, document *> userIsWorkingOnDocument(const std::string &username, int resourceId) const;

        /**
         * @brief Handles the access of @e actionUser to the document to change the privileges of a target user
         * @param actionUser the name of the user who is changing the privileges of the target user
         * @param resName the resource name
         * @param pathFromUserHome the resource path, stating form @e actionUser's home directory
         * @param actionU the user who is changing the privileges of the target user
         * @return the resId of the document involved in the privilege change
         */
         int
         handleAccessToDoc(const std::string &actionUser, const std::string &resName,
                           const std::string &pathFromUserHome, const user &actionU);

        /**
         * @brief Handles control on the state of @e targetUser, to impose that it should or should not work on the resource
         * @param targetUser the user whose state is to be controlled
         * @param docId the resource for which the working state of the user should be controlled
         * @param working indicates whether an exception is to be raised if the user work or do not work on the resource
         * @throws SymServerException thrown if @e targetUser has state @e working on resource @e docId
         */
         void handleUserState(const std::string &targetUser, int docId, bool working=true);

         /**
          * @brief extract the siteIds associated to the given @e resId, excluding @e siteIdToExclude from the result
          * @param resId the resource id of the resource we want the siteIds associated with
          * @param siteIdToExclude a siteId to exclude from the result, tipically the one of the user who asked for this
          * @return a list of siteIds
          */
         virtual std::forward_list<int> siteIdsFor(int resId, int siteIdToExclude=-1) const;

         /**
          * @brief extract the resIds of the documents associated with the user names @e username
          * @param username the name of the user for which the mapping is needed
          * @return a list of resIds
          */
         virtual std::forward_list<int> resIdOfDocOfUser(const std::string& username) const;

         /**
          * @brief extract the siteIds of the users that are associated with at least one of resIds in @e resIds
          * @param resIds a list of resource ids for which a mapping with the working user's siteId is needed
          * @param siteIdToExclude a siteId to exclude from the result, tipically the one of the user who asked for this
          * @return a list of siteIds
          */
         virtual std::forward_list<int> siteIdOfUserOfDoc(const std::forward_list<int> &resIds, int siteIdToExclude=-1) const;

         /**
          * @brief Insert a copy of the message @e toSend in the message queue associated with every siteIds in @e siteIds
          * @param siteIds the list of user (by means of their siteId) the message @e toSend should be forwarded to
          * @param toSend the message to send to every user that has siteId in @e siteIds
          */
         void insertMessageForSiteIds(const std::forward_list<int>& siteIds, std::shared_ptr<serverMessage> toSend);

        /**
         * @brief Call document::close on all the documents left opened by the user that just logged out and propagate
         * the message about the disjoin of the user on that documents
         * @param loggedOut the user that just logged out
         * @param listOfDocs the user of docs the user was working on
         */
         void closeAllDocsAndPropagateMex(const user &loggedOut, const std::forward_list<document*>& listOfDocs);

         /**
          * @brief Removes the siteId of the user that just logged out from the list
          * of siteIds associated with every resource that the user left opened
          * @param loggedOut the user that just logged out
          */
         void handleLeavingUser(const user &loggedOut);

         /**
          * @brief generate a simple response with msgOutcome::success for a client request
          * @param recvSiteId the siteId of the client to send the confirm to
          * @param action the msgType of the received clientMessage
          */
         void generateSimpleResponse(int recvSiteId, msgType action);
     };
 }

#endif //SYMPOSIUM_SYMSERVER_H
