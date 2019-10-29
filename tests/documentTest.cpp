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
 * File:   documentTest.cpp
 * Project: Symposium
 * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Alice Morano <s259158 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 09 agosto 2019, 13:22
 */
#include <gtest/gtest.h>
#include "../document.h"
#include "../symbol.h"
#include "../user.h"


using namespace Symposium;
struct documentTest : testing::Test{
    document d;
    documentTest(){};
    ~documentTest()= default;
};

TEST_F(documentTest, differentDocumentsHaveDifferentIds){
    document d2;
    EXPECT_NE(d.getId(), d2.getId());
}

TEST_F(documentTest, userCanAccessDocument){
    user someUser("username", "AP@ssw0rd!", "noempty", "", 0, nullptr);
    d.access(someUser, privilege ::modify);
    EXPECT_FALSE(d.getActiveUsers().empty());
}

TEST_F(documentTest, userIsNoLongerInActiveListAfterClosing){
    user someUser("username", "AP@ssw0rd!", "noempty", "", 0, nullptr);
    d.access(someUser, privilege ::modify);
    d.close(someUser);
    EXPECT_TRUE(d.getActiveUsers().empty());
}

TEST_F(documentTest, localAddingSymbolsInSequence){
    int i1[]={0,0}, i2[]={0,1}, i3[]={0,2};
    symbol s1('a', 0, 0, std::vector<int>()),
           s2('b', 0, 1, std::vector<int>()),
           s3('c', 0, 2, std::vector<int>());
    d.localInsert(i1, s1);
    d.localInsert(i2, s2);
    d.localInsert(i3, s3);
    EXPECT_EQ(L"abc", d.toText());
}

TEST_F(documentTest, localAddingSymbolsInTheMiddle){
    int i1[]={0,0}, i2[]={0,1};
    symbol s1('a', 0, 0, std::vector<int>()),
            s2('b', 0, 1, std::vector<int>()),
            s3('c', 0, 2, std::vector<int>());
    d.localInsert(i1, s1);
    d.localInsert(i2, s2);
    d.localInsert(i2, s3);
    EXPECT_EQ(L"acb", d.toText());
}

TEST_F(documentTest, localRemovingSymbolsFromBottom){
    int i1[]={0,0}, i2[]={0,1}, i3[]={0,2};
    symbol s1('a', 0, 0, std::vector<int>()),
            s2('b', 0, 1, std::vector<int>()),
            s3('c', 0, 2, std::vector<int>());
    d.localInsert(i1, s1);
    d.localInsert(i2, s2);
    d.localInsert(i3, s3);
    d.localRemove(i3);
    EXPECT_EQ(L"ab", d.toText());
}

TEST_F(documentTest, localRemovingSymbolsFromMiddle){
    int i1[]={0,0}, i2[]={0,1}, i3[]={0,2};
    symbol s1('a', 0, 0, std::vector<int>()),
            s2('b', 0, 1, std::vector<int>()),
            s3('c', 0, 2, std::vector<int>());
    d.localInsert(i1, s1);
    d.localInsert(i2, s2);
    d.localInsert(i3, s3);
    d.localRemove(i2);
    EXPECT_EQ(L"ac", d.toText());
}

struct Insertion{
    symbol s1;
    symbol s2;
    std::vector<symbol> expected;

    Insertion(symbol s1, symbol s2, std::vector<symbol> result): s1{s1}, s2{s2}, expected{result}{};
};

struct docRemoteSymbolTest: public testing::TestWithParam<Insertion>{
    document d;
    docRemoteSymbolTest()= default;
};

TEST_P(docRemoteSymbolTest, InsertionPosOrder){
    Insertion input=GetParam();
    d.remoteInsert(input.s1);
    d.remoteInsert(input.s2);
    ASSERT_FALSE(d.getSymbols().empty());
    ASSERT_FALSE(d.getSymbols().front().empty());

    EXPECT_EQ(input.expected, d.getSymbols().front());
}
Insertion inserts[]={
        Insertion(symbol('a', 0, 1, {1}), symbol('b', 1, 1, {2}),
                { symbol('a', 0, 1, {1}), symbol('b', 0, 1, {2}) }),

        Insertion(symbol('a', 0, 1, {1}), symbol('b', 1, 1, {1}),
                  { symbol('b', 1, 1, {1}), symbol('a', 0, 1, {1}) }),

        Insertion(symbol('a', 0, 1, {1}), symbol('b', 1, 1, {1,2}),
                  { symbol('a', 0, 1, {1}), symbol('b', 0, 1, {1,2}) }),
};
INSTANTIATE_TEST_CASE_P(TwoSymbolsFromDifferentSiteIds, docRemoteSymbolTest, testing::ValuesIn(inserts));

TEST_P(docRemoteSymbolTest, RemovalPosOrder){
    Insertion input=GetParam();
    d.remoteInsert(input.s1);
    d.remoteRemove(input.s2);

    ASSERT_FALSE(d.getSymbols().empty());
    ASSERT_FALSE(d.getSymbols().front().empty());

    EXPECT_EQ(input.expected, d.getSymbols().front());
}
Insertion inserts2[]={
        Insertion(symbol('c', 1, 1, {1}), symbol('c', 1, 1, {1}),
                  std::vector<symbol>()),

        Insertion(symbol('c', 1, 1, {1}), symbol('b', 1, 1, {1}),
                  { symbol('c', 1, 1, {1})}),

        Insertion(symbol('c', 1, 1, {1}), symbol('c', 1, 1, {1,2}),
                  { symbol('c', 1, 1, {1}), }),
};
INSTANTIATE_TEST_CASE_P(RemoveRemoteSymbols, docRemoteSymbolTest, testing::ValuesIn(inserts2));

TEST_F(documentTest, canRetrieveSiteIds){
    int i1[]={0,0}, i2[]={0,1}, i3[]={0,2};
    symbol s1('a', 0, 0, std::vector<int>()),
            s2('b', 1, 0, std::vector<int>()),
            s3('c', 2, 2, std::vector<int>());
    d.localInsert(i1, s1);
    d.localInsert(i2, s2);
    d.localInsert(i3, s3);

    std::set<int> expected;
    expected.insert(0);
    expected.insert(1);
    expected.insert(2);

    EXPECT_EQ(expected, d.retrieveSiteIds());
}