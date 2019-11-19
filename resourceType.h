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
 * File:   resourceType.h
 * Project: Symposium
 * Authors:
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Alice Morano <s259158 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 19 Novembre 2019, 20.50
 */

#ifndef SYMPOSIUM_RESOURCETYPE_H
#define SYMPOSIUM_RESOURCETYPE_H

#include <iostream>
#include "Symposium.h"

namespace Symposium {
    /**
     * @brief defines the type of a filesystem object
     */
    enum class resourceType : char {
        directory, file, symlink
    };
}
using namespace Symposium;
/**
 * @brief output operator for @e resType
 *
 * @relatesalso resType
 */
std::ostream& operator<<(std::ostream& output, resourceType type);


#endif //SYMPOSIUM_RESOURCETYPE_H
