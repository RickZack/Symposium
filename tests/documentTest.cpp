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

//Introduced on 19/05/2020, after suspected bug in retrieving the content of a document after freeing memory
TEST_F(documentTest, retrievingAfterStoring){
    user someUser("username", "AP@ssw0rd!", "noempty", "", 0, nullptr);
    document::doLoadAndStore=true; //make sure that this mechanism is active. By default is disabled if creating a SymClient object
    d.access(someUser, privilege ::modify);
    ASSERT_TRUE(d.getSymbols().size()>0 && d.getSymbols()[0].size()>0);
    d.close(someUser);
    d.access(someUser, privilege ::modify);
    EXPECT_TRUE(d.getSymbols().size()>0 && d.getSymbols()[0].size()>0);
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
    d.localRemove(i3, 0);
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
    d.localRemove(i2, 0);
    EXPECT_EQ(L"ac", d.toText());
}
TEST_F(documentTest, verifySymbol){
    std::pair<int, int> i1={0,0}, i2={0,1}, i3={0,2};
    symbol s1('a', 0, 0, std::vector<int>(), false),
            s2('b', 0, 1, std::vector<int>(), false),
            s3('c', 0, 2, std::vector<int>(), false);
    d.localInsert(i1, s1);
    d.localInsert(i2, s2);
    d.localInsert(i3, s3);

    ASSERT_TRUE(d.getSymbols().size()>0 && d.getSymbols()[0].size()>=3);

    auto coord=d.verifySymbol(d.getSymbols()[0][1]);
    ASSERT_TRUE(coord.first==0 && coord.second==1);
    EXPECT_TRUE(d.getSymbols()[coord.first][coord.second].isVerified());
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
INSTANTIATE_TEST_SUITE_P(TwoSymbolsFromDifferentSiteIds, docRemoteInsertSymbolTest, testing::ValuesIn(inserts));

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
    std::pair<unsigned, unsigned> u1index={u1.row, u1.col};
    std::pair<unsigned, unsigned> u2index={u2.row, u2.col};

    EXPECT_EQ(input.newIndex1, u1index);
    EXPECT_EQ(input.newIndex2, u2index);
}
updatePos updates[]={
        //0)position updated according index of insertion, one insertion does not affect the other user's cursor
        updatePos{symbol('a', 0, 1, {1}, true), symbol('b', 1, 1, {2}, true), {0,1}, {0,2}},
        //1)insertion of the second symbol affects the other user's cursor: a|(1) ->b|(0)a|(1)
        updatePos{symbol('a', 1, 1, {1}, true), symbol('b', 0, 1, {1}, true), {0,1}, {0,2}},
        //2)both symbols from the same user, does not affect the initial position of other user's cursor: |(1) ->|(1)a|(0) -> |(1)ab|(0)
        updatePos{symbol('a', 0, 1, {1}, true), symbol('b', 0, 2, {2}, true), {0,2}, {0,0}},
        //3)like 1, but s2 contains "new line" and it's inserted before s1, so affects other user's cursor
        updatePos{symbol('a', 0, 1, {2}, true), symbol('\r', 1, 1, {1}, true), {1,1}, {1,0}}
};
INSTANTIATE_TEST_SUITE_P(TwoSymbolsFromDifferentSiteIds, docRemoteInsertUpdateCursor, testing::ValuesIn(updates));

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
    d.remoteInsert(input.s2.getSiteId(), input.s2);
    d.remoteRemove(input.s1.getSiteId(), input.s1);

    sessionData u1=std::find_if(d.getActiveUsers().begin(), d.getActiveUsers().end(), [](auto el){return el.first->getSiteId()==0;})->second;
    sessionData u2=std::find_if(d.getActiveUsers().begin(), d.getActiveUsers().end(), [](auto el){return el.first->getSiteId()==1;})->second;
    std::pair<unsigned, unsigned> u1index={u1.row, u1.col};
    std::pair<unsigned, unsigned> u2index={u2.row, u2.col};

    EXPECT_EQ(input.newIndex1, u1index);
    EXPECT_EQ(input.newIndex2, u2index);
}
updatePos updates2[]={
        //0)position updated according index of removal, one removal does not affect the other user's cursor: a|(0)b|(1) -> a|(0)|(1)
        updatePos{symbol('b', 1, 1, {2}, true), symbol('a', 0, 1, {1}, true), {0,1}, {0,1}},
        //1) removal of the second symbol affects the other user's cursor: a|(0)b|(1) ->|(0)b|(1)
        updatePos{symbol('a', 0, 1, {1}, true), symbol('b', 1, 1, {2}, true), {0,0}, {0,1}},
        //2) removal of "new line" character: \r|(1)b|(0) -> |(1)b|(0)
        updatePos{symbol('\r', 1, 1, {1}, true), symbol('a', 0, 1, {2}, true), {0,1}, {0,0}},
};
INSTANTIATE_TEST_SUITE_P(TwoSymbolsFromDifferentSiteIds, docRemoteRemoveUpdateCursor, testing::ValuesIn(updates2));


TEST_F(documentTest, RemovalPosOrder){
    symbol s1('c', 1, 1, {1}, false);
    symbol s2('c', 1, 1, {1}, false);
    d.remoteInsert(1, s1);
    d.remoteRemove(1, s2);

    ASSERT_FALSE(d.getSymbols().empty());
    EXPECT_EQ(document::emptySymbol, d.getSymbols().front()[0]);
}

TEST_P(docRemoteRemoveSymbolTest, RemovalPosOrderErrors){
    Insertion input=GetParam();
    d.remoteInsert(1, input.s1);
    EXPECT_THROW(d.remoteRemove(1, input.s2), documentException);
}
Insertion inserts3[]={
        Insertion(symbol('c', 1, 1, {1}, false), symbol('b', 1, 1, {1}, false),
                  {symbol('c', 1, 1, {1}, false)}),

        Insertion(symbol('c', 1, 1, {1}, false), symbol('c', 1, 1, {1, 2}, false),
                  {symbol('c', 1, 1, {1}, false)}),
};
INSTANTIATE_TEST_SUITE_P(RemoveRemoteSymbolsErrors, docRemoteRemoveSymbolTest, testing::ValuesIn(inserts3));

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

TEST_F(documentTest, editLineStyleChangeStyle){
    user someUser("username", "AP@ssw0rd!", "noempty", "", 0, nullptr);
    d.access(someUser, privilege ::modify);
    auto oldStyle=d.getAlignmentStyle()[0];
    std::pair<alignType, unsigned> newStyle{alignType::right, 1};
    ASSERT_NE(oldStyle, newStyle)<<"New style must be different than oldStyle in test";
    d.editLineStyle(newStyle, 0);
    EXPECT_EQ(newStyle, d.getAlignmentStyle()[0]);
}

TEST_F(documentTest, editLineStyleChangeStyleThrowsOnInvalidIndexes){
    user someUser("username", "AP@ssw0rd!", "noempty", "", 0, nullptr);
    d.access(someUser, privilege ::modify);
    std::pair<alignType, unsigned> newStyle{alignType::right, 1};
    EXPECT_THROW(d.editLineStyle(newStyle, rand()%1000+1), documentException);
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