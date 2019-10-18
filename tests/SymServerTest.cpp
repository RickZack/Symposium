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
 * File:   SymServerTest.cpp
 * Project: Symposium
  * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Cristian Gianetto <s265387 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 18 ottobre 2019, 16:45
 */
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../SymServer.h"

/*
 * Used to access read-only the members of SymServer without adding getters in production class
 */
struct SymServerAccesser: public SymServer{
    std::unordered_map<std::string, user> getRegistered(){
        return registered;
    }
};

struct SymServerTestUserFunctionality : testing::Test {
    user newUser, alreadyPresent, newDifferentUser;
    SymServerAccesser server;
    static const std::string validIconPath;

    SymServerTestUserFunctionality():
    newUser("mario", "a123@bty!!", "m@ario", validIconPath, 0, nullptr),
    alreadyPresent(newUser),
    newDifferentUser("lucio", "a123@bty!!", "lupoLucio", validIconPath, 0, nullptr),
    server(){};
    bool userAlreadyPresent(user& toCheck){
        auto users=server.getRegistered();
        return users.find(toCheck.getUsername())!=users.end();
    }

    ~SymServerTestUserFunctionality() = default;
};
const std::string SymServerTestUserFunctionality::validIconPath="./userIcons/test.jpg";

TEST_F(SymServerTestUserFunctionality, addUserAddingUserNotPresent){
    server.addUser(newUser);
    EXPECT_TRUE(userAlreadyPresent(newUser));
}

TEST_F(SymServerTestUserFunctionality, addUserAddingUserAlreadyPresent){
    server.addUser(newUser);
    EXPECT_THROW(server.addUser(alreadyPresent), SymServerException);
}

TEST_F(SymServerTestUserFunctionality, addUserAddingUserHavingWrongIconPath){
    newUser.setIconPath("");
    EXPECT_THROW(server.addUser(newUser), SymServerException);
}

TEST_F(SymServerTestUserFunctionality, addUserAddingUserHavingWrongNickname){
    newUser.setNickname("");
    EXPECT_THROW(server.addUser(newUser), SymServerException);
}

TEST_F(SymServerTestUserFunctionality, DISABLED_addUserAssignesHomeToUser){
    auto newUserComplete=server.addUser(newUser);
    EXPECT_TRUE(newUserComplete.getHome()!= nullptr);
}

TEST_F(SymServerTestUserFunctionality, addUserGivesDifferentSiteId){
    auto newUserComplete=server.addUser(newUser);
    auto newDifferentUserComplete=server.addUser(newDifferentUser);
    EXPECT_NE(newUserComplete.getSiteId(), newDifferentUserComplete.getSiteId());
}