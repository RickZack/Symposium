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
#include <iostream>
#include <boost/serialization/export.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include "symbol.h"

using namespace Symposium;

void symbol::setCharFormat(const format &value)
{
    charFormat = value;
}

format symbol::getCharFormat() const
{
    return charFormat;
}

symbol::symbol(wchar_t ch, int siteId, int counter, const std::vector<int> &pos, bool verified) : ch(ch), siteId(siteId),
                                                                                                  counter(counter), pos(pos), verified(verified) {}
template<class Archive>
void format::serialize(Archive &ar, const unsigned int){
    ar & familyType & isBold & isUnderlined & isItalic & size & col &indexStyle &type;
};
BOOST_CLASS_EXPORT(Symposium::format);

template<class Archive>
void symbol::serialize(Archive &ar, const unsigned int){
    ar & ch & siteId & counter & pos & verified & charFormat;
};
BOOST_CLASS_EXPORT(Symposium::symbol);

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
    auto size=std::min(this->pos.size(), rhs.pos.size());
    for(unsigned long long i=0; i<size; i++) {
        if (this->pos[i] < rhs.pos[i])
            return true;
        if (this->pos[i] > rhs.pos[i])
            return false;
    }
    if(this->pos.size()==rhs.pos.size()){
            if(this->siteId < rhs.siteId)
                return true;
            else
                return false;
        }
    else if(this->pos.size()>rhs.pos.size())
            return false;
  return true;
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

int symbol::getCounter() const {
    return counter;
}


std::ostream& operator<<(std::ostream& os, symbol s){
    os<<"char= "<<s.getCh()<<", siteId= "<<s.getSiteId()<<", pos= "<<"[";
    for(auto val:s.getPos())
        os<<" "<<val;
    os<<" ]"<<std::endl;
    return os;
}
