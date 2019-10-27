/*
 * Copyright (c) 2019 Riccardo Zaccone, Ksenia Del Conte Akimova, Cristian Gianetto, Martina Bellissimo
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
 * File:   AccessStrategyTest.cpp
 * Project: Symposium
  * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Cristian Gianetto <s265387 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 17 ottobre 2019, 11:59
 */
#include <gtest/gtest.h>
#include "../AccessStrategy.h"
#include "../user.h"

using namespace Symposium;
struct RMOAccessTest : testing::Test {
    AccessStrategy *s;
    user u;
    RMOAccessTest(): u("", "", "", "", 0, nullptr) {
        s=new RMOAccess();
    };

    ~RMOAccessTest(){
        delete s;
    }
};


TEST_F(RMOAccessTest, StartsWithoutPriviledges){
    EXPECT_FALSE(s->validateAction(u, privilege::modify));
}

TEST_F(RMOAccessTest, SetPrivilegeReturnsThePreviousPrivilege){
    auto p=privilege::readOnly;
    EXPECT_EQ(privilege::none, s->setPrivilege(u, p));
    EXPECT_EQ(p, s->setPrivilege(u, privilege::modify));
}

TEST_F(RMOAccessTest, AddsUserToCorrectGroup){
    std::vector<privilege> priv({privilege::owner, privilege::modify, privilege::readOnly});
    for(auto p:priv){
        s->setPrivilege(u, p);
        EXPECT_TRUE(s->validateAction(u, p));
    }
}

TEST_F(RMOAccessTest, UserCanHaveNoMorePrivilege){
    auto p=privilege::modify;
    s->setPrivilege(u, p);
    ASSERT_TRUE(s->validateAction(u, p));
    s->setPrivilege(u, privilege::none);
    EXPECT_FALSE(s->validateAction(u, p));
}

TEST_F(RMOAccessTest, AskingForLowerPrivilegeReturnTrue){
    std::vector<privilege> priv({privilege::readOnly, privilege::modify, privilege::owner});
    for(int i=priv.size()-1; i>=0; i--){
        s->setPrivilege(u, priv[i]);
        for(int j=i; j>=0; j--)
            EXPECT_TRUE(s->validateAction(u, priv[j]));
    }
}

TEST_F(RMOAccessTest, AskingForHigherPrivilegeReturnFalse){
    std::vector<privilege> priv({privilege::readOnly, privilege::modify, privilege::owner});
    for(int i=0; i<priv.size(); i++){
        s->setPrivilege(u, priv[i]);
        for(int j=i+1; j<priv.size(); j++)
            EXPECT_FALSE(s->validateAction(u, priv[j]));
    }
}

struct TrivialAccessTest : testing::Test {
    AccessStrategy *s;
    user u;
    TrivialAccessTest(): u("", "", "", "", 0, nullptr) {
        s=new TrivialAccess();
    };

    ~TrivialAccessTest(){
        delete s;
    }
};

TEST_F(TrivialAccessTest, DoesNotSetPrivilege){
    std::vector<privilege> priv({privilege::readOnly, privilege::modify, privilege::owner});
    for(auto p:priv)
        EXPECT_EQ(privilege::none, s->setPrivilege(u, p));
}

TEST_F(TrivialAccessTest, ReturnAlwaysTrue){
    std::vector<privilege> priv({privilege::readOnly, privilege::modify, privilege::owner});
    for(auto p:priv)
        EXPECT_TRUE(s->validateAction(u, p));
}