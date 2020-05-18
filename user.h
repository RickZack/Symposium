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
 * File:   user.h
 * Project: Symposium
 * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Alice Morano <s259158 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 17 Giugno 2019, 22.41
 */
#ifndef SYMPOSIUM_USER_H
#define SYMPOSIUM_USER_H

#include <string>
#include <memory>
#include <random>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <tuple>

#include "Symposium.h"
#include "lib/hash/sha256.h"


namespace Symposium {
/**
 * @brief class used to model a user of the system
 *
 * A user is identified by its @e username, that is used in login step, and by a @e siteId, that is
 * a number given by server the user is hosted by, used in the CRDT algorithm to distinguish @link
 * symbol symbols @endlink from different users. For each user the system store a cryptographic hash
 * value of its password, to ensure more privacy and security.
 * Each user has a reference to a part of the server's virtual filesystem,a directory under "/" called
 * as the username. The server and the clients share the same set of icons, so @e iconPath is the common
 * path to icons for the server and the client.
 * When an @e incomplete user is built client side to be sent inside a @ref signUpMessage, @e pwdHash
 * contains the plaintext password, @e hashSalt, @e siteId and @e home are empty.
 */
    class user {
        std::string username;              /**< unique identifier for the user, used also for login */
        std::string pwdHash;               /**< user password's hash value */
        std::string hashSalt;              /**< random generated string, used as a salt to @e pwdHash */
        uint_positive_cnt::type siteId;    /**< unique identifier for the user, used in CRDT logic */
        std::string nickname;              /**< name chosen by the user to be showed to other users */
        std::string iconPath;              /**< path to the user's icon in program installation folder */
        std::shared_ptr<directory> home;   /**< user's virtual filesystem */

        static constexpr char noChar[] ="1234567890?!$+-/.,@ˆ_ ";
        static constexpr char noNum[] ="abcdefghijklmnopqrstuvwxyz?!$+-/.,@ˆ_ ";
        static constexpr char noSpecialChar[] ="abcdefghijklmnopqrstuvwxyz1234567890 ";

        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version);

    public:
        user() = default;

        /**
      * @brief creates a new user
      * @throws @ref userException if the password is not chosen appropriately
      * @return the pointer to the file just created
      */
        user(const std::string &username, const std::string &pwd, const std::string &nickname,
             const std::string &iconPath,
             uint_positive_cnt::type siteId, std::shared_ptr<directory> home);

        const std::string &getUsername() const;

        uint_positive_cnt::type getSiteId() const;

        const std::string &getNickname() const;

        const std::string &getIconPath() const;

        const std::shared_ptr<directory> &getHome() const;

        const std::string &getPwdHash() const;

        void setPwd(const std::string &pwd);

        void setNickname(const std::string &nickname);

        void setSiteId(uint_positive_cnt::type siteId);

        void setIconPath(const std::string &iconPath);

        void setHome(std::shared_ptr<directory> home);

        virtual void setNewData(const user &newData);

        virtual bool hasPwd(const std::string &pwd) const;

        bool operator==(const user &rhs) const;

        bool operator!=(const user &rhs) const;

        /**
         * @brief shows the entire user's home directory
         * @param recursive indicates whether the visit must be recursive or not
         * @return a concatenation string of the content of the home directory to be showed by the GUI
         */
        virtual std::string showDir(bool recursive = false) const;

        /**
         * @brief creates a new file named @e resId in @e pathFromHome
         * @param resName name to be assigned to the new file
         * @param resPath path inside the home to put the file. By default is the home itself
         * @return the pointer to the file just created
         */
        virtual std::shared_ptr<file>
        newFile(const std::string &resName, const std::string &resPath= ".", uint_positive_cnt::type idToAssign= 0) const;

        /**
         * @brief creates a new directory named @e dirName in @e pathFromHome
         * @param resName name to be assigned to the new directory
         * @param resPath path inside the home to put the directory. By default is the home itself
         * @return the pointer to the directory just created
         */

        virtual std::shared_ptr<directory>
        newDirectory(const std::string &resName, const std::string &resPath = ".", uint_positive_cnt::type idToAssign= 0) const;

        /**
         * @brief adds a link to the resource for which the user has been granted a privilege
         * @param absolutePath the uri of the resource to be linked by the symlink
         * @param destPath the path to put the resource into
         * @param destName the name of the new link
         * @throws @ref userException "User don't have any privilege to this file anymore"
         * @return the file just added
         */
        virtual std::pair<int, std::shared_ptr<file>>
        accessFile(const std::string &absolutePath, const std::string &destPath, const std::string &destName = "") const;

        /**
         * @brief open a @link file file @endlink already present in the user's filesystem
         * @param resPath location of the file relative to the @e home directory
         * @param resId id of the file to be opened
         * @param accessMode the privilege asked by the user for opening the file
         * @return the document contained in the file
         *
         * This method calls the @ref directory::access method on @e home passing the current user as the one asking the action
         */
        virtual std::shared_ptr<file> openFile(const std::string &resPath, const std::string &resId, privilege accessMode) const;

        /**
         * @brief edit the privilege of @e otherUser user for the resource @e resName in @e resPath to @e newPrivilege
         * @param otherUser the user whose privilege has to be modified
         * @param resPath relative path to the resource from the current user's @e home
         * @param resId the id of the target resource
         * @param newPrivilege the new privilege to be granted to @e targetUser
         * @throws @ref userException "User cannot upgrade this privilege"
         * @return the old privilege of @e targetUser had on the resource
         * @warning the current user must be a owner of the target resource
         *
         * This method calls @ref directory::getFile method on the @e home, retrieving the file, and calls
         * @ref file::setUserPrivilege passing @e otherUser as the one on which take action
         */
        virtual privilege editPrivilege(const std::string &otherUser, const std::string &resPath, const std::string &resId,
                                        privilege newPrivilege) const;



        /**
         * @brief set new sharing preferences for a resource
         * @param resPath the relative path of the resource
         * @param resId the id of the resource
         * @param newPrefs new sharing preferences for the resource
         * @return the old @e sharingPolicy
         *
         * Calls @ref directory::getFile on @e home and then file::setSharingPolicy on the retrieved file
         */
        virtual std::shared_ptr<filesystem> shareResource(const std::string &resPath, const std::string &resId, const uri &newPrefs) const;

        /**
         * @brief renames a resource
         * @param resPath the relative path to the @e home directory where to find the resource
         * @param resId the resource's id
         * @param newName the new resource's name
         * @return the resource just renamed
         *
         * Changes the attribute @e name of the filesystem object.
         * Please note that, in case the filesystem object is a symlink, this method renames the symlink, not the resource pointed.
         */
        virtual std::shared_ptr<filesystem>
        renameResource(const std::string &resPath, const std::string &resId, const std::string &newName) const;

        /**
         * @brief removes a filesystem object
         * @param resPath the path inside the user's home directory where the target file is located
         * @param resId the file id
         * @return the filesystem object just removed from the user's filesystem
         */
        virtual std::shared_ptr<filesystem> removeResource(const std::string &resPath, const std::string &resId) const;

        /**
         * @brief constructs a copy of the current user object clearing the @e pwdHash and the @e hashSalt
         * @return the constructed copy
         */
        user makeCopyNoPwd() const;
        /**
         * @param pass the password to control
         * @return true if the @pass don't have any alphabetic character and false if it does
          */
        static bool noCharPwd(const std::string &pass);

        /**
         * @param pass the password to control
         * @return true if the @pass don't have any number and false if it does
         */
        static bool noNumPwd(const std::string &pass);

        /**
         * @param pass the password to control
         * @return true if the @pass don't have any special character and false if it does
         */
        static bool noSpecialCharPwd(const std::string &pass);
    private:
        /**
         * @brief generate random salt for password
         * @return salt
         */
        static std::string saltGenerate();



    };
}
#endif //SYMPOSIUM_USER_H