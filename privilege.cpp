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
 * File:   privilege.cpp
 * Project: Symposium
 * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Alice Morano <s259158 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 07 agosto 2019, 15:46
 */
#include <iostream>
#include "privilege.h"
using namespace Symposium;

bool Symposium::operator<(const privilege a, const privilege b){
    return (static_cast<char>(a)- static_cast<char>(b))<0;
}


bool Symposium::operator>(const privilege a, const privilege b){
    return (static_cast<char>(a)- static_cast<char>(b))>0;
}


bool Symposium::operator==(const privilege a, const privilege b){
    return (static_cast<char>(a)- static_cast<char>(b))==0;
}

bool Symposium::operator<=(const privilege a, const privilege b)
{
    return a == b || a < b;
}


privilege Symposium::operator++(privilege& oldPriv, int){
    privilege ret{oldPriv};
    switch(oldPriv){
        case privilege::readOnly: oldPriv=privilege::modify; break;
        case privilege::modify:   oldPriv=privilege::owner; break;
        default: ;
    }
    return ret;
}


privilege Symposium::operator--(privilege& oldPriv, int){
    privilege ret{oldPriv};
    switch(oldPriv){
        case privilege::modify:   oldPriv=privilege::readOnly; break;
        case privilege::owner:    oldPriv=privilege::modify; break;
        default:                  ;
    }
    return ret;
}


privilege& Symposium::operator++(privilege& oldPriv){
    switch(oldPriv){
        case privilege::readOnly: return oldPriv=privilege::modify;
        case privilege::modify:   return oldPriv=privilege::owner;
        default:                  return oldPriv;
    }
}


privilege& Symposium::operator--(privilege& oldPriv){
    switch(oldPriv){
        case privilege::modify:   return oldPriv=privilege::readOnly;
        case privilege::owner:    return oldPriv=privilege::modify;
        default:                  return oldPriv;
    }
}


std::ostream& Symposium::operator<<(std::ostream& output, privilege priv){
    switch(priv){
        case privilege::readOnly: return output<<"reader";
        case privilege::modify: return output<<"writer";
        case privilege::owner: return output<<"owner";
        default: return output<<"none";
    }
}

