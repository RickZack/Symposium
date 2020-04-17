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
#include <fstream>
#include <boost/archive/text_iarchive.hpp>
#include "QDebug"

using namespace Symposium;
uint_positive_cnt document::idCounter;
const symbol document::emptySymbol(emptyChar, 0, 0, {0, 0});
const std::string document::basePath="./docs/";
bool document::serializeFull=true;

#define UnpackFileLineFunction()  __FILE__, __LINE__, __PRETTY_FUNCTION__

void assertIndexes(bool(*predicate)(unsigned toCheck, unsigned reference),
                   unsigned toCheck,unsigned reference,
                   const char* file, int line, const char* func){
    if(!predicate(toCheck, reference)){
        //std::stringstream err;
        std::cout<<"toCheck is: "<<toCheck;
        std::cout<< "reference is: " << reference;
        std::cout<<file<<", line "<<line<<" "<<func;
        //throw std::out_of_range(err.str());
    }
}

/**
 * @brief check if one pair of indexes is included in another
 * @param toCheck the pair to be tested
 * @param reference the other pair @ref toCheck is to be compared to
 * @return result of check
 */
bool included(unsigned ind0, unsigned ind){
    return ind0<ind;
}




document::document(uint_positive_cnt::type id) : id(id), symbols(1, std::vector<symbol>(1, emptySymbol)) {
    id=idCounter;
    idCounter++;

}

uint_positive_cnt::type document::getId() const {
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
    if(!loaded) loaded=load();
    if(std::find_if(activeUsers.begin(), activeUsers.end(), [&](auto p){return p.first->getSiteId()==newActive.getSiteId();})==activeUsers.end()) {
        std::pair<const user *, sessionData> p{&newActive, accessPriv};
        activeUsers.push_front(p);
    }
    return *this;
}


void document::checkIndex(unsigned int i0, unsigned int i1) {
    int mult_fac=2;

    if(i0>=symbols.size()){
         //symbols.resize((i0 + 1)*mult_fac, std::vector<symbol>(1,emptySymbol));
        symbols.resize((i0 + 1)*mult_fac, std::vector<symbol>(1,emptySymbol));

}
    assertIndexes(included,i0,symbols.size(),UnpackFileLineFunction());
    if(i1>=symbols[i0].size()){
       symbols[i0].resize((i1 + 1)*mult_fac, emptySymbol);


}
}

symbol document::localInsert(const std::pair<unsigned int, unsigned int> &indexes, symbol &toInsert) {

    int i0=indexes.first;
    int i1=indexes.second;
    qDebug()<<"I0"<<i0;
    qDebug()<<"i1"<<i1;
    checkIndex(i0,i1);
    this->updateCursorPos(toInsert.getSiteId(),i0,i1);
    //symbol newSymb= generatePosition(indexes,toInsert);
    //newSymb.setCharFormat(toInsert.getCharFormat());

    assertIndexes(included,i0,symbols.size(),UnpackFileLineFunction());
    assertIndexes(included,i1,symbols[i0].size(),UnpackFileLineFunction());
   // wchar_t sym=symbols[i0][i1].getCh();
    symbols[i0].insert(symbols[i0].begin()+i1,toInsert);
    symbol s1=findPosBefore(indexes);
    symbol s2=findPosAfter(indexes);
    /*
    if(sym==emptyChar){
        //symbols[i0][i1]=newSymb;
        //symbols[i0].insert(symbols[i0].begin()+i1,newSymb);
        //symbols[i0].insert(symbols[i0].begin()+i1,toInsert);
    }
    else {
        symbols[i0].insert(symbols[i0].begin() + i1, newSymb);
        }
*/
    return toInsert;


}

symbol document::generatePosition(const std::pair<unsigned int, unsigned int> indexes, const symbol &toInsert){
    std::vector<int> posBefore;
    int siteIdB;
    symbol symB=findPosBefore(indexes);
    if(symB==emptySymbol){
        siteIdB=-1;
    }else{
        posBefore=symB.getPos();
        siteIdB=symB.getSiteId();
    }

    std::vector <int> posAfter;
    int siteIdA;
    symbol symA=findPosAfter(indexes);
    if(symA==emptySymbol){
        siteIdA=-1;
    }else{
        posAfter=symA.getPos();
        siteIdA=symB.getSiteId();
    }

    int level=0;
    std::vector<int> inPos;
    std::vector<int> newPos= generatePosBetween(posBefore, posAfter, inPos, level, siteIdB, siteIdA);

    symbol newSymbol(toInsert.getCh(),toInsert.getSiteId(),toInsert.getCounter(),newPos,false);
    return newSymbol;
}



symbol document::findPosBefore(const std::pair<unsigned int, unsigned int> indexes) const {
    int line=indexes.first;
    int ch=indexes.second;
    assertIndexes(included,line,symbols.size(),UnpackFileLineFunction());
    assertIndexes(included,ch,symbols[line].size(),UnpackFileLineFunction());

    // I don't have position before the considered one
    // FIRST LIMIT CASE
    if(ch==0 && line==0){
        symbol sym=emptySymbol;
        return sym;
    }
        // SECOND LIMIT CASE: I have to change line
    else if(ch==0 && line!=0){
        line=line-1;
        assertIndexes(included,line,symbols.size(),UnpackFileLineFunction());

       ch=countCharsInLine(line)-1;
       std::cout<<"Ch"<<ch;
        assertIndexes(included,ch,symbols[line].size(),UnpackFileLineFunction());

    }else{
        ch=ch-1;

    }


    symbol sym=symbols[line][ch];
    return sym;
}


int document::countCharsInLine(int line)const {
    int ch=0;
    for(size_t i=0;i<symbols[line].size();i++){
        if(symbols[line][i]!=emptySymbol)
            ch++;
    }
    return ch;
}


symbol document::findPosAfter(const std::pair<unsigned int, unsigned int> indexes) const {
    int line=indexes.first;
    int ch=indexes.second;
    int numChars=countCharsInLine(line);
    symbol sym=emptySymbol;


    if(ch<numChars-1){
        assertIndexes(included,line,symbols.size(),UnpackFileLineFunction());
        assertIndexes(included,ch,symbols[line].size(),UnpackFileLineFunction());
       sym=symbols[line][ch];


    }else if(ch+1<(int)symbols[line].size() && symbols[line][ch+1]==emptySymbol){
        assertIndexes(included,line,symbols.size(),UnpackFileLineFunction());
        assertIndexes(included,ch+1,symbols[line].size(),UnpackFileLineFunction());
         sym=symbols[line][ch];
    }
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
        id1=0;
    }else{
        id1=posBefore[0];
    }
     if(posAfter.empty()){
        id2=base;
    }else{
        id2=posAfter[0];
    }

    if(id2-id1>1){
        unsigned newDigit= generateIdBetween(id1,id2,boundaryStrategy);
        newPos.push_back(newDigit);
        return newPos;
    }else if(id2-id1==1){
        newPos.push_back(id1);
        // pos1.slice(1) will remove from the posBefore the first element
        std::vector<int> pos1=posBefore;
        pos1.erase(pos1.begin());
        std::vector<int> pos2;
        return generatePosBetween(pos1, pos2, newPos, level+1 , siteIdB, siteIdA);

    }else if(id1==id2){
        if(siteIdB<siteIdA){
            newPos.push_back(id1);
            //pos1.slice(1)
            std::vector<int> pos1=posBefore;
            pos1.erase(pos1.begin());
            std::vector<int> pos2;
            return generatePosBetween(pos1, pos2, newPos, level + 1, siteIdB, siteIdA);

        }else if(siteIdB==siteIdA){
            newPos.push_back(id1);
            //pos1.slice(1)
            std::vector<int> pos1=posBefore;
            pos1.erase(pos1.begin());
            //pos2.slice(1)
            std::vector<int> pos2=posAfter;
            pos2.erase(pos2.begin());
            return generatePosBetween(pos1, pos2, newPos, level + 1, siteIdB, siteIdA);
        }
        else{
            //throw "Fix position sorting";
            std::cout<<"Sono entrato qui";

        }
    }
}


char document::retrieveStrategy(const int level){

   int sizeSC=strategyCache.size();
    assertIndexes(included,level,strategyCache.size(),UnpackFileLineFunction());
   if(level<sizeSC){
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

symbol document::localRemove(const std::pair<unsigned int, unsigned int> &indexes, uint_positive_cnt::type siteId) {
    /*
    int i0=indexes.first;
    int i1=indexes.second;
    //checkIndex(i0,i1);
    symbol sym=symbols[i0][i1];
    //taking into account the position of the cursor.
    // TO DO
    this->updateCursorPos(siteId,i0,i1);
    symbols[i0].erase(symbols[i0].begin()+i1);

    return sym;
     */
}

std::pair<unsigned int, unsigned int> document::remoteInsert(uint_positive_cnt::type siteId, const symbol &toInsert) {
    /*
    std::pair<int,int> indexes=findInsertIndex(toInsert);
    int i0=indexes.first;
    int i1=indexes.second;
    // taking into account the position of the cursor.
    this->updateCursorPos(siteId,i0,i1);
    checkIndex(i0,i1);
    char sym=symbols[i0][i1].getCh();

    if(sym==emptyChar){ symbols[i0][i1]=toInsert;}

    else {
        symbols[i0].insert(symbols[i0].begin() + i1, toInsert);
    }

    return indexes;
     */
}


std::pair<unsigned int, unsigned int> document::remoteRemove(uint_positive_cnt::type siteId, const symbol &toRemove) {
    /*
    //TODO: take into account new position of cursor
    std::pair<int,int> pos=findPosition(toRemove);
    int i0=pos.first;
    int i1=pos.second;
    this->updateCursorPos(siteId,i0,i1);
    if(i0==-1 || i1==-1){
        //FIXME: dummy return, fix
        return std::pair<unsigned int, unsigned int>();
    }
    //FIXME: se lo lasci come ultima opzione, puoi evitare di scrivere questo codice
    else if(symbols[i0][i1]!=toRemove){
        //FIXME: dummy return, fix
        return std::pair<unsigned int, unsigned int>();
    }
    else {
        symbols[i0].erase(symbols[i0].begin()+i1);
    }
     */
}

std::wstring document::toText() const {
    std::wstring str;
    std::wostringstream str1;
    for (size_t i=0;i<symbols.size();i++) {
        for (size_t j = 0; j < symbols[i].size(); j++) {
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
            break;
        }
    }
    if(activeUsers.empty()){
        store();
        std::vector<std::vector<symbol>>().swap(symbols); //free the content of the symbol matrix
        loaded=false;
    }
}


void document::store() const {
    std::string storePath=basePath+std::to_string(id)+".dat";
    std::ofstream out{storePath, std::ios::out | std::ios::trunc};
    if(out.good()) {
        try {
            boost::archive::text_oarchive oa(out);
            oa << *this;
        }
        catch(std::exception& e) {
            std::cerr << "Unable to store data on disk: " << e.what() << std::endl;
            remove(storePath.c_str());
        }

    } else
        std::cerr<<"Error opening output file"<<std::endl;
}

bool document::load() {
    std::ifstream input{basePath+std::to_string(id)+".dat", std::ios::in};
    if(input.good()){
        try {
            boost::archive::text_iarchive ia(input);
            ia>>*this;
            return true;
        }
        catch(std::exception& e) {
            std::cerr << "Unable to load data from disk: " << e.what() << std::endl;
        }
    }
    return false;
}

std::set<uint_positive_cnt::type> document::retrieveSiteIds() const{
    std::set<uint_positive_cnt::type> siteIds;
    std::set<uint_positive_cnt::type>::iterator it=siteIds.begin();
    for(size_t i=0;i<symbols.size();i++){
        for(size_t j=0;j<symbols[i].size();j++){
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



std::pair<unsigned int, unsigned int> document::findInsertIndex(const symbol &symbol) const {
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

std::pair<unsigned int, unsigned int>
document::findEndPosition(const symbol aChar, const std::vector<Symposium::symbol> vector, int lines) const {
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

std::pair<unsigned int, unsigned int> document::findPosition(const symbol &symbol) const {
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
        //i0=-1; i1=-1; ind={i0,i1}; return ind;
        i0=0; i1=0; ind={i0,i1};return ind;
    }

    auto lastChar=lastLine[lastLine.size()-1];

    //char is greater than all existing chars(insert at end)
    if(symbol.getCh()>lastChar.getCh()){
       // i0=-1; i1=-1; ind={i0,i1}; return ind;
        return this->findEndPosition(lastChar,lastLine,totalLines);
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

unsigned int document::findIndexInLine(const symbol &symbol, const std::vector<Symposium::symbol> vector) const {
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

void document::updateCursorPos(uint_positive_cnt::type targetSiteId, unsigned int newRow, unsigned int newCol) {
   for(auto& i: activeUsers){
       if(i.first->getSiteId()==targetSiteId){
           i.second.row=newRow;
           i.second.col=newCol;
           break;
       }
   }
}

std::pair<unsigned int, unsigned int> document::verifySymbol(const symbol &toVerify) {
    std::pair<unsigned int,unsigned int> indexes;
    for(size_t i=0;i<symbols.size();i++){
        for(size_t j=0;j<symbols[i].size();j++){
            if(symbols[i][j]==toVerify){
                symbols[i][j].setVerified();
                indexes={i,j};
            }
        }
    }
    return indexes;
}











