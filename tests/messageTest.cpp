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
#include <gmock/gmock.h>
#include "../message.h"
#include "../SymServer.h"
#include "../SymClient.h"
#include "../filesystem.h"

using namespace Symposium;

TEST(messageTest, msgIdIsSet){
    clientMessage m(msgType::registration, {"", ""});
    EXPECT_NE(0, m.getMsgId());
}

TEST(messageTest, relatedMessagesHasSameId){
    clientMessage m(msgType::registration, {"", ""});
    serverMessage m2(m.getAction(), msgOutcome::failure, m.getMsgId());

    ASSERT_EQ(m.getAction(), m2.getAction());
    ASSERT_EQ(m.getMsgId(), m2.getMsgId());

    EXPECT_TRUE(m2.isRelatedTo(m));
}

//Tests for action consistency with the type of message

struct MessageActionTest: ::testing::TestWithParam<msgType>{
    message *m;
    user u;
    file f;
    symbol s;

    MessageActionTest(): u("username", "AP@ssw0rd!", "noempty", "", 0, nullptr),
                         f("name", "./somedir", 0),
                         s('a', 0, 0, std::vector<int>(), false) {
        m=nullptr;
    }
    ~MessageActionTest(){
        delete m;
    }

#define EXPECT_THROW_MESSAGE_CONSTRUCTION(statement, action, exceptionType) \
        EXPECT_THROW(statement, exceptionType) \
        <<"action: "<<action<<" should not be compatible with this message class";
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

        default: return output<<"operator<< not defined for this value";
    }
}

TEST_P(MessageActionTest, askResMsgForbiddenActions){
    msgType action=GetParam();
    EXPECT_THROW_MESSAGE_CONSTRUCTION(m= new askResMessage(action, {"", ""}, "", "", "", uri::getDefaultPrivilege(), 0), action, messageException);
}
msgType askResForbidden[]={
        msgType::registration, msgType::login, msgType::changeUserPwd,
         msgType::mapChangesToUser, msgType::changePrivileges, msgType::shareRes,
        msgType::insertSymbol, msgType::removeSymbol, msgType::addActiveUser, msgType::removeActiveUser,
        msgType::closeRes
};
INSTANTIATE_TEST_CASE_P(askResThrowExceptionInConstruction, MessageActionTest, testing::ValuesIn(askResForbidden));

TEST_P(MessageActionTest, signUpMsgForbiddenActions){
    msgType action=GetParam();
    EXPECT_THROW_MESSAGE_CONSTRUCTION(m=new signUpMessage(action, {"",""}, u), action, messageException);
}
msgType signUpForbidden[]={
        msgType::login, msgType::changeUserPwd, msgType::createRes, msgType::createNewDir,
         msgType::mapChangesToUser, msgType::changePrivileges, msgType::shareRes,
        msgType::insertSymbol, msgType::removeSymbol, msgType::addActiveUser, msgType::removeActiveUser,
        msgType::closeRes, msgType::openNewRes, msgType::changeResName, msgType::removeRes
};
INSTANTIATE_TEST_CASE_P(signUpThrowExceptionInConstruction, MessageActionTest, testing::ValuesIn(signUpForbidden));

TEST_P(MessageActionTest, updateDocMsgForbiddenActions) {
    msgType action = GetParam();
    EXPECT_THROW_MESSAGE_CONSTRUCTION(m = new updateDocMessage(action, {"", ""}, 0), action, messageException);
}
msgType updateDocForbidden[]={
        msgType::registration, msgType::login, msgType::changeUserPwd, msgType::createRes, msgType::createNewDir,
         msgType::changePrivileges, msgType::shareRes,
        msgType::insertSymbol, msgType::removeSymbol, msgType::addActiveUser, msgType::removeActiveUser,
        msgType::openNewRes, msgType::changeResName, msgType::removeRes
};
INSTANTIATE_TEST_CASE_P(updateDocThrowExceptionInConstruction, MessageActionTest, testing::ValuesIn(updateDocForbidden));

TEST_P(MessageActionTest, loginMsgForbiddenActions) {
    msgType action = GetParam();
    EXPECT_THROW_MESSAGE_CONSTRUCTION(m = new loginMessage(action, msgOutcome::success, u), action, messageException);
}
msgType loginForbidden[]={
        msgType::registration, msgType::closeRes, msgType::changeUserPwd, msgType::createRes, msgType::createNewDir,
         msgType::changePrivileges, msgType::shareRes,
        msgType::insertSymbol, msgType::removeSymbol, msgType::addActiveUser, msgType::removeActiveUser,
        msgType::openNewRes, msgType::changeResName, msgType::removeRes, msgType::mapChangesToUser
};
INSTANTIATE_TEST_CASE_P(loginThrowExceptionInConstruction, MessageActionTest, testing::ValuesIn(loginForbidden));

TEST_P(MessageActionTest, mapMsgForbiddenActions) {
    msgType action = GetParam();
    EXPECT_THROW_MESSAGE_CONSTRUCTION(m = new mapMessage(action, msgOutcome::success, std::map<int, user>()), action, messageException);
}
msgType mapForbidden[]={
        msgType::registration, msgType::closeRes, msgType::changeUserPwd, msgType::createRes, msgType::createNewDir,
         msgType::changePrivileges, msgType::shareRes,
        msgType::insertSymbol, msgType::removeSymbol, msgType::addActiveUser, msgType::removeActiveUser,
        msgType::openNewRes, msgType::changeResName, msgType::removeRes, msgType::login
};
INSTANTIATE_TEST_CASE_P(mapThrowExceptionInConstruction, MessageActionTest, testing::ValuesIn(mapForbidden));

TEST_P(MessageActionTest, sendResMsgForbiddenActions) {
    msgType action = GetParam();
    EXPECT_THROW_MESSAGE_CONSTRUCTION(m = new sendResMessage(action, msgOutcome::success, f), action, messageException);
}
msgType sendResForbidden[]={
        msgType::registration, msgType::closeRes, msgType::changeUserPwd,
         msgType::changePrivileges, msgType::shareRes,
        msgType::insertSymbol, msgType::removeSymbol, msgType::addActiveUser, msgType::removeActiveUser,
        msgType::mapChangesToUser
};
INSTANTIATE_TEST_CASE_P(sendResThrowExceptionInConstruction, MessageActionTest, testing::ValuesIn(sendResForbidden));

TEST_P(MessageActionTest, updateActiveMsgForbiddenActions) {
    msgType action = GetParam();
    EXPECT_THROW_MESSAGE_CONSTRUCTION(m = new updateActiveMessage(action, msgOutcome::success, u, 0), action, messageException);
}
msgType updateActiveForbidden[]={
        msgType::registration, msgType::closeRes, msgType::changeUserPwd, msgType::createRes, msgType::createNewDir,
         msgType::changePrivileges, msgType::shareRes,
        msgType::insertSymbol, msgType::removeSymbol, msgType::mapChangesToUser,
        msgType::openNewRes, msgType::changeResName, msgType::removeRes, msgType::login
};
INSTANTIATE_TEST_CASE_P(updateActiveThrowExceptionInConstruction, MessageActionTest, testing::ValuesIn(updateActiveForbidden));

TEST_P(MessageActionTest, privMsgForbiddenActions) {
    msgType action = GetParam();
    EXPECT_THROW_MESSAGE_CONSTRUCTION(m = new privMessage(action, {"", ""}, msgOutcome::success, 0, "", privilege::modify), action, messageException);
}
msgType privMsgForbidden[]={
        msgType::registration, msgType::closeRes, msgType::changeUserPwd, msgType::createRes, msgType::createNewDir,
         msgType::login, msgType::shareRes,
        msgType::insertSymbol, msgType::removeSymbol, msgType::addActiveUser, msgType::removeActiveUser,
        msgType::openNewRes, msgType::changeResName, msgType::removeRes, msgType::mapChangesToUser
};
INSTANTIATE_TEST_CASE_P(privThrowExceptionInConstruction, MessageActionTest, testing::ValuesIn(privMsgForbidden));

TEST_P(MessageActionTest, symbolMsgForbiddenActions) {
    msgType action = GetParam();
    EXPECT_THROW_MESSAGE_CONSTRUCTION(m = new symbolMessage(action, {"", ""}, msgOutcome::success, 0, 0, s), action, messageException);
}
msgType symbolMsgForbidden[]={
        msgType::registration, msgType::closeRes, msgType::changeUserPwd, msgType::createRes, msgType::createNewDir,
         msgType::login, msgType::shareRes,
        msgType::changePrivileges, msgType::addActiveUser, msgType::removeActiveUser,
        msgType::openNewRes, msgType::changeResName, msgType::removeRes, msgType::mapChangesToUser
};
INSTANTIATE_TEST_CASE_P(symbolThrowExceptionInConstruction, MessageActionTest, testing::ValuesIn(symbolMsgForbidden));

TEST_P(MessageActionTest, uriMsgForbiddenActions) {
    msgType action = GetParam();
    EXPECT_THROW_MESSAGE_CONSTRUCTION(m = new uriMessage(action, {"", ""}, msgOutcome::success, "path",
                                                         "name", uri(), 0), action, messageException);
}
msgType uriMsgForbidden[]={
        msgType::registration, msgType::closeRes, msgType::changeUserPwd, msgType::createRes, msgType::createNewDir,
         msgType::changePrivileges, msgType::login,
        msgType::insertSymbol, msgType::removeSymbol, msgType::addActiveUser, msgType::removeActiveUser,
        msgType::openNewRes, msgType::changeResName, msgType::removeRes, msgType::mapChangesToUser
};
INSTANTIATE_TEST_CASE_P(uriThrowExceptionInConstruction, MessageActionTest, testing::ValuesIn(uriMsgForbidden));

TEST_P(MessageActionTest, userDataMsgForbiddenActions) {
    msgType action = GetParam();
    EXPECT_THROW_MESSAGE_CONSTRUCTION(m = new userDataMessage(action, {"", ""}, msgOutcome::success, u), action, messageException);
}
msgType userDataMsgForbidden[]={
        msgType::registration, msgType::closeRes, msgType::createRes, msgType::createNewDir,
        msgType::changePrivileges, msgType::shareRes, msgType::login,
        msgType::insertSymbol, msgType::removeSymbol, msgType::addActiveUser, msgType::removeActiveUser,
        msgType::openNewRes, msgType::changeResName, msgType::removeRes, msgType::mapChangesToUser
};
INSTANTIATE_TEST_CASE_P(userDataThrowExceptionInConstruction, MessageActionTest, testing::ValuesIn(userDataMsgForbidden));

//Basic logic tests for messages sent ONLY by the client (clientMessage)

class SymServerMock: public SymServer{
public:
    MOCK_METHOD2(login, const user(const std::string&, const std::string&));
    MOCK_METHOD2(logout, void(const std::string&, const std::string&));
    MOCK_METHOD2(removeUser, void(const std::string&, const std::string&));
    MOCK_METHOD1(addUser, const user(const user&));
    MOCK_METHOD3(createNewSource, document&(const user&, const std::string&, const std::string&));
    MOCK_METHOD5(openNewSource, const document&(const user&, const std::string&, const std::string&, privilege, const std::string&));
    MOCK_METHOD4(renameResource, std::shared_ptr<filesystem>(const user&, const std::string&, const std::string&, const std::string&));
    MOCK_METHOD3(createNewDir, std::shared_ptr<directory>(const user&, const std::string&, const std::string&));
    MOCK_METHOD3(removeResource, std::shared_ptr<filesystem>(const user&, const std::string&, const std::string&));
    MOCK_METHOD2(mapSiteIdToUser, std::map<int, user>(const std::string&, int));

    MOCK_METHOD5(editPrivilege, privilege(const user&, const user&, const std::string&, const std::string&, privilege));
    MOCK_METHOD3(remoteInsert, void(const std::string&, int, symbolMessage&));
    MOCK_METHOD3(remoteRemove, void(const std::string&, int, const symbol&));
    MOCK_METHOD4(shareResource, std::shared_ptr<filesystem>(const user& actionUser, const std::string&, const std::string&, uri&));
    MOCK_METHOD3(editUser, const user&(const std::string&, const std::string&, user&));
    MOCK_METHOD2(closeSource, void(const user&, document&));
};

struct clientMessageTest: public testing::Test{
    clientMessage *m;
    user u;
    ::testing::NiceMock<SymServerMock> server;
    clientMessageTest(): u("username", "AP@ssw0rd!", "noempty", "", 0, nullptr){
        m= nullptr;
    }
    ~clientMessageTest(){
        delete m;
    }
};

TEST_F(clientMessageTest, clientMessageTestCallsLoginOnServer){
    m=new clientMessage(msgType::login, {"", ""});
    EXPECT_CALL(server, login(m->getActionOwner().first, m->getActionOwner().second)).WillOnce(::testing::Return(u));
    m->invokeMethod(server);
}

TEST_F(clientMessageTest, clientMessageTestCallsLogoutOnServer){
    m=new clientMessage(msgType::logout, {"", ""});
    EXPECT_CALL(server, logout(m->getActionOwner().first, m->getActionOwner().second));
    m->invokeMethod(server);
}

TEST_F(clientMessageTest, clientMessageTestCallsRemoveUserOnServer){
    m=new clientMessage(msgType::removeUser, {"", ""});
    EXPECT_CALL(server, removeUser(m->getActionOwner().first, m->getActionOwner().second));
    m->invokeMethod(server);
}

TEST_F(clientMessageTest, signUpMsgTestCallsAddUserOnServer){
    m=new signUpMessage(msgType::registration, {"", ""}, u);
    EXPECT_CALL(server, addUser(u)).WillOnce(::testing::Return(u));
    m->invokeMethod(server);
}

TEST_F(clientMessageTest, askResMsgTestCallsCreateNewSource){
    m= new askResMessage(msgType::createRes, {"", ""}, "", "", "", uri::getDefaultPrivilege(), 0);
    document returned;
    EXPECT_CALL(server, createNewSource(u, "", "")).WillOnce(::testing::ReturnRef(returned));
    m->invokeMethod(server);
}

TEST_F(clientMessageTest, askResMsgTestCallsOpenNewSource){
    m= new askResMessage(msgType::openNewRes, {"", ""}, "", "", "", uri::getDefaultPrivilege(), 0);
    document returned;
    EXPECT_CALL(server, openNewSource(u, "", "", privilege::modify, "")).WillOnce(::testing::ReturnRef(returned));
    m->invokeMethod(server);
}

TEST_F(clientMessageTest, askResMsgTestCallsRenameResource){
    m= new askResMessage(msgType::changeResName, {"", ""}, "", "", "", uri::getDefaultPrivilege(), 0);
    EXPECT_CALL(server, renameResource(u, "", "", "")).WillOnce(::testing::Return(std::shared_ptr<filesystem>()));
    m->invokeMethod(server);
}

TEST_F(clientMessageTest, askResMsgTestCallsCreateNewDir){
    m= new askResMessage(msgType::createNewDir, {"", ""}, "", "", "", uri::getDefaultPrivilege(), 0);
    EXPECT_CALL(server, createNewDir(u, "", "")).WillOnce(::testing::Return(std::shared_ptr<directory>()));
    m->invokeMethod(server);
}

TEST_F(clientMessageTest, updateDocMsgTestCallsMapSiteIdToUser){
    m=new updateDocMessage(msgType::mapChangesToUser, {"",""},0);
    EXPECT_CALL(server, mapSiteIdToUser("", 0)).WillOnce(::testing::Return(std::map<int, user>()));
    m->invokeMethod(server);
}

TEST_F(clientMessageTest, updateDocMsgTestCallsCloseSource){
    m=new updateDocMessage(msgType::closeRes, {"",""},0);
    document dummy=document();
    EXPECT_CALL(server, closeSource(u, dummy)).WillOnce(::testing::Return());
    m->invokeMethod(server);
}

//Basic logic tests for messages sent ONLY by the server (serverMessage)

class SymClientMock: public SymClient{
public:
    MOCK_METHOD1(setLoggedUser, void(const user&));
    MOCK_METHOD1(setUserColors, void(const std::map<int, user>&));
    MOCK_METHOD3(createNewSource, void(const std::string&, const std::string&, int));
    MOCK_METHOD3(createNewDir, void(const std::string&, const std::string&, int));
    MOCK_METHOD1(openSource, void(const std::shared_ptr<file>));
    MOCK_METHOD6(openNewSource, void(const std::string &resId, privilege reqPriv, const std::string &destPath, const std::string &destName, int idToAssign, const std::shared_ptr<file> fileAsked));
    MOCK_METHOD4(renameResource, std::shared_ptr<filesystem>(const std::string&, const std::string&, const std::string&, bool));
    MOCK_METHOD3(removeResource, std::shared_ptr<filesystem>(const std::string&, const std::string&, bool));
    MOCK_METHOD2(addActiveUser, void(int, user &));
    MOCK_METHOD2(removeActiveUser, void(int, user&));

    MOCK_METHOD5(editPrivilege, privilege(const std::string&, const std::string&, const std::string&, privilege, bool));
    MOCK_METHOD2(remoteInsert, void(int, const symbol&));
    MOCK_METHOD2(remoteRemove, void(int, const symbol&));
    MOCK_METHOD4(shareResource, uri(const std::string&, const std::string&, uri&, bool msgRcv));
    MOCK_METHOD2(editUser, const user(user&, bool));
    MOCK_METHOD2(verifySymbol, void(int, const symbol&));
};

struct serverMessageTest: public testing::Test{
    serverMessage *m;
    user u;
    ::testing::NiceMock<SymClientMock> client;

    serverMessageTest(): u("username", "AP@ssw0rd!", "noempty", "", 0, nullptr){
        m= nullptr;
    }
    ~serverMessageTest(){
        delete m;
    }
};

TEST_F(serverMessageTest, loginMsgTestCallsSetLoggedUser){
    m=new loginMessage(msgType::login, msgOutcome::success, u);
    EXPECT_CALL(client, setLoggedUser(u)).WillOnce(::testing::Return());
    m->invokeMethod(client);
}

TEST_F(serverMessageTest, mapMsgTestCallsSetUserColors){
    std::map<int, user> siteIdToUser;
    m=new mapMessage(msgType::mapChangesToUser, msgOutcome::success, siteIdToUser);
    EXPECT_CALL(client, setUserColors(siteIdToUser));
    m->invokeMethod(client);
}

TEST_F(serverMessageTest, sendResMsgTestCallsCreateNewSource){
    std::shared_ptr<file> dummyFile(new file("file", "./somedir", 0));
    m=new sendResMessage(msgType::createRes, msgOutcome::success, *dummyFile);
    EXPECT_CALL(client, createNewSource(::testing::_, ::testing::_, ::testing::_));
    m->invokeMethod(client);
}

TEST_F(serverMessageTest, sendResMsgTestCallsCreateNewDir){
    m=new sendResMessage(msgType::createNewDir, msgOutcome::success, *directory::nullDir());
    EXPECT_CALL(client, createNewDir(::testing::_, ::testing::_, ::testing::_));
    m->invokeMethod(client);
}

TEST_F(serverMessageTest, sendResMsgTestCallsOpenSource){
    std::shared_ptr<file> dummyFile(new file("file", "./somedir", 0));
    m=new sendResMessage(msgType::openRes, msgOutcome::success, *dummyFile);
    EXPECT_CALL(client, openSource(dummyFile));
    m->invokeMethod(client);
}

TEST_F(serverMessageTest, sendResMsgTestCallsOpenNewSource){
    std::shared_ptr<filesystem> dummyFile(new file("file", "./somedir", 0));
    m=new sendResMessage(msgType::openNewRes, msgOutcome::success, *dummyFile);
    EXPECT_CALL(client, openNewSource(::testing::_, ::testing::_, ::testing::_, "file", dummyFile->getId(), std::dynamic_pointer_cast<file>(dummyFile)));
    m->invokeMethod(client);
}

TEST_F(serverMessageTest, sendResMsgTestCallsRenameResource){
    file dummyFile("file", "./somedir", 0);
    m=new sendResMessage(msgType::changeResName, msgOutcome::success, dummyFile);
    EXPECT_CALL(client, renameResource("", "", "", true)).WillOnce(::testing::Return(std::shared_ptr<filesystem>()));
    m->invokeMethod(client);
}

TEST_F(serverMessageTest, sendResMsgTestCallsRemoveResource){
    file dummyFile("file", "./somedir", 0);
    m=new sendResMessage(msgType::removeRes, msgOutcome::success, dummyFile);
    EXPECT_CALL(client, removeResource("", "", true)).WillOnce(::testing::Return(std::shared_ptr<filesystem>()));
    m->invokeMethod(client);
}

TEST_F(serverMessageTest, updateActiveMsgTestCallsAddActiveUsers){
    m=new updateActiveMessage(msgType::addActiveUser, msgOutcome::success, u, 0);
    updateActiveMessage* uam=dynamic_cast<updateActiveMessage*>(m);
    EXPECT_CALL(client, addActiveUser(uam->getResourceId(), const_cast<user&>(uam->getNewUser()))).WillOnce(::testing::Return());
    m->invokeMethod(client);
}

TEST_F(serverMessageTest, updateActiveMsgTestCallsRemoveActiveUsers){
    m=new updateActiveMessage(msgType::removeActiveUser, msgOutcome::success, u, 0);
    updateActiveMessage* uam=dynamic_cast<updateActiveMessage*>(m);
    EXPECT_CALL(client, removeActiveUser(uam->getResourceId(), const_cast<user&>(uam->getNewUser()))).WillOnce(::testing::Return());
    m->invokeMethod(client);
}

//Basic logic tests for messages that may be sent from client or server (message)

struct DoubleEndMessageTest: public testing::Test{
    serverMessage *fromServer;
    clientMessage *fromClient;
    user u;
    ::testing::NiceMock<SymClientMock> client;
    ::testing::NiceMock<SymServerMock> server;
    symbol dummySymbol;
    uri dummyUri;
    privilege priv;

    DoubleEndMessageTest(): dummySymbol('a', 0, 1, std::vector<int>(), false),
                            u("username", "AP@ssw0rd!", "noempty", "", 0, nullptr),
                            priv(privilege::modify){
        fromServer=nullptr;
        fromClient= nullptr;
    }
    ~DoubleEndMessageTest(){
        delete fromServer;
        delete fromClient;
    }
};

TEST_F(DoubleEndMessageTest, privMsgCallsEditPrivilege){
    fromClient=new privMessage(msgType::changePrivileges,{"", ""}, msgOutcome::success, "", "", privilege::modify);
    privMessage* fc= dynamic_cast<privMessage*>(fromClient);
    //message handler has to retrieve the correct actionOwner user knowing the pair (username, pwd) in message's actionOwner
    //(here we suppose it is the dummy user u)
    //Same thing with targetUser
    EXPECT_CALL(server, editPrivilege(u, u, "", "", fc->getNewPrivilege())).WillOnce(::testing::Return(privilege::modify));
    fromClient->invokeMethod(server);

    fromServer= new serverMessage(msgType::changePrivileges, msgOutcome::success, fromClient->getMsgId());
    //client uses the message previously sent to the server to retrieve the parameters for the required action:
    //targetUser from fc->getTargetUser() (from std::string to user)
    //resPath and resName from fc->getResourceId
    EXPECT_CALL(client, editPrivilege(u.getUsername(), "", "", fc->getNewPrivilege(), true)).WillOnce(::testing::Return(fc->getNewPrivilege()));
    //when a serverMessage is received, a related message from the client is searched and completeAction() called
    fromClient->completeAction(client);
}

TEST_F(DoubleEndMessageTest, privMsgCallsEditPrivilegeOnOtherClient){
    fromClient=new privMessage(msgType::changePrivileges,{"user", "pwd"}, msgOutcome::success, "", "", privilege::modify);
    privMessage* fc= dynamic_cast<privMessage*>(fromClient);

    //the message from client is forwarded to the other client, but the password is cleaned
    fromServer=new privMessage(msgType::changePrivileges,{"user", ""}, msgOutcome::success, "", "", privilege::modify);
    EXPECT_CALL(client, editPrivilege(u.getUsername(), "", "", fc->getNewPrivilege(), true)).WillOnce(::testing::Return(fc->getNewPrivilege()));
    fromServer->invokeMethod(client);
}

TEST_F(DoubleEndMessageTest, symbolMsgCallsRemoteInsertOnSuccess){
    fromClient=new symbolMessage(msgType::insertSymbol,{"", ""}, msgOutcome::success, 0,0, dummySymbol);
    symbolMessage* fc= dynamic_cast<symbolMessage*>(fromClient);
    //message handler has to retrieve the correct actionOwner user knowing the pair (username, pwd) in message's actionOwner
    //(here we suppose it is the dummy user u)
    //Same thing with targetUser
    EXPECT_CALL(server, remoteInsert("", 0, *fc)).WillOnce(::testing::Return());
    fromClient->invokeMethod(server);

    fromServer= new serverMessage(msgType::insertSymbol, msgOutcome::success, fromClient->getMsgId());
    //client uses the message previously sent to the server to retrieve the parameters for the required action:
    //targetUser from fc->getTargetUser() (from std::string to user)
    //resPath and resName from fc->getResourceId

    //We expect verifySymbol to be called by fromClient when the outcome is positive
    EXPECT_CALL(client, verifySymbol(0, fc->getSym()));
    fromClient->completeAction(client);
}

TEST_F(DoubleEndMessageTest, symbolMsgCallsRemoteRemoveOnFailure){
    fromClient=new symbolMessage(msgType::insertSymbol,{"", ""}, msgOutcome::success, 0,0, dummySymbol);
    symbolMessage* fc= dynamic_cast<symbolMessage*>(fromClient);
    //message handler has to retrieve the correct actionOwner user knowing the pair (username, pwd) in message's actionOwner
    //(here we suppose it is the dummy user u)
    //Same thing with targetUser
    EXPECT_CALL(server, remoteInsert("", 0, *fc)).WillOnce(::testing::Return());
    fromClient->invokeMethod(server);

    //Suppose now that the server returns a failure
    fromServer= new serverMessage(msgType::insertSymbol, msgOutcome::failure, fromClient->getMsgId());
    //client uses the message previously sent to the server to retrieve the parameters for the required action:
    //targetUser from fc->getTargetUser() (from std::string to user)
    //resPath and resName from fc->getResourceId

    //We expect remoteRemove to be called by fromClient when the outcome is negative!
    EXPECT_CALL(client, remoteRemove(0, fc->getSym()));
    fromClient->completeAction(client);
}

TEST_F(DoubleEndMessageTest, symbolMsgCallsRemoteInsertOnOtherClient){
    fromClient=new symbolMessage(msgType::insertSymbol,{"user", "pwd"}, msgOutcome::success, 0,0, dummySymbol);
    symbolMessage* fc= dynamic_cast<symbolMessage*>(fromClient);

    //the message from client is forwarded to the other client, but the password is cleaned and it's now verified
    symbol s=fc->getSym();
    fromServer= new symbolMessage(msgType::insertSymbol,{"user", ""}, msgOutcome::success, 0,0, s.setVerified());
    //client uses the message previously sent to the server to retrieve the parameters for the required action:
    //targetUser from fc->getTargetUser() (from std::string to user)
    //resPath and resName from fc->getResourceId
    EXPECT_CALL(client, remoteInsert(0, dummySymbol)).WillOnce(::testing::Return());
    fromServer->invokeMethod(client);
}

TEST_F(DoubleEndMessageTest, symbolMsgCallsRemoteRemoveOnSuccess){
    fromClient=new symbolMessage(msgType::removeSymbol,{"", ""}, msgOutcome::success, 0,0, dummySymbol);
    symbolMessage* fc= dynamic_cast<symbolMessage*>(fromClient);

    fromServer= new serverMessage(msgType::removeSymbol, msgOutcome::success, fromClient->getMsgId());
    //client uses the message previously sent to the server to retrieve the parameters for the required action:
    //targetUser from fc->getTargetUser() (from std::string to user)
    //resPath and resName from fc->getResourceId

    //We expect no function called by fromClient when the outcome is positive
    fromClient->completeAction(client);
}

TEST_F(DoubleEndMessageTest, symbolMsgCallsRemoteInsertOnFailure){
    fromClient=new symbolMessage(msgType::removeSymbol,{"", ""}, msgOutcome::success, 0,0, dummySymbol);
    symbolMessage* fc= dynamic_cast<symbolMessage*>(fromClient);

    fromServer= new serverMessage(msgType::removeSymbol, msgOutcome::failure, fromClient->getMsgId());
    //client uses the message previously sent to the server to retrieve the parameters for the required action:
    //targetUser from fc->getTargetUser() (from std::string to user)
    //resPath and resName from fc->getResourceId

    //We expect remoteInsert to be called by fromClient when the outcome is negative, so that the removal is annulled
    EXPECT_CALL(client, remoteInsert(0, fc->getSym()));
    fromClient->completeAction(client);
}

TEST_F(DoubleEndMessageTest, symbolMsgCallsRemoteRemoveOnOtherClient){
    fromClient=new symbolMessage(msgType::removeSymbol,{"user", "pwd"}, msgOutcome::success, 0,0, dummySymbol);
    symbolMessage* fc= dynamic_cast<symbolMessage*>(fromClient);

    //the message from client is forwarded to the other client, but the password is cleaned
    fromServer= new symbolMessage(msgType::removeSymbol,{"user", ""}, msgOutcome::success, 0,0, fc->getSym());
    //client uses the message previously sent to the server to retrieve the parameters for the required action:
    //targetUser from fc->getTargetUser() (from std::string to user)
    //resPath and resName from fc->getResourceId
    EXPECT_CALL(client, remoteRemove(0, dummySymbol)).WillOnce(::testing::Return());
    fromServer->invokeMethod(client);
}

TEST_F(DoubleEndMessageTest, uriMsgCallsRemoteRemove){
    fromClient= new uriMessage(msgType::shareRes, {"", ""}, msgOutcome::success, "path", "name",
                               dummyUri, 0);
    uriMessage* fc= dynamic_cast<uriMessage*>(fromClient);
    //message handler has to retrieve the correct actionOwner user knowing the pair (username, pwd) in message's actionOwner
    //(here we suppose it is the dummy user u)
    //Same thing with targetUser
    EXPECT_CALL(server, shareResource(u, "path", "name", const_cast<uri&>(fc->getSharingPrefs())));
    fromClient->invokeMethod(server);

    fromServer= new serverMessage(msgType::shareRes, msgOutcome::success, fromClient->getMsgId());
    //client uses the message previously sent to the server to retrieve the parameters for the required action:
    //targetUser from fc->getTargetUser() (from std::string to user)
    //resPath and resName from fc->getResourceId
    EXPECT_CALL(client, shareResource("path", "name", const_cast<uri&>(fc->getSharingPrefs()), true)).WillOnce(::testing::Return(fc->getSharingPrefs()));
    fromClient->completeAction(client);
}

TEST_F(DoubleEndMessageTest, uriMsgCallsRemoteRemoveOnOtherClient){
    fromClient= new uriMessage(msgType::shareRes, {"user", "pwd"}, msgOutcome::success, "path",
                               "name",
                               dummyUri, 0);
    uriMessage* fc= dynamic_cast<uriMessage*>(fromClient);

    //the message from client is forwarded to the other client, but the password is cleaned
    fromServer= new uriMessage(msgType::shareRes, {"", ""}, msgOutcome::success, "path", "name",
                               dummyUri, fc->getMsgId());
    //client uses the message previously sent to the server to retrieve the parameters for the required action:
    //targetUser from fc->getTargetUser() (from std::string to user)
    //resPath and resName from fc->getResourceId
    EXPECT_CALL(client, shareResource("path", "name", const_cast<uri&>(fc->getSharingPrefs()), false)).WillOnce(::testing::Return(fc->getSharingPrefs()));
    fromServer->invokeMethod(client);
}

TEST_F(DoubleEndMessageTest, userDataMsgCallsEditUser){
    fromClient=new userDataMessage(msgType::changeUserPwd, {"", ""}, msgOutcome::success, u);
    //uriMessage* fc= dynamic_cast<uriMessage*>(fromClient);
    //message handler has to retrieve the correct actionOwner user knowing the pair (username, pwd) in message's actionOwner
    //(here we suppose it is the dummy user u)
    //Same thing with targetUser
    EXPECT_CALL(server, editUser("", "", u)).WillOnce(::testing::ReturnRef(u));
    fromClient->invokeMethod(server);

    fromServer= new serverMessage(msgType::shareRes, msgOutcome::success, fromClient->getMsgId());
    //client uses the message previously sent to the server to retrieve the parameters for the required action:
    //targetUser from fc->getTargetUser() (from std::string to user)
    //resPath and resName from fc->getResourceId
    EXPECT_CALL(client, editUser(u, true)).WillOnce(::testing::Return(u));
    fromClient->completeAction(client);
}

TEST_F(DoubleEndMessageTest, userDataMsgCallsEditUserOnOtherClient){
    fromClient=new userDataMessage(msgType::changeUserPwd, {"user", "pwd"}, msgOutcome::success, u);
    //uriMessage* fc= dynamic_cast<uriMessage*>(fromClient);

    fromServer= new userDataMessage(msgType::changeUserPwd, {"user", ""}, msgOutcome::success, u, fromClient->getMsgId());
    //client uses the message previously sent to the server to retrieve the parameters for the required action:
    //targetUser from fc->getTargetUser() (from std::string to user)
    //resPath and resName from fc->getResourceId
    EXPECT_CALL(client, editUser(u, true)).WillOnce(::testing::Return(u));
    fromServer->invokeMethod(client);
}