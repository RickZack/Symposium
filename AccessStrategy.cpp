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

bool RMOAccess::validateAction(const std::string &targetUser, privilege requested) {
    std::unordered_map<std::string,privilege >::const_iterator got = permission.find(targetUser);
    privilege attuale;
    if ( got == permission.end() )
    {
        attuale=privilege::none;
        return attuale==requested;
    }
    attuale=got->second;
    return attuale >= requested;
}


privilege RMOAccess::setPrivilege(const std::string &targetUser, privilege toGrant) {
    if(permission.empty() && toGrant==privilege::owner)
        permission.insert (std::make_pair(targetUser, toGrant));
    std::unordered_map<std::string,privilege >::const_iterator got = permission.find(targetUser);
    if ( got == permission.end() )
    {
        permission.insert (std::make_pair(targetUser, toGrant));
        return privilege::none;
    }
    privilege vecchio=got->second;
    permission.erase (targetUser);
    permission.insert(std::make_pair(targetUser, toGrant));
    return vecchio;
}

privilege RMOAccess::getPrivilege(const std::string &targetUser)
{
    std::unordered_map<std::string,privilege >::const_iterator got = permission.find(targetUser);
    if ( got == permission.end() )
            return privilege::none;
    return got->second;
}

bool RMOAccess::moreOwner(std::string username)
{
    int i=0;
    bool own=false;
    for(auto& tuple : permission)
    {
        if(tuple.second==privilege::owner)
        {
            i++;
            if(tuple.first==username)
                own=true;
        }
    }

    return ((i > 1)&&own);
}

bool RMOAccess::deleteUser(const std::string &targetUser)
{
    auto it=permission.find(targetUser);
    if(it==permission.end())
        return false;
    permission.erase(targetUser);
    return true;
}

bool RMOAccess::operator==(const RMOAccess &rhs) const {
    return permission == rhs.permission;
}

bool RMOAccess::operator!=(const RMOAccess &rhs) const {
    return !(rhs == *this);
}

bool TrivialAccess::validateAction(const std::string &targetUser, privilege requested) {
    return true;
}

privilege TrivialAccess::setPrivilege(const std::string &targetUser, privilege toGrant) {
    return privilege::none;
}

privilege TrivialAccess::getPrivilege(const std::string &targetUser) {
    return privilege::none;
}

bool TrivialAccess::moreOwner(std::string username){
    return false;
}

bool TrivialAccess::deleteUser(const std::string &targetUser)
{
    return true;
}
BOOST_CLASS_EXPORT(Symposium::RMOAccess)