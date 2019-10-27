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
 * File:   AccessStrategy.h
 * Project: Symposium
 * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Alice Morano <s259158 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 05 Luglio 2019, 16.22
 */
#ifndef SYMPOSIUM_ACCESSSTRATEGY_H
#define SYMPOSIUM_ACCESSSTRATEGY_H


#include "privilege.h"
#include <unordered_map>
#include "Symposium.h"
/*
 * OPTIMIZE: use template to implement strategy pattern
 */

namespace Symposium {
/**
 * @brief Defines how the permissions on objects of type @link filesystem are handled
 */
    class AccessStrategy {
    public:
        /**
         * @brief validate an action from user @ref targetUser that requires @ref requested
         * @param targetUser the user who is doing the action
         * @param requested the permission requested by the action
         * @return true if the user is granted the privilege @ref requested
         */
        virtual bool validateAction(user &targetUser, privilege requested) = 0;

        /**
         * @brief set the privilege of an user
         * @param targetUser the user the privilege is to be granted
         * @param toGrant the privilege to grant to @ref targetUser
         * @return the privilege previously owned by @ref targetUser, none if no privilege previously owned
         */
        virtual privilege setPrivilege(user &targetUser, privilege toGrant) = 0;

        virtual privilege getPrivilege(user &targetUser) = 0;
    };

/**
 * @brief class used to model a ReadModifyOwn privilege handling on a resource.
 */
    class RMOAccess : public AccessStrategy {
        std::unordered_map<std::string, privilege> permission; /**< username and related privilege for the resource */
    public:
        bool validateAction(user &targetUser, privilege requested) override;

        privilege setPrivilege(user &targetUser, privilege toGrant) override;

        privilege getPrivilege(user &targetUser) override;
    };


/**
 * @brief class used to model the absence of privilege handling on a resource
 */
    class TrivialAccess : public AccessStrategy {
    public:
        bool validateAction(user &targetUser, privilege requested) override;

        privilege setPrivilege(user &targetUser, privilege toGrant) override;

        privilege getPrivilege(user &targetUser) override;
    };
}
#endif //SYMPOSIUM_ACCESSSTRATEGY_H
