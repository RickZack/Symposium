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
 * File:   symbol.h
 * Project: Symposium
 * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Alice Morano <s259158 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 13 Giugno 2019, 16.44
 */
#ifndef SYMPOSIUM_SYMBOL_H
#define SYMPOSIUM_SYMBOL_H


#include <vector>
#include <iostream>
#include <algorithm>
#include "Color.h"
#include <boost/serialization/access.hpp>




namespace Symposium {

enum class alignType{
    left,                           /**< to declare that the row has a left alignment */
    right,                          /**< to declare that the row has a right alignment */
    center,                         /**< to declare that the row has a center alignment */
    justify                         /**< to declare that the row has a justified alignment */
};

    struct format{

        /* format */
        std::string familyType;
        bool isBold;
        bool isUnderlined;
        bool isItalic;
        unsigned size;
        Color col;

        /* alignment */
        unsigned indexStyle;            /**< to set the style of the text **/
        alignType type;                 /**< to set the kind of alignment of the text **/


        format():format("", false, false, false, 12,Color(0,0,0),0,alignType::left){}
        format(const std::string& ft, bool bold, bool underline, bool italic, unsigned size, Color col,unsigned indexStyle, alignType type):
        familyType(ft), isBold(bold), isUnderlined(underline), isItalic(italic), size(size),col(col),indexStyle(indexStyle), type(type){}

        template<class Archive>
        void serialize(Archive &ar, const unsigned int);

    };


    class symbol {
        wchar_t ch;             /**< the character the symbols carries */
        int siteId;             /**< identifies the user that generated the symbol */
        int counter;
        std::vector<int> pos;   /**< vector that defines the fractional indices of the symbol inside a document */
        bool verified;          /**< true if the symbol has been verified by the server or has been inserted by remoteInsert */
        format charFormat;      /**< attributes of the symbol for rich text editor */

        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version);
        //Needed by boost::serialization
        symbol()=default;

    public:
        symbol(wchar_t ch, int siteId, int counter, const std::vector<int> &pos, bool verified=false);

        wchar_t getCh() const;

        int getSiteId() const;

        int getCounter() const;

        const std::vector<int> &getPos() const;

        bool isVerified() const;
        symbol & setVerified();

        bool operator<(const symbol &rhs) const;

        bool operator>(const symbol &rhs) const;

        bool operator<=(const symbol &rhs) const;

        bool operator>=(const symbol &rhs) const;

        bool operator==(const symbol &rhs) const;

        bool operator!=(const symbol &rhs) const;

        void setCharFormat(const format &value);

        format getCharFormat() const;
    };
}

/*
 * Just to have a way to print the symbols in a intellegible way
 * when the tests fail
 */
std::ostream& operator<<(std::ostream& os, Symposium::symbol s);

#endif //SYMPOSIUM_SYMBOL_H
