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

#include <c++/8.2.0/sstream>
#include "document.h"
#include "symbol.h"
#include "user.h"

using namespace Symposium;
int document::idCounter=0;
const symbol document::emptySymbol(emptyChar, 0, 0, {0, 0});

document::document(int id) : id(id), symbols(10,std::vector<symbol>(10,emptySymbol)) {
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

//FIXME: discuss to change the forward list to <const user*, privilege>,
// avoid casts as much as possible
document & document::access(const user &newActive, privilege accessPriv) {
    std::pair<user*, privilege> p{const_cast<user*>(&newActive),accessPriv};
    activeUsers.push_front(p);
    return *this;
}


symbol document::localInsert(int *indexes, symbol &toInsert) {

    int i0=indexes[0];
    int i1=indexes[1];
    float mul_fct=1.5; //just to avoid too many reallocations

    if(i0>=symbols.capacity())
        symbols.resize((i0+1)*mul_fct);
    if(i1>=symbols[i0].capacity())
        symbols[i0].resize((i1 + 1) * mul_fct, emptySymbol);

    generatePosition(indexes);
    char sym=symbols[i0][i1].getCh();

    if(sym==emptyChar){ symbols[i0][i1]=toInsert;}
    else {
        symbols[i0].insert(symbols[i0].begin() + i1, toInsert);
    }

    return toInsert;

}

void document::generatePosition(int *indexes) {
    int i0=indexes[0];
    int i1=indexes[1];
    int c=symbols[i0].size();

    // vectors that maintain the position for all the rows of symbols.
    std::vector<int> posA;
    std::vector<int> posB;

    int siteIdB; int siteIdA;

    if(!symbols.empty()) {
        posA = symbols[i0][i1].getPos();
    }

        else if (i1 >= symbols[i0].size()) {
            int ind_new = symbols[i0].size() - 1;
            posB = symbols[i0][ind_new].getPos();
        } else {
            // I have to generate the pos after and the pos before
            int i1_new = i1 - 1;
            posB = symbols[i0][i1_new].getPos();
            siteIdB = symbols[i0][i1_new].getSiteId();

            posA = symbols[i0][i1].getPos();
            siteIdA = symbols[i0][i1].getSiteId();

        }


}

symbol document::localRemove(int *indexes) {
    int i0=indexes[0];
    int i1=indexes[1];

    symbol sym=symbols[i0][i1];
    symbols[i0].erase(symbols[i0].begin()+i1);

    return sym;
}

void document::remoteInsert(const symbol &toInsert) {
    int *indexes=findInsertIndex(toInsert);
    int i0=indexes[0];
    int i1=indexes[1];
    float mul_fct=1.5; //just to avoid too many reallocations

    if(i0>=symbols.capacity())
        symbols.resize((i0+1)*mul_fct);
    if(i1>=symbols[i0].capacity())
        symbols[i0].resize((i1 + 1) * mul_fct, emptySymbol);

    generatePosition(indexes);
    char sym=symbols[i0][i1].getCh();

    if(sym==emptyChar){ symbols[i0][i1]=toInsert;}
    else {
        symbols[i0].insert(symbols[i0].begin() + i1, toInsert);
    }
}

void document::remoteRemove(const symbol &toRemove) {
    //TODO:implement
}

std::wstring document::toText() {
    std::wstring str;
    std::wostringstream str1;
    int size= symbols.size();
    int i=0;
    int sizes= symbols[i].size();
    for (int i=0;i<symbols.size();i++) {
        for (int j = 0; j < symbols[i].size(); j++) {
            wchar_t value = symbols[i][j].getCh();
            if (value != emptyChar)
                str1.put(value);

        }
    }
    str=str1.str();
   return str;
}

void document::close(const user &noLongerActive) {
    auto first = activeUsers.begin();
    auto last = activeUsers.end();

    while (first != last) {
        std::pair<user *, privilege> p = *first;
        user old_User = *p.first;
        if (old_User == noLongerActive) {
            activeUsers.remove(p);
            first++;
        } else {
            first++;
        }
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



int * document::findInsertIndex(const symbol &symbol) {
    int *indixes;
    int i0=0; int i1=0;
    int minLine=0;
    int totalLines=symbols.size();
    int maxLine= totalLines-1;
    std::vector<Symposium::symbol> lastLine= symbols[maxLine];

    int midLine=0,charIdx=0,minLastChar=0, maxLastChar=0;
    std::vector<Symposium::symbol> maxCurrentLine;
    std::vector<Symposium::symbol> minCurrentLine;
    std::vector<Symposium::symbol> currentLine;
    char lastChar;

    // check if struct is empty or char is less than first char
    if (symbols.empty()||symbol.getCh()<symbols[0][0].getCh()) {indixes[0]=0; indixes[1]=0; return indixes;}

    lastChar=lastLine[lastLine.size()-1].getCh();

    //char is greater than all existing chars (insert and end)

    if(symbol.getCh()>lastChar){
        indixes= findEndPosition(lastChar,lastLine,totalLines);
        return indixes;
    }

    //binary search
    while(minLine+1<maxLine){
        midLine=minLine+(maxLine-minLine)/2;
        currentLine=symbols[midLine];
        lastChar=currentLine[currentLine.size()-1].getCh();

        if(symbol.getCh()==lastChar){
            i0=midLine; i1=currentLine.size()-1;
        } else if(symbol.getCh()<lastChar){
            maxLine=midLine;
        } else{
            minLine=midLine;
        }
    }

    //check between min and max line
    minCurrentLine=symbols[minLine];
    minLastChar=minCurrentLine[minCurrentLine.size()-1].getCh();
    maxCurrentLine=symbols[maxLine];
    maxLastChar=maxCurrentLine[maxCurrentLine.size()-1].getCh();

    if(symbol.getCh()<=minLastChar){
        charIdx=findInsertInLine(symbol.getCh(),minCurrentLine);
        i0=minLine; i1=charIdx;
        indixes[0]=i0; indixes[1]=i1;
        return indixes;

    } else{
        charIdx=findInsertInLine(symbol.getCh(),maxCurrentLine);
        i0=maxLine; i1=charIdx;
        indixes[0]=i0; indixes[1]=i1;
        return indixes;
    }

}

int* document::findEndPosition(char aChar, std::vector<Symposium::symbol> vector, int lines) {
    int *indixes;
    if(reinterpret_cast<const char *>(aChar) == "\n"){
        indixes[0]=lines; indixes[1]=0; return indixes;
    } else{
        indixes[0]=lines-1; indixes[1]=vector.size();
    }
    return indixes;

}

int document::findInsertInLine(wchar_t ch, std::vector<Symposium::symbol> vector) {
    int ind=0;
    int left=0;
    int right= vector.size()-1;
    int mid;

    if(vector.size()==0 ||ch>vector[left].getCh()){
        ind=left;
        return ind;
    } else if(ch>vector[right].getCh()){
        ind=vector.size();
        return ind;
    }

    while(left+1<right){
        mid=left-(right-left)/2;

        if(ch==vector[mid].getCh()){
            id=mid; return id;
        } else if(ch>vector[mid].getCh()){
            left=mid;
        } else{
            right=mid;
        }
    }

    if(ch==vector[left].getCh()){
        id=left; return id;
    } else{
        id=right; return right;
    }

}




