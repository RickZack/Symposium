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
 * File:   SymClient.h
 * Project: Symposium
 * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Alice Morano <s259158 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 24 Giugno 2019, 19.25
 */
#ifndef SYMPOSIUM_SYMCLIENT_H
#define SYMPOSIUM_SYMCLIENT_H

#include <string>
#include <forward_list>
#include <queue>
#include <map>

#include "Symposium.h"
#include "Color.h"
#include "user.h"
#include "document.h"
#include "message.h"

#include "onoff_networkinteraction.h"

namespace Symposium {

    constexpr bool operator<(const std::tuple<int, int, Color>& lhs, const std::tuple<int, int, Color>& rhs){
        return std::get<0>(lhs)<std::get<0>(rhs) && std::get<1>(lhs)<std::get<1>(rhs);
    }
    #ifdef DISPATCHER_ON
    class clientdispatcher;
    #endif
    /**
     * @brief class used to model a client of Symposium system
     *
     * An instance of this class is intended to represent a logged user working on some documents. If a user wants to open
     * multiple documents at the same time, then it can have multiple windows all sharing the same @e loggedUser object.
     * The server is enabled to accept request for different documents by the same user
     */

    class SymClient {
    protected:
        user loggedUser;                                                                                                /**< users map with whom we have open documents in common, with counter for each user  */
        std::map<std::string, std::pair<const user, int>> usersOnDocuments;                                             /**< active users, indexed by username */
        std::forward_list<std::shared_ptr<file>> activeFile;                                                            /**< list of active documents */
        std::forward_list<std::pair<document *, colorGen>> activeDoc;                                                   /**< list of files the active documents are related to */
        std::map<std::pair<uint_positive_cnt::type, uint_positive_cnt::type>, std::pair<user, Color>> userColors;       /**< map {siteId, documentId}->{user, color}  */
        #ifdef DISPATCHER_ON
        clientdispatcher* dispatcher;                                                                                   /**< pointer to client dispatcher */
        #endif
        std::forward_list<std::shared_ptr<clientMessage>> unanswered;                                                   /**< messages sent by client that have not been received an answer */

        /*
         * Use this function to access to loggedUser, because it allows the tests to work with a mock class
         * of user and verify expectations about calls on loggedUser
         */
        virtual const user& getLoggedUser() const;

    public:
        //Some methods are virtual in order to use the mocks in tests
        SymClient();

        /**
         * @brief constructs a @ref signUpMessage to send to the server to ask for registration
         * @param username the username of the user to register
         * @param pwd the password the user has chosen
         * @param nickname the nickName chosen by the user
         * @param iconPath the path (relative to the software directory) of the chosen icon
         * @return a properly constructed @ref signUpMessage to send to the server
         */
        signUpMessage signUp(const std::string &username, const std::string &pwd, const std::string &nickname,
                             const std::string &iconPath);

        /**
         * @brief assign to @e loggedUser the user returned by the server after having sent a @ref signUpMessage
         * @param logged the user sent back by the server in a @ref signUpMessage
         */
        virtual void signUp(const user& logged);

        /**
         * @brief constructs a @ref clientMessage to send to the server to aks for authentication
         * @param username the username of the user to login
         * @param pwd the password the user chase
         * @return a properly constructed @ref clientMessage to send to the server
         */
        clientMessage logIn(const std::string &username, const std::string &pwd);

        /**
         * @brief assign to @e loggedUser the user returned by the server after having sent a @ref clientMessage
         * @param logged the user sent back by the server in a @ref signUpMessage
         */
        virtual void logIn(const user& logged);

        /**
         * @brief constructs a @ref askResMessage to send to the server to ask to open a document
         * @param resPath the path of the file to open, relative to user's home directory
         * @param resId the id of the file to open
         * @param reqPriv the privilege requested opening the file
         * @return a properly constructed @ref askResMessage to send to the server
         */
        askResMessage openSource(const std::string &resPath, const std::string &resId, privilege reqPriv);

        /**
         * @brief add to @e activeFile the @e fileAsked and opens the document adding it to @e activeDoc
         * @param resPath the path of the file to open, relative to user's home directory
         * @param resId the id of the file to open
         * @param fileAsked the file sent back by the server in a @ref sendResMessage
         */
        virtual void openSource(const std::string &resPath, const std::string &resId, const std::shared_ptr<file> fileAsked,
                                privilege reqPriv);

        /**
         * @brief constructs a @ref askResMessage to send to the server to ask to open a document
         * @param absolutePath the absolute path to the resource to open
         * @param reqPriv the privilege requested opening the file
         * @param destPath the path inside user's home directory where to put a symlink to the file
         * @return a properly constructed @ref askResMessage to send to the server
         */
        virtual askResMessage
        openNewSource(const std::string &absolutePath, privilege reqPriv, const std::string &destPath, const std::string& destName= "");

        /**
         * @brief add to @e activeFile the @e fileAsked and opens the document adding it to @e activeDoc
         * @param absolutePath the absolute path to the resource to open
         * @param reqPriv the privilege requested opening the file
         * @param destPath the path where to put the @ref symlink to @e name, inside @e opener 's home directory
         * @param destName the name to assign to the symlink
         * @param idToAssign is the id assigned to the file from the server
         * @param fileAsked the file sent back by the server in a @ref sendResMessage
         */
        virtual void openNewSource(const std::string &absolutePath, privilege reqPriv, const std::string &destPath,
                                   const std::string &destName,
                                   uint_positive_cnt::type idToAssign, const std::shared_ptr<file> fileAsked);

        /**
         * @brief constructs a @ref askResMessage to send to the server to ask to create a file
         * @param resPath the path of the file to create, relative to user's home directory
         * @param resName the name of the file to create
         * @return a properly constructed @ref askResMessage to send to the server
         */
        virtual askResMessage createNewSource(const std::string &resPath, const std::string &resName);

        /**
         * @brief add to @e activeFile the @e fileAsked and opens the document adding it to @e activeDoc
         * @param resPath is the path where the user want to put the file into
         * @param resName is the name to assign to the new file
         * @param idToAssign is the id assigned to the file from the server
         * @param fileCreated the file sent back by the server in a @ref sendResMessage
         *
         * The new file is created by calling user::newFile with the (@e path, @e name) taken from
         * the previously sent @ref askResMessage and @e idToAssign taken from the file object contained
         * in the @ref sendResMessage just received
         */
        virtual void
        createNewSource(const std::string &resPath, const std::string &resName, uint_positive_cnt::type idToAssign,
                        const std::shared_ptr<file> fileCreated);

        /**
         * @brief constructs a @ref askResMessage to send to the server to ask to create a directory
         * @param resPath the path of the directory to create, relative to user's home directory
         * @param resName the name of the directory to create
         * @return the name of the file to create
         */
        virtual askResMessage createNewDir(const std::string &resPath, const std::string &resName);

        /**
         * @brief add the directory to user's filesystem
         * @param resPath is the path where the user want to put the directory into
         * @param resName is the name to assign to the new directory
         * @param idToAssign is the id assigned to the directory from the server
         *
         * The new directory is created by calling user::newDirectory with the (@e path, @e name) taken from
         * the previously sent @ref askResMessage and @e idToAssign taken from the directory object contained
         * in the @ref sendResMessage just received
         */
        virtual void createNewDir(const std::string &resPath, const std::string &resName, uint_positive_cnt::type idToAssign);

        /**
         * @brief insert a symbol on an opened document and constructs a message to sent to the server
         * @param docId the document the insertion refers to
         * @param newSym the symbol to insert
         * @return a properly constructed @ref symbolMessage to send to the server
         */
        virtual symbolMessage localInsert(uint_positive_cnt::type docId, const symbol &newSym, const std::pair<unsigned int, unsigned int> &index);

        /**
         * @brief remove a symbol on an opened document and constructs a message to sent to the server
         * @param docId the document the deletion refers to
         * @param indexes the position of the symbol to remove
         * @return a properly constructed @ref symbolMessage to send to the server
         */
        virtual symbolMessage localRemove(uint_positive_cnt::type docId, const std::pair<unsigned int, unsigned int> &indexes);

        /**
         * @brief propagate a symbol insertion on document content made by another user
         * @param siteId the site id of the user performing the insertion
         * @param docId the document the insertion refers to
         * @param newSym the symbol to insert
         *
         * This method is called after having received a @ref symbolMessage
         */
        virtual void
        remoteInsert(uint_positive_cnt::type siteId, uint_positive_cnt::type docId, const symbol &newSym);

        /**
         * @brief propagate a symbol deletion on document content made by another user
         * @param siteId the site id of the user performing the removal
         * @param docId the document the deletion refers to
         * @param rmSym the symbol to remove
         *
         * This method is called after having received a @ref symbolMessage
         */
        virtual void
        remoteRemove(uint_positive_cnt::type siteId, uint_positive_cnt::type docId, const symbol &rmSym);

        /**
         * @brief set the symbol of the opened document that has @e resourceId to "verified"
         * @param docId the id of the document the symbol refers to
         * @param sym the symbol to verify
         *
         * When the server answers to a client's @ref symbolMessage, it sends a @ref serverMessage containing
         * the outcome of the action. When this message is received by the client, the related previously sent
         * message is retrieved and @ref symbolMessage::completeAction is called: it calls this method if the
         * outcome is positive.
         */
        virtual void verifySymbol(uint_positive_cnt::type docId, const symbol &sym);

        /**
         * @brief constructs a message to sent to the server to inform other users that local user has moved his cursor
         * @param docId the id of the document
         * @param row X coordinate of the new position of cursor
         * @param col Y coordinate of the new position of cursor
         * @return a properly constructed @ref cursorMessage to send to the server
         */
        virtual cursorMessage updateCursorPos(uint_positive_cnt::type docId, unsigned int row, unsigned int col);

        /**
         * @brief update the position of user's cursor that has that @ref userSiteId on the document
         * @param userSiteId the site id of the user moving his cursor
         * @param docId the id of the document
         * @param row X coordinate of the new position of user's cursor
         * @param col Y coordinate of the new position of user's cursor
         *
         * This method is called after having received a @ref cursorMessage
         *
         */
        virtual void updateCursorPos(uint_positive_cnt::type userSiteId, uint_positive_cnt::type docId, unsigned int row, unsigned int col);

        /**
         * @brief constructs a @ref privMessage to send to the server to ask to change privileges for a resource
         * @param targetUser the user whose privilege has to be modified
         * @param resPath the relative path of the resource
         * @param resId the id of the resource
         * @param newPrivilege the new privilege to be granted to @e targetUser
         * @return a properly constructed @ref privMessage to send to the server
         *
         * When a user client side want to edit the privilege of another user on a resource, it sends a
         * @ref PrivMessage. The server will answer with a @ref serveMessage indicating if the action has been done successfully.
         * The message is put on @e unanswered, so when the client will receive an answer for a message, it will invoke
         * privilege SymClient::editPrivilege that will actually perform the privilege change.
         */
        virtual privMessage editPrivilege(const std::string &targetUser, const std::string &resPath, const std::string &resId,
                                          privilege newPrivilege);

        /**
         * @brief edit the privilege of @e targetUser user for the resource @e resName in @e resPath to @e newPrivilege
         * @param targetUser the user whose privilege has to be modified
         * @param resPath the relative path of the resource
         * @param resId the id of the resource
         * @param newPrivilege the new privilege to be granted to @e targetUser
         * @param msgRcv indicate whether the method is called after having sent a @ref privMessage
         * @return the old privilege of @e targetUser had on the resource
         *
         * When a server answers to a privMessage, it sends a @ref serverMessage indicating if the action has been done successfully.
         * When a client receives this message, it searches for a related message in @e unanswered, and calls @e completeAction
         * on it passing itself. The method @e completeAction of @ref privMessage calls this function with msgRcv=true, while
         * the method @e invokeMethod calls with msgRcv=false. This parameter msgRcv is used only to distinguish method signatures
         * of this method and the one that returns a privMessage.
         */
        virtual privilege editPrivilege(const std::string &targetUser, const std::string &resPath, const std::string &resId,
                                        privilege newPrivilege, bool msgRcv);

        /**
         * @brief constructs a @ref uriMessage to send to the server to ask to change sharing preferences for a resource
         * @param resPath the relative path of the resource
         * @param resId the id of the resource
         * @param newPrefs new sharing preferences to set the resource to
         * @return a properly constructed @ref uriMessage to send to the server
         *
         * When a user client side want to share a resource with another user, it sends a
         * @ref uriMessage. The server will answer with a @ref serveMessage indicating if the action has been done successfully.
         * The message is put on @e unanswered, so when the client will receive an answer for a message, it will invoke
         * uri SymClient::shareResource that will actually perform the sharing preference change.
         */
        virtual uriMessage shareResource(const std::string &resPath, const std::string &resId, const uri &newPrefs);

        /**
         * @brief set new sharing preferences for a resource
         * @param resPath the relative path of the resource
         * @param resId the id of the resource
         * @param newPrefs new sharing preferences for the resource
         * @param msgRcv indicate whether the method is called after having sent a @ref uriMessage
         * @return the old @e sharingPolicy
         *
         * When a server answers to a uriMessage, it sends a @ref serverMessage indicating if the action has been done successfully.
         * When a client receives this message, it searches for a related message in @e unanswered, and calls @e completeAction
         * on it passing itself. The method @e completeAction of @ref uriMessage calls this function with msgRcv=true, while
         * the method @e invokeMethod calls with msgRcv=false. This parameter msgRcv is used only to distinguish method signatures
         * of this method and the one that returns a uriMessage.
         */
        virtual std::shared_ptr<filesystem>
        shareResource(const std::string &actionUser, const std::string &resPath, const std::string &resId,
                      const uri &newPrefs,
                      bool msgRcv);

        /**
         * @brief constructs a @ref uriMessage to send to the server to ask to change the name of a resource
         * @param resPath the relative path to the user's @e home directory where to create the file
         * @param resId the resource's id
         * @param newName the new resource's name
         * @return a properly constructed @ref askResMessage to send to the server
         *
         * When a user client side wants to set a new name for a resource, it sends a @ref askResMessage.
         * The server will answer with a @ref serveMessage indicating if the action has been done successfully.
         * The message is put on @e unanswered, so when the client will receive an answer for a message, it will invoke
         * std::shared_ptr<filesystem> SymClient::renameResource that will actually perform the renaming.
         */
        virtual askResMessage
        renameResource(const std::string &resPath, const std::string &resId, const std::string &newName);

        /**
         * @brief removes a resource from @e remover 's @e home directory
         * @param resPath the relative path to the user's @e home directory where to create the file
         * @param resId the resource's id
         * @param newName the new resource's name
         * @param msgRcv indicates whether this method is called after having received a @ref serverMessage
         * @return the resource just renamed
         *
         * When a server answers to a askResMessage, it sends a @ref serverMessage indicating if the action has been done successfully.
         * When a client receives this message, it searches for a related message in @e unanswered, and calls @e completeAction
         * on it passing itself. The method @e completeAction of @ref askResMessage calls this function with msgRcv=true, while
         * the method @e invokeMethod calls with msgRcv=false. This parameter msgRcv is used only to distinguish method signatures
         * of this method and the one that returns a askResMessage.
         */
        virtual std::shared_ptr<filesystem>
        renameResource(const std::string &resPath, const std::string &resId, const std::string &newName, bool msgRcv);

        /**
         * @brief constructs a @ref askResMessage to send to the server to ask to remove a resource
         * @param resPath the relative path to the user's @e home directory where to delete the file
         * @param resId the resource's id
         * @return a properly constructed @ref askResMessage to send to the server
         *
         * When a user client side wants remove a resource, it sends a @ref askResMessage.
         * The server will answer with a @ref serveMessage indicating if the action has been done successfully.
         * The message is put on @e unanswered, so when the client will receive an answer for a message, it will invoke
         * std::shared_ptr<filesystem> SymClient::removeResource that will actually perform the removing.
         */
        virtual askResMessage
        removeResource(const std::string &resPath, const std::string &resId);

        /**
         * @brief removes a resource from @e remover 's @e home directory
         * @param resPath the relative path to the user's @e home directory where to create the file
         * @param resId the resource's id
         * @param msgRcv indicates whether this method is called after having received a @ref serverMessage
         * @return the resource just removed
         *
         * When a server answers to a askResMessage, it sends a @ref serverMessage indicating if the action has been done successfully.
         * When a client receives this message, it searches for a related message in @e unanswered, and calls @e completeAction
         * on it passing itself. The method @e completeAction of @ref askResMessage calls this function with msgRcv=true, while
         * the method @e invokeMethod calls with msgRcv=false. This parameter msgRcv is used only to distinguish method signatures
         * of this method and the one that returns a askResMessage.
         */
        virtual std::shared_ptr<filesystem>
        removeResource(const std::string &resPath, const std::string &resId, bool msgRcv);

        /**
         * @brief show the content of user's root directory
         * @param recursive true if the method should be recursive, false otherwise
         * @return a string with all content of user's root directory
         */
        std::string showDir(bool recursive=false) const;

        /**
        * @brief close a @ref document for a user
        * @param docId document to be closed
        *
        * When a user client side wants to remove a resource, it sends a @ref updateDocMessage and removes the document with
        * id @e resourceId from @e activeDoc and @e activeFile.
        */
        updateDocMessage closeSource(uint_positive_cnt::type docId);

        /**
         * @brief changes user's data
         * @param newUserData a user object filled with new data
         * @return the old user data
         *
         * When a user client side wants to change its data, it sends a @ref userDataMessage.
         * The server will answer with a @ref serveMessage indicating if the action has been done successfully.
         * The message is put on @e unanswered, so when the client will receive an answer for a message, it will invoke
         * the analogous function @e editUser(user&, bool) to actually perform the changing if the outcome is positive.
         */
        virtual userDataMessage editUser(user &newUserData);

        /**
         * @brief changes user's data
         * @param newUserData a user object filled with new data
         * @param msgRcv indicates if the function is called because a userDataMessage was sent or not
         * @return the old user data
         *
         * When the response for a userDataMessage is received, this function is called with msgRcv=true, elseif
         * the function is called because a changing in user data has to be propagated to this client msgRcv=false
         */
        virtual const user editUser(user &newUserData, bool msgRcv);

        /**
         * @brief delete a user from the system
         * @param pwd the password the user chase
         * @return a properly constructed @ref clientMessage to send to the server
         */
        clientMessage removeUser(const std::string &pwd);

        /**
         * @brief confirm deletion of user
         * @param msgRcv indicates if the function is called because a userDataMessage was sent or not
         */
        virtual void removeUser(bool msgRcv);

        /**
         * @brief ask to disconnect a user from the system
         * @return a properly constructed @ref clientMessage to send to the server
         */
        clientMessage logout();

        /**
         * @brief disconnect a user from the system
         */
        virtual void logout(bool msgRcv);

        /**
         * @brief create a @ref updateDocMessage to get from the server the mapping between username ad siteID of @e currentDoc 's activeUsers
         * @param currentDoc document you are working on
         * @return a properly constructed @ref updateDocMessage to send to the server
         */
        updateDocMessage mapSiteIdToUser(const document &currentDoc);

        /**
         * @brief edit the alignment and/or the indexStyle of a document's paragraph @e row
         * @param docId the id of the document that has to be modified
         * @param oldLineStyle the actual alignment and indexStyle of the document at row @e row
         * @param newLineStyle the new alignment and indexStyle to apply at row @e row
         * @param row the document's row that has to be modified
         * @return a properly constructed @ref editLineStyleMessage to send to the server
         */
        editLineStyleMessage
        localEditLineStyle(uint_positive_cnt::type docId, const std::pair<alignType, unsigned>& oldLineStyle, const std::pair<alignType, unsigned>& newLineStyle, unsigned row);

        /**
         * @brief propagate an update of line style on document made by another user
         * @param docId the id of the document that has been modified
         * @param newLineStyle the new alignment and indexStyle applied at row @e row
         * @param row the document's row that has been modified
         */
        virtual void remoteEditLineStyle(uint_positive_cnt::type docId, const std::pair<alignType, unsigned>& newLineStyle, unsigned row);

        /**
         * @brief assign to each user working on the same document on which @e loggedUser is working a unique color
         * @param docId the identified of the document the mapping was requested for
         * @param siteIdToUser the mapping siteId->user asked to the server
         *
         * Receive the mapping siteId->user from the server and assigns to each user a unique color among the colors
         * assigned to the users working on the same document.
         */
        virtual void
        setUserColors(uint_positive_cnt::type docId, const std::map<uint_positive_cnt::type, user> &siteIdToUser);

        /**
         * @brief add @e targetUser to the list of active users of the document
         * @param docId the id of the document the update refers to
         * @param targetUser the user to add
         * @param Priv the privilege of the user to add
         */
        virtual void addActiveUser(uint_positive_cnt::type docId, user &targetUser, privilege Priv);

        /**
         * @brief remove @e targetUser to the list of active users of the document
         * @param docId the id of the document the update refers to
         * @param targetUser the user to remove
         */
        virtual void removeActiveUser(uint_positive_cnt::type docId, user &targetUser);

        /**
         * @brief retrieve a @ref clientMessage previously sent by the client related to @e smex, removing it from @e unanswered
         * @param smex the @ref serverMessage sent by the server that is supposed to be a response for a previous @ref clientMessage
         * @return the clientMessage that is related to the received @e smex
         */
        virtual std::shared_ptr<clientMessage> retrieveRelatedMessage(const serverMessage& smex);

        /**
         * @brief the default destroyer
         */
        virtual ~SymClient() = default;

        #ifdef DISPATCHER_ON
        /**
         * @brief set dispatcher pointer to a correct client dispatcher
         * @param cl pointer to client dispatcher
         *
         * this function is important to invoke it for allow Symclient to communicate with other Symposium module
         */
        void setClientDispatcher(clientdispatcher *cl);
        #endif

        /**
         * @brief it provides the online users list on the specified document
         * @param documentID the id of the document
         * @return the list of online users
         */
        const std::forward_list<std::pair<const user *, sessionData>> onlineUsersonDocument(uint_positive_cnt::type documentID);

        /**
         * @brief it provides the list of all users who modified the specified document
         * @param documentID the id of the document
         * @return the list of all users
         */
        const std::unordered_map<std::string, privilege> allUsersonDocument(uint_positive_cnt::type documentID);

        /**
        * @brief it provides the logged user with all data
        * @return the logged user
        */
        const user& userData();

        /**
        * @brief show the content of directory with ID @ref ID_Cartella
        * @param ID_Cartella the ID of directory
        * @param path path where the directory is located
        * @return a string with all content of request directory
        */
        std::string directoryContent(std::string &ID_Cartella, std::string &path);

        /**
         * @brief retrieve the color assigned to an user for a document
         * @param resId the document the mapping is required for
         * @param siteId the siteId of the user for which the color is asked
         * @return the @ref Color corresponding to the user having @e siteId for the document having @e resId
         */
        Color colorOfUser(uint_positive_cnt::type resId, uint_positive_cnt::type siteId);

        Color colorOfUserbyUsername(uint_positive_cnt::type resId, const std::string& username);

        const document& getActiveDocumenttoOpenbyID(uint_positive_cnt::type id);

        const file& getActiveFiletoOpenbyID(uint_positive_cnt::type id);

        bool controlFileIsActive(uint_positive_cnt::type id);

        bool controlDocumentIsActive(uint_positive_cnt::type id);

    private:
        /**
         * @brief get the document with specified ID
         * @param id the id of the request document
         * @return the request document
         */
        document* getActiveDocumentbyID(uint_positive_cnt::type id);

        /**
         * @brief get the @ref colorGen associated with document that it has the specified id
         * @param id the document the colorGen is required for
         * @return the colorGen associated with specified document
         */
        colorGen & getColorGeneratorbyDocumentiID(uint_positive_cnt::type id);

        /**
         * @brief get the file that content the document with specified ID
         * @param id the id of the content document into the file
         * @return the file that content the document with specified ID
         */
        const std::shared_ptr<file> getFilebyDocumentID(uint_positive_cnt::type id);

        /**
         * @brief set all the details of the user just logged
         * @param loggedUser the user object containing all the information of the logged user
         *
         * When the client wants to perform login it calls the logIn method. When the server answers, the loginMessage
         * calls setLoggedUser, passing the user object transmitted by the user
         */
        virtual void setLoggedUser(const user &loggedUser);

        const user& addUsersOnDocument(const user& toInsert);

        const user& getUsersOnDocument(const std::string &username);

        void removeUsersOnDocument(const std::string &username);

        void assignUsersColor(colorGen& c, document& d);

        bool handleDocException(const std::function<void(void)>& op);
    };
}
#endif //SYMPOSIUM_SYMCLIENT_H
