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

#include <sstream>
#include "document.h"
#include "symbol.h"
#include "user.h"
#include <cmath>



using namespace Symposium;
int document::idCounter=0;
const symbol document::emptySymbol(emptyChar, 0, 0, {0, 0});

//FIXME: valore di default di id ignorato, potenziale errore
document::document(int id) : id(id), symbols(1,std::vector<symbol>(1,emptySymbol)) {
    id=idCounter;
    idCounter++;

}

int document::getId() const {
    return id;
}

const std::vector<std::vector<symbol>> &document::getSymbols() const {
    return symbols;
}

const std::forward_list<std::pair<const user *, sessionData>> &document::getActiveUsers() const {
    return activeUsers;
}

int document::getNumchar() const {
    return numchar;
}

document & document::access(const user &newActive, privilege accessPriv) {
    if(std::find_if(activeUsers.begin(), activeUsers.end(), [&](auto p){return p.first->getSiteId()==newActive.getSiteId();})==activeUsers.end()) {
        std::pair<const user *, sessionData> p{&newActive, accessPriv};
        activeUsers.push_front(p);
    }
    return *this;
}

void document::checkIndex(int i0, int i1) {
    float mul_fct=1.5; //just to avoid too many reallocations
    if(i0>=symbols.capacity())
        symbols.resize((i0+1)*mul_fct);
    if(i1>=symbols[i0].capacity())
        symbols[i0].resize((i1 + 1) * mul_fct, emptySymbol);
}


symbol document::localInsert(const std::pair<int, int> &indexes, symbol &toInsert) {
    //TODO: take into account new position of cursor
    int i0=indexes.first;
    int i1=indexes.second;
    checkIndex(i0,i1);

    symbol newSymb= generatePosition(indexes,toInsert);
    char sym=symbols[i0][i1].getCh();

    if(sym==emptyChar){
        //symbols[i0][i1]=toInsert;
        symbols[i0][i1]=newSymb;
    }
    else {
        //symbols[i0].insert(symbols[i0].begin() + i1, toInsert);
        symbols[i0].insert(symbols[i0].begin() + i1, newSymb);
        }

    return newSymb;
}

symbol document::generatePosition(const std::pair<int, int> indexes,const symbol &toInsert){
    std::vector<int> posBefore;
    int siteIdB;
    symbol symB=findPosBefore(indexes);
    if(symB==emptySymbol){
        siteIdB=-1;
    }else{
        posBefore.push_back(symB.getPos().front());
        siteIdB=symB.getSiteId();
    }

    std::vector <int> posAfter;
    int siteIdA;
    symbol symA=findPosAfter(indexes);
    if(symA==emptySymbol){
        siteIdA=-1;
    }else{
        posAfter.push_back(symA.getPos().front());
        siteIdA=symB.getSiteId();
    }
    int level=0;
    std::vector<int> inPos;
    std::vector<int> newPos= generatePosBetween(posBefore, posAfter, inPos, level, siteIdB, siteIdA);

    symbol newSymbol(toInsert.getCh(),toInsert.getSiteId(),toInsert.getCounter(),newPos,false);
    return newSymbol;
}




symbol document::findPosBefore(const std::pair<int, int> indexes) const {

    int line=indexes.first;
    int ch=indexes.second;

    // I don't have position before the considered one
    // FIRST LIMIT CASE
    if(ch==0 && line==0){
        symbol sym=emptySymbol;
        return sym;
    }
        // SECOND LIMIT CASE: I have to change line
    else if(ch==0 && line!=0){
        line=line-1;
        ch=symbols[line].size();
    }else{
        ch=ch-1;
    }
    symbol sym=symbols[line][ch];
    return sym;
}

symbol document::findPosAfter(const std::pair<int, int> indexes) const {
    int line=indexes.first;
    int ch=indexes.second;

    int numLines=symbols.size();
    int numChars=0;
    for(int i=0;i<symbols[line].size();i++){
        if (symbols[line][i]!=emptySymbol){
            numChars++;
        }
    }

    // If I have lines after the considered one, but I'm at the end of the considered line
    if(line==numLines-1 && ch==numChars){
        symbol sym=emptySymbol;
        return sym;
    }else if(line<numLines-1 && ch==numChars){
        line=line+1;
        ch=0;
    }else if(line>numLines-1&& ch==0){
        symbol sym=emptySymbol;
        return sym;
    }
    symbol sym=symbols[line][ch];
    return sym;
}


std::vector<int>
document::generatePosBetween(std::vector<int> posBefore, std::vector<int> posAfter, std::vector<int> newPos, int level,
                             int siteIdB,int siteIdA) {

    // change 2 to any other number to change base multiplication
    int base=pow(2,level)*32;
    char boundaryStrategy= retrieveStrategy(level);
    int id1,id2;

    if(posBefore.empty()){
        //id1=indexes.first;
        id1=0;
    }else{
        id1=posBefore[0];
    }
    if(posAfter.empty()){
        //id1=indexes.second;
        id2=base;
    }else{
        id2=posAfter[0];
    }

    if(id2-id1>1){
        int newDigit= generateIdBetween(id1,id2,boundaryStrategy);
        newPos.push_back(newDigit);
    }else if( id2-id1==1){
        newPos.push_back(id1);
        // pos1.slice(1) will remove from the posBefore the first element
        std::vector<int> pos1=posBefore;
        pos1.erase(pos1.begin());
        std::vector<int> pos2;
        return generatePosBetween(pos1, pos2, newPos, level + 1, siteIdB, siteIdA);

    }else if(id1==id2){
        if(siteIdB<siteIdA){
            newPos.push_back(id1);
            //pos1.slice(1)
            std::vector<int> pos1=posBefore;
            pos1.erase(pos1.begin());
            std::vector<int> pos2;
            return generatePosBetween(pos1, pos2, newPos, level + 1, siteIdB, siteIdA);
        }else{
           newPos.push_back(id1);
            //pos1.slice(1)
            std::vector<int> pos1=posBefore;
            pos1.erase(pos1.begin());
            //pos2.slice(1)
            std::vector<int> pos2=posAfter;
            pos2.erase(pos2.begin());
            return generatePosBetween(pos1, pos2, newPos, level + 1, siteIdB, siteIdA);
        }
    }

    return newPos;

}


char document::retrieveStrategy(const int level){

   if(!strategyCache.empty()){
        return strategyCache[level];
    }

    int value=rand()%2;
    switch (strategy){
        case 'p': strategy='+'; break;
        case 'm': strategy='-'; break;
        case 'r': strategy=value==0? '+':'-';break;
        default: strategy=(level%2)==0 ?'+':'-'; break;
    }
    strategyCache.insert(strategyCache.begin()+level,strategy);
    return strategy;

}


int document::generateIdBetween(int id1, int id2,const char boundaryStrategy) const {
    int boundary=10;
    if((id2-id1)<boundary){
        id1+=1;
    }else{
        if(boundaryStrategy=='-'){
            id1=id2-boundary;
        }else {
            id1+=1;
            id2=id1+boundary;
        }
    }
    return floor(rand()%(id2-id1))+id1;
}

symbol document::localRemove(const std::pair<int, int> &indexes) {
    //TODO: take into account new position of cursor
    int i0=indexes.first;
    int i1=indexes.second;
    checkIndex(i0,i1);
    symbol sym=symbols[i0][i1];
    symbols[i0].erase(symbols[i0].begin()+i1);

    return sym;
}

void document::remoteInsert(const symbol &toInsert) {
    //TODO: take into account new position of cursor
    std::pair<int,int> indexes=findInsertIndex(toInsert);
    int i0=indexes.first;
    int i1=indexes.second;
    checkIndex(i0,i1);
    char sym=symbols[i0][i1].getCh();

    if(sym==emptyChar){ symbols[i0][i1]=toInsert;}

    else {
        symbols[i0].insert(symbols[i0].begin() + i1, toInsert);
    }

}


void document::remoteRemove(const symbol &toRemove) {
    //TODO: take into account new position of cursor
    std::pair<int,int> pos=findPosition(toRemove);
    int i0=pos.first;
    int i1=pos.second;
    if(i0==-1 || i1==-1){
        return;
    }
    //FIXME: se lo lasci come ultima opzione, puoi evitare di scrivere questo codice
    else if(symbols[i0][i1]!=toRemove){
        return;
    }
    else {
        symbols[i0].erase(symbols[i0].begin()+i1);
    }
}

std::wstring document::toText() const {
    std::wstring str;
    std::wostringstream str1;
    int i=0;
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
    for(auto p:activeUsers) {
        user old_User = *p.first;
        if (old_User == noLongerActive) {
            activeUsers.remove(p);
            return;
        }
    }
}


void document::store(const std::string &storePath) {
    //TODO:implement

}

void document::load(const std::string &loadPath) {
    //TODO:implement
}

std::set<int> document::retrieveSiteIds() const{
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



std::pair<int, int> document::findInsertIndex(const symbol &symbol) const {
    std::pair<int,int> ind;
    int i0=0; int i1=0;
    int minLine=0;
    int totalLines=symbols.size();
    int maxLine= totalLines-1;
    std::vector<Symposium::symbol> lastLine= symbols[maxLine];

    int midLine=0;int charIdx=0;
    std::vector<Symposium::symbol> maxCurrentLine;
    std::vector<Symposium::symbol> minCurrentLine;
    std::vector<Symposium::symbol> currentLine;


    // check if struct is empty or char is less than first char
    if (symbols.empty()||symbol.getCh()<symbols[0][0].getCh()) {ind={i0,i1};return ind;}

    auto lastSymbol=lastLine[lastLine.size() - 1];

    //char is greater than all existing chars (insert and end)

    if(symbol>lastSymbol){
        ind= findEndPosition(lastSymbol,lastLine,totalLines);
        return ind;
    }

    //binary search
    while(minLine+1<maxLine){
        midLine=minLine+(maxLine-minLine)/2;
        currentLine=symbols[midLine];
        lastSymbol=currentLine[currentLine.size()-1];

        if(symbol==lastSymbol){
            i0=midLine; i1=currentLine.size()-1;
        } else if(symbol<lastSymbol){
            maxLine=midLine;
        } else{
            minLine=midLine;
        }
    }

    //check between min and max line
    minCurrentLine=symbols[minLine];
    auto minLastSymbol=minCurrentLine[minCurrentLine.size()-1];
    maxCurrentLine=symbols[maxLine];
    auto maxLastSymbol=maxCurrentLine[maxCurrentLine.size()-1];

    if(symbol<=minLastSymbol){
        charIdx=findInsertInLine(symbol,minCurrentLine);
        i0=minLine; i1=charIdx;
        ind={i0,i1};
        return ind;

    } else{
        charIdx=findInsertInLine(symbol,maxCurrentLine);
        i0=maxLine; i1=charIdx;
        ind={i0,i1};
        return ind;
    }

}

std::pair<int, int> document::findEndPosition(const symbol aChar, const std::vector<Symposium::symbol> vector, int lines) const {
    std::pair<int,int> ind;
    if(aChar== emptySymbol){
        ind={lines,0}; return ind;
    } else{
        ind={lines-1,vector.size()};
    }
    return ind;

}

int document::findInsertInLine(const symbol ch, const std::vector<Symposium::symbol> vector) const {
    int ind=0;
    int left=0;
    int right= vector.size()-1;
    int mid;

    if(vector.size()==0 ||ch>vector[left]){
        ind=left;
        return ind;
    } else if(ch>vector[right]){
        ind=vector.size();
        return ind;
    }

    while(left+1<right){
        mid=left-(right-left)/2;

        if(ch==vector[mid]){
            ind=mid; return ind;
        } else if(ch>vector[mid]){
            left=mid;
        } else{
            right=mid;
        }
    }

    if(ch==vector[left]){
        ind=left; return ind;
    } else{
        ind=right; return right;
    }

}

std::pair<int, int> document::findPosition(const symbol &symbol) const {
    std::pair<int,int> ind;
    int i0=0; int i1=0;
    int minLine=0;
    int totalLines=symbols.size();
    int maxLine= totalLines-1;
    std::vector<Symposium::symbol> lastLine= symbols[maxLine];

    int midLine=0;int charIdx=0;
    std::vector<Symposium::symbol> maxCurrentLine;
    std::vector<Symposium::symbol> minCurrentLine;
    std::vector<Symposium::symbol> currentLine;

    //if the struct is empty or char is less than first char
    if(symbols.empty()||symbol.getCh()<symbols[0][0].getCh()){
        i0=-1; i1=-1; ind={i0,i1}; return ind;
    }

    auto lastChar=lastLine[lastLine.size()-1];

    //char is greater than all existing chars(insert at end)
    if(symbol.getCh()>lastChar.getCh()){
        i0=-1; i1=-1; ind={i0,i1}; return ind;
    }

    // binary search
    while(minLine+1<maxLine){
        midLine=minLine+(maxLine-minLine)/2;
        currentLine=symbols[midLine];
        lastChar=currentLine[currentLine.size()-1];

        if(symbol==lastChar){
            ind={midLine,currentLine.size()-1}; return ind;
        } else if(symbol<lastChar){
            maxLine=midLine;
        } else{
            minLine=midLine;
        }
    }

    // Check between min and max line
    minCurrentLine=symbols[minLine];
    auto minLastSymbol=minCurrentLine[minCurrentLine.size()-1];
    maxCurrentLine=symbols[maxLine];
    auto maxLastSymbol=maxCurrentLine[maxCurrentLine.size()-1];

    if(symbol<=minLastSymbol){
        charIdx=findIndexInLine(symbol,minCurrentLine);
        ind={minLine,charIdx};
        return ind;
    } else{
        charIdx=findIndexInLine(symbol,maxCurrentLine);
        ind={maxLine,charIdx};
        return ind;
    }

}

int document::findIndexInLine(const symbol &symbol, const std::vector<Symposium::symbol> vector) const {
    int left=0;
    int right=vector.size()-1;
    int mid;

    if(vector.size()==0||symbol<vector[left]){
        return left;
    } else if(symbol>vector[right]){
        return symbols.size();
    }
    while(left+1<right){
        mid=left+(right-left)/2;

        if(symbol==vector[left]){
            return mid;
        } else if(symbol>vector[left]){
            left=mid;
        } else{
            right=mid;
        }
    }

    if(symbol==vector[left]){
        return left;
    }

    else{
        return right;
    }

}

void document::updateCursorPos(unsigned int targetSiteId, unsigned int newRow, unsigned int newCol) {
    //TODO:implement
}











