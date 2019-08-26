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
#include <set>

class user;

/*
 * OPTIMIZE: use template to implement strategy pattern
 */
class AccessStrategy {
public:
    virtual bool validateAction(privilege requested)=0;
    virtual privilege setPrivilege(user& targetUser, privilege toGrant)=0;
};

/**
 * @brief class used to model a ReadModifyOwn privilege handling on a resource.
 */
class RMOAccess: public AccessStrategy{
    std::set<user*> owners;    /**< set of users with the owner's privilege for the resource */
    std::set<user*> writers;   /**< set of users with the writer's privilege for the resource */
    std::set<user*> readers;   /**< set of users with the reader's privilege for the resource */
public:
    bool validateAction(privilege requested) override;
    privilege setPrivilege(user &targetUser, privilege toGrant) override;
};

class TrivialAccess: public AccessStrategy{
public:
    bool validateAction(privilege requested) override;

    privilege setPrivilege(user &targetUser, privilege toGrant) override;
};

#endif //SYMPOSIUM_ACCESSSTRATEGY_H
