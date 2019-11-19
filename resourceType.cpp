//
// Created by akimo on 19/11/2019.
//

#include "resourceType.h"
using namespace Symposium;

std::ostream& operator<<(std::ostream& output, resourceType type){
    switch(type){
        case resourceType ::directory: return output<<"directory";
        case resourceType::file: return output<<"file";
        case resourceType::symlink: return output<<"symlink";
    }
}