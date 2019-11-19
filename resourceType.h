//
// Created by akimo on 19/11/2019.
//

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
