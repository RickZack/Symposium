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
#include "Symposium.h"

namespace Symposium {
    class symbol {
        wchar_t ch;
        int siteId;
        int counter;
        std::vector<int> pos;
        bool verified;  /**< true if the symbol has been verified by the server or has been inserted by remoteInsert */
    public:
        symbol(wchar_t ch, int siteId, int counter, const std::vector<int> &pos, bool verified=false);

        wchar_t getCh() const;

        int getSiteId() const;

        const std::vector<int> &getPos() const;

        bool isVerified() const;
        symbol & setVerified();

        bool operator<(const symbol &rhs) const;

        bool operator>(const symbol &rhs) const;

        bool operator<=(const symbol &rhs) const;

        bool operator>=(const symbol &rhs) const;

        bool operator==(const symbol &rhs) const;

        bool operator!=(const symbol &rhs) const;
    };
}

/*
 * Just to have a way to print the symbols in a intellegible way
 * when the tests fail
 */
std::ostream& operator<<(std::ostream& os, Symposium::symbol s);

#endif //SYMPOSIUM_SYMBOL_H
