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
 * File:   messageData.h
 * Project: Symposium
 * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Alice Morano <s259158 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 07 agosto 2019, 13:14
 */
#ifndef SYMPOSIUM_MESSAGEDATA_H
#define SYMPOSIUM_MESSAGEDATA_H

#include "Symposium.h"

namespace Symposium {
/**
 * @brief defines the possible outcomes of an operation followed by a message
 */
//TODO: specify more details about "success" and "failure", like messages that can be showed to the user or stored in a log file
    enum class msgOutcome : char {
        failure, success
    };
/**
 * @brief defines the possible type of messages, that corresponds to an action on server or client
 */
    enum class msgType {
        registration,         /**< used by client, when asking for a new user registration, see @ref signUpMessage */
        login,                /**< used by server and client, when asking or answering for login of existing user, see @ref clientMessage, @ref loginMessage */
        changeUserData,       /**< used by server and client, when asking for changing the data of an user, see @ref userDataMessage */
        changeUserPwd,        /**< used by server and client, when asking for changing the data of an user, see @ref userDataMessage */
        removeUser,           /**< used by client, when asking to remove the user it is logged with, see @ref clientMessage */
        logout,

        createRes,            /**< used by server and client, when asking for creating a new document, see @ref sendResMessage, @ref askResMessage */
        createNewDir,         /**< used by server and client, when asking for creating a new directory, see @ref sendResMessage, @ref askResMessage */
        openRes,              /**< user by client, when asking for opening a document that is already in its filesystem,; see @ref askResMessage */
        openNewRes,           /**< used by server and client, when asking for an existing resource via URI, @ref sendResMessage, @ref askResMessage */
        changeResName,        /**< used by server and client, when asking for changing a resource's name, see @ref serverMessage, @ref askResMessage */
        removeRes,            /**< used by client, when asking for a resource removal, @ref serverMessage, @ref askResMessage */

        mapChangesToUser,     /**< used by server and client, when asking for a siteId->username mapping, see @ref mapMessage, @ref updateDocMessage */
        changePrivileges,     /**< used by server and client, when asking for updating or propagating privilege changes, see @ref privMessage */
        shareRes,             /**< used by server and client, when asking for updating or propagating sharing changes, see @ref uriMessage */
        insertSymbol,         /**< used by server and client, when asking for updating or propagating the insertion, see @ref symbolMessage */
        removeSymbol,         /**< used by server and client, when asking for updating or propagating the deletion, see @ref symbolMessage */

        addActiveUser,        /**< used by server, for propagating the document opening of an user, see @ref updateActiveMessage */
        removeActiveUser,     /**< used by server, for propagating the document closing of an user, see @ref updateActiveMessage */
        closeRes,             /**< used by client, when a user wants to close a document, see @ref updateDocMessage */

        updateCursor,         /**< used by client, when a user changes the cursor's position, see @ref cursorMessage */
        editLineStyle         /**< used by client, when changing alignment and/or index style of a paragraph, see @ref editLineStyleMessage*/
    };
}
#endif //SYMPOSIUM_MESSAGEDATA_H
