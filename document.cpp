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

using namespace Symposium;
int document::idCounter=0;
const symbol document::emptySymbol(emptyChar, 0, 0, {0, 0});


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


symbol document::localInsert(const std::pair<int, int> &indexes, symbol &toInsert) {

    int i0=indexes.first;
    int i1=indexes.second;
    float mul_fct=1.5; //just to avoid too many reallocations

    //FIXME: usi questo codice spesso, racchiudilo in una funzione e usa quella
    if(i0>=symbols.capacity())
        symbols.resize((i0+1)*mul_fct);
    if(i1>=symbols[i0].capacity())
        symbols[i0].resize((i1 + 1) * mul_fct, emptySymbol);

    generatePosition(indexes);
    char sym=symbols[i0][i1].getCh();

    if(sym==emptyChar){
        symbols[i0][i1]=toInsert;
    }
    else {
        symbols[i0].insert(symbols[i0].begin() + i1, toInsert);
        }
    return toInsert;
}

//FIXME: questo metodo deve essere ripensato, ci sono problemi nelle intenzioni
// e nell'implementazione, rivedere Conclave e il CRDT. Ad esempio, dove calcoli l'indice quando metti
// due simboli che hanno già i vettori delle posizioni?
// Ad esempio (https://conclave-team.github.io/conclave-site/#globally-ordered-characters):
// il doc contiene già: 'c' (pos [0]), 'a' (pos [1]), 't' (pos[2])
// vogliamo inserire 'h' tra 'a' e 'b'
// il risultato dovrebbe essere qualcosa del tipo
// 'c' (pos [0]), 'h' (pos[0, x]) 'a' (pos [1]), 'b' (pos [2])
// il valore di x dipende da come scrivi l'algoritmo (nell'esempio di conclave è 5),
// ma qui non vedo nessun punto in cui viene calcolato.
// NOTA: Conclave, dopo aver capito quali sono i vettori delle posizioni precedenti e successiva,
// calcola la posizione da assegnare al carattere, https://github.com/conclave-team/conclave/blob/master/lib/crdt.js
// linea 352 chiama genPosBetween() definita alla linea 376.
void document::generatePosition(const std::pair<int, int> indexes) const {
    int i0=indexes.first;
    int i1=indexes.second;
    int c=symbols[i0].size(); //FIXME: anche qui, variabile mai usata...

    // vectors that maintain the position for all the rows of symbols.
    //FIXME: mi sembra che ci sia un problema fondamentale in questo metodo: cosa fa?
    // Praticamente nulla, perchè posA e posB vengono assegnati giù ma all'uscita della funzione
    // vengono persi. Manca la documentazione quindi mi viene scomodo pensare a cosa avresti voluto fare.
    std::vector<int> posA;
    std::vector<int> posB;

    //FIXME: queste variabili non sono mai usate, a che servono?
    int siteIdB; int siteIdA;

    //FIXME: questa condizione è sempre vera (perchè all'inizio i vettori contengono il '~'),
    // puoi vedere con il debugger se vuoi. Inoltre i vettori delle posizioni sono sempre vuoti,
    // e questo non dovrebbe mai succedere.
    if(!symbols.empty()) {
        posA = symbols[i0][i1].getPos();
    }
    else if (i1 >= symbols[i0].size()) {
        int ind_new = symbols[i0].size() - 1;
        posB = symbols[i0][ind_new].getPos();
    }
    else {
        // I have to generate the pos after and the pos before
        int i1_new = i1 - 1;
        posB = symbols[i0][i1_new].getPos();
        siteIdB = symbols[i0][i1_new].getSiteId();

        posA = symbols[i0][i1].getPos();
        siteIdA = symbols[i0][i1].getSiteId();
    }
}

symbol document::localRemove(const std::pair<int, int> &indexes) {
    int i0=indexes.first;
    int i1=indexes.second;
    //FIXME: non c'è nessun controllo sugli indici, non è sicuro
    symbol sym=symbols[i0][i1];
    symbols[i0].erase(symbols[i0].begin()+i1);

    return sym;
}

void document::remoteInsert(const symbol &toInsert) {
    std::pair<int,int> indexes=findInsertIndex(toInsert);
    int i0=indexes.first;
    int i1=indexes.second;

    //FIXME: usi questo codice spesso, racchiudilo in una funzione e usa quella
    float mul_fct=1.5; //just to avoid too many reallocations

    if(i0>=symbols.capacity())
        symbols.resize((i0+1)*mul_fct);

    if(i1>=symbols[i0].capacity())
        symbols[i0].resize((i1 + 1) * mul_fct, emptySymbol);

    char sym=symbols[i0][i1].getCh();

    if(sym==emptyChar){ symbols[i0][i1]=toInsert;}

    else {
        symbols[i0].insert(symbols[i0].begin() + i1, toInsert);
    }

}


void document::remoteRemove(const symbol &toRemove) {
    std::pair<int,int> pos=findPosition(toRemove);
    int i0=pos.first;
    int i1=pos.second;
    //FIXME: doppia condizione inutile, basta solo una delle due
    if(i0==-1 && i1==-1){
        return;
    }
    //FIXME: se lo lasci come ultima opzione, puoi evitare di scrivere questo codice
    else if(symbols[i0][i1]!=toRemove){
        return;
    }
    else {
        //FIXME: a che serve sym?
        symbol sym=symbols[i0][i1];
        symbols[i0].erase(symbols[i0].begin()+i1);
    }
}

std::wstring document::toText() const {
    std::wstring str;
    std::wostringstream str1;
    //FIXME: il compilatore dice che size e sizes non sono mai usate. Potresti toglierle
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
    for(auto p:activeUsers) {
        //FIXME: non usi mai c, non capisco a cosa serve.
        // Anche la prima istruzione non mi sembri serva a qualcosa.
        // Se usi la remove_if viene anche più semplice.
        std::pair<user *, privilege> c = *first;
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

//FIXME: se aChar non viene modificato, passalo come const reference, lo stesso per il vettore
std::pair<int, int> document::findEndPosition(symbol aChar, std::vector<Symposium::symbol> vector, int lines) const {
    std::pair<int,int> ind;
    if(aChar== emptySymbol){
        ind={lines,0}; return ind;
    } else{
        ind={lines-1,vector.size()};
    }
    return ind;

}
//FIXME: se ch non viene modificato, passalo come const reference, lo stesso per il vettore
int document::findInsertInLine(symbol ch, std::vector<Symposium::symbol> vector) const {
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
    //FIXME: se usi la ricerca binaria o qualsiasi altro pezzo di codice più di una volta,
    // specialmente se lungo, mettilo in una funzione a parte e chiamala dove ti serve
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

    //FIXME: se usi la ricerca binaria o qualsiasi altro pezzo di codice più di una volta,
    // specialmente se lungo, mettilo in una funzione a parte e chiamala dove ti serve

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
//FIXME: se il vettore non viene modificato, passalo come const reference
int document::findIndexInLine(const symbol &symbol, std::vector<Symposium::symbol> vector) const {
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
    //FIXME: perchè else if? A logica dovrebbe essere un else.
    // Se c'è possibilità di non entrare in nessuno dei due rami
    // è un problema logico importante.
    else if(symbol==vector[left]){
        return right;
    }

}




