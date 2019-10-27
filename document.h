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
 * File:   document.h
 * Project: Symposium
 * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Alice Morano <s259158 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 22 Giugno 2019, 12.10
 */
#ifndef SYMPOSIUM_DOCUMENT_H
#define SYMPOSIUM_DOCUMENT_H

#include <vector>
#include <forward_list>
#include <string>
#include <set>
//#include "symbol.h"
#include "privilege.h"
#include "Symposium.h"
#include "symbol.h"
namespace Symposium {
    class document {
        static int idCounter;                                           /**< id to be assigned to the next created document */
        int id;                                                         /**< unique identifier for the document */
        std::vector<std::vector<symbol> > symbols;                     /**< container of characters and metadata for CRDT*/
        std::forward_list<std::pair<user *, privilege>> activeUsers;     /**< list of users currently active on the document, with the current privilege*/
        int numchar;                                                    /**< number of printable characters */
    public:
        document(int id = document::idCounter);

        int getId() const;

        const std::vector<std::vector<symbol>> &getSymbols() const;

        const std::forward_list<std::pair<user *, privilege>> &getActiveUsers() const;

        int getNumchar() const;

        bool operator==(const document &rhs) const;

        bool operator!=(const document &rhs) const;

        /**
         * @brief open the current document, loading it from disk if needed
         * @param newActiveUser the user who attempts to open the document
         * @param accessPriv the privilege granted to the user for this access
         * @return the document itself
         *
         * This method is called by @ref file::access and perform the actions needed to
         * open a document, such as loading or storing it, and add @e newActiveUser to
         * @e activeUsers.
         */
        virtual document &access(const user &newActive, privilege accessPriv);

        /**
         * @brief insert a symbol in the document as consequence of an user's action on the GUI
         * @param toInsert symbol to insert
         */
        void localInsert(int indexes[2], symbol &toInsert);

        /**
         * @brief remove a symbol in the document as consequence of an user's action on the GUI
         * @param indexes symbol to remove
         */
        void localRemove(int indexes[2]);

        /**
         * @brief insert a symbol in the document as consequence of a remote user's action
         * @param toInsert symbol to insert
         */
        virtual void remoteInsert(symbol toInsert);

        /**
         * @brief remove a symbol in the document as consequence of a remote user's action
         * @param toRemove symbol to remove
         */
        virtual void remoteRemove(const symbol &toRemove);

        /**
         * @brief give a representation of the document ad sequence of wide characters
         * @return a string of wide characters with the document's content
         */
        std::wstring toText();

        /**
         * @brief removes the user @e noLongerActive from @e activeUsers
         * @param noLongerActive the user that is no longer active on the current document
         *
         * This method is called when a client closes the document it is working on.
         */
        virtual void close(const user &noLongerActive);

        void store(const std::string &storePath);

        void load(const std::string &loadPath);

        /**
         * @brief retrieves the set of siteId in the current document
         * @return a set of siteIds
         */
        virtual std::set<int> retrieveSiteIds();
    };
}

#endif //SYMPOSIUM_DOCUMENT_H
