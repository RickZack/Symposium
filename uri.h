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
 * File:   uri.h
 * Project: Symposium
 * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Alice Morano <s259158 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 17 Giugno 2019, 10.40
 */
#ifndef SYMPOSIUM_URI_H
#define SYMPOSIUM_URI_H


#include <chrono>
#include "privilege.h"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "boost/serialization/binary_object.hpp"

namespace Symposium {
/**
 * @brief defines the policy on an object of class @link uri uri @endlink
 */
    enum class uriPolicy {
        inactive, activeAlways, activeCount, activeTimer
    };

/**
 * @brief class used to model resource sharing preferences
 *
 * The intend of this class is to enable users to share their own resources and obtain a privilege
 * on other's ones. The settings used in an object of this class are not retroactive, meaning that
 * a changing in a uri object does not affect users who already have a privilege on the resource
 * the uri object refers to
 */
    class uri {
        uriPolicy activePolicy;    /**< the policy currently active */
        int sharesLeft;            /**< number of shares that can still be accepted */
        std::chrono::system_clock::time_point stopTime; /**< end time of sharing */
        privilege granted;         /**< privilege that the resource owner decided to grant via uri */

        static constexpr privilege defaultPrivilege = privilege::modify;

        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int /* file_version */){
            ar & activePolicy & sharesLeft
            & boost::serialization::make_binary_object(&stopTime, sizeof(stopTime))
            & granted;
        }
    public:
        uri(uriPolicy activePolicy = uriPolicy::inactive);

        uriPolicy getActivePolicy() const;

        int getSharesLeft() const;

        privilege getGranted() const;

        /**
        * @brief operator == overload for @e uri
        *
        * @relatesalso uri
        */

        bool operator==(const uri &rhs) const;

        /**
        * @brief operator != overload for @e uri
        *
        * @relatesalso uri
        */

        bool operator!=(const uri &rhs) const;

        static const privilege getDefaultPrivilege();

        /**
         * @brief set activePolicy as activateAlways with @ref newPrivilege
         * @param newPrivilege the privilege to set
         */

        void activateAlways(privilege newPrivilege = defaultPrivilege);

        /**
         * @brief set activePolicy as activeCount with @ref newPrivilege
         * @param shares the number of possible shares
         * @param newPrivilege the privilege to set
         */

        void activateCount(int shares, privilege newPrivilege = defaultPrivilege);

        /**
         * @brief set activePolicy as activeTimer with @ref newPrivilege
         * @param endTime the end of the uri validity
         * @param newPrivilege the privilege to set
         */

        void activateTimer(std::chrono::system_clock::time_point endTime, privilege newPrivilege = defaultPrivilege);

        /**
         * @brief set activePolicy as inactive
         */
        void deactivate();

        /**
         * @brief check uri validity with requested privilege @ref requested by user
         * @param requested the privilege which user want
         * @return or requested privilege if it is <= than granted privilege or granted privilege if requested privilege is > than granted
         */
        privilege getShare(privilege requested);

        virtual ~uri()= default;
    };
}


#endif //SYMPOSIUM_URI_H
