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
 * File:   SymClientTest.cpp
 * Project: Symposium
  * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Cristian Gianetto <s265387 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 31 ottobre 2019, 17:03
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../SymClient.h"

using namespace Symposium;


struct SymClientAccesser: public SymClient{
    user& getLoggedUser(){
        return loggedUser;
    }
    std::map<std::pair<int, int>, std::pair<user, MyColor>>& getUserColors(){
        return userColors;
    };
};



struct SymClientTest : ::testing::Test{
    user userReceived;
    SymClientAccesser client;
    SymClientTest():userReceived("username", "123abc!!", "nickname", "", 0, nullptr){};
    bool everyUserHasDifferentColor(){
        auto mapped=client.getUserColors();
        std::set<std::tuple<int, int, MyColor>> colors; //{siteId, documentId, color}
        return find_if(mapped.begin(), mapped.end(), [&colors](auto& elem){
            if(colors.find({elem.first.first, elem.first.second, elem.second.second})!=colors.end()){
                return true;
            }
            colors.insert({elem.first.first, elem.first.second, elem.second.second});
            return false;
        })!=mapped.end();
    }
};

TEST_F(SymClientTest, setLoggedUserAssignesUserReceivedToClient){
    client.setLoggedUser(userReceived);
    EXPECT_EQ(userReceived, client.getLoggedUser());
}

TEST_F(SymClientTest, setUserColorsAssignesDifferentColorToUsers){
    //correctness of mapping has already been verified on SymServer class, here we don't care about the wrong mapping
    std::map<int, user> sampleMapping({{0, userReceived},{1, userReceived}});
    client.setUserColors(sampleMapping);
    EXPECT_TRUE(everyUserHasDifferentColor());
}