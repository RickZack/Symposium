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
 * File:   messageTest.cpp
 * Project: Symposium
 * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Alice Morano <s259158 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 08 agosto 2019, 16:01
 */

#include <gtest/gtest.h>
#include "../uri.h"
#include <cstdlib>

using namespace Symposium;

TEST(uriTestBasic, StartsInactiveByDefault) {
    uri u;
    EXPECT_EQ(uriPolicy::inactive, u.getActivePolicy());
}

TEST(uriTestBasic, StartsWithPolicyParameter) {
    uri u(uriPolicy::activeAlways);
    EXPECT_EQ(uriPolicy::activeAlways, u.getActivePolicy());
}

struct uriTest : testing::Test {
    uri u;

    uriTest() {};

    ~uriTest() = default;
};

//Test behaviour common to all uri(s)

TEST_F(uriTest, canBeActivatedForeverWithDefaultPrivilege) {
    u.activateAlways();
    EXPECT_EQ(uri::getDefaultPrivilege(), u.getGranted());
}

TEST_F(uriTest, canBeActivatedForeverWithCustomPrivilege) {
    u.activateAlways(privilege::readOnly);
    EXPECT_EQ(privilege::readOnly, u.getGranted());

    u.activateAlways(privilege::none);
    EXPECT_NE(privilege::none, u.getGranted());
}

TEST_F(uriTest, askForLowerPrivilegeThanSetted) {
    u.activateAlways(privilege::owner);
    //asking for lower privilege than setted, must give the one requested
    EXPECT_EQ(privilege::readOnly, u.getShare(privilege::readOnly));
    EXPECT_EQ(u.getShare(privilege::modify), privilege::modify);
    EXPECT_EQ(privilege::owner, u.getShare(privilege::owner));
}

TEST_F(uriTest, askForHigherPrivilegeThanSetted) {
    u.activateAlways();
    //asking for higher privilege than setted, must give only what it has
    EXPECT_EQ(uri::getDefaultPrivilege(), u.getShare(privilege::owner));
}

TEST_F(uriTest, canBeDeactivated) {
    u.activateAlways();
    ASSERT_EQ(u.getActivePolicy(), uriPolicy::activeAlways);
    u.deactivate();
    EXPECT_EQ(uriPolicy::inactive, u.getActivePolicy());
}

//Test Counted URI

TEST_F(uriTest, zeroCounterIsInvalid) {
    u.activateCount(0);
    EXPECT_EQ(uriPolicy::inactive, u.getActivePolicy());
}

TEST_F(uriTest, canActivateCounter) {
    int count = rand()%100+1;
    u.activateCount(count);
    EXPECT_EQ(uriPolicy::activeCount, u.getActivePolicy());
    EXPECT_EQ(count, u.getSharesLeft());
}

TEST_F(uriTest, askForDefaultPrivilegeWithCounter) {
    u.activateCount(1);
    EXPECT_EQ(uri::getDefaultPrivilege(), u.getShare(uri::getDefaultPrivilege()));
}

TEST_F(uriTest, counterAfterSharingIsDecremented) {
    int count = rand()%100+1;
    u.activateCount(count);
    ASSERT_EQ(u.getSharesLeft(), count);

    u.getShare(privilege::modify);
    EXPECT_EQ(count - 1, u.getSharesLeft());
}

TEST_F(uriTest, counterReachingZeroGivesNoPrivilege) {
    u.activateCount(1);
    u.getShare(privilege::modify);
    EXPECT_EQ(privilege::none, u.getShare(uri::getDefaultPrivilege()));
}

//Test Timed URI

TEST_F(uriTest, canActivateTimer) {
    auto oneHourFromNow = std::chrono::system_clock::now() + std::chrono::hours(1);
    u.activateTimer(oneHourFromNow);
    EXPECT_EQ(uriPolicy::activeTimer, u.getActivePolicy());
}

TEST_F(uriTest, askForDefaultPrivilegeWithTimer) {
    auto oneHourFromNow = std::chrono::system_clock::now() + std::chrono::hours(1);
    u.activateTimer(oneHourFromNow);
    EXPECT_EQ(uri::getDefaultPrivilege(), u.getShare(uri::getDefaultPrivilege()));
}

TEST_F(uriTest, reachingEndTimeGivesNoPrivilege) {
    auto now = std::chrono::system_clock::now();
    u.activateTimer(now);
    EXPECT_EQ(privilege::none, u.getShare(uri::getDefaultPrivilege()));
}

struct uriSerializationTest: ::testing::Test{
    uri toStore, toLoad;
    std::stringstream stream;
    std::chrono::time_point<std::chrono::system_clock> oneHourFromNow = std::chrono::system_clock::now() + std::chrono::hours(1);

    void storeUri(const uri& u){
        boost::archive::text_oarchive oa(stream);
        oa<<u;
    }
    void loadUri(uri& u){
        boost::archive::text_iarchive ia(stream);
        ia>>u;
    }
};

TEST_F(uriSerializationTest, saveAndLoadActivateAlways){
    toStore.activateAlways();
    ASSERT_NE(toStore, toLoad);
    storeUri(toStore);
    loadUri(toLoad);
    EXPECT_EQ(toStore, toLoad);
}

TEST_F(uriSerializationTest, saveAndLoadActivateCount){
    toStore.activateCount(10, privilege::readOnly);
    ASSERT_NE(toStore, toLoad);
    storeUri(toStore);
    loadUri(toLoad);
    EXPECT_EQ(toStore, toLoad);
}

TEST_F(uriSerializationTest, saveAndLoadActivateTimer){
    toStore.activateTimer(oneHourFromNow);
    ASSERT_NE(toStore, toLoad);
    storeUri(toStore);
    loadUri(toLoad);
    EXPECT_EQ(toStore, toLoad);
}
