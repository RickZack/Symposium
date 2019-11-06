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
 * File:   symbol.cpp
 * Project: Symposium
 * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Alice Morano <s259158 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 13 Giugno 2019, 16.44
 */
#include <tuple>
#include "symbol.h"
#include <iostream>
using namespace Symposium;

symbol::symbol(wchar_t ch, int siteId, int counter, const std::vector<int> &pos, bool verified) : ch(ch), siteId(siteId),
                                                                                                  counter(counter), pos(pos) {}

wchar_t symbol::getCh() const {
    return ch;
}

int symbol::getSiteId() const {
    return siteId;
}

const std::vector<int> &symbol::getPos() const {
    return pos;
}

bool symbol::operator<(const symbol &rhs) const {
    //TODO: implement
	return false;
}

bool symbol::operator>(const symbol &rhs) const {
    return rhs < *this;
}

bool symbol::operator<=(const symbol &rhs) const {
    return !(rhs < *this);
}

bool symbol::operator>=(const symbol &rhs) const {
    return !(*this < rhs);
}

bool symbol::operator==(const symbol &rhs) const {
    return ch == rhs.ch &&
           siteId == rhs.siteId &&
           counter == rhs.counter &&
           pos == rhs.pos &&
           verified == rhs.verified;
}

bool symbol::operator!=(const symbol &rhs) const {
    return !(rhs == *this);
}

symbol & symbol::setVerified() {
    symbol::verified = true;
    return *this;
}

bool symbol::isVerified() const{
    return verified;
}

std::ostream& operator<<(std::ostream& os, symbol s){
    os<<"char= "<<s.getCh()<<", siteId= "<<s.getSiteId()<<", pos= "<<"[";
    for(auto val:s.getPos())
        os<<" "<<val;
    os<<" ]"<<std::endl;
    return os;
}