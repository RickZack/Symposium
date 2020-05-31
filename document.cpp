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

using namespace Symposium;
uint_positive_cnt document::idCounter;
const symbol document::emptySymbol(emptyChar, 0, 0, {0, 0});
const std::string document::basePath="./docs/";
bool document::serializeFull=true;
bool document::doLoadAndStore=true;


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




const std::vector<std::pair<alignType, unsigned int>> & document::getAlignmentStyle() const
{
    return alignmentStyle;
}

document::document(uint_positive_cnt::type id) : id(id), symbols(1, std::vector<symbol>(1, emptySymbol)),
    alignmentStyle(1,std::pair(alignType::left,0))
{
    if(id==0){
        this->id=idCounter;
        idCounter++;
    }
    this->numchar=0;
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

unsigned int document::getNumchar() const {
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
    /* resize the symbols vector*/
    if(i0>=symbols.size()){
        symbols.resize((i0 + 1)*mult_fac, std::vector<symbol>(1,emptySymbol));
        alignmentStyle.resize((i0 + 1)*mult_fac,std::pair(alignType::left,0));
}
    assertIndexes(included,i0,symbols.size(),UnpackFileLineFunction());
    if(i1>=symbols[i0].size()){
       symbols[i0].resize((i1 + 1)*mult_fac, emptySymbol);
    }

}

symbol document::localInsert(const std::pair<unsigned int, unsigned int> &indexes, symbol &toInsert) {

    if(toInsert.getCh()!='\r')
        this->numchar++;
    int i0=indexes.first;
    int i1=indexes.second;
    checkIndex(i0,i1);

    /* handle the position of the following cursors */
    this->updateOtherCursorPos(toInsert.getSiteId(),i0,i1,toInsert,true);

    symbol newSymb= generatePosition(indexes,toInsert);
    format charFormat=toInsert.getCharFormat();
    newSymb.setCharFormat(charFormat);

    /* set the alignmentStyle vector */
    std::pair<alignType,unsigned> styleValues;
    styleValues={charFormat.type,charFormat.indexStyle};

    assertIndexes(included,i0,symbols.size(),UnpackFileLineFunction());
    assertIndexes(included,i1,symbols[i0].size(),UnpackFileLineFunction());

    if(toInsert.getCh()=='\r'){
        // I'm inserting a symbol in the position (i0,i1), but the cursor is moving in the position (i0+1,0)
        this->updateCursorPos(toInsert.getSiteId(),i0+1,0);
        symbol checkSym=symbols[i0][i1];
        if(checkSym.getCh()=='\r'){
            alignmentStyle[i0]=styleValues;
            return newSymb;
    }else{
        symbols.emplace(symbols.begin()+i0+1,symbols[i0].begin()+i1,symbols[i0].end());
        symbols[i0].erase(symbols[i0].begin()+i1,symbols[i0].end());
        symbols.push_back(std::vector<symbol>(1,emptySymbol));
        alignmentStyle.emplace(alignmentStyle.begin()+i0+1,alignmentStyle[i0]);
        alignmentStyle.erase(alignmentStyle.begin()+i0+1,alignmentStyle.end());
        alignmentStyle.push_back(std::pair(alignType::left,0));
        //alignmentStyle.resize(symbols.size(),std::pair(alignType::left,0));
        }
    }else{
        // I'm inserting a symbol in the position (i0,i1), but the cursor is moving in the position (i0,i1+1)
        this->updateCursorPos(toInsert.getSiteId(),i0,i1+1);
    }
    symbols[i0].insert(symbols[i0].begin()+i1,newSymb);
    alignmentStyle[i0]=styleValues;
    //alignmentStyle.resize(symbols.size(),std::pair(alignType::left,0));

    return newSymb;

}


symbol document::generatePosition(const std::pair<unsigned int, unsigned int> indexes, const symbol &toInsert){
    std::vector<int> posBefore;
    //int siteIdB=0;
    symbol symB=findPosBefore(indexes);
    if(symB!=emptySymbol){
        posBefore=symB.getPos();
        //siteIdB=symB.getSiteId();
    }

    std::vector <int> posAfter;
    //int siteIdA;
    symbol symA=findPosAfter(indexes);
    if(symA!=emptySymbol){
        posAfter=symA.getPos();
        //siteIdA=symB.getSiteId();
    }

    int level=0;
    std::vector<int> inPos;
    std::vector<int> newPos= generatePosBetween(posBefore, posAfter, inPos, level, symB, symA);
    symbol newSymbol(toInsert.getCh(),toInsert.getSiteId(),toInsert.getCounter(),newPos,false);
    return newSymbol;
}



symbol document::findPosBefore(const std::pair<unsigned int, unsigned int> &indexes) const {
    unsigned int line=indexes.first;
    unsigned ch=indexes.second;
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


unsigned int document::countCharsInLine(unsigned int line)const {
    unsigned ch=0;
    for(size_t i=0;i<symbols[line].size();i++){
        if(symbols[line][i]!=emptySymbol)
            ch++;
    }
    return ch;
}


symbol document::findPosAfter(const std::pair<unsigned int, unsigned int> &indexes) const {
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
document::generatePosBetween(const std::vector<int> &posBefore, const std::vector<int> &posAfter, std::vector<int> newPos, int level,
                             const symbol &b, const symbol &a) {

    /* change 2 to any other number to change base multiplication */
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
        unsigned int newDigit= generateIdBetween(id1,id2,boundaryStrategy);
        newPos.push_back(newDigit);
        return newPos;
    }else if(id2-id1==1){
        newPos.push_back(id1);
        std::vector<int> pos1=posBefore;
        if(!pos1.empty()){
            pos1.erase(pos1.begin());
        }
        std::vector<int> pos2;
        return generatePosBetween(pos1, pos2, newPos, level+1 , b,a);

    }else if(id1==id2){
        if(b.getSiteId()<a.getSiteId()){
            newPos.push_back(id1);
            std::vector<int> pos1=posBefore;
            if(!pos1.empty()){
                 pos1.erase(pos1.begin());
            }
            std::vector<int> pos2;
            return generatePosBetween(pos1, pos2, newPos, level + 1,b,a);

        }else
            newPos.push_back(id1);
            std::vector<int> pos1=posBefore;
            if(!pos1.empty()){
                pos1.erase(pos1.begin());
            }
            //pos2.slice(1)
            std::vector<int> pos2=posAfter;
            if(!pos2.empty()){
                pos2.erase(pos2.begin());
            }
            return generatePosBetween(pos1, pos2, newPos, level + 1,b,a);
        }
    return std::vector<int>();
}


char document::retrieveStrategy(unsigned int level){

   unsigned int sizeSC=strategyCache.size();
   if(level<sizeSC){
        return strategyCache[level];
    }

    unsigned value=rand()%2;
    switch (strategy){
        case 'p': strategy='+'; break;
        case 'm': strategy='-'; break;
        case 'r': strategy=value==0? '+':'-';break;
        default: strategy=(level%2)==0 ?'+':'-'; break;
    }
    strategyCache.insert(strategyCache.begin()+level,strategy);
    return strategy;

}


unsigned document::generateIdBetween(uint_positive_cnt::type id1, uint_positive_cnt::type id2, char boundaryStrategy) const {
    uint_positive_cnt::type boundary=10;
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

    int i0=indexes.first;
    int i1=indexes.second;
    //checkIndex(i0,i1);
    symbol sym=symbols[i0][i1];
    if(sym.getCh()!='\r')
        this->numchar--;
    //taking into account the position of the cursor.

    this->updateOtherCursorPos(siteId,i0,i1,sym,false);
    this->updateCursorPos(siteId,i0,i1);

    symbols[i0].erase(symbols[i0].begin()+i1);
    if(symbols[i0].empty()){
        size_t i=i0;
        while(symbols[i][0]!=emptySymbol){
            symbols.emplace(symbols.begin()+i,symbols[i+1].begin(),symbols[i+1].end());
            alignmentStyle.emplace(alignmentStyle.begin()+i, alignmentStyle[i+1]);
            i++;
        }
        symbols.erase(symbols.begin()+i,symbols.end());
        alignmentStyle.erase(alignmentStyle.begin()+i, alignmentStyle.end());

    }
    /*
    if(i1==0 && symbols[0][0]==emptySymbol)
        alignmentStyle.erase(alignmentStyle.begin()+i0);
        */
    return sym;

}



std::pair<unsigned, unsigned> document::remoteInsert(uint_positive_cnt::type siteId, const symbol &toInsert) {

    if(toInsert.getCh()!='\r')
        this->numchar++;
    std::pair<unsigned,unsigned> indexes=findInsertIndex(toInsert);
    int i0=indexes.first;
    int i1=indexes.second;
    // I have to handle the position of the following cursors
    this->updateOtherCursorPos(siteId,i0,i1,toInsert,true);
    if(toInsert.getCh()=='\r'){
        this->updateCursorPos(toInsert.getSiteId(),i0+1,0);
    }
    else{
        this->updateCursorPos(toInsert.getSiteId(),i0,i1+1);
    }
    checkIndex(i0,i1);

    /* set the alignmentStyle vector */
    format charFormat=toInsert.getCharFormat();
    std::pair<alignType,unsigned> styleValues;
    styleValues={charFormat.type,charFormat.indexStyle};
    /*
    if(alignmentStyle[i0].first==alignType::left){
      alignmentStyle.insert(alignmentStyle.begin()+i0,styleValues);
    }
    */
    alignmentStyle[i0]=styleValues;
    symbols[i0].insert(symbols[i0].begin()+i1,toInsert);
    return indexes;

}


std::pair<unsigned int, unsigned int> document::remoteRemove(uint_positive_cnt::type siteId, const symbol &toRemove) {
    if(toRemove.getCh()!='\r')
        this->numchar--;
    std::pair<int,int> pos=findPosition(toRemove);
    int i0=pos.first;
    int i1=pos.second;
    this->updateOtherCursorPos(siteId,i0,i1,toRemove,false);
    this->updateCursorPos(siteId,i0,i1);
    if(i0!=-1 && i1!=-1 && symbols[i0][i1]==toRemove)
         symbols[i0].erase(symbols[i0].begin()+i1);
    else
        return std::pair<unsigned int, unsigned int>();
/*
    if(i1==0 && symbols[0][0]==emptySymbol)
        alignmentStyle.erase(alignmentStyle.begin()+i0);
*/
    return pos;
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
    if(!doLoadAndStore) return;
    document::serializeFull=true; //this is public attribute, just to be sure that here saving is complete
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
    if(!doLoadAndStore) return false;
    std::ifstream input{basePath+std::to_string(id)+".dat", std::ios::in};
    if(input.good()){
        try {
            boost::archive::text_iarchive ia(input);
            document temp(0);
            ia>>temp;
            *this=std::move(temp);
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

unsigned int document::countsNumLines() const{
    unsigned numLines=0;
    for(size_t i=0;i<symbols.size();i++){
        if(symbols[i][0]!=emptySymbol)
            numLines++;
        else
            i=symbols.size();
    }
    return numLines;
}



std::pair<unsigned int, unsigned int> document::findInsertIndex(const symbol &symbol) const {
    std::pair<int,int> ind;
    std::vector<Symposium::symbol> lastLine;
    int maxLine;
    int i0=0; int i1=0;
    int minLine=0;
    int totalLines=countsNumLines();
    if(totalLines!=0){
        maxLine= totalLines-1;
        lastLine= symbols[maxLine];
    }

    int midLine=0;int charIdx=0;
    std::vector<Symposium::symbol> maxCurrentLine;
    std::vector<Symposium::symbol> minCurrentLine;
    std::vector<Symposium::symbol> currentLine;


    // check if struct is empty or char is less than first char
    if(totalLines==0 || symbol<symbols[0][0]){ind={0,0}; return ind;}
    int numCharsInLine=countCharsInLine(maxLine);
    auto lastSymbol=lastLine[numCharsInLine-1];

    //char is greater than all existing chars (insert and end)
    if(symbol>lastSymbol){
        return findEndPosition(totalLines,lastSymbol);
    }

    //binary search
    while(minLine+1<maxLine){
        midLine=floor(minLine+(maxLine-minLine)/2);
        currentLine=symbols[midLine];
        int numCharsInLine=countCharsInLine(midLine);
        lastSymbol=currentLine[numCharsInLine-1];

        if(symbol==lastSymbol){
            i0=midLine; i1=countCharsInLine(midLine)-1;
        } else if(symbol<lastSymbol){
            maxLine=midLine;
        } else{
            minLine=midLine;
        }
    }

    //check between min and max line
    minCurrentLine=symbols[minLine];
    auto minLastSymbol=minCurrentLine[countCharsInLine(minLine)-1];
    maxCurrentLine=symbols[maxLine];
    auto maxLastSymbol=maxCurrentLine[countCharsInLine(maxLine)-1];

    if(symbol<=minLastSymbol){
        charIdx=findInsertInLine(symbol,minCurrentLine,minLine);
        i0=minLine; i1=charIdx;
        ind={i0,i1};
        return ind;

    } else{
        charIdx=findInsertInLine(symbol,maxCurrentLine,maxLine);
        i0=maxLine; i1=charIdx;
        ind={i0,i1};
        return ind;
    }

}

std::pair<unsigned int, unsigned int>
document::findEndPosition(unsigned int lines, const symbol &lastSymbol) const {
    std::pair<int,int> ind;
    if(lastSymbol.getCh()=='\r'){
        ind={lines,0};
    }else{
        int numCharsinLine=countCharsInLine(lines-1);
        ind={lines-1,numCharsinLine};
    }
    return ind;
}


unsigned document::findInsertInLine(const symbol &ch, const std::vector<symbol> &vector, unsigned int line) const {
    unsigned ind=0;
    int left=0;
    int right= countCharsInLine(line)-1;
    int mid;

    if(ch<vector[left]){
        ind=left;
        return ind;
    } else if(ch>vector[right]){
        ind=countsNumLines();
        return ind;
    }

    while(left+1<right){
        mid=floor(left+(right-left)/2);

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
    int totalLines=this->countsNumLines();
    int maxLine= totalLines-1;
    std::vector<Symposium::symbol> lastLine= symbols[maxLine];

    int midLine=0;int charIdx=0;
    std::vector<Symposium::symbol> maxCurrentLine;
    std::vector<Symposium::symbol> minCurrentLine;
    std::vector<Symposium::symbol> currentLine;

    //if the struct is empty or char is less than first char
    auto firstSymbol=symbols[0][0];
    if(symbols.empty()||symbol<firstSymbol){
        i0=-1; i1=-1; ind={i0,i1}; return ind;
    }

    // counts the number of chars in the last line
    int chars=this->countCharsInLine(maxLine);
    auto lastChar=lastLine[chars-1];

    //char is greater than all existing chars(insert at end)
    if(symbol>lastChar){
        i0=-1; i1=-1; ind={i0,i1}; return ind;
    }


    // binary search
    while(minLine+1<maxLine){
        midLine=floor(minLine+(maxLine-minLine)/2);
        currentLine=symbols[midLine];
        int charsInLine=this->countCharsInLine(midLine);
        lastChar=currentLine[charsInLine-1];

        if(symbol==lastChar){
            ind={midLine,charsInLine-1}; return ind;
        } else if(symbol<lastChar){
            maxLine=midLine;
        } else{
            minLine=midLine;
        }
    }

    // Check between min and max line
    minCurrentLine=symbols[minLine];
    int min_charsInLine=this->countCharsInLine(minLine);
    auto minLastSymbol=minCurrentLine[min_charsInLine-1];
    maxCurrentLine=symbols[maxLine];
    int max_charsInLine=this->countCharsInLine(maxLine);
    auto maxLastSymbol=maxCurrentLine[max_charsInLine-1];

    if(symbol<=minLastSymbol){
        charIdx=findIndexInLine(symbol,minCurrentLine,min_charsInLine);
        ind={minLine,charIdx};
        return ind;
    } else{
        charIdx=findIndexInLine(symbol,maxCurrentLine,max_charsInLine);
        ind={maxLine,charIdx};
        return ind;
    }

}

unsigned int document::findIndexInLine(const symbol &sym, const std::vector<symbol> &vector, unsigned int dimLine) const {
    int left=0;
    int right=dimLine-1;
    int mid;

    if(vector[0]==emptySymbol || sym < vector[left]){
        return left;
    } else if(sym > vector[right]){
        return this->countsNumLines();
    }
    while(left+1<right){
        mid=floor(left+(right-left)/2);

        if(sym == vector[mid]){
            return mid;
        } else if(sym > vector[mid]){
            left=mid;
        } else{
            right=mid;
        }
    }

    if(sym == vector[left]){
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

void document::updateOtherCursorPos(uint_positive_cnt::type targetSiteId, unsigned int newRow, unsigned int newCol,
                                    const symbol &symb, bool ins) {
    for(auto& i: activeUsers){
        // On the same line, there are cursors with different siteId
        if(symb.getCh()!='\r' && i.second.row==newRow && i.second.col>newCol && i.first->getSiteId()!=targetSiteId){
            // The action is to insert a character
            if(ins){
                i.second.col+=1;
            }else{
                i.second.col-=1;
            }

        } // I'm changing the line
        else if(symb.getCh()=='\r'){
            // inserting the \r character
            if(ins){
                // There are different cursor on the same line: they have to change the row index and the column index
               if(i.second.row==newRow){
                   if(i.second.col>newCol && i.first->getSiteId()!=targetSiteId){
                   i.second.row+=1;
                   i.second.col=i.second.col-newCol;
                   }
               }else
                   // there are different cursors on different lines: they have to change only the row index
                   if(i.first->getSiteId()!=targetSiteId){
                   i.second.row+=1;
                   }
                // deleting the \r character
            }else{
                i.second.row-=1;
                i.second.col=i.second.col+newCol;
            }

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

void document::doLightSerializing(const std::function<void(void)> &op) {
    document::serializeFull=false;
    op();
    document::serializeFull=true;
}
