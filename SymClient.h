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

/**
 * @brief class used to model a client of Symposium system
 *
 * An instance of this class is intended to represent a logged user working on some documents. If a user wants to open
 * multiple documents at the same time, then it can have multiple windows all sharing the same @e loggedUser object.
 * The server is enabled to accept request for different documents by the same user
 */
    class SymClient {
        //ADD: stuff for connectivity, necessary to define constructor
        user loggedUser;                                                        /**< logged user and its data */
        std::forward_list<std::shared_ptr<file>> activeFile;                    /**< list of active documents */
        std::forward_list<document *> activeDoc;                                  /**< list of files the active documents are related to */
        std::forward_list<std::map<int, std::pair<user, MyColor>>> userColors;   /**< map siteId->{user, color} for every active document */

        std::queue<message> unanswered;                                         /**< messages sent by client that have not been received an answer */
    public:
        //Some methods are virtual in order to use the mocks in tests
        SymClient();

        virtual void setLoggedUser(const user &loggedUser);

        virtual void setUserColors(const std::map<int, std::pair<user, MyColor>> &userColors);

        void signUp(const std::string &username, const std::string &pwd);

        void logIn(const std::string &username, const std::string &pwd);


        document openSource(const std::string &path, const std::string &name, privilege reqPriv);

        virtual document
        openNewSource(const std::string &path, const std::string &name, privilege reqPriv, const std::string &destPath);

        virtual document createNewSource(const std::string &path, const std::string &name);

        virtual std::shared_ptr<directory> createNewDir(const std::string &path, const std::string &name);

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
         * @brief edit the privilege of @e targetUser user for the resource @e resName in @e resPath to @e newPrivilege
         * @param targetUser the user whose privilege has to be modified
         * @param resPath the absolute path of the resource
         * @param resName the name of the resource
         * @param newPrivilege the new privilege to be granted to @e targetUser
         * @param msgRcv indicates whether this method is called after having received a @ref privMessage
         * @return the old privilege of @e targetUser had on the resource
         *
         * When a user client side want to edit the privilege of another user on a resource, it sends a
         * @ref PrivMessage. The server will answer with a @ref serveMessage indicating if the action has been done successfully.
         * The message is put on @e unanswered, so when the client will receive an answer for a message, it will invoke
         * again this function to actually perform the changing if the outcome is positive.
         */
        virtual privilege editPrivilege(const user &targetUser, const std::string &resPath, const std::string &resName,
                                        privilege newPrivilege, bool msgRcv);

        /**
         * @brief set new sharing preferences for a resource
         * @param resPath the absolute path of the resource
         * @param resName the name of the resource
         * @param newPrefs new sharing preferences for the resource
         * @return the old @e sharingPolicy
         *
         * When a user client side wants to set a new sharing policy for a resource, it sends a @ref uriMessage.
         * The server will answer with a @ref serveMessage indicating if the action has been done successfully.
         * The message is put on @e unanswered, so when the client will receive an answer for a message, it will invoke
         * again this function to actually perform the changing if the outcome is positive.
         */
        virtual uri shareResource(const std::string &resPath, const std::string &resName, uri &newPrefs);

        /**
         * @brief removes a resource from @e remover 's @e home directory
         * @param resPath the relative path to the user's @e home directory where to create the file
         * @param resName the resource's name
         * @param newName the new resource's name
         * @param msgRcv indicates whether this method is called after having received a @ref serverMessage
         * @return the resource just renamed
         *
         * When a user client side wants to set a new name for a resource, it sends a @ref askResMessage.
         * The server will answer with a @ref serveMessage indicating if the action has been done successfully.
         * The message is put on @e unanswered, so when the client will receive an answer for a message, it will invoke
         * again this function to actually perform the changing if the outcome is positive.
         */
        virtual std::shared_ptr<filesystem>
        renameResource(const std::string &resPath, const std::string &resName, const std::string &newName, bool msgRcv);

        /**
         * @brief removes a resource from @e remover 's @e home directory
         * @param resPath the relative path to the user's @e home directory where to create the file
         * @param resName the resource's name
         * @param msgRcv indicates whether this method is called after having received a @ref serverMessage
         * @return the resource just removed
         *
         * When a user client side wants remove a resource, it sends a @ref askResMessage.
         * The server will answer with a @ref serveMessage indicating if the action has been done successfully.
         * The message is put on @e unanswered, so when the client will receive an answer for a message, it will invoke
         * again this function to actually perform the changing if the outcome is positive.
         */
        virtual std::shared_ptr<filesystem>
        removeResource(const std::string &resPath, const std::string &resName, bool msgRcv);

        std::string printDir();


        /**
         * @brief list the document according a @e condition
         * @tparam C a function or functional object (@ref filterShared, @ref filterPrivilege)
         * @param condition a filter condition to apply to each document to decide whether to list it or not
         * @return the string containing the list of files that respect @e condition
         */
        template<typename C>
        std::string show(C condition);

        /**
         * @brief visualize which user made which change in the document, using @e userColors
         * @tparam C a function or functional object
         * @param condition a filter condition to apply to document's text to decide what to show (e.g only changes made by a specific user)
         */
        template<typename C>
        void showChanges(C condition);

        /**
        * @brief close a @ref document for a user
        * @param resourceId document to be closed
        *
        * When a user client side wants to remove a resource, it sends a @ref updateDocMessage and removes the document with
        * id @e resourceId from @e activeDoc and @e activeFile.
        */
        void closeSource(int resourceId);

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

        void removeUser();

        void logout();

        /**
         * @brief maps a user's siteId with the user object and a random color to use in the GUI
         * @param currentDoc the document fo which perform the mapping
         * @param msgRcv indicates whether this method is called after having received a @ref mapMessage or by the user
         * @return the mapping siteId->{user, color}
         *
         * This method provides a siteId->user mapping for @e currentDoc, first trying to use the data
         * contained in the @ref file structure or asking the server for this mapping if in the document
         * symbols belonging to users not anymore in any privilege set are found.
         * To retrieve the set of user ids of the current document calls @ref document::retrieveSideIds
         */
        std::map<int, std::pair<user, MyColor>> mapSiteIdToUser(const document &currentDoc, bool msgRcv);


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
