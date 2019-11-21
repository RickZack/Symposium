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
 * File:   document.cpp
 * Project: Symposium
 * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Alice Morano <s259158 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 22 Giugno 2019, 12.10
 */
#include "document.h"
#include "symbol.h"
using namespace Symposium;
int document::idCounter=0;

document::document(int id) : id(id) {
    //TODO: implement
}

int document::getId() const {
    return id;
}

const std::vector<std::vector<symbol>> &document::getSymbols() const {
    return symbols;
}

const std::forward_list<std::pair<user *, privilege>> &document::getActiveUsers() const {
    return activeUsers;
}

int document::getNumchar() const {
    return numchar;
}

document & document::access(const user &newActive, privilege accessPriv) {
    std::pair<user*, privilege> p{const_cast<user*>(&newActive),accessPriv};
    activeUsers.push_front(p);
    return *this;
}


symbol document::localInsert(int *indexes, symbol &toInsert) {
    int i0=indexes[0];
    int i1=indexes[1];
    symbol def_symb (' ', 0, 0, {0,0});

    //vector is empty or i0 is greater than its dimension
    if(symbols.empty()|| i0>symbols.size()) {
        symbols.resize(i0+1);
        symbols[i0].resize(i1+1,def_symb);
       // symbols[i0].reserve(i1+1);
        std::vector<symbol> s=symbols.at(i0);
       // symbols.erase(symbols.begin()+i0);
        s.insert(s.begin()+i1,toInsert);
        symbols.insert(symbols.begin()+i0,s);
        }

    else {
        std::vector<symbol> s=symbols.at(i0);
        if(i1<s.size()+1){

          s.resize(i1+1,def_symb);
          //s.reserve(i1+1);
        }

        s.insert(s.begin()+i1,toInsert);
        symbols.insert(symbols.begin()+i0,s);
    }

    return symbol('z', 0, 0, {0,0});
}

symbol document::localRemove(int *indexes) {
    int i0=indexes[0];
    int i1=indexes[1];

    std::vector<symbol> s= symbols.at(i0);
    s.erase(s.begin()+i1);
    symbols.insert(symbols.begin()+i0,s);

    return symbol('z', 0, 0, {0,0});
}

void document::remoteInsert(const symbol &toInsert) {
    //TODO:implement
}

void document::remoteRemove(const symbol &toRemove) {
    //TODO:implement
}

std::wstring document::toText() {
    //TODO:implement
    return std::wstring();
}

void document::close(const user &noLongerActive) {
    //TODO:implement

}

void document::store(const std::string &storePath) {
    //TODO:implement
}

void document::load(const std::string &loadPath) {
    //TODO:implement
}

std::set<int> document::retrieveSiteIds() {
    //TODO:implement
    return std::set<int>();
}

bool document::operator==(const document &rhs) const {
    return(this->id==rhs.id);

}

bool document::operator!=(const document &rhs) const {
    return !(rhs == *this);
}

