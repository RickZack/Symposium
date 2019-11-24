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
#include "user.h"

using namespace Symposium;
int document::idCounter=0;

document::document(int id) : id(id) {
    id=idCounter;
    idCounter++;
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
    float mul_fct=1.5; //just to avoid too many reallocations
    symbol def_symbol('~', 0, 0, {0,0});

    if(i0>=symbols.capacity())
        symbols.resize((i0+1)*mul_fct);
    if(i1>=symbols[i0].capacity())
        symbols[i0].resize((i1 + 1) * mul_fct, def_symbol);

    char sym=symbols[i0][i1].getCh();
    if(sym=='~'){ symbols[i0][i1]=toInsert;}
    else {
        symbols[i0].insert(symbols[i0].begin() + i1, toInsert);
    }
    return symbol('z', 0, 0, {0,0});

}

symbol document::localRemove(int *indexes) {
    int i0=indexes[0];
    int i1=indexes[1];

    symbols[i0].erase(symbols[i0].begin()+i1);

    return symbol('z', 0, 0, {0,0});
}

void document::remoteInsert(const symbol &toInsert) {
    //TODO:implement
}

void document::remoteRemove(const symbol &toRemove) {
    //TODO:implement
}

std::wstring document::toText() {
    std::wstring str;
    int size= symbols.size();
    int i=0;
    int sizes= symbols[i].size();
    for (int i=0;i<symbols.size();i++){
        for(int j=0;j<symbols[i].size();j++){
          wchar_t value= symbols[i][j].getCh();
          if(value!='~')
            str=str+value;
          }

        }
   return str;
}

void document::close(const user &noLongerActive) {
    //TODO:implement
    std::forward_list<std::pair<user *, privilege>>::const_iterator iter;
    for(iter=activeUsers.begin();iter!=activeUsers.end();iter++){
        std::pair<user*,privilege> p=*iter;
        user oldUser= reinterpret_cast<const user &>(p.first);
        if(oldUser==noLongerActive)
            activeUsers.remove(p);
    }
}

void document::store(const std::string &storePath) {
    //TODO:implement
}

void document::load(const std::string &loadPath) {
    //TODO:implement
}

std::set<int> document::retrieveSiteIds() {
    std::set<int> siteIds;
    std::set<int>::iterator it=siteIds.begin();
    for(int i=0;i<symbols.size();i++){
        for(int j=0;j<symbols[i].size();j++){
            siteIds.insert(it,symbols[i][j].getSiteId());
            it++;
        }
    }
    return siteIds;
}

bool document::operator==(const document &rhs) const {
    return(this->id==rhs.id);

}

bool document::operator!=(const document &rhs) const {
    return !(rhs == *this);
}

