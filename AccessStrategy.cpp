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
 * File:   AccessStrategy.cpp
 * Project: Symposium
 * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Alice Morano <s259158 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 05 Luglio 2019, 16.22
 */
#include "AccessStrategy.h"
#include "user.h"

using namespace Symposium;

bool RMOAccess::validateAction(user &targetUser, privilege requested) {
    std::string name=targetUser.getUsername();
    std::unordered_map<std::string,privilege >::const_iterator got = permission.find(name);
    privilege attuale;
    if ( got == permission.end() )
    {
        attuale=privilege::none;
        return attuale==requested;
    }
    attuale=got->second;
    return attuale >= requested;
}

privilege RMOAccess::setPrivilege(user &targetUser, privilege toGrant) {
    std::string name=targetUser.getUsername();
    std::unordered_map<std::string,privilege >::const_iterator got = permission.find(name);
    if ( got == permission.end() )
    {
        permission.insert (std::make_pair(name, toGrant));
        return privilege::none;
    }
    privilege vecchio=got->second;
    permission.erase (name);
    permission.insert(std::make_pair(name, toGrant));
    return vecchio;
}

privilege RMOAccess::getPrivilege(user& targetUser)
{
    std::string name=targetUser.getUsername();
    std::unordered_map<std::string,privilege >::const_iterator got = permission.find(name);
    if ( got == permission.end() )
            return privilege::none;
    return got->second;
}

bool TrivialAccess::validateAction(user &targetUser, privilege requested) {
    return true;
}

privilege TrivialAccess::setPrivilege(user &targetUser, privilege toGrant) {
    return privilege::none;
}

privilege TrivialAccess::getPrivilege(user &targetUser) {
    return privilege::none;
}
