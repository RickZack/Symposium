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

#include <string>
/*
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "boost/serialization/unordered_map.hpp"
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>
 */
/*
 * OPTIMIZE: use template to implement strategy pattern
 */

namespace Symposium {
/**
 * @brief Defines how the permissions on objects of type @link filesystem are handled
 */
    class AccessStrategy {
        /*
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version){};
         */
    public:
        /**
         * @brief validate an action from user @ref targetUser that requires @ref requested
         * @param targetUser the user who is doing the action
         * @param requested the permission requested by the action
         * @return true if the user is granted the privilege @ref requested
         */
        virtual bool validateAction(const std::string &targetUser, privilege requested)=0 ;

        /**
         * @brief set the privilege of an user
         * @param targetUser the user the privilege is to be granted
         * @param toGrant the privilege to grant to @ref targetUser
         * @return the privilege previously owned by @ref targetUser, none if no privilege previously owned
         */
        virtual privilege setPrivilege(const std::string &targetUser, privilege toGrant) =0;

        virtual privilege getPrivilege(const std::string &targetUser) =0;
    };
    //BOOST_SERIALIZATION_ASSUME_ABSTRACT(AccessStrategy)

    /**
     * @brief class used to model a ReadModifyOwn privilege handling on a resource.
     */
    class RMOAccess : public AccessStrategy {
        std::unordered_map<std::string, privilege> permission; /**< username and related privilege for the resource */
/*
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version)
        {
            // save/load base class information
            ar & boost::serialization::base_object<Symposium::AccessStrategy>(*this);
            ar & permission;
        }
        */
    public:
        bool validateAction(const std::string &targetUser, privilege requested) override;

        privilege setPrivilege(const std::string &targetUser, privilege toGrant) override;

        privilege getPrivilege(const std::string &targetUser) override;

        bool operator==(const RMOAccess &rhs) const;

        bool operator!=(const RMOAccess &rhs) const;
    };

    /**
     * @brief class used to model the absence of privilege handling on a resource
     */
    class TrivialAccess : public AccessStrategy {
        /*
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version)
        {
            // save/load base class information
            ar & boost::serialization::base_object<Symposium::AccessStrategy>(*this);
        }
         */
    public:
        bool validateAction(const std::string &targetUser, privilege requested) override;

        privilege setPrivilege(const std::string &targetUser, privilege toGrant) override;

        privilege getPrivilege(const std::string &targetUser) override;
    };
}
#endif //SYMPOSIUM_ACCESSSTRATEGY_H
