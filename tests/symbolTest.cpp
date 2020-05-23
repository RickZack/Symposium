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
 * File:   symbolTest.cpp
 * Project: Symposium
  * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Cristian Gianetto <s265387 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 18 ottobre 2019, 15:52
 */

#define BOOST_SERIALIZATION_DYN_LINK 1
#include <gtest/gtest.h>
#include <iostream>
#include "../symbol.h"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>


using namespace Symposium;
TEST(symbolTest, symbolComparisonFromSameHost){
    symbol s1('a', 0, 0, {1}, false);
    symbol s2('b', 0, 1, {1, 5}, false);
    EXPECT_GT(s2, s1);
}

TEST(symbolTest, symbolComparisonFromDifferentHost){
    symbol s1('a', 0, 0, {1}, false);
    symbol s2('b', 1, 0, {1}, false);
    EXPECT_GT(s2, s1);
    EXPECT_FALSE(s2<s1);
}

struct symbolSerialization: ::testing::Test{
    std::stringstream stream;

    void storeSymbol(const symbol& u){
        boost::archive::text_oarchive oa(stream);
        oa<<u;
    }
    void loadSymbol(symbol& u){
        boost::archive::text_iarchive ia(stream);
        ia>>u;
    }
};

TEST_F(symbolSerialization, serialize){
    symbol toStore('a', 0, 0, {1}, false);
    format f("Times New Roman", true, true, false, 0,Color(255,0,0), 0,alignType::left);
    toStore.setCharFormat(f);
    symbol toLoad('b', 0, 1, {1, 5}, false);
    ASSERT_NE(toStore, toLoad);
    storeSymbol(toStore);
    loadSymbol(toLoad);
    EXPECT_EQ(toLoad, toStore);
}