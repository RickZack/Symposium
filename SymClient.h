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
#include "user.h"
#include "document.h"
#include "message.h"
#include <type_traits>


namespace Symposium {
/*
 * //TODO: dummy class to represent a color, maybe use some Qt component
 */
    class MyColor {
        int dummy;
    public:
        bool operator==(const MyColor &rhs) const {
            return dummy == rhs.dummy;
        }

        bool operator!=(const MyColor &rhs) const {
            return !(rhs == *this);
        }

    };
    constexpr bool operator<(const std::tuple<int, int, MyColor>& lhs, const std::tuple<int, int, MyColor>& rhs){
        return std::get<0>(lhs)<std::get<0>(rhs) && std::get<1>(lhs)<std::get<1>(rhs);
    }

    /**
     * @brief class used to model a client of Symposium system
     *
     * An instance of this class is intended to represent a logged user working on some documents. If a user wants to open
     * multiple documents at the same time, then it can have multiple windows all sharing the same @e loggedUser object.
     * The server is enabled to accept request for different documents by the same user
     */
    class SymClient {
    protected:
        //ADD: stuff for connectivity, necessary to define constructor
        user loggedUser;                                                          /**< logged user and its data */
        std::forward_list<std::shared_ptr<file>> activeFile;                      /**< list of active documents */
        std::forward_list<document *> activeDoc;                                  /**< list of files the active documents are related to */
        std::map<std::pair<int, int>, std::pair<user, MyColor>> userColors;       /**< map {siteId, documentId}->{user, color}  */

        std::queue<clientMessage> unanswered;                                     /**< messages sent by client that have not been received an answer */

        /*
         * Use this function to access to loggedUser, because it allows the tests to work with a mock class
         * of user and verify expectations about calls on loggedUser
         */
        virtual user& getLoggedUser();
    public:
        //Some methods are virtual in order to use the mocks in tests
        SymClient();
        /**
         * @brief set all the details of the user just logged
         * @param loggedUser the user object containing all the information of the logged user
         *
         * When the client wants to perform login it calls the logIn method. When the server answers, the loginMessage
         * calls setLoggedUser, passing the user object transmitted by the user
         */
        virtual void setLoggedUser(const user &loggedUser);

        /**
         * @brief constructs a @ref signUpMessage to send to the server to ask for registration
         * @param username the username of the user to register
         * @param pwd the password the user has chosen
         * @return a properly constructed @ref signUpMessage to send to the server
         */
        signUpMessage signUp(const std::string &username, const std::string &pwd, const std::string &nickname);

        /**
         * @brief assign to @e loggedUser the user returned by the server after having sent a @ref signUpMessage
         * @param logged the user sent back by the server in a @ref signUpMessage
         */
        void signUp(const user& logged);

        /**
         * @brief constructs a @ref clientMessage to send to the server to aks for authentication
         * @param username the username of the user to login
         * @param pwd he password the user chase
         * @return a properly constructed @ref clientMessage to send to the server
         */
        clientMessage logIn(const std::string &username, const std::string &pwd);

        /**
         * @brief assign to @e loggedUser the user returned by the server after having sent a @ref clientMessage
         * @param logged the user sent back by the server in a @ref signUpMessage
         */
        void logIn(const user& logged);

        /**
         * @brief constructs a @ref askResMessage to send to the server to ask to open a document
         * @param path the path of the file to open, relative to user's home directory
         * @param name the name of the file to open
         * @param reqPriv the privilege requested opening the file
         * @return a properly constructed @ref askResMessage to send to the server
         */
        askResMessage openSource(const std::string &path, const std::string &name, privilege reqPriv);

        /**
         * @brief add to @e activeFile the @e fileAsked and opens the document adding it to @e activeDoc
         * @param fileAsked the file sent back by the server in a @ref sendResMessage
         */
        virtual void openSource(const std::shared_ptr<file> fileAsked);

        /**
         * @brief constructs a @ref askResMessage to send to the server to ask to open a document
         * @param resourceId the universal resource identifier for the resource to open
         * @param reqPriv the privilege requested opening the file
         * @param destPath the path inside user's home directory where to put a symlink to the file
         * @return a properly constructed @ref askResMessage to send to the server
         */
        virtual askResMessage
        openNewSource(const std::string &resourceId, privilege reqPriv, const std::string &destPath, const std::string& destName="");

        /**
         * @brief add to @e activeFile the @e fileAsked and opens the document adding it to @e activeDoc
         * @param fileAsked the file sent back by the server in a @ref sendResMessage
         */
        virtual void openNewSource(const std::shared_ptr<file> fileAsked);

        /**
         * @brief constructs a @ref askResMessage to send to the server to ask to create a file
         * @param path the path of the file to create, relative to user's home directory
         * @param name the name of the file to create
         * @return a properly constructed @ref askResMessage to send to the server
         */
        virtual askResMessage createNewSource(const std::string &path, const std::string &name);

        /**
         * @brief add to @e activeFile the @e fileAsked and opens the document adding it to @e activeDoc
         * @param fileCreated the file sent back by the server in a @ref sendResMessage
         */
        virtual void createNewSource(const std::shared_ptr<file> fileCreated);

        /**
         * @brief constructs a @ref askResMessage to send to the server to ask to create a directory
         * @param path the path of the directory to create, relative to user's home directory
         * @param name the name of the directory to create
         * @return the name of the file to create
         */
        virtual askResMessage createNewDir(const std::string &path, const std::string &name);

        /**
         * @brief add the directory to user's filesystem
         * @param dirCreated the directory sent back by the server in a @ref sendResMessage
         *
         * The new directory is created by calling user::newDirectory with the data taken from @e dirCreated
         */
        virtual void createNewDir(const std::shared_ptr<directory> dirCreated);

        /**
         * @brief propagate a symbol insertion on document content made by another user
         * @param resourceId the document the insertion refers to
         * @param newSym the symbol to insert
         *
         * This method is called after having received a @ref symbolMessage
         */
        virtual void remoteInsert(int resourceId, const symbol &newSym);

        /**
         * @brief propagate a symbol deletion on document content made by another user
         * @param resourceId the document the deletion refers to
         * @param rmSym the symbol to remove
         *
         * This method is called after having received a @ref symbolMessage
         */
        virtual void remoteRemove(int resourceId, const symbol &rmSym);

        /**
         * @brief constructs a @ref privMessage to send to the server to ask to change privileges for a resource
         * @param targetUser the user whose privilege has to be modified
         * @param resPath the absolute path of the resource
         * @param resName the name of the resource
         * @param newPrivilege the new privilege to be granted to @e targetUser
         * @return a properly constructed @ref privMessage to send to the server
         *
         * When a user client side want to edit the privilege of another user on a resource, it sends a
         * @ref PrivMessage. The server will answer with a @ref serveMessage indicating if the action has been done successfully.
         * The message is put on @e unanswered, so when the client will receive an answer for a message, it will invoke
         * privilege SymClient::editPrivilege that will actually perform the privilege change.
         */
        virtual privMessage editPrivilege(const std::string &targetUser, const std::string &resPath, const std::string &resName,
                                          privilege newPrivilege);

        /**
         * @brief edit the privilege of @e targetUser user for the resource @e resName in @e resPath to @e newPrivilege
         * @param targetUser the user whose privilege has to be modified
         * @param resPath the absolute path of the resource
         * @param resName the name of the resource
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
        virtual privilege editPrivilege(const std::string &targetUser, const std::string &resPath, const std::string &resName,
                                        privilege newPrivilege, bool msgRcv);

        /**
         * @brief constructs a @ref uriMessage to send to the server to ask to change sharing preferences for a resource
         * @param resPath the absolute path of the resource
         * @param resName the name of the resource
         * @param newPrefs new sharing preferences to set the resource to
         * @return a properly constructed @ref uriMessage to send to the server
         *
         * When a user client side want to edit the privilege of another user on a resource, it sends a
         * @ref uriMessage. The server will answer with a @ref serveMessage indicating if the action has been done successfully.
         * The message is put on @e unanswered, so when the client will receive an answer for a message, it will invoke
         * uri SymClient::shareResource that will actually perform the sharing preference change.
         */
        virtual uriMessage shareResource(const std::string &resPath, const std::string &resName, uri &newPrefs);

        /**
         * @brief set new sharing preferences for a resource
         * @param resPath the absolute path of the resource
         * @param resName the name of the resource
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
        virtual uri shareResource(const std::string &resPath, const std::string &resName, uri &newPrefs, bool msgRcv);

        /**
         * @brief constructs a @ref uriMessage to send to the server to ask to change the name of a resource
         * @param resPath the relative path to the user's @e home directory where to create the file
         * @param resName the resource's name
         * @param newName the new resource's name
         * @return a properly constructed @ref askResMessage to send to the server
         *
         * When a user client side wants to set a new name for a resource, it sends a @ref askResMessage.
         * The server will answer with a @ref serveMessage indicating if the action has been done successfully.
         * The message is put on @e unanswered, so when the client will receive an answer for a message, it will invoke
         * std::shared_ptr<filesystem> SymClient::renameResource that will actually perform the renaming.
         */
        virtual askResMessage
        renameResource(const std::string &resPath, const std::string &resName, const std::string &newName);

        /**
         * @brief removes a resource from @e remover 's @e home directory
         * @param resPath the relative path to the user's @e home directory where to create the file
         * @param resName the resource's name
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
        renameResource(const std::string &resPath, const std::string &resName, const std::string &newName, bool msgRcv);

        /**
         * @brief constructs a @ref askResMessage to send to the server to ask to remove a resource
         * @param resPath the relative path to the user's @e home directory where to create the file
         * @param resName the resource's name
         * @return a properly constructed @ref uriMessage to send to the server
         *
         * When a user client side wants remove a resource, it sends a @ref askResMessage.
         * The server will answer with a @ref serveMessage indicating if the action has been done successfully.
         * The message is put on @e unanswered, so when the client will receive an answer for a message, it will invoke
         * std::shared_ptr<filesystem> SymClient::removeResource that will actually perform the removing.
         */
        virtual askResMessage
        removeResource(const std::string &resPath, const std::string &resName);

        /**
         * @brief removes a resource from @e remover 's @e home directory
         * @param resPath the relative path to the user's @e home directory where to create the file
         * @param resName the resource's name
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
        removeResource(const std::string &resPath, const std::string &resName, bool msgRcv);

        std::string showDir(bool recursive=false) const;


        /**
         * @brief list the document according a @e condition
         * @tparam C a function or functional object (@ref filterShared, @ref filterPrivilege)
         * @param condition a filter condition to apply to each document to decide whether to list it or not
         * @return the string containing the list of files that respect @e condition
         */
        template<typename C, typename=std::enable_if<std::is_invocable_r<bool,C>::value>>
        std::string show(C condition){}

        /**
         * @brief visualize which user made which change in the document, using @e userColors
         * @tparam C a function or functional object
         * @param condition a filter condition to apply to document's text to decide what to show (e.g only changes made by a specific user)
         */
        template<typename C, typename=std::enable_if<std::is_invocable_r<bool,C>::value>>
        void showChanges(C condition){}

        /**
        * @brief close a @ref document for a user
        * @param resourceId document to be closed
        *
        * When a user client side wants to remove a resource, it sends a @ref updateDocMessage and removes the document with
        * id @e resourceId from @e activeDoc and @e activeFile.
        */
        updateDocMessage closeSource(int resourceId);

        /**
         * @brief changes user's data
         * @param newUserData a user object filled with new data
         * @return the old user data
         *
         * When a user client side wants to change its data, it sends a @ref userDataMessage.
         * The server will answer with a @ref serveMessage indicating if the action has been done successfully.
         * The message is put on @e unanswered, so when the client will receive an answer for a message, it will invoke
         * again this function to actually perform the changing if the outcome is positive.
         */
        virtual const user editUser(user &newUserData, bool msgRcv);

        clientMessage removeUser();

        clientMessage logout();

        updateDocMessage mapSiteIdToUser(const document &currentDoc);

        /**
         * @brief assign to each user working on the same document on which @e loggedUser is working a unique color
         * @param siteIdToUser the mapping siteId->user asked to the server
         *
         * Receive the mapping siteId->user from the server and assigns to each user a unique color among the colors
         * assigned to the users working on the same document.
         */
        virtual void setUserColors(const std::map<int, user> &siteIdToUser);

        void exportPDF(const std::string &resPath, const std::string &resName);

        /**
         * @brief add @e targetUser to the list of active users of the document
         * @param resourceId the id of the document the update refers to
         * @param targetUser the user to add
         */
        virtual void addActiveUser(int resourceId, user &targetUser);

        /**
         * @brief remove @e targetUser to the list of active users of the document
         * @param resourceId the id of the document the update refers to
         * @param targetUser the user to remove
         */
        virtual void removeActiveUser(int resourceId, user &targetUser);

        virtual ~SymClient() = default;
    };

//TODO: add methods to allow consumption of messages sent by server as answer to client messages. E.g:
// - when a message is received, the client must search if there is a message with the same id previously
// - sent, to understand what message the server is answering to and to detect errors. Then it will call
// - the invokeMethod of the message to perform the action

    /**
     * @brief class used to create functional objects to be used in @ref SymClient::show
     *
     * Filter user's home directory print basing on the fact that a file has not been created by @e currentUser
     */
    class filterShared {
        const user &currentUser;
    public:
        explicit filterShared(const user &currentUser);

        bool operator()(std::shared_ptr<file> file);
    };

    /**
     * @brief class used to create functional objects to be used in @ref SymClient::show
     *
     * Filter user's home directory print basing on the privilege that has been granted to @e currentUser
     */
    class filterPrivilege {
        const user &currentUser;
        privilege filter;
    public:
        explicit filterPrivilege(const user &currentUser, privilege filter = privilege::readOnly);

        bool operator()(std::shared_ptr<file> file);
    };
}
#endif //SYMPOSIUM_SYMCLIENT_H
