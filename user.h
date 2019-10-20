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
#include "privilege.h" //for privileges on the resource
#include "filesystem.h"
#include "document.h"

//TODO: complete information about the functions: do they throw exceptions? What is the exception safety?

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
    int siteId;                        /**< unique identifier for the user, used in CRDT logic */
    std::string nickname;              /**< name chosen by the user to be showed to other users */
    std::string iconPath;              /**< path to the user's icon in program installation folder */
    std::shared_ptr<directory> home;   /**< user's virtual filesystem */
public:
    user()= default;
    user(const std::string &username, const std::string &pwd, const std::string &nickname, const std::string &iconPath,
         int siteId, std::shared_ptr<directory> home);

    const std::string &getUsername() const;
    int getSiteId() const;
    const std::string &getNickname() const;
    const std::string &getIconPath() const;

    const std::shared_ptr<directory> &getHome() const;

    void setPwd(const std::string &pwd);
    void setNickname(const std::string &nickname);
    void setSiteId(int siteId);
    void setIconPath(const std::string &iconPath);
    void setHome(std::shared_ptr<directory> home);
    bool hasPwd(const std::string pwd);
    bool operator==(const user &rhs) const;

    bool operator!=(const user &rhs) const;

    /**
     * @brief shows the entire user's home directory
     * @param recursive indicates whether the visit must be recursive or not
     * @return a concatenation string of the content of the home directory to be showed by the GUI
     */
    std::string showDir(bool recursive = false) const;

    /**
     * @brief creates a new file named @e fileName in @e pathFromHome
     * @param fileName name to be assigned to the new file
     * @param pathFromHome path inside the home to put the file. By default is the home itself
     */
    virtual std::shared_ptr<file> newFile(const std::string& fileName, const std::string& pathFromHome= ".") const;

    /**
     * @brief creates a new directory named @e dirName in @e pathFromHome
     * @param dirName name to be assigned to the new directory
     * @param pathFromHome path inside the home to put the directory. By default is the home itself
     */
    virtual std::shared_ptr<directory> newDirectory(const std::string& dirName, const std::string& pathFromHome= ".") const;

    /**
     * @brief adds a link to the resource for which the user has been granted a privilege
     * @param resId the uri of the resource to be linked by the symlink
     * @param path the path to put the resource into
     * @param fileName the name of the new link
     * @return the file just added
     */
    virtual std::shared_ptr<file> accessFile(const std::string &resId, const std::string &path,  const std::string &fileName = "") const;

    /**
     * @brief open a @link file file @endlink already present in the user's filesystem
     * @param path location of the file relative to the @e home directory
     * @param fileName name of the file to be opened
     * @param accessMode the privilege asked by the user for opening the file
     * @return the document contained in the file
     *
     * This method calls the @ref directory::access method on @e home passing the current user as the one asking the action
     */
    virtual document openFile(const std::string &path, const std::string &fileName, privilege accessMode) const;

    /**
     * @brief edit the privilege of @e otherUser user for the resource @e resName in @e resPath to @e newPrivilege
     * @param otherUser the user whose privilege has to be modified
     * @param resPath relative path to the resource from the current user's @e home
     * @param resName the name of the target resource
     * @param newPrivilege the new privilege to be granted to @e targetUser
     * @return the old privilege of @e targetUser had on the resource
     * @warning the current user must be a owner of the target resource
     *
     * This method calls @ref directory::getFile method on the @e home, retrieving the file, and calls
     * @ref file::setUserPrivilege passing @e otherUser as the one on which take action
     */
    privilege editPrivilege(const user &otherUser, const std::string &resPath, const std::string &resName,
                            privilege newPrivilege);

    /**
     * @brief edit the privilege of the current user for the resource @e resName in @e resPath
     * @param resPath the path to the target resource
     * @param resName name of the target resource
     * @param newPrivilege newPrivilege the new privilege to be granted
     * @return @e newPrivilege
     * @warning the current user must be a owner of the target resource
     *
     * This method calls @ref directory::getFile method on the @e home, retrieving the file, and calls
     * @ref file::setUserPrivilege passing the current user as the one on which take action
     */
    privilege changePrivilege(const std::string &resPath, const std::string &resName, privilege newPrivilege);

    /**
     * @brief set new sharing preferences for a resource
     * @param resPath the absolute path of the resource
     * @param resName the name of the resource
     * @param newPrefs new sharing preferences for the resource
     * @return the old @e sharingPolicy
     *
     * Calls @ref directory::getFile on @e home and then file::setSharingPolicy on the retrieved file
     */
    uri shareResource(const std::string &resPath, const std::string &resName, uri& newPrefs);

    /**
  * @brief removes a file object given a condition on it
  * @tparam C a function or an object function that represent the condition
  * @param path the path inside the user's home directory where the target file is located
  * @param name the file name
  * @param condition a condition that is to be meet to delete the target file
  * @return the file just removed from the user's filesystem
  */
    template <typename C>
    std::shared_ptr<file> deleteFile(const std::string &path, const std::string &name, C condition){
        std::shared_ptr<file> newF(nullptr);
        newF=std::dynamic_pointer_cast<file>(home->remove(*this, path, name));
        return newF;

    }

    /**
     * @brief removes a file object given a condition on it
     * @tparam C a function or an object function that represent the condition
     * @param path path the path inside the user's home directory where the target directory is located
     * @param name the directory name
     * @param condition a condition that is to be meet to delete the target directory
     */
    template <typename C>
    std::shared_ptr<directory> deleteDirectory(const std::string &path, const std::string &name, C condition){
        std::shared_ptr<directory> newD(nullptr);
        newD=std::dynamic_pointer_cast<directory>(home->remove(*this, path, name));
        return newD;

    }
};


#endif //SYMPOSIUM_USER_H