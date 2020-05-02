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
#include "Symposium.h"
#include "privilege.h"
#include "symbol.h"

#include <boost/serialization/vector.hpp>
#include <boost/serialization/forward_list.hpp>
#include <boost/serialization/utility.hpp>
#include <chrono>
#include <boost/serialization/access.hpp>
#include <boost/serialization/binary_object.hpp>


namespace Symposium {
    struct sessionData{
        friend class boost::serialization::access;
        unsigned int row, col;
        std::chrono::system_clock::time_point tmstmp;
        privilege p;
        sessionData(privilege p=privilege::none, unsigned int row=0, unsigned int col=0): p{p}, row{row}, col{col}{
            tmstmp=std::chrono::system_clock::now();
        }
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version){
            ar & row & col & boost::serialization::make_binary_object(&tmstmp, sizeof(tmstmp)) & p;
        };

        bool operator==(const sessionData &rhs) const {
            return row == rhs.row &&
                   col == rhs.col &&
                   tmstmp == rhs.tmstmp &&
                   p == rhs.p;
        }

        bool operator!=(const sessionData &rhs) const {
            return !(rhs == *this);
        }
    };

    class document {
        static uint_positive_cnt idCounter;                                           /**< id to be assigned to the next created document */
        uint_positive_cnt::type id;                                                         /**< unique identifier for the document */
        std::vector<std::vector<symbol>> symbols;                      /**< container of characters and metadata for CRDT*/
        std::forward_list<std::pair<const user *, sessionData>> activeUsers;    /**< list of users currently active on the document, with the current privilege*/
        int numchar;                                                    /**< number of printable characters */
        std::vector<char> strategyCache ;
        wchar_t  strategy='r';
        int level=0;
        bool loaded;
        static const std::string basePath;

        static constexpr wchar_t  emptyChar='~';
        static const symbol emptySymbol;

        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version){
            ar & id & symbols  & activeUsers  & numchar & strategyCache & strategy;
        };
    public:
        static bool serializeFull;
        document(uint_positive_cnt::type id = document::idCounter);

        uint_positive_cnt::type getId() const;

        const std::vector<std::vector<symbol>> &getSymbols() const;

        const std::forward_list<std::pair<const user *, sessionData>> &getActiveUsers() const;

        int getNumchar() const;

        /**
         * @brief countsNumLines counts the number of line effectively present in the symbols vector
         * @return number of lines
         */
        int countsNumLines() const;

        /**
         * @brief countCharsInLine counts the number of chars that are present in the @e line
         * @param line
         * @return
         */
        int countCharsInLine(int line) const;




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
        virtual symbol localInsert(const std::pair<unsigned int, unsigned int> &indexes, symbol &toInsert);

        /**
         * @brief remove a symbol in the document as consequence of an user's action on the GUI
         * @param indexes symbol to remove
         */
        virtual symbol
        localRemove(const std::pair<unsigned int, unsigned int> &indexes, uint_positive_cnt::type siteId);

        /**
         * @brief insert a symbol in the document as consequence of a remote user's action
         * @param siteId the site id of the user performing the insertion
         * @param toInsert symbol to insert
         * @return the position of the inserted symbol
         */
        virtual std::pair<unsigned int, unsigned int> remoteInsert(uint_positive_cnt::type siteId, const symbol &toInsert);

        /**
         * @brief remove a symbol in the document as consequence of a remote user's action
         * @param siteId the site id of the user performing the removal
         * @param toRemove symbol to remove
         * @return the position of the removed symbol
         */
        virtual std::pair<unsigned int, unsigned int> remoteRemove(uint_positive_cnt::type siteId, const symbol &toRemove);

        /**
         * @brief set a symbol inside the document as verified
         * @param toVerify the symbol to be marked as verified
         */
        virtual std::pair<unsigned int, unsigned int> verifySymbol(const symbol& toVerify);

        /**
         * @brief update the position of the cursor for the user @e target
         * @param targetSiteId the user whose cursor's position has been changed
         * @param newRow the new row number
         * @param newCol the new column number
         */
        virtual void updateCursorPos(uint_positive_cnt::type targetSiteId, unsigned int newRow, unsigned int newCol);

        /**
         * @brief give a representation of the document ad sequence of wide characters
         * @return a string of wide characters with the document's content
         */
        std::wstring toText() const;

        /**
         * @brief removes the user @e noLongerActive from @e activeUsers
         * @param noLongerActive the user that is no longer active on the current document
         *
         * This method is called when a client closes the document it is working on.
         */
        virtual void close(const user &noLongerActive);

        /**
         * @brief Store permanently the content of the document onto the disk
         */
        void store() const;

        /**
         * @brief Load the content of the document from disk
         * @return a bool indicating success of failure on loading
         */
        bool load();

        /**
         * @brief retrieves the set of siteId in the current document
         * @return a set of siteIds
         */
        virtual std::set<uint_positive_cnt::type> retrieveSiteIds() const;

        /**
         * @brief it checks if the index i0 and i1 are coherent with the capacity of @e symbols
         * @param i0 the first index to check
         * @param i1 the second index to check
         */
        void checkIndex(unsigned int i0, unsigned int i1);


        void checkVector();

    private:
        /**
         * @brief it determines the globally unique fractional index position of the new character.
         * @param indexes position of the adjacent characters used to generate the position of the new one
         * @param toInsert the value to insert
         */
        symbol generatePosition(const std::pair<unsigned int, unsigned int> indexes, const symbol &toInsert);

        /**
         * @brief it searches for the position of the inserted symbol
         * @param symbol inserted symbol
         * @return position
         */
        std::pair<unsigned int, unsigned int> findInsertIndex(const symbol &symbol) const;

        /**
         * @brief it finds the position of the last Symbol to insert the symbol at the end of @e vector
         * @param aChar the lastSymbol in the vector
         * @param vector is the LastLine in the vector
         * @param lines is the number of lines in the vector
         * @return the position
         */
        std::pair<unsigned int, unsigned int>
        findEndPosition(const symbol aChar, int lines) const;

        /**
         * @brief it searches for the symbol that is inserted in the middle of the line
         * @param ch symbol
         * @param vector line in which the symbol is searched
         * @return
         */
        int findInsertInLine(const symbol ch, const std::vector<Symposium::symbol> vector, int line) const;

        /**
         * @brief it searches for the position of a symbol in order to find it and eliminate it
         * @param symbol the symbol to search
         * @return the position of @e symbol
         */
        std::pair<unsigned int, unsigned int> findPosition(const symbol &symbol) const;


        unsigned int findIndexInLine(const symbol &symbol, const std::vector<Symposium::symbol> vector) const;

        /**
         * @brief searches the position before the one of the considered value
         * @param pair the indexes
         * @return the searched position
         */
        symbol findPosBefore(const std::pair<unsigned int, unsigned int> pair) const;

        /**
         * @brief searches the position after the one of the considered value
         * @param pair the indexes
         * @return the searched position
         */
        symbol findPosAfter(const std::pair<unsigned int, unsigned int> pair) const;

        /**
         * @brief recoursive algorithm to dynamically generates the relative position of a symbol inserted in between two other ones
         * @param vector the posBefore
         * @param vector1 the posAfter
         * @return the searched position
         */
        std::vector<int>
        generatePosBetween(std::vector<int> vector, std::vector<int> vector1, std::vector<int> vector2, int level,
                           int i,
                           int i1);

        /**
         * @brief finds the id of the symbol inserted in between two other onws
         * @param id1
         * @param id2
         * @param boundaryStrategy
         * @return
         */

        int generateIdBetween(int id1, int id2,const char boundaryStrategy) const;

        /**
         * @brief it modifies the @e strategy parameter
         * @param level
         * @return
         */
        char retrieveStrategy(const int level);





    };
}


#endif //SYMPOSIUM_DOCUMENT_H
