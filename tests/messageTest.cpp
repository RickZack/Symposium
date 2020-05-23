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
 * Created on 13 agosto 2019, 16:09
 */

#include <gtest/gtest.h>
#include <boost/serialization/unique_ptr.hpp>
#include <gmock/gmock.h>
#include "../message.h"
#include "../SymServer.h"
#include "../SymClient.h"
#include "../filesystem.h"

using namespace Symposium;

TEST(messageTest, msgIdIsSet){
    clientMessage m(msgType::login, {"", ""});
    EXPECT_NE(0, m.getMsgId());
}

TEST(messageTest, relatedMessagesHasSameId){
    clientMessage m(msgType::login, {"", ""});
    serverMessage m2(m.getAction(), msgOutcome::failure, m.getMsgId());

    ASSERT_EQ(m.getAction(), m2.getAction());
    ASSERT_EQ(m.getMsgId(), m2.getMsgId());

    EXPECT_TRUE(m2.isRelatedTo(m));
}

/*
 * Tests for action consistency with the type of message
 */

/*
 * Data vectors with msgType variables
 * Names firstGroup, secondGroup, ... are with reference to groups of msgType in messageData.h
 */
const std::vector<msgType> firstGroup={msgType::registration, msgType::login, msgType::changeUserPwd, msgType::changeUserData, msgType::removeUser,
                      msgType::logout};
const std::vector<msgType> secondGroup={msgType::createRes, msgType::createNewDir, msgType::openRes, msgType::openNewRes, msgType::changeResName,
                                  msgType::removeRes};
const std::vector<msgType> thirdGroup={msgType::mapChangesToUser, msgType::changePrivileges, msgType::shareRes,
                                 msgType::insertSymbol, msgType::removeSymbol};
const std::vector<msgType> fourthGroup={msgType::addActiveUser, msgType::removeActiveUser, msgType::closeRes, msgType::updateCursor};

/*
 * An EXPECT_THROW by Google Test that also prints the action the caused the not-throwing
 */
#define EXPECT_THROW_MESSAGE_CONSTRUCTION(statement, action, exceptionType) \
        EXPECT_THROW(statement, exceptionType) \
        <<"action: "<<action<<" should not be compatible with message class";

#define EXPECT_NO_THROW_MESSAGE_CONSTRUCTION(statement, action) \
        EXPECT_NO_THROW(statement) \
        <<"action: "<<action<<" should be compatible with message class";

struct simpleMsgTypeTest: ::testing::TestWithParam<msgType>{
    message *m;

    simpleMsgTypeTest(){
        m=nullptr;
    }
    virtual ~simpleMsgTypeTest(){
        /*
         * If construction succeed (and it shouldn't), m points to some memory that has to be freed
         */
        if(m!= nullptr)
            delete m;
    }
};

//This is useful to print the enum names in case of test failing
std::ostream& operator<<(std::ostream& output, msgType m){
    switch(m){
        case msgType::registration: return output<<"msgType::registration";
        case msgType::login: return output<<"msgType::login";
        case msgType::changeUserPwd: return output<<"msgType::changeUserPwd";
        case msgType::changeUserData: return output<<"msgType::changeUserData";
        case msgType::removeUser: return output<<"msgType::removeUser";
        case msgType::logout: return output<<"msgType::logout";

        case msgType::createRes: return output<<"msgType::createRes";
        case msgType::createNewDir: return output<<"msgType::createNewDir";
        case msgType::openRes: return output<<"msgType::openRes";
        case msgType::openNewRes: return output<<"msgType::openNewRes";
        case msgType::changeResName: return output<<"msgType::changeResName";
        case msgType::removeRes: return output<<"msgType::removeRes";

        case msgType::mapChangesToUser: return output<<"msgType::mapChangesToUser";
        case msgType::changePrivileges: return output<<"msgType::changePrivileges";
        case msgType::shareRes: return output<<"msgType::shareRes";
        case msgType::insertSymbol: return output<<"msgType::insertSymbol";
        case msgType::removeSymbol: return output<<"msgType::removeSymbol";

        case msgType::addActiveUser: return output<<"msgType::addActiveUser";
        case msgType::removeActiveUser: return output<<"msgType::removeActiveUser";
        case msgType::closeRes: return output<<"msgType::closeRes";
        case msgType::updateCursor: return output<<"msgType::updateCursor";

        default: return output<<"operator<< not defined for this value";
    }
}
//After bug found 10/12/2019: serverMessage is compatible with every msgType, mostly because
//it's used to signal failure
struct serverMsgNotForbidden: simpleMsgTypeTest{};
TEST_P(serverMsgNotForbidden, serverMsgNoThrowInConstruction){
    msgType action=GetParam();
    EXPECT_NO_THROW_MESSAGE_CONSTRUCTION(m= new serverMessage(action, msgOutcome::success), action)
}
INSTANTIATE_TEST_CASE_P(firstGroupMsgTypeSet, serverMsgNotForbidden, testing::ValuesIn(firstGroup));
INSTANTIATE_TEST_CASE_P(secondGroupMsgTypeSet, serverMsgNotForbidden, testing::ValuesIn(secondGroup));
INSTANTIATE_TEST_CASE_P(thirdGroupMsgTypeSet, serverMsgNotForbidden, testing::ValuesIn(thirdGroup));
INSTANTIATE_TEST_CASE_P(fourthGroupMsgTypeSet, serverMsgNotForbidden, testing::ValuesIn(fourthGroup));

struct askResMsgForbiddenActions: simpleMsgTypeTest{};

TEST_P(askResMsgForbiddenActions, askResThrowExceptionInConstruction){
    msgType action=GetParam();
    EXPECT_THROW_MESSAGE_CONSTRUCTION(m= new askResMessage(action, {"", ""}, "", "", "", uri::getDefaultPrivilege(), 0), action, messageException);
}

INSTANTIATE_TEST_CASE_P(firstGroupMsgTypeSet, askResMsgForbiddenActions, testing::ValuesIn(firstGroup));
INSTANTIATE_TEST_CASE_P(thirdGroupMsgTypeSet, askResMsgForbiddenActions, testing::ValuesIn(thirdGroup));
INSTANTIATE_TEST_CASE_P(fourthGroupMsgTypeSet, askResMsgForbiddenActions, testing::ValuesIn(fourthGroup));

struct signUpMsgForbiddenActions: simpleMsgTypeTest {
    user u;

    signUpMsgForbiddenActions() : u("username", "AP@ssw0rd!", "noempty", "", 0, nullptr) {}
    ~signUpMsgForbiddenActions()=default;
};

TEST_P(signUpMsgForbiddenActions, signUpThrowExceptionInConstruction){
    msgType action=GetParam();
    EXPECT_THROW_MESSAGE_CONSTRUCTION(m=new signUpMessage(action, {"",""}, u), action, messageException);
}
INSTANTIATE_TEST_CASE_P(AllButRegistrationFromFirstGroup, signUpMsgForbiddenActions, testing::ValuesIn(std::vector<msgType>(firstGroup.begin()+1, firstGroup.end())));
INSTANTIATE_TEST_CASE_P(secondGroupMsgTypeSet, signUpMsgForbiddenActions, testing::ValuesIn(secondGroup));
INSTANTIATE_TEST_CASE_P(thirdGroupMsgTypeSet, signUpMsgForbiddenActions, testing::ValuesIn(thirdGroup));
INSTANTIATE_TEST_CASE_P(fourthGroupMsgTypeSet, signUpMsgForbiddenActions, testing::ValuesIn(fourthGroup));


struct updateDocMsgForbiddenActions: simpleMsgTypeTest {};

TEST_P(updateDocMsgForbiddenActions, updateDocThrowExceptionInConstruction) {
    msgType action = GetParam();
    EXPECT_THROW_MESSAGE_CONSTRUCTION(m = new updateDocMessage(action, {"", ""}, 0), action, messageException);
}
INSTANTIATE_TEST_CASE_P(firstGroupMsgTypeSet, updateDocMsgForbiddenActions, testing::ValuesIn(firstGroup));
INSTANTIATE_TEST_CASE_P(secondGroupMsgTypeSet, updateDocMsgForbiddenActions, testing::ValuesIn(secondGroup));
INSTANTIATE_TEST_CASE_P(AllButMapChangesFromThirdGroup, updateDocMsgForbiddenActions, testing::ValuesIn(std::vector<msgType>(thirdGroup.begin()+1, thirdGroup.end())));
INSTANTIATE_TEST_CASE_P(AllButCloseResFromFourthGroup, updateDocMsgForbiddenActions, testing::ValuesIn(std::vector<msgType>(fourthGroup.begin(), fourthGroup.end()-2)));

struct loginMsgForbiddenActions: signUpMsgForbiddenActions {};
TEST_P(loginMsgForbiddenActions, loginThrowExceptionInConstruction) {
    msgType action = GetParam();
    EXPECT_THROW_MESSAGE_CONSTRUCTION(m = new loginMessage(action, msgOutcome::success, u), action, messageException);
}
INSTANTIATE_TEST_CASE_P(AllButRegistrationAndLoginFromFirstGroup, loginMsgForbiddenActions, testing::ValuesIn(std::vector<msgType>(firstGroup.begin()+2, firstGroup.end())));
INSTANTIATE_TEST_CASE_P(secondGroupMsgTypeSet, loginMsgForbiddenActions, testing::ValuesIn(secondGroup));
INSTANTIATE_TEST_CASE_P(thirdGroupMsgTypeSet, loginMsgForbiddenActions, testing::ValuesIn(thirdGroup));
INSTANTIATE_TEST_CASE_P(fourthGroupMsgTypeSet, loginMsgForbiddenActions, testing::ValuesIn(fourthGroup));


struct mapMsgForbiddenActions: simpleMsgTypeTest {};
TEST_P(mapMsgForbiddenActions, mapThrowExceptionInConstruction) {
    msgType action = GetParam();
    EXPECT_THROW_MESSAGE_CONSTRUCTION(m = new mapMessage(action, msgOutcome::success, std::map<uint_positive_cnt::type, user>()), action, messageException);
}
INSTANTIATE_TEST_CASE_P(firstGroupMsgTypeSet, mapMsgForbiddenActions, testing::ValuesIn(firstGroup));
INSTANTIATE_TEST_CASE_P(secondGroupMsgTypeSet, mapMsgForbiddenActions, testing::ValuesIn(secondGroup));
INSTANTIATE_TEST_CASE_P(AllButMapChangesFromThirdGroup, mapMsgForbiddenActions, testing::ValuesIn(std::vector<msgType>(thirdGroup.begin()+1, thirdGroup.end())));
INSTANTIATE_TEST_CASE_P(fourthGroupMsgTypeSet, mapMsgForbiddenActions, testing::ValuesIn(fourthGroup));

struct sendResMsgForbiddenActions: simpleMsgTypeTest {
    std::shared_ptr<filesystem> f;
    sendResMsgForbiddenActions(): f(new file("name", 0)){};
    ~sendResMsgForbiddenActions()=default;
};
TEST_P(sendResMsgForbiddenActions, sendResThrowExceptionInConstruction) {
    msgType action = GetParam();
    EXPECT_THROW_MESSAGE_CONSTRUCTION(m = new sendResMessage(action, msgOutcome::success, f), action, messageException);
}
INSTANTIATE_TEST_CASE_P(firstGroupMsgTypeSet, sendResMsgForbiddenActions, testing::ValuesIn(firstGroup));
INSTANTIATE_TEST_CASE_P(thirdGroupMsgTypeSet, sendResMsgForbiddenActions, testing::ValuesIn(thirdGroup));
INSTANTIATE_TEST_CASE_P(fourthGroupMsgTypeSet, sendResMsgForbiddenActions, testing::ValuesIn(fourthGroup));
INSTANTIATE_TEST_CASE_P(changeResNameAndRemove, sendResMsgForbiddenActions, testing::Values(msgType::changeResName, msgType::removeRes));


struct updateActiveMsgForbiddenActions: signUpMsgForbiddenActions {};
TEST_P(updateActiveMsgForbiddenActions, updateActiveThrowExceptionInConstruction) {
    msgType action = GetParam();
    EXPECT_THROW_MESSAGE_CONSTRUCTION(m = new updateActiveMessage(action, msgOutcome::success, u, 0), action, messageException);
}
INSTANTIATE_TEST_CASE_P(firstGroupMsgTypeSet, updateActiveMsgForbiddenActions, testing::ValuesIn(firstGroup));
INSTANTIATE_TEST_CASE_P(secondGroupMsgTypeSet, updateActiveMsgForbiddenActions, testing::ValuesIn(secondGroup));
INSTANTIATE_TEST_CASE_P(thirdGroupMsgTypeSet, updateActiveMsgForbiddenActions, testing::ValuesIn(thirdGroup));
INSTANTIATE_TEST_CASE_P(OnlyCloseResFromFourthGroup, updateActiveMsgForbiddenActions, testing::Values(msgType::closeRes));


struct privMsgForbiddenActions: simpleMsgTypeTest {};

TEST_P(privMsgForbiddenActions, privThrowExceptionInConstruction) {
    msgType action = GetParam();
    EXPECT_THROW_MESSAGE_CONSTRUCTION(m = new privMessage(action, {"", ""}, msgOutcome::success, "", "", privilege::modify), action, messageException);
}
INSTANTIATE_TEST_CASE_P(firstGroupMsgTypeSet, privMsgForbiddenActions, testing::ValuesIn(firstGroup));
INSTANTIATE_TEST_CASE_P(secondGroupMsgTypeSet, privMsgForbiddenActions, testing::ValuesIn(secondGroup));
std::vector<msgType> ThirdWtChangePriv(thirdGroup);
auto res=ThirdWtChangePriv.erase(ThirdWtChangePriv.begin()+1);
INSTANTIATE_TEST_CASE_P(AllButChangePrivilgesFromThirdGroup, privMsgForbiddenActions, testing::ValuesIn(ThirdWtChangePriv));
INSTANTIATE_TEST_CASE_P(fourthGroupMsgTypeSet, privMsgForbiddenActions, testing::ValuesIn(fourthGroup));

struct symbolMsgForbiddenActions: simpleMsgTypeTest {
    symbol s;
    symbolMsgForbiddenActions(): s('a', 0, 0, std::vector<int>(), false){};
};
TEST_P(symbolMsgForbiddenActions, symbolThrowExceptionInConstruction) {
    msgType action = GetParam();
    EXPECT_THROW_MESSAGE_CONSTRUCTION(m = new symbolMessage(action, {"", ""}, msgOutcome::success, 0, 0, s), action, messageException);
}
INSTANTIATE_TEST_CASE_P(firstGroupMsgTypeSet, symbolMsgForbiddenActions, testing::ValuesIn(firstGroup));
INSTANTIATE_TEST_CASE_P(secondGroupMsgTypeSet, symbolMsgForbiddenActions, testing::ValuesIn(secondGroup));
INSTANTIATE_TEST_CASE_P(AllButSymInsRemFromThirdGroup, symbolMsgForbiddenActions, testing::ValuesIn(std::vector<msgType>(thirdGroup.begin(), thirdGroup.end()-2)));
INSTANTIATE_TEST_CASE_P(fourthGroupMsgTypeSet, symbolMsgForbiddenActions, testing::ValuesIn(fourthGroup));

struct uriMsgForbiddenActions: simpleMsgTypeTest {};
TEST_P(uriMsgForbiddenActions, uriThrowExceptionInConstruction) {
    msgType action = GetParam();
    EXPECT_THROW_MESSAGE_CONSTRUCTION(m = new uriMessage(action, {"", ""}, msgOutcome::success, "path",
                                                         "name", uri(), 0), action, messageException);
}
INSTANTIATE_TEST_CASE_P(firstGroupMsgTypeSet, uriMsgForbiddenActions, testing::ValuesIn(firstGroup));
INSTANTIATE_TEST_CASE_P(secondGroupMsgTypeSet, uriMsgForbiddenActions, testing::ValuesIn(secondGroup));
std::vector<msgType> ThirdWtShareRes(thirdGroup);
auto res2=ThirdWtShareRes.erase(ThirdWtShareRes.begin()+2);
INSTANTIATE_TEST_CASE_P(AllButShareResFromThirdGroup, uriMsgForbiddenActions, testing::ValuesIn(ThirdWtShareRes));
INSTANTIATE_TEST_CASE_P(fourthGroupMsgTypeSet, uriMsgForbiddenActions, testing::ValuesIn(fourthGroup));

struct userDataMsgForbiddenActions: signUpMsgForbiddenActions {};
TEST_P(userDataMsgForbiddenActions, userDataThrowExceptionInConstruction) {
    msgType action = GetParam();
    EXPECT_THROW_MESSAGE_CONSTRUCTION(m = new userDataMessage(action, {"", ""}, msgOutcome::success, u), action, messageException);
}
std::vector<msgType> FirstWtChangeUserData(firstGroup);
auto res3=FirstWtChangeUserData.erase(FirstWtChangeUserData.begin()+2, FirstWtChangeUserData.begin()+4);
INSTANTIATE_TEST_CASE_P(FirstWtChangeUserData, userDataMsgForbiddenActions, testing::ValuesIn(FirstWtChangeUserData));
INSTANTIATE_TEST_CASE_P(secondGroupMsgTypeSet, userDataMsgForbiddenActions, testing::ValuesIn(secondGroup));
INSTANTIATE_TEST_CASE_P(thirdGroupMsgTypeSet, userDataMsgForbiddenActions, testing::ValuesIn(thirdGroup));
INSTANTIATE_TEST_CASE_P(fourthGroupMsgTypeSet, userDataMsgForbiddenActions, testing::ValuesIn(fourthGroup));

//Added on 17/03/2020, after introduction of cursorMessage
struct cursorMsgForbiddenActions: simpleMsgTypeTest {};
TEST_P(cursorMsgForbiddenActions, cursorThrowExceptionInConstruction) {
    msgType action = GetParam();
    EXPECT_THROW_MESSAGE_CONSTRUCTION(m = new cursorMessage(action, {"", ""}, msgOutcome::success, 0, 0, 0, 0, 0), action, messageException);
}
INSTANTIATE_TEST_CASE_P(firstGroupMsgTypeSet, cursorMsgForbiddenActions, testing::ValuesIn(firstGroup));
INSTANTIATE_TEST_CASE_P(secondGroupMsgTypeSet, cursorMsgForbiddenActions, testing::ValuesIn(secondGroup));
INSTANTIATE_TEST_CASE_P(AllButSymInsRemFromThirdGroup, cursorMsgForbiddenActions, testing::ValuesIn(thirdGroup));
INSTANTIATE_TEST_CASE_P(allButCursorUpdateFromFourthGroup, cursorMsgForbiddenActions, testing::ValuesIn(std::vector<msgType>(fourthGroup.begin(), fourthGroup.end()-1)));

//After bug found 11/12/2019: not only we need to throw when an action is illegal, but we do need to NOT
//throw when it's legal! The following test should assure complete coverage for this consistency check
struct askResMsgLegalActions: simpleMsgTypeTest{};

TEST_P(askResMsgLegalActions, askResNoThrowExceptionInConstruction){
    msgType action=GetParam();
    EXPECT_NO_THROW_MESSAGE_CONSTRUCTION(m= new askResMessage(action, {"", ""}, "", "", "", uri::getDefaultPrivilege(), 0), action);
}
INSTANTIATE_TEST_CASE_P(secondGroupMsgTypeSet, askResMsgLegalActions, testing::ValuesIn(secondGroup));

struct signUpMsgLegalActions: simpleMsgTypeTest {
    user u;

    signUpMsgLegalActions() : u("username", "AP@ssw0rd!", "noempty", "", 0, nullptr) {}
    ~signUpMsgLegalActions()=default;
};

TEST_P(signUpMsgLegalActions, signUpNoThrowExceptionInConstruction){
    msgType action=GetParam();
    EXPECT_NO_THROW_MESSAGE_CONSTRUCTION(m=new signUpMessage(action, {"",""}, u), action);
}
INSTANTIATE_TEST_CASE_P(Registration, signUpMsgLegalActions, testing::Values(msgType::registration));

struct updateDocMsgLegalActions: simpleMsgTypeTest {};

TEST_P(updateDocMsgLegalActions, updateDocNoThrowExceptionInConstruction) {
    msgType action = GetParam();
    EXPECT_NO_THROW_MESSAGE_CONSTRUCTION(m = new updateDocMessage(action, {"", ""}, 0), action);
}
INSTANTIATE_TEST_CASE_P(MapChangesAndCloseRes, updateDocMsgLegalActions, testing::Values(msgType::mapChangesToUser, msgType::closeRes));

struct sendResMsgLegalActions: simpleMsgTypeTest {
    std::shared_ptr<filesystem> f;
    sendResMsgLegalActions(): f(new file("name", 0)){};
    ~sendResMsgLegalActions()=default;
};
TEST_P(sendResMsgLegalActions, sendResNoThrowExceptionInConstruction) {
    msgType action = GetParam();
    EXPECT_NO_THROW_MESSAGE_CONSTRUCTION(m = new sendResMessage(action, msgOutcome::success, f), action);
}
INSTANTIATE_TEST_CASE_P(secondGroupMsgTypeSet, sendResMsgLegalActions, testing::ValuesIn(std::vector<msgType>(secondGroup.begin(), secondGroup.begin()+4)));

struct updateActiveMsgLegalActions: signUpMsgForbiddenActions {};
TEST_P(updateActiveMsgLegalActions, updateActiveThrowExceptionInConstruction) {
    msgType action = GetParam();
    EXPECT_NO_THROW_MESSAGE_CONSTRUCTION(m = new updateActiveMessage(action, msgOutcome::success, u, 0), action);
}
INSTANTIATE_TEST_CASE_P(CloseRes, updateActiveMsgLegalActions, testing::Values(msgType::addActiveUser, msgType::removeActiveUser));

struct privMsgLegalActions: simpleMsgTypeTest {};

TEST_P(privMsgLegalActions, privNoThrowExceptionInConstruction) {
    msgType action = GetParam();
    EXPECT_NO_THROW_MESSAGE_CONSTRUCTION(m = new privMessage(action, {"", ""}, msgOutcome::success, "", "", privilege::modify), action);
}
INSTANTIATE_TEST_CASE_P(fourthGroupMsgTypeSet, privMsgLegalActions, testing::Values(msgType::changePrivileges));

struct symbolMsgLegalActions: simpleMsgTypeTest {
    symbol s;
    symbolMsgLegalActions(): s('a', 0, 0, std::vector<int>(), false){};
};
TEST_P(symbolMsgLegalActions, symbolNoThrowExceptionInConstruction) {
    msgType action = GetParam();
    EXPECT_NO_THROW_MESSAGE_CONSTRUCTION(m = new symbolMessage(action, {"", ""}, msgOutcome::success, 0, 0, s), action);
}
INSTANTIATE_TEST_CASE_P(InsertSymbol, symbolMsgLegalActions, testing::Values(msgType::insertSymbol));

struct uriMsgLegalActions: simpleMsgTypeTest {};
TEST_P(uriMsgLegalActions, uriNoThrowExceptionInConstruction) {
    msgType action = GetParam();
    EXPECT_NO_THROW_MESSAGE_CONSTRUCTION(m = new uriMessage(action, {"", ""}, msgOutcome::success, "path",
                                                         "name", uri(), 0), action);
}
INSTANTIATE_TEST_CASE_P(ShareRes, uriMsgLegalActions, testing::Values(msgType::shareRes));

struct userDataMsgLegalActions: signUpMsgForbiddenActions {};
TEST_P(userDataMsgLegalActions, userNoDataThrowExceptionInConstruction) {
    msgType action = GetParam();
    EXPECT_NO_THROW_MESSAGE_CONSTRUCTION(m = new userDataMessage(action, {"", ""}, msgOutcome::success, u), action);
}
INSTANTIATE_TEST_CASE_P(ChangeUserData, userDataMsgLegalActions, testing::Values(msgType::changeUserData));

//Added on 17/03/2020, after introduction of cursorMessage
struct cursorMsgLegalActions: simpleMsgTypeTest {};
TEST_P(cursorMsgLegalActions, cursorUpdateNoThrowExceptionInConstruction) {
    msgType action = GetParam();
    EXPECT_NO_THROW_MESSAGE_CONSTRUCTION(m = new cursorMessage(action, {"", ""}, msgOutcome::success, 1, 1, 1, 1), action);
}
INSTANTIATE_TEST_CASE_P(updateCursorPos, cursorMsgLegalActions, testing::Values(msgType::updateCursor));

/*
 * Basic logic tests for messages sent ONLY by the client (clientMessage)
 * These tests verify that, once received by the server, the invokeMethod calls
 * the proper function on the server itself
 */

class SymServerMock: public SymServer{
public:
    SymServerMock() : SymServer(false, false){}
    MOCK_METHOD3(login, const user(const std::string&, const std::string&, uint_positive_cnt::type));
    MOCK_METHOD2(logout, void(const std::string&, uint_positive_cnt::type));
    MOCK_METHOD3(removeUser, void(const std::string&, const std::string&, uint_positive_cnt::type));
    MOCK_METHOD2(addUser, const user&(user&, uint_positive_cnt::type));
    MOCK_METHOD4(createNewSource, const document&(const std::string&, const std::string&, const std::string&, uint_positive_cnt::type));
    MOCK_METHOD5(openSource, std::shared_ptr<file>(const std::string&, const std::string&, const std::string&, privilege, uint_positive_cnt::type));
    MOCK_METHOD6(openNewSource, std::shared_ptr<file>(const std::string&, const std::string&, const std::string&, const std::string&, privilege, uint_positive_cnt::type));
    MOCK_METHOD5(renameResource, std::shared_ptr<filesystem>(const std::string&, const std::string&, const std::string&, const std::string&, uint_positive_cnt::type));
    MOCK_METHOD4(createNewDir, std::shared_ptr<directory>(const std::string&, const std::string&, const std::string&, uint_positive_cnt::type));
    MOCK_METHOD4(removeResource, std::shared_ptr<filesystem>(const std::string&, const std::string&, const std::string&, uint_positive_cnt::type));
    MOCK_METHOD3(mapSiteIdToUser, std::map<uint_positive_cnt::type, user>(const std::string&, uint_positive_cnt::type, uint_positive_cnt::type));

    MOCK_METHOD6(editPrivilege, privilege(const std::string&, const std::string&, const std::string&, const std::string&, privilege, uint_positive_cnt::type));
    MOCK_METHOD3(remoteInsert, void(const std::string&, uint_positive_cnt::type resourceId, const symbolMessage&));
    MOCK_METHOD3(remoteRemove, void(const std::string&, uint_positive_cnt::type, const symbolMessage& rmMsg));
    MOCK_METHOD5(shareResource, std::shared_ptr<filesystem>(const std::string& actionUser, const std::string&, const std::string&, const uri&, uint_positive_cnt::type));
    MOCK_METHOD3(editUser, const user&(const std::string&, user&, uint_positive_cnt::type));
    MOCK_METHOD3(closeSource, void(const std::string&, uint_positive_cnt::type, uint_positive_cnt::type));
    MOCK_METHOD3(updateCursorPos, void(const std::string&, uint_positive_cnt::type, const cursorMessage&));
};

struct clientMessageTest: public testing::Test{
    clientMessage *m;
    user u;
    static const std::string path;
    static const std::string name;
    static const std::string resId;
    static const std::string username;
    static const std::string pwd;
    static const int resourceId;
    ::testing::NiceMock<SymServerMock> server;
    clientMessageTest(): u(username, pwd, "noempty", "", 0, nullptr){
        m= nullptr;
    }
    ~clientMessageTest(){
        delete m;
    }
};
const std::string clientMessageTest::path="./dir1/dir2";
const std::string clientMessageTest::name="somefile";
const std::string clientMessageTest::resId="./someUserDir/hisDir/hisFile";
const std::string clientMessageTest::username="mario";
const std::string clientMessageTest::pwd="AP@ssw0rd!";
const int clientMessageTest::resourceId=10;

TEST_F(clientMessageTest, clientMessageTestCallsLoginOnServer){
    m=new clientMessage(msgType::login, {username, pwd});
    EXPECT_CALL(server, login(m->getActionOwner().first, m->getActionOwner().second, m->getMsgId())).WillOnce(::testing::Return(u));
    m->invokeMethod(server);
}

TEST_F(clientMessageTest, clientMessageTestCallsLogoutOnServer){
    m=new clientMessage(msgType::logout, {username, ""});
    EXPECT_CALL(server, logout(m->getActionOwner().first, m->getMsgId()));
    m->invokeMethod(server);
}

TEST_F(clientMessageTest, clientMessageTestCallsRemoveUserOnServer){
    m=new clientMessage(msgType::removeUser, {username, pwd});
    EXPECT_CALL(server, removeUser(m->getActionOwner().first, m->getActionOwner().second, m->getMsgId()));
    m->invokeMethod(server);
}

TEST_F(clientMessageTest, signUpMsgTestCallsAddUserOnServer){
    m=new signUpMessage(msgType::registration, {username, ""}, u);
    EXPECT_CALL(server, addUser(u, m->getMsgId())).WillOnce(::testing::ReturnRef(u));
    m->invokeMethod(server);
}

TEST_F(clientMessageTest, askResMsgTestCallsCreateNewSource){
    document d;
    m= new askResMessage(msgType::createRes, {username, ""}, path, name, "", uri::getDefaultPrivilege(), 0);
    EXPECT_CALL(server, createNewSource(m->getActionOwner().first, path, name, m->getMsgId())).WillOnce(::testing::ReturnRef(d));
    m->invokeMethod(server);
}

TEST_F(clientMessageTest, askResMsgTestCallsOpenSource){
    m= new askResMessage(msgType::openRes, {username, ""}, path, name, "", uri::getDefaultPrivilege(), 0);
    EXPECT_CALL(server, openSource(m->getActionOwner().first, path, name, uri::getDefaultPrivilege(), m->getMsgId()));
    m->invokeMethod(server);
}

TEST_F(clientMessageTest, askResMsgTestCallsOpenNewSource){
    m= new askResMessage(msgType::openNewRes, {username, ""}, path, name, resId, uri::getDefaultPrivilege(), 0);
    EXPECT_CALL(server, openNewSource(username, resId, path, name, uri::getDefaultPrivilege(), m->getMsgId()));
    m->invokeMethod(server);
}

TEST_F(clientMessageTest, askResMsgTestCallsRenameResource){
    //If @e action is "changeResName" then @e resourceId is the new file name. See def. of askResMessage in message.h
    m= new askResMessage(msgType::changeResName, {username, ""}, path, name, resId, uri::getDefaultPrivilege(), 0);
    EXPECT_CALL(server, renameResource(username, path, name, resId, m->getMsgId())).WillOnce(::testing::Return(std::shared_ptr<filesystem>()));

    //resID non è un nuovo nome della risorsa, sistemare!
    m->invokeMethod(server);
}

TEST_F(clientMessageTest, askResMsgTestCallsCreateNewDir){
    m= new askResMessage(msgType::createNewDir, {username, ""}, path, name, "", uri::getDefaultPrivilege(), 0);
    EXPECT_CALL(server, createNewDir(username, path, name, m->getMsgId()));
    m->invokeMethod(server);
}

TEST_F(clientMessageTest, askResMsgTestCallsRemoveResource){
    m= new askResMessage(msgType::removeRes, {username, ""}, path, name, "", uri::getDefaultPrivilege(), 0);
    EXPECT_CALL(server, removeResource(username, path, name, m->getMsgId())).WillOnce(::testing::Return(std::shared_ptr<directory>()));
    m->invokeMethod(server);
}

TEST_F(clientMessageTest, updateDocMsgTestCallsMapSiteIdToUser){
    m=new updateDocMessage(msgType::mapChangesToUser, {username,""},resourceId);
    EXPECT_CALL(server, mapSiteIdToUser(username, resourceId, m->getMsgId()));
    m->invokeMethod(server);
}

TEST_F(clientMessageTest, updateDocMsgTestCallsCloseSource){
    m=new updateDocMessage(msgType::closeRes, {username,""},resourceId);
    EXPECT_CALL(server, closeSource(username, resourceId, m->getMsgId()));
    m->invokeMethod(server);
}


/*
 * Basic logic tests for messages sent ONLY by the server (serverMessage)
 * These tests verify that, once received by the client, the invokeMethod calls
 * the proper function on the client itself
 */

class SymClientMock: public SymClient{
public:
    MOCK_METHOD2(setUserColors, void(uint_positive_cnt::type, const std::map<uint_positive_cnt::type, user>&));
    MOCK_METHOD4(createNewSource, void(const std::string&, const std::string&, uint_positive_cnt::type, const std::shared_ptr<file>));
    MOCK_METHOD3(createNewDir, void(const std::string&, const std::string&, uint_positive_cnt::type));
    MOCK_METHOD4(openSource, void(const std::string&, const std::string&, const std::shared_ptr<file>, privilege priv));
    MOCK_METHOD6(openNewSource, void(const std::string &resId, privilege reqPriv, const std::string &destPath, const std::string &destName, uint_positive_cnt::type idToAssign, const std::shared_ptr<file> fileAsked));
    MOCK_METHOD4(renameResource, std::shared_ptr<filesystem>(const std::string&, const std::string&, const std::string&, bool));
    MOCK_METHOD3(removeResource, std::shared_ptr<filesystem>(const std::string&, const std::string&, bool));
    MOCK_METHOD3(addActiveUser, void(uint_positive_cnt::type, user&, privilege));
    MOCK_METHOD2(removeActiveUser, void(uint_positive_cnt::type, user&));

    MOCK_METHOD5(editPrivilege, privilege(const std::string&, const std::string&, const std::string&, privilege, bool));
    MOCK_METHOD3(remoteInsert, void(uint_positive_cnt::type, uint_positive_cnt::type, const symbol&));
    MOCK_METHOD3(remoteRemove, void(uint_positive_cnt::type, uint_positive_cnt::type, const symbol&));
    MOCK_METHOD4(shareResource, std::shared_ptr<filesystem>(const std::string&, const std::string&, const uri&, bool msgRcv));
    MOCK_METHOD2(editUser, const user(user&, bool));
    MOCK_METHOD2(verifySymbol, void(uint_positive_cnt::type, const symbol&));
    MOCK_METHOD1(retrieveRelatedMessage, std::shared_ptr<clientMessage>(const serverMessage&));
    MOCK_METHOD1(logout, void(bool msgRcv));
    MOCK_METHOD1(removeUser, void(bool msgRcv));

    MOCK_METHOD4(updateCursorPos, void(uint_positive_cnt::type, uint_positive_cnt::type, unsigned int, unsigned int));
    MOCK_METHOD1(signUp, void(const user&));
    MOCK_METHOD1(logIn, void(const user&));
};

struct serverMessageTest: public testing::Test{
    clientMessage *cm;
    serverMessage *m;
    user u;
    ::testing::NiceMock<SymClientMock> client;

    serverMessageTest(): u("username", "AP@ssw0rd!", "noempty", "", 0, nullptr){
        cm=nullptr;
        m= nullptr;
    }
    ~serverMessageTest(){
        delete m;
    }
};

TEST_F(serverMessageTest, loginMsgTestCallsLogin){
    m=new loginMessage(msgType::login, msgOutcome::success, u);
    EXPECT_CALL(client, logIn(u));
    //Even if the retrieved message is not useful to call logIn(), it's necessary to
    //notify that the server answered and check if the asked action has been completed successfully
    EXPECT_CALL(client, retrieveRelatedMessage(*m));

    m->invokeMethod(client);
}

//Added on 11/04/2020 after having found bug on signUp flow implementation
TEST_F(serverMessageTest, loginMsgTestCallsSignUpOnClient){
    m=new loginMessage(msgType::registration, msgOutcome::success, u);
    EXPECT_CALL(client, signUp(u));
    //Even if the retrieved message is not useful to call signUp(), it's necessary to
    //notify that the server answered and check if the asked action has been completed successfully
    EXPECT_CALL(client, retrieveRelatedMessage(*m));

    m->invokeMethod(client);
}

TEST_F(serverMessageTest, mapMsgTestCallsSetUserColors){
    std::map<uint_positive_cnt::type, user> siteIdToUser;
    cm= new updateDocMessage(msgType::mapChangesToUser, {clientMessageTest::username, ""}, clientMessageTest::resourceId);
    m=new mapMessage(msgType::mapChangesToUser, msgOutcome::success, siteIdToUser);
    EXPECT_CALL(client, setUserColors(clientMessageTest::resourceId, siteIdToUser));
    //Even if the retrieved message is not useful to call setLoggedUser(), it's necessary to
    //notify that the server answered and check if the asked action has been completed successfully
    EXPECT_CALL(client, retrieveRelatedMessage(*m)).WillOnce(::testing::Return(std::shared_ptr<clientMessage>(cm)));

    m->invokeMethod(client);
}

TEST_F(serverMessageTest, sendResMsgTestCallsCreateNewSource){
    std::shared_ptr<file> dummyFile(new file("file", 5));
    //data to call createNewSource() with is retrieved by the previously sent askResMessage, so suppose
    //the client has sent the following message
    cm= new askResMessage(msgType::createRes, {clientMessageTest::username, ""}, clientMessageTest::path, clientMessageTest::name, "", uri::getDefaultPrivilege(), 0);
    m=new sendResMessage(msgType::createRes, msgOutcome::success, dummyFile);
    EXPECT_CALL(client, createNewSource(clientMessageTest::path, clientMessageTest::name, dummyFile->getId(), dummyFile));
    EXPECT_CALL(client, retrieveRelatedMessage(*m)).WillOnce(::testing::Return(std::shared_ptr<clientMessage>(cm)));

    m->invokeMethod(client);
}

TEST_F(serverMessageTest, sendResMsgTestCallsCreateNewDir){
    //data to call createNewDir() with is retrieved by the previously sent askResMessage, so suppose
    //the client has sent the following message
    cm= new askResMessage(msgType::createNewDir, {clientMessageTest::username, ""}, clientMessageTest::path, clientMessageTest::name, "", uri::getDefaultPrivilege(), 0);
    auto dirCreated=directory::emptyDir();
    m=new sendResMessage(msgType::createNewDir, msgOutcome::success, dirCreated);
    EXPECT_CALL(client, createNewDir(clientMessageTest::path, clientMessageTest::name, dirCreated->getId()));
    EXPECT_CALL(client, retrieveRelatedMessage(*m)).WillOnce(::testing::Return(std::shared_ptr<clientMessage>(cm)));

    m->invokeMethod(client);
}

TEST_F(serverMessageTest, sendResMsgTestCallsOpenSource){
    std::shared_ptr<file> dummyFile(new file("file", 1));
    dummyFile->setUserPrivilege(clientMessageTest::username, uri::getDefaultPrivilege());
    cm= new askResMessage(msgType::openRes, {clientMessageTest::username, {}}, clientMessageTest::path, clientMessageTest::name, "", uri::getDefaultPrivilege(), 0);
    m=new sendResMessage(msgType::openRes, msgOutcome::success, dummyFile);
    EXPECT_CALL(client, openSource(clientMessageTest::path, clientMessageTest::name, dummyFile, dummyFile->getUserPrivilege(clientMessageTest::username)));
    EXPECT_CALL(client, retrieveRelatedMessage(*m)).WillOnce(::testing::Return(std::shared_ptr<clientMessage>(cm)));

    m->invokeMethod(client);
}

TEST_F(serverMessageTest, sendResMsgTestCallsOpenNewSource){
    //data to call createNewDir() with is retrieved by the previously sent askResMessage, so suppose
    //the client has sent the following message
    cm= new askResMessage(msgType::openNewRes, {clientMessageTest::username, ""}, clientMessageTest::path, clientMessageTest::name, clientMessageTest::resId, uri::getDefaultPrivilege(), 0);
    std::shared_ptr<filesystem> dummyFile(new file("file", 1));
    dummyFile->setUserPrivilege(clientMessageTest::username, uri::getDefaultPrivilege());
    m=new sendResMessage(msgType::openNewRes, msgOutcome::success, dummyFile);
    EXPECT_CALL(client, openNewSource(clientMessageTest::resId, uri::getDefaultPrivilege(), clientMessageTest::path, clientMessageTest::name, dummyFile->getId(), std::dynamic_pointer_cast<file>(dummyFile)));
    EXPECT_CALL(client, retrieveRelatedMessage(*m)).WillOnce(::testing::Return(std::shared_ptr<clientMessage>(cm)));

    m->invokeMethod(client);
}

TEST_F(serverMessageTest, serverMsgTestCallsRenameResource){
    //data to call createNewDir() with is retrieved by the previously sent askResMessage, so suppose
    //the client has sent the following message
    cm= new askResMessage(msgType::changeResName, {clientMessageTest::username, ""}, clientMessageTest::path, clientMessageTest::name, "newName", uri::getDefaultPrivilege(), 0);
    m=new serverMessage(msgType::changeResName, msgOutcome::success);
    EXPECT_CALL(client, renameResource(clientMessageTest::path, clientMessageTest::name, "newName", true));
    EXPECT_CALL(client, retrieveRelatedMessage(*m)).WillOnce(::testing::Return(std::shared_ptr<clientMessage>(cm)));

    m->invokeMethod(client);
}

TEST_F(serverMessageTest, serverResMsgTestCallsRemoveResource){
    //data to call createNewDir() with is retrieved by the previously sent askResMessage, so suppose
    //the client has sent the following message
    cm= new askResMessage(msgType::removeRes, {clientMessageTest::username, ""}, clientMessageTest::path, clientMessageTest::name, "", uri::getDefaultPrivilege(), 0);
    m=new serverMessage(msgType::removeRes, msgOutcome::success);
    EXPECT_CALL(client, removeResource(clientMessageTest::path, clientMessageTest::name, true));
    EXPECT_CALL(client, retrieveRelatedMessage(*m)).WillOnce(::testing::Return(std::shared_ptr<clientMessage>(cm)));

    m->invokeMethod(client);
}

TEST_F(serverMessageTest, serverMsgTestCallsLogoutOnClient){
    //data to call createNewDir() with is retrieved by the previously sent askResMessage, so suppose
    //the client has sent the following message
    cm= new clientMessage(msgType::logout, {clientMessageTest::username, {}});
    m=new serverMessage(msgType::logout, msgOutcome::success);
    EXPECT_CALL(client, logout(true));
    EXPECT_CALL(client, retrieveRelatedMessage(*m)).WillOnce(::testing::Return(std::shared_ptr<clientMessage>(cm)));

    m->invokeMethod(client);
}

TEST_F(serverMessageTest, updateActiveMsgTestCallsAddActiveUsers){
    user sentByServer=u.makeCopyNoPwd();
    m=new updateActiveMessage(msgType::addActiveUser, msgOutcome::success, sentByServer, 0);
    //m is a serverMessage, we need a cast to extract the resource id
    //this will not be necessary in code because SymClient::addActiveUser() is called from within the invokeMethod()
    updateActiveMessage* uam=static_cast<updateActiveMessage*>(m);
    EXPECT_CALL(client, addActiveUser(uam->getResourceId(), sentByServer, uam->getUserPrivilege()));
    m->invokeMethod(client);
}

TEST_F(serverMessageTest, updateActiveMsgTestCallsRemoveActiveUsers){
    user sentByServer=u.makeCopyNoPwd();
    m=new updateActiveMessage(msgType::removeActiveUser, msgOutcome::success, sentByServer, 0);
    //m is a serverMessage, we need a cast to extract the resource id
    //this will not be necessary in code because SymClient::removeActiveUser() is called from within the invokeMethod()
    updateActiveMessage* uam=static_cast<updateActiveMessage*>(m);
    EXPECT_CALL(client, removeActiveUser(uam->getResourceId(), sentByServer));
    m->invokeMethod(client);
}

//Added on 24/03/2020 after adding void removeUser(bool) on client
TEST_F(serverMessageTest, serverMsgTestCallsRemovUserOnClient){
    cm= new clientMessage(msgType::removeUser, {clientMessageTest::username, {}});
    m=new serverMessage(msgType::removeUser, msgOutcome::success);
    EXPECT_CALL(client, removeUser(true));
    EXPECT_CALL(client, retrieveRelatedMessage(*m)).WillOnce(::testing::Return(std::shared_ptr<clientMessage>(cm)));

    m->invokeMethod(client);
}

/*
 * Basic logic tests for messages that may be sent from client or server
 */
struct DoubleEndMessageTest: public testing::Test{
    serverMessage *fromServer;
    clientMessage *fromClient;
    user u;
    ::testing::StrictMock<SymClientMock> client;
    ::testing::StrictMock<SymServerMock> server;
    symbol dummySymbol;
    uri dummyUri;
    privilege priv;

    /*
     * Example data for messages and invocations
     */
    static const std::string path;
    static const std::string name;
    static const std::string resId;
    static const std::string username;
    static const std::string pwd;
    static const std::string anotherUsername;
    static const int resourceId;

    DoubleEndMessageTest(): dummySymbol('a', 0, 1, std::vector<int>(), false),
                            u(username, pwd, "noempty", "", 0, nullptr),
                            priv(uri::getDefaultPrivilege()){
        fromServer=nullptr;
        fromClient= nullptr;
    }
    ~DoubleEndMessageTest(){
        delete fromServer;
        delete fromClient;
    }
};
const std::string DoubleEndMessageTest::path="./dir1/dir2";
const std::string DoubleEndMessageTest::name="somefile";
const std::string DoubleEndMessageTest::resId=".mario/dir1/dir2/somefile";
const std::string DoubleEndMessageTest::username="mario";
const std::string DoubleEndMessageTest::pwd="AP@ssw0rd!";
const std::string DoubleEndMessageTest::anotherUsername="lucio";
const int DoubleEndMessageTest::resourceId=10;

TEST_F(DoubleEndMessageTest, privMsgCallsEditPrivilege){
    fromClient=new privMessage(msgType::changePrivileges,{username, {}}, msgOutcome::success, resId, anotherUsername, uri::getDefaultPrivilege());
    EXPECT_CALL(server, editPrivilege(username, anotherUsername, path, name, uri::getDefaultPrivilege(), fromClient->getMsgId())).WillOnce(::testing::Return(uri::getDefaultPrivilege()));
    fromClient->invokeMethod(server);

    fromServer= new serverMessage(msgType::changePrivileges, msgOutcome::success, fromClient->getMsgId());
    //client uses the message previously sent to the server to retrieve the parameters for the required action:
    EXPECT_CALL(client, retrieveRelatedMessage(*fromServer)).WillOnce(::testing::Return(std::shared_ptr<clientMessage>(fromClient)));
    EXPECT_CALL(client, editPrivilege(anotherUsername, path, name, uri::getDefaultPrivilege(), true));
    //when a serverMessage is received, a related message from the client is searched and completeAction() called
    //the retrived clientMessage has all the data needed to perform the action required
    fromServer->invokeMethod(client);

    fromClient=nullptr; //avoid double deletion, object pointed by fromClient is now owned by the shared_ptr
}

TEST_F(DoubleEndMessageTest, privMsgCallsEditPrivilegeOnOtherClient){
    fromClient=new privMessage(msgType::changePrivileges,{username, {}}, msgOutcome::success, resId, anotherUsername, uri::getDefaultPrivilege());

    //the message from client is forwarded to the other client, but the password is cleaned
    fromServer=new privMessage(msgType::changePrivileges,{username, {}}, msgOutcome::success, resId, anotherUsername, uri::getDefaultPrivilege());
    EXPECT_CALL(client, editPrivilege(anotherUsername, path, name, uri::getDefaultPrivilege(), false));
    fromServer->invokeMethod(client);
}

TEST_F(DoubleEndMessageTest, symbolMsgCallsRemoteInsertOnSuccess){
    fromClient=new symbolMessage(msgType::insertSymbol,{username, {}}, msgOutcome::success, 10,resourceId, dummySymbol);
    //symbolMessage::invokeMethod() pass itself to server function. To do this in test we need a cast
    symbolMessage* fc= static_cast<symbolMessage*>(fromClient);
    EXPECT_CALL(server, remoteInsert(username, resourceId, *fc));
    fromClient->invokeMethod(server);

    fromServer= new serverMessage(msgType::insertSymbol, msgOutcome::success, fromClient->getMsgId());
    //client uses the message previously sent to the server to retrieve the parameters for the required action:
    EXPECT_CALL(client, retrieveRelatedMessage(*fromServer)).WillOnce(::testing::Return(std::shared_ptr<clientMessage>(fromClient)));
    //We expect verifySymbol to be called by fromClient when the outcome is positive
    EXPECT_CALL(client, verifySymbol(resourceId, fc->getSym()));
    //when a serverMessage is received, a related message from the client is searched and completeAction() called
    //the retrived clientMessage has all the data needed to perform the action required
    fromServer->invokeMethod(client);

    fromClient=nullptr; //avoid double deletion, object pointed by fromClient is now owned by the shared_ptr

}

TEST_F(DoubleEndMessageTest, symbolMsgCallsRemoteRemoveOnFailure){
    fromClient=new symbolMessage(msgType::insertSymbol,{username, {}}, msgOutcome::success, 10,resourceId, dummySymbol);
    //symbolMessage::invokeMethod() pass itself to server function. To do this in test we need a cast
    symbolMessage* fc= static_cast<symbolMessage*>(fromClient);
    EXPECT_CALL(server, remoteInsert(username, resourceId, *fc));
    fromClient->invokeMethod(server);

    //Suppose now that the user return a failure
    fromServer= new serverMessage(msgType::insertSymbol, msgOutcome::failure, fromClient->getMsgId());
    //client uses the message previously sent to the server to retrieve the parameters for the required action:
    EXPECT_CALL(client, retrieveRelatedMessage(*fromServer)).WillOnce(::testing::Return(std::shared_ptr<clientMessage>(fromClient)));
    //We expect remoteRemove to be called by fromClient when the outcome is negative
    EXPECT_CALL(client, remoteRemove(fc->getSiteId(), resourceId, fc->getSym()));
    //when a serverMessage is received, a related message from the client is searched and completeAction() called
    //the retrived clientMessage has all the data needed to perform the action required
    fromServer->invokeMethod(client);

    fromClient=nullptr; //avoid double deletion, object pointed by fromClient is now owned by the shared_ptr

}

TEST_F(DoubleEndMessageTest, symbolMsgCallsRemoteInsertOnOtherClient){
    fromClient=new symbolMessage(msgType::insertSymbol,{username, {}}, msgOutcome::success, 10,resourceId, dummySymbol);
    symbolMessage* fc= static_cast<symbolMessage*>(fromClient);

    //the message from client is forwarded to and the symbol contained it's now verified
    symbol s=fc->getSym(); s.setVerified();
    fromServer=new symbolMessage(msgType::insertSymbol,{username, {}}, msgOutcome::success, 10,resourceId, s);
    EXPECT_CALL(client, remoteInsert(fc->getSiteId(), resourceId, s));
    fromServer->invokeMethod(client);
}

TEST_F(DoubleEndMessageTest, symbolMsgCallsRemoteRemoveOnSuccess){
    fromClient=new symbolMessage(msgType::removeSymbol,{username, {}}, msgOutcome::success, 10,resourceId, dummySymbol);
    //symbolMessage::invokeMethod() pass itself to server function. To do this in test we need a cast
    symbolMessage* fc= static_cast<symbolMessage*>(fromClient);
    EXPECT_CALL(server, remoteRemove(username, resourceId, *fc));
    fromClient->invokeMethod(server);

    fromServer= new serverMessage(msgType::removeSymbol, msgOutcome::success, fromClient->getMsgId());
    //client uses the message previously sent to the server to retrieve the parameters for the required action:
    EXPECT_CALL(client, retrieveRelatedMessage(*fromServer)).WillOnce(::testing::Return(std::shared_ptr<clientMessage>(fromClient)));
    //We expect no other function to be called when the outcome is positive, the symbol has already been removed

    fromServer->invokeMethod(client);

    fromClient=nullptr; //avoid double deletion, object pointed by fromClient is now owned by the shared_ptr

}

TEST_F(DoubleEndMessageTest, symbolMsgCallsRemoteInsertOnFailure){
    fromClient=new symbolMessage(msgType::removeSymbol,{username, {}}, msgOutcome::success, 10,resourceId, dummySymbol);
    //symbolMessage::invokeMethod() pass itself to server function. To do this in test we need a cast
    symbolMessage* fc= static_cast<symbolMessage*>(fromClient);
    EXPECT_CALL(server, remoteRemove(username, resourceId, *fc));
    fromClient->invokeMethod(server);

    //Suppose now that the user return a failure
    fromServer= new serverMessage(msgType::removeSymbol, msgOutcome::failure, fromClient->getMsgId());
    //client uses the message previously sent to the server to retrieve the parameters for the required action:
    EXPECT_CALL(client, retrieveRelatedMessage(*fromServer)).WillOnce(::testing::Return(std::shared_ptr<clientMessage>(fromClient)));
    //We expect remoteInsert to be called by fromClient when the outcome is negative, so that the removal done in advance is annulled
    EXPECT_CALL(client, remoteInsert(fc->getSiteId(), resourceId, fc->getSym()));
    //when a serverMessage is received, a related message from the client is searched and completeAction() called
    //the retrived clientMessage has all the data needed to perform the action required
    fromServer->invokeMethod(client);

    fromClient=nullptr; //avoid double deletion, object pointed by fromClient is now owned by the shared_ptr

}

TEST_F(DoubleEndMessageTest, symbolMsgCallsRemoteRemoveOnOtherClient){
    fromClient=new symbolMessage(msgType::removeSymbol,{username, {}}, msgOutcome::success, 10,resourceId, dummySymbol);
    symbolMessage* fc= static_cast<symbolMessage*>(fromClient);

    //the message from client is forwarded to the other client, but the password is cleaned and it's now verified
    symbol s=fc->getSym(); s.setVerified();
    fromServer=new symbolMessage(msgType::removeSymbol,{username, {}}, msgOutcome::success, 10,resourceId, s);
    EXPECT_CALL(client, remoteRemove(fc->getSiteId(), resourceId, s));
    fromServer->invokeMethod(client);
}

TEST_F(DoubleEndMessageTest, uriMsgCallsRemoteRemove){
    fromClient= new uriMessage(msgType::shareRes, {username, {}}, msgOutcome::success, path, name,
                               dummyUri);
    uriMessage* fc= static_cast<uriMessage*>(fromClient);
    EXPECT_CALL(server, shareResource(username, path, name, dummyUri, fromClient->getMsgId()));
    fromClient->invokeMethod(server);

    fromServer= new serverMessage(msgType::shareRes, msgOutcome::success, fromClient->getMsgId());
    //client uses the message previously sent to the server to retrieve the parameters for the required action:
    EXPECT_CALL(client, retrieveRelatedMessage(*fromServer)).WillOnce(::testing::Return(std::shared_ptr<clientMessage>(fromClient)));
    EXPECT_CALL(client, shareResource(path, name, fc->getSharingPrefs(), true));
    fromServer->invokeMethod(client);

    fromClient=nullptr; //avoid double deletion, object pointed by fromClient is now owned by the shared_ptr
}

TEST_F(DoubleEndMessageTest, uriMsgCallsRemoteRemoveOnOtherClient){
    fromClient= new uriMessage(msgType::shareRes, {username, {}}, msgOutcome::success, path,
                               name,
                               dummyUri);
    uriMessage* fc= static_cast<uriMessage*>(fromClient);

    //the message from client is forwarded to the other client, but the password is cleaned
    fromServer= new uriMessage(msgType::shareRes, {username, {}}, msgOutcome::success, path, name,
                               dummyUri, fc->getMsgId());
    EXPECT_CALL(client, shareResource(path, name, fc->getSharingPrefs(), false));
    fromServer->invokeMethod(client);
}

TEST_F(DoubleEndMessageTest, userDataMsgCallsEditUser){
    fromClient=new userDataMessage(msgType::changeUserData, {username, {}}, msgOutcome::success, u);

    EXPECT_CALL(server, editUser(username, u, fromClient->getMsgId())).WillOnce(::testing::ReturnRef(u));
    fromClient->invokeMethod(server);

    fromServer= new serverMessage(msgType::changeUserData, msgOutcome::success, fromClient->getMsgId());
    //client uses the message previously sent to the server to retrieve the parameters for the required action:
    EXPECT_CALL(client, retrieveRelatedMessage(*fromServer)).WillOnce(::testing::Return(std::shared_ptr<clientMessage>(fromClient)));
    EXPECT_CALL(client, editUser(u, true)).WillOnce(::testing::Return(u));
    fromServer->invokeMethod(client);

    fromClient=nullptr; //avoid double deletion, object pointed by fromClient is now owned by the shared_ptr
}

TEST_F(DoubleEndMessageTest, userDataMsgCallsEditUserOnOtherClient){
    fromClient=new userDataMessage(msgType::changeUserData, {username, {}}, msgOutcome::success, u);
    //uriMessage* fc= dynamic_cast<uriMessage*>(fromClient);

    fromServer= new userDataMessage(msgType::changeUserData, {username, {}}, msgOutcome::success, u, fromClient->getMsgId());
    EXPECT_CALL(client, editUser(u, false)).WillOnce(::testing::Return(u));
    fromServer->invokeMethod(client);
}

//Added on 18/03/2020, after introduction of cursorMessage
TEST_F(DoubleEndMessageTest, cursorMessageCallsUpdateCursor){
    srand(time(NULL)); unsigned int row=rand()%1000, col=rand()%1000;
    fromClient=new cursorMessage(msgType::updateCursor, {username, {}}, msgOutcome::success, u.getSiteId(), resourceId, row, col);
    cursorMessage* cr= static_cast<cursorMessage*>(fromClient);

    EXPECT_CALL(server, updateCursorPos(username, resourceId, *cr));
    fromClient->invokeMethod(server);

    fromServer= new serverMessage(msgType::updateCursor, msgOutcome::success, fromClient->getMsgId());
    //client uses the message previously sent to the server to retrieve the parameters for the required action:
    EXPECT_CALL(client, retrieveRelatedMessage(*fromServer)).WillOnce(::testing::Return(std::shared_ptr<clientMessage>(fromClient)));
    //no action is required when the response is received
    fromServer->invokeMethod(client);

    fromClient=nullptr; //avoid double deletion, object pointed by fromClient is now owned by the shared_ptr
}

TEST_F(DoubleEndMessageTest, cursorMessageCallsUpdateCursorOnOtherClient){
    srand(time(NULL)); unsigned int row=rand()%1000, col=rand()%1000;
    fromClient=new cursorMessage(msgType::updateCursor, {username, {}}, msgOutcome::success, u.getSiteId(), resourceId, row, col);

    fromServer= new cursorMessage(msgType::updateCursor, {username, {}}, msgOutcome::success, u.getSiteId(), resourceId, row, col, fromClient->getMsgId());
    EXPECT_CALL(client, updateCursorPos(u.getSiteId(), resourceId, row, col));
    fromServer->invokeMethod(client);
}

struct messageSerialization: public testing::Test{
    std::unique_ptr<message> toStore, toLoad;
    std::stringstream stream;

    messageSerialization(): toStore(nullptr), toLoad(nullptr){}

    void storeMessage(std::unique_ptr<message> &as){
        boost::archive::text_oarchive oa(stream);
        oa<<as;
    }

    void loadMessage(std::unique_ptr<message> &as){
        boost::archive::text_iarchive ia(stream);
        ia>>as;
    }
    ~messageSerialization()=default;
};

TEST_F(messageSerialization, clientMessage){
    toStore=std::unique_ptr<clientMessage>(new clientMessage(msgType::login, {"try","pass"},10));
    toLoad=std::unique_ptr<message>(new clientMessage(msgType::login, {"",""},10));
    ASSERT_NE(*dynamic_cast<clientMessage*>(toStore.get()), *dynamic_cast<clientMessage*>(toLoad.get()));
    storeMessage(toStore);
    loadMessage(toLoad);
    EXPECT_EQ(*dynamic_cast<clientMessage*>(toStore.get()), *dynamic_cast<clientMessage*>(toLoad.get()));
}

TEST_F(messageSerialization, serverMessage){
    toStore=std::unique_ptr<serverMessage>(new serverMessage(msgType::login, msgOutcome::success,10));
    toLoad=std::unique_ptr<serverMessage>(new serverMessage(msgType::login, msgOutcome::failure,10));
    ASSERT_NE(*dynamic_cast<serverMessage*>(toStore.get()), *dynamic_cast<serverMessage*>(toLoad.get()));
    storeMessage(toStore);
    loadMessage(toLoad);
    EXPECT_EQ(*dynamic_cast<serverMessage*>(toStore.get()), *dynamic_cast<serverMessage*>(toLoad.get()));
}

TEST_F(messageSerialization, askResMessage){
    toStore=std::unique_ptr<askResMessage>(new askResMessage(msgType::openRes, {"", ""}, "", "","",privilege::readOnly,10));
    toLoad=std::unique_ptr<askResMessage>(new askResMessage(msgType::openRes, {"", ""}, "path", "name", "",privilege::modify, 10));
    ASSERT_NE(*dynamic_cast<askResMessage*>(toStore.get()), *dynamic_cast<askResMessage*>(toLoad.get()));
    storeMessage(toStore);
    loadMessage(toLoad);
    EXPECT_EQ(*dynamic_cast<askResMessage*>(toStore.get()), *dynamic_cast<askResMessage*>(toLoad.get()));
}

TEST_F(messageSerialization, updateDocMessage){
    toStore=std::unique_ptr<updateDocMessage>(new updateDocMessage(msgType::mapChangesToUser, {"",""}, 100,10));
    toLoad=std::unique_ptr<updateDocMessage>(new updateDocMessage(msgType::mapChangesToUser, {"",""}, 10,10));
    ASSERT_NE(*dynamic_cast<updateDocMessage*>(toStore.get()), *dynamic_cast<updateDocMessage*>(toLoad.get()));
    storeMessage(toStore);
    loadMessage(toLoad);
    EXPECT_EQ(*dynamic_cast<updateDocMessage*>(toStore.get()), *dynamic_cast<updateDocMessage*>(toLoad.get()));
}

TEST_F(messageSerialization, signUpMessage){
    toStore=std::unique_ptr<signUpMessage>(new signUpMessage(msgType::registration, {"", ""}, user("user", "AP@ssw0rd!", "noempty", "", 0, nullptr),10));
    toLoad=std::unique_ptr<signUpMessage>(new signUpMessage(msgType::registration, {"", ""}, user(),10));
    ASSERT_NE(*dynamic_cast<signUpMessage*>(toStore.get()), *dynamic_cast<signUpMessage*>(toLoad.get()));
    storeMessage(toStore);
    loadMessage(toLoad);
    EXPECT_EQ(*dynamic_cast<signUpMessage*>(toStore.get()), *dynamic_cast<signUpMessage*>(toLoad.get()));
}

TEST_F(messageSerialization, loginMessage){
    toStore=std::unique_ptr<loginMessage>(new loginMessage(msgType::login, msgOutcome::success, user("user", "AP@ssw0rd!", "noempty", "", 0, nullptr),10));
    toLoad=std::unique_ptr<loginMessage>(new loginMessage(msgType::login, msgOutcome::success, user(),10));
    ASSERT_NE(*dynamic_cast<loginMessage*>(toStore.get()), *dynamic_cast<loginMessage*>(toLoad.get()));
    storeMessage(toStore);
    loadMessage(toLoad);
    EXPECT_EQ(*dynamic_cast<loginMessage*>(toStore.get()), *dynamic_cast<loginMessage*>(toLoad.get()));
}

TEST_F(messageSerialization, sendResMessage){
    toStore=std::unique_ptr<sendResMessage>(new sendResMessage(msgType::openRes, msgOutcome::success, directory::getRoot(), 10));
    toLoad=std::unique_ptr<sendResMessage>(new sendResMessage(msgType::openRes, msgOutcome::success, directory::emptyDir(), 10));
    ASSERT_NE(*dynamic_cast<sendResMessage*>(toStore.get()), *dynamic_cast<sendResMessage*>(toLoad.get()));
    storeMessage(toStore);
    loadMessage(toLoad);
    EXPECT_EQ(*dynamic_cast<sendResMessage*>(toStore.get()), *dynamic_cast<sendResMessage*>(toLoad.get()));
}

TEST_F(messageSerialization, updateActiveMessage){
    toStore=std::unique_ptr<updateActiveMessage>(new updateActiveMessage(msgType::addActiveUser, msgOutcome::success, user("user", "AP@ssw0rd!", "noempty", "", 0, nullptr), 10,privilege::readOnly, 10));
    toLoad=std::unique_ptr<updateActiveMessage>(new updateActiveMessage(msgType::addActiveUser, msgOutcome::success, user(), 10, privilege::readOnly, 10));
    ASSERT_NE(*dynamic_cast<updateActiveMessage*>(toStore.get()), *dynamic_cast<updateActiveMessage*>(toLoad.get()));
    storeMessage(toStore);
    loadMessage(toLoad);
    EXPECT_EQ(*dynamic_cast<updateActiveMessage*>(toStore.get()), *dynamic_cast<updateActiveMessage*>(toLoad.get()));
}

TEST_F(messageSerialization, mapMessage){
    std::map<uint_positive_cnt::type, user> expected({
                                         std::pair<int, user>(2, SymServer::unknownUser),
                                         std::pair<int, user>(3, SymServer::unknownUser)});
    toStore=std::unique_ptr<mapMessage>(new mapMessage(msgType::mapChangesToUser, msgOutcome::success, expected,10));
    toLoad=std::unique_ptr<mapMessage>(new mapMessage(msgType::mapChangesToUser, msgOutcome::success, std::map<uint_positive_cnt::type, user>(),10));
    ASSERT_NE(*dynamic_cast<mapMessage*>(toStore.get()), *dynamic_cast<mapMessage*>(toLoad.get()));
    storeMessage(toStore);
    loadMessage(toLoad);
    EXPECT_EQ(*dynamic_cast<mapMessage*>(toStore.get()), *dynamic_cast<mapMessage*>(toLoad.get()));
}

TEST_F(messageSerialization, privMessage){
    toStore=std::unique_ptr<privMessage>(new privMessage(msgType::changePrivileges, {"", ""}, msgOutcome::success, "./10/11", "user", privilege::readOnly, 10));
    toLoad=std::unique_ptr<privMessage>(new privMessage(msgType::changePrivileges, {"", ""}, msgOutcome::success, "./10/11", "user2", privilege::readOnly,10));
    ASSERT_NE(*dynamic_cast<privMessage*>(toStore.get()), *dynamic_cast<privMessage*>(toLoad.get()));
    storeMessage(toStore);
    loadMessage(toLoad);
    EXPECT_EQ(*dynamic_cast<privMessage*>(toStore.get()), *dynamic_cast<privMessage*>(toLoad.get()));
}

TEST_F(messageSerialization, symbolMessage){
    toStore=std::unique_ptr<symbolMessage>(new symbolMessage(msgType::insertSymbol, {"",""},msgOutcome ::success, 10, 10, symbol('a', 0, 1, std::vector<int>(), false), 10));
    toLoad=std::unique_ptr<symbolMessage>(new symbolMessage(msgType::insertSymbol, {"",""},msgOutcome ::success, 10, 10, symbol('b', 0, 1, std::vector<int>(), false), 10));
    ASSERT_NE(*dynamic_cast<symbolMessage*>(toStore.get()), *dynamic_cast<symbolMessage*>(toLoad.get()));
    storeMessage(toStore);
    loadMessage(toLoad);
    EXPECT_EQ(*dynamic_cast<symbolMessage*>(toStore.get()), *dynamic_cast<symbolMessage*>(toLoad.get()));
}

TEST_F(messageSerialization, uriMessage){
toStore=std::unique_ptr<uriMessage>(new uriMessage(msgType::shareRes, {"",""},msgOutcome ::success, "path", "name", uri(uriPolicy::activeAlways), 10));
toLoad=std::unique_ptr<uriMessage>(new uriMessage(msgType::shareRes, {"",""},msgOutcome ::success, "path", "name", uri(), 10));
ASSERT_NE(*dynamic_cast<uriMessage*>(toStore.get()), *dynamic_cast<uriMessage*>(toLoad.get()));
storeMessage(toStore);
loadMessage(toLoad);
EXPECT_EQ(*dynamic_cast<uriMessage*>(toStore.get()), *dynamic_cast<uriMessage*>(toLoad.get()));
}

TEST_F(messageSerialization, userDataMessage){
    toStore=std::unique_ptr<userDataMessage>(new userDataMessage(msgType::changeUserData, {"",""},msgOutcome ::success, user("user", "AP@ssw0rd!", "noempty", "", 0, nullptr), 10));
    toLoad=std::unique_ptr<userDataMessage>(new userDataMessage(msgType::changeUserData, {"",""},msgOutcome ::success, user(), 10));
    ASSERT_NE(*dynamic_cast<userDataMessage*>(toStore.get()), *dynamic_cast<userDataMessage*>(toLoad.get()));
    storeMessage(toStore);
    loadMessage(toLoad);
    EXPECT_EQ(*dynamic_cast<userDataMessage*>(toStore.get()), *dynamic_cast<userDataMessage*>(toLoad.get()));
}

TEST_F(messageSerialization, cursorMessage){
    toStore=std::unique_ptr<cursorMessage>(new cursorMessage(msgType::updateCursor, {"",""},msgOutcome ::success, 0, 0, 0, 0, 10));
    toLoad=std::unique_ptr<cursorMessage>(new cursorMessage(msgType::updateCursor, {"",""},msgOutcome ::success, 0, 0, 0, 10, 10));
    ASSERT_NE(*dynamic_cast<cursorMessage*>(toStore.get()), *dynamic_cast<cursorMessage*>(toLoad.get()));
    storeMessage(toStore);
    loadMessage(toLoad);
    EXPECT_EQ(*dynamic_cast<cursorMessage*>(toStore.get()), *dynamic_cast<cursorMessage*>(toLoad.get()));
}