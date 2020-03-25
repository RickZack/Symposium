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
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include "../filesystem.h"
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
    std::pair<int, int> i1={0,0}, i2={0,1}, i3={0,2};
    symbol s1('a', 0, 0, std::vector<int>(), false),
          s2('b', 0, 1, std::vector<int>(), false),
    s3('c', 0, 2, std::vector<int>(), false);
    d.localInsert(i1, s1);
    d.localInsert(i2, s2);
    d.localInsert(i3, s3);
    EXPECT_EQ(L"abc", d.toText());
 }

TEST_F(documentTest, localAddingSymbolsInTheMiddle){
    std::pair<int, int> i1={0,0}, i2={0,1};
    symbol s1('a', 0, 0, std::vector<int>(), false),
            s2('b', 0, 1, std::vector<int>(), false),
            s3('c', 0, 2, std::vector<int>(), false);
    d.localInsert(i1, s1);
    d.localInsert(i2, s2);
    d.localInsert(i2, s3);
    EXPECT_EQ(L"acb", d.toText());
}

TEST_F(documentTest, localRemovingSymbolsFromBottom){
    std::pair<int, int> i1={0,0}, i2={0,1}, i3={0,2};
    symbol s1('a', 0, 0, std::vector<int>(), false),
            s2('b', 0, 1, std::vector<int>(), false),
            s3('c', 0, 2, std::vector<int>(), false);
    d.localInsert(i1, s1);
    d.localInsert(i2, s2);
    d.localInsert(i3, s3);
    d.localRemove(i3);
    EXPECT_EQ(L"ab", d.toText());
}

TEST_F(documentTest, localRemovingSymbolsFromMiddle){
    std::pair<int, int> i1={0,0}, i2={0,1}, i3={0,2};
    symbol s1('a', 0, 0, std::vector<int>(), false),
            s2('b', 0, 1, std::vector<int>(), false),
            s3('c', 0, 2, std::vector<int>(), false);
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

struct docRemoteInsertSymbolTest: public testing::TestWithParam<Insertion>{
    document d;
    docRemoteInsertSymbolTest()= default;
};

TEST_P(docRemoteInsertSymbolTest, InsertionPosOrder){
    Insertion input=GetParam();
    d.remoteInsert(input.s1.getSiteId(), input.s1);
    d.remoteInsert(input.s2.getSiteId(), input.s2);
    ASSERT_FALSE(d.getSymbols().empty());
    ASSERT_FALSE(d.getSymbols().front().empty());
    // bisogna considerare solo i primi due perchè gli altri sono emptyChar.
    EXPECT_EQ(input.expected[0].getCh(), d.getSymbols().front()[0].getCh());
    EXPECT_EQ(input.expected[1].getCh(), d.getSymbols().front()[1].getCh());
}
Insertion inserts[]={
        Insertion(symbol('a', 0, 1, {1}, true), symbol('b', 1, 1, {2}, true),
                  {symbol('a', 0, 1, {1}, true), symbol('b', 1, 1, {2}, true)}),

        Insertion(symbol('a', 1, 1, {1}, true), symbol('b', 0, 1, {1}, true),
                  {symbol('b', 0, 1, {1}, true), symbol('a', 1, 1, {1}, true)}),

        Insertion(symbol('a', 0, 1, {1}, true), symbol('b', 1, 1, {1, 2}, true),
                  {symbol('a', 0, 1, {1}, true), symbol('b', 0, 1, {1, 2}, true)}),
};
INSTANTIATE_TEST_CASE_P(TwoSymbolsFromDifferentSiteIds, docRemoteInsertSymbolTest, testing::ValuesIn(inserts));

struct docRemoteRemoveSymbolTest: public testing::TestWithParam<Insertion>{
    document d;
    docRemoteRemoveSymbolTest()= default;
};

struct updatePos{
    symbol s1;
    symbol s2;
    std::pair<unsigned, unsigned> newIndex1;
    std::pair<unsigned, unsigned> newIndex2;

    updatePos(symbol s1, symbol s2, std::pair<unsigned, unsigned> n1, std::pair<unsigned, unsigned> n2): s1{s1}, s2{s2}, newIndex1{n1}, newIndex2{n2}{};
};

struct docRemoteInsertUpdateCursor: public testing::TestWithParam<updatePos>{
    document d;
    user user1;
    user user2;
    docRemoteInsertUpdateCursor(): user1("username", "AP@ssw0rd!", "noempty", "", 0, nullptr), user2("username", "AP@ssw0rd!", "noempty", "", 1, nullptr){}
    ~docRemoteInsertUpdateCursor()= default;
};

TEST_P(docRemoteInsertUpdateCursor, InsertionUpdatePosition){
    updatePos input=GetParam();
    d.access(user1, privilege::modify); d.access(user2, privilege::modify);

    d.remoteInsert(input.s1.getSiteId(), input.s1);
    d.remoteInsert(input.s2.getSiteId(), input.s2);

    sessionData u1=std::find_if(d.getActiveUsers().begin(), d.getActiveUsers().end(), [](auto el){return el.first->getSiteId()==0;})->second;
    sessionData u2=std::find_if(d.getActiveUsers().begin(), d.getActiveUsers().end(), [](auto el){return el.first->getSiteId()==1;})->second;

    EXPECT_TRUE(u1.row==input.newIndex1.first && u1.col==input.newIndex1.second);
    EXPECT_TRUE(u1.row==input.newIndex2.first && u1.col==input.newIndex2.second);
}
updatePos updates[]={
        updatePos{symbol('a', 0, 1, {1}, true), symbol('b', 1, 1, {2}, true), {0,1}, {0,2}},
        updatePos{symbol('a', 1, 1, {1}, true), symbol('b', 0, 1, {1}, true), {0,2}, {0,1}}
};
INSTANTIATE_TEST_CASE_P(TwoSymbolsFromDifferentSiteIds, docRemoteInsertUpdateCursor, testing::ValuesIn(updates));

//TODO: da rivedere
struct docRemoteRemoveUpdateCursor: public testing::TestWithParam<updatePos>{
    document d;
    user user1;
    user user2;
    docRemoteRemoveUpdateCursor(): user1("username", "AP@ssw0rd!", "noempty", "", 0, nullptr), user2("username", "AP@ssw0rd!", "noempty", "", 1, nullptr){}
    ~docRemoteRemoveUpdateCursor()= default;
};

TEST_P(docRemoteRemoveUpdateCursor, RemovalUpdatePosition){
    updatePos input=GetParam();
    d.access(user1, privilege::modify); d.access(user2, privilege::modify);

    d.remoteInsert(input.s1.getSiteId(), input.s1);
    d.remoteRemove(input.s2.getSiteId(), input.s1);

    sessionData u1=std::find_if(d.getActiveUsers().begin(), d.getActiveUsers().end(), [](auto el){return el.first->getSiteId()==0;})->second;
    sessionData u2=std::find_if(d.getActiveUsers().begin(), d.getActiveUsers().end(), [](auto el){return el.first->getSiteId()==1;})->second;

    EXPECT_TRUE(u1.row==input.newIndex1.first && u1.col==input.newIndex1.second);
    EXPECT_TRUE(u1.row==input.newIndex2.first && u1.col==input.newIndex2.second);
}
updatePos updates2[]={
        updatePos{symbol('a', 0, 1, {1}, true), symbol('b', 1, 1, {2}, true), {0,1}, {0,0}}
};
INSTANTIATE_TEST_CASE_P(TwoSymbolsFromDifferentSiteIds, docRemoteRemoveUpdateCursor, testing::ValuesIn(updates2));

TEST_P(docRemoteRemoveSymbolTest, RemovalPosOrder){
    Insertion input=GetParam();
    d.remoteInsert(1, input.s1);
    d.remoteRemove(1, input.s2);

    ASSERT_FALSE(d.getSymbols().empty());

    EXPECT_EQ(input.expected, d.getSymbols().front());
}
Insertion inserts2[]={
        Insertion(symbol('c', 1, 1, {1}, false), symbol('c', 1, 1, {1}, false),
                  std::vector<symbol>()),

        Insertion(symbol('c', 1, 1, {1}, false), symbol('b', 1, 1, {1}, false),
                  {symbol('c', 1, 1, {1}, false)}),

        Insertion(symbol('c', 1, 1, {1}, false), symbol('c', 1, 1, {1, 2}, false),
                  {symbol('c', 1, 1, {1}, false), }),
};
INSTANTIATE_TEST_CASE_P(RemoveRemoteSymbols, docRemoteRemoveSymbolTest, testing::ValuesIn(inserts2));

TEST_F(documentTest, canRetrieveSiteIds){
    std::pair<int, int> i1={0,0}, i2={0,1}, i3={0,2};
    symbol s1('a', 0, 0, std::vector<int>(), false),
            s2('b', 1, 0, std::vector<int>(), false),
            s3('c', 2, 2, std::vector<int>(), false);
    d.localInsert(i1, s1);
    d.localInsert(i2, s2);
    d.localInsert(i3, s3);

    std::set<uint_positive_cnt::type> expected;
    expected.insert(0);
    expected.insert(1);
    expected.insert(2);

    EXPECT_EQ(expected, d.retrieveSiteIds());
}

TEST_F(documentTest, updateCursorPos){
    srand(time(NULL)); //to have some randomness on row and exp_col numbers
    unsigned int exp_row=rand()%1000, exp_col=rand()%1000;
    user someUser("username", "AP@ssw0rd!", "noempty", "", 0, nullptr);
    user someOtherUser("username2", "AP@ssw0rd!", "noempty", "", 1, nullptr);
    d.access(someUser, privilege ::modify);
    d.access(someOtherUser, privilege ::modify);


    d.updateCursorPos(someUser.getSiteId(), exp_row, exp_col);
    std::forward_list<std::pair<const user *, sessionData>> expected=d.getActiveUsers();
    auto entrySomeUser=std::find_if(expected.begin(), expected.end(), [&](std::pair<const user *, sessionData> el){return el.first->getSiteId()==someUser.getSiteId();});
    ASSERT_NE(entrySomeUser, expected.end());
    EXPECT_EQ(entrySomeUser->second.row, exp_row);
    EXPECT_EQ(entrySomeUser->second.col, exp_col);
}

TEST_F(documentTest, updateCursorPosNoThrowIfNoExistingUser){
    user someUser("username", "AP@ssw0rd!", "noempty", "", 0, nullptr);
    d.access(someUser, privilege ::modify);
    EXPECT_NO_THROW(d.updateCursorPos(someUser.getSiteId()+1, 10, 20));
}

struct documentSerializationTest: ::testing::Test{
    document d1, d2;
    std::stringstream stream;

    void store(const document &as){
        boost::archive::text_oarchive oa(stream);
        oa<<as;
    }
    void load(document &as){
        boost::archive::text_iarchive ia(stream);
        ia>>as;
    }
};

TEST_F(documentSerializationTest, serialize){
    symbol s1('a', 0, 0, std::vector<int>(), false);
    d1.localInsert({0,0}, s1);
    ASSERT_NE(d1, d2);
    store(d1);
    load(d2);
    EXPECT_EQ(d2, d1);
}