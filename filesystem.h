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
 * File:   filesystem.h
 * Project: Symposium
 * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Alice Morano <s259158 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 18 Giugno 2019, 22.02
 */
#ifndef SYMPOSIUM_FILESYSTEM_H
#define SYMPOSIUM_FILESYSTEM_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <sstream>
#include <algorithm>
#include <regex>

#include "privilege.h"
#include "Symposium.h"
#include "resourceType.h"
#include "uri.h"

#include "document.h"

#include "AccessStrategy.h"
#include "SymposiumException.h"



/*
 * Forward declaration: filesystem must use "user" only with pointers.
 * Filesystem uses "user" only to verify that a user has the right to
 * access a resource, through an AccessStrategy. Filesystem mustn't call
 * any of the functions of "user"
 */
namespace Symposium {
   /**
     * @interface filesystem filesystem.h filesystem
     * @brief class used as interface for a filesystem, made using @e Composite pattern
     *
     * In the application design only objects of subclass @link file file @endlink can be shared, but
     * @e sharingPolicy could refer to a directory. This is done on purpose to allow future extensions
     * of this module. As long as the mentioned behaviour is desired, for objects of subclasses @link directory directory @endlink
     * and @link symlink symlink @endlink @e sharingPolicy must indicate that the resource is not sharable
     */
    class filesystem {
    protected:
        static int idCounter;      /**< id to be assigned to the next created filesystem object */
        int id;                    /**< unique identifier for the filesystem object, used also for identifying objects along a path */
        std::string name;          /**< resource name */
        uri sharingPolicy;         /**< sharing policy applied to the resource */
        std::unique_ptr<AccessStrategy> strategy;
    public:
        filesystem(const std::string &name, const int idToAssign=0);

        int getId() const;

        const std::string &getName() const;

        /**
         * @brief retrieve the privilege of a user on the current filesystem object
         * @param targetUser the user whose privilege is to be retrieved
         * @return the privilege of @e targetUser
         * @throws @ref filesystemException "Object @e name is not shareable" if called
         *
         * Calling @e getUserPrivilege on a filesystem object is considered an error, so an exception is raised, since
         * each subclass must specify whether it is allowed to share objects of its type among users, overriding @e setUserPrivilege
         * and this method.
         */
        virtual privilege getUserPrivilege(const std::string &targetUser) const;

        virtual uri &getSharingPolicy();

        /**
         * @brief set the privilege of @e targetUser to @e newPrivilege for the current filesystem object
         * @param targetUser the user whose privilege is to be modified
         * @param newPrivilege the privilege to grant to @e targetUser
         * @throws @ref filesystemException "Object @e name is not shareable" if called
         *
         * On client side this method asks the server for privilege changing sending a message of type
         * @ref privMessage: if the message outcome from the server is positive, then confirm the action,
         * otherwise revert it.
         * On server side validate the action, perform the action and send a @ref serverMessage with the outcome.
         *
         * Calling @e setUserPrivilege on a filesystem object is considered an error, so an exception is raised, since
         * each subclass must specify whether it is allowed to share objects of its type among users, overriding @e setUserPrivilege
         * and this method.
         */
        virtual privilege setUserPrivilege(const std::string &targetUser, privilege newPrivilege);


        /**
         * @brief identify the type of current filesystem resource
         * @return the type of the current filesystem object (file, directory, symlink)
         */
        virtual resourceType resType() const = 0;

        /**
         * @brief change the current file's name
         * @param newName newName new name to assign to the resource
         * @return the old name of the current resource
         *
         * On client side this method sends a request to the server to change the current file's name
         * sending a message of type @ref askResMessage: if the message outcome from the server is positive,
         * then confirm the action, otherwise revert it.
         * On server side validate the action, perform the action and send a @ref serverMessage with the outcome.
         */
        std::string setName(const std::string &newName);

        /**
         * @brief set new @e sharingPolicy for a filesystem object
         * @param actionUser the user who is performing the action
         * @param newSharingPrefs new sharing preferences for the resource
         * @return the old @e sharingPolicy
         * @throws @ref filesystemException "Object @e name is not shareable" if called
         *
         * Calling @e setSharingPolicy on a filesystem object is considered an error, so an exception is raised, since
         * each subclass must specify whether it is allowed to share objects of its type among users, overriding this method.
         */
        virtual uri setSharingPolicy(const std::string &actionUser, const uri &newSharingPrefs);

        virtual void store(const std::string &storePath) const = 0;

        virtual void load(const std::string &loadPath) = 0;

        virtual void send() const = 0; //not clear how to set this

        virtual std::string print(const std::string &targetUser, bool recursive = false, int indent = 0) const = 0;
        /**
         * @brief separate the last part of path which indicate the id of the resource, example: path=./1/2/3 result 1/2 and 3
         * @param path the path to divide
         * @return path to resource and id of the resource
         */
        static std::tuple<std::string, std::string> separate(const std::string &path);

        /**
         * @brief check if the path has a valid format
         * @param toCheck path to check
         * @return true if the format is ok, false instead
         */
        static bool pathIsValid2(const std::string &toCheck);

        virtual ~filesystem()= default;
    };

/**
 * @brief class used to model a file in the filesystem
 */
    class file : public filesystem {
        std::string realPath;      /**< file's path internal to the actual working directory of the system */
        document doc;              /**< document to handle */
    public:
        file(const std::string &name, const std::string &realPath, int idToAssign=0);


        const document &getDoc() const;

        /**
         * @brief retrieve the privilege of a user on the current file
         * @param targetUser the user whose privilege is to be retrieved
         * @return the privilege of @e targetUser
         */
        virtual privilege getUserPrivilege(const std::string &targetUser) const override;

        /**
         * @brief set the privilege of @e targetUser to @e newPrivilege for the current file
         * @param targetUser the user whose privilege is to be modified
         * @param newPrivilege the privilege to grant to @e targetUser
         *
         * On client side this method asks the server for privilege changing sending a message of type
         * @ref privMessage: if the message outcome from the server is positive, then confirm the action,
         * otherwise revert it.
         * On server side validate the action, perform the action and send a @ref serverMessage with the outcome.
         */
        virtual privilege setUserPrivilege(const std::string &targetUser, privilege newPrivilege) override;

        /**
         * @brief set new @e sharingPolicy for a file
         * @param actionUser the user who is performing the action
         * @param newSharingPrefs new sharing preferences for the resource
         * @return the old @e sharingPolicy
         *
         * Verifies that @e actionUser is enabled to make such an action and replace the old @e sharingPolicy
         */
        virtual uri setSharingPolicy(const std::string &actionUser, const uri &newSharingPrefs) override;

        resourceType resType() const override;

        /**
         * @brief open the document associated with the current file
         * @param targetUser the user who asked for this action
         * @param accessMode the privilege asked for the resource
         * @return the document contained in the file object
         *
         * On server side, send the document to the client that has requested it
         * checking for the privilege granted to it. Calls @ref document::access on the document.
         * On client side, request the server to send a document object and, after
         * having received it, return it to the GUI
         */
        virtual document &access(const user &targetUser, privilege accessMode);

        void store(const std::string &storePath) const override;

        void load(const std::string &loadPath) override;

        void send() const override; //TODO: check connectivity requirements

        /**
         * @brief give a textual representation of the file
         * @param targetUser the user who asked for this action
         * @param recursive for a file is meaningless
         * @param indent an optional indentation level to distinguish nested objects
         * @return a string containing the representation
         *
         * For a file, @e print(targetUser) shows the name of the file and the privilege
         * that @e targetUser has on it
         */
        std::string print(const std::string &targetUser, bool recursive = false, int indent = 0) const override;

        /**
         * @brief check if the file has other owners besides the name of the username pass as parameter
         * @param username
         * @return true if there is only one user and this user is @e username, false instead
         */

        bool moreOwner(const std::string &username);

        /**
         * @brief invoke @ref AccessStrategy::deleteUser
         * @param userName to delete from strategy object
         * @return true if the operation was successful, false instead
         */

        bool deleteFromStrategy(const std::string &userName);

        virtual ~file() override=default;

    };


    /**
     * @brief class used to model a pointer to an object of class @link file file @endlink
     *
     * In the application design only pointers to objects of class @link file file @enlink are allowed,
     * but @e pathToFile could point to a directory. As long as the mentioned behaviour is desired,
     * @e pathToFile must point to a @link file file @endlink
     */
    class symlink : public filesystem {
        std::string pathToFile;    /**< absolute path to a @e file, obtained as concatenation of @e id */
        std::string fileName;      /**< name of the file pointed, meaning its @e id */
        
    public:
        symlink(const std::string &name, const std::string &pathToFile, const std::string &fileName,
                int idToAssign=0);

        const std::string &getFileName() const;

        std::string getPath();

        resourceType resType() const override;

        /**
         * @brief access the file named @e fileName located in @e pathToFile
         *
         * Retrieves the resource indicated in parameters and call the method @ access on it.
         * The resource should be a file, because pointers to directories are not allowed in this
         * system design and pointers to symlink are meaningless.
         */
        document& access(const user &targetUser, privilege accessMode);

        void store(const std::string &storePath) const override;

        void load(const std::string &loadPath) override;

        void send() const override;

        /**
         * @brief give a textual representation of the symlink
         * @param targetUser the user who asked for this action
         * @param recursive for a symlink is meaningless
         * @param indent an optional identation level to distinguish nested objects
         * @return a string containing the representation
         *
         * For a symlink, @e print(targetUser) shows the name of the symlink and the
         * privileges granted to @e targetUser for the file pointed by the symlink
         */
        virtual std::string print(const std::string &targetUser, bool recursive = false, int indent = 0) const override;

        ~symlink() override = default;
    };


/**
 * @brief class used to model a directory, uses @e Singleton pattern
 */
    class directory : public filesystem {
    protected:
        static std::shared_ptr<directory> root;                 /**< root directory of the system */
        std::vector<std::shared_ptr<filesystem> > contained;    /**< filesystem objects contained in the directory */
        std::weak_ptr<directory> parent;                        /**< pointer to the parent directory */
        std::weak_ptr<directory> self;                          /**< pointer to itself */

    protected:
        directory(const std::string &name, const int &idToAssign=0);
    public:
        static std::shared_ptr<directory> emptyDir(); //necessary to build a new user client side
        static std::shared_ptr<directory> getRoot();

        virtual std::shared_ptr<filesystem> get(const std::string &path, const std::string &name);

        virtual std::shared_ptr<directory> getDir(const std::string &path, const std::string &name);

        virtual std::shared_ptr<file> getFile(const std::string &path, const std::string &name);

        std::string
        setName(const std::string &path, const std::string &fileName, const std::string &newName);


        virtual std::shared_ptr<directory> addDirectory(const std::string &name, int idToAssign=filesystem::idCounter);

        virtual std::shared_ptr<file> addFile(const std::string &path, const std::string &name, int idToAssign=0);

        virtual std::shared_ptr<Symposium::symlink>
        addLink(const std::string &path, const std::string &name, const std::string &filePath,
                const std::string &fileName, int idToAssign=0);

        virtual resourceType resType() const override;

        /**
         * @brief traverse the filesystem and invoke @e access on @e resName
         * @param targetUser the user who asked for this action
         * @param path relative path to the resource from the current directory
         * @param resName the name of the resource to access (file or symlink)
         * @param accessMode the privilege asked by the user for opening the file
         * @return the document contained in the file object
         */
        virtual document &
        access(const user &targetUser, const std::string &path, const std::string &resName, privilege accessMode);

        /**
         * @brief traverse the filesystem and invoke @e remove on @e resName
         * @param targetUser the user who asked for this action
         * @param path relative path to the resource from the current directory
         * @param resName the name of the resource to remove
         * @return the resource just removed from the filesystem
         *
         * Removes a file, a symlink or a directory from the current directory. The parameter
         * @e targetUser is used to authenticate the action in case of the target resource @e resName
         * is a file
         */
        virtual std::shared_ptr<filesystem>
        remove(const user &targetUser, const std::string &path, const std::string &resName);

        void store(const std::string &storePath) const override;

        void load(const std::string &loadPath) override;

        void send() const override; //not clear how to set this

        /**
         * @brief give a textual representation of the content of the current directory
         * @param targetUser the user who asked for this action
         * @param recursive indicates whether the action is to be executed recursively on subdirectories
         * @param indent an optional identation level to distinguish nested objects
         * @return a string containing the representation
         */
        virtual std::string print(const std::string &targetUser, bool recursive = true, int indent = 0) const override;
        virtual ~directory() override= default;

        /**
             * @brief separate the first part of the path which indicate the id of the directory, example: path=./1/2/3 result 1 and 2/3
             * @param path the path to divide
             * @return path that has remained and the id of the directory
             */
        static std::tuple<std::string, std::string> separateFirst(std::string path);

    private:

        /**
         * @brief print the single element, this method is used in the @ref directory::print
         * @param it the resource
         * @param targetUser the owner of the resource
         * @param indent indent if present
         * @return the string which represent the name of the resource
         */
        static std::string printElement(const std::shared_ptr<filesystem> &it, const std::string &targetUser, int indent);
    };


}
#endif //SYMPOSIUM_FILESYSTEM_H