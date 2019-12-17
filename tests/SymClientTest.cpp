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
#include "../filesystem.h"
#include "../user.h"

using namespace Symposium;


struct SymClientUserMock: public user{
    SymClientUserMock(const std::string &username, const std::string &pwd, const std::string &nickname,
                      const std::string &iconPath, int siteId, const std::shared_ptr<directory> &home) :
            user(username, pwd, nickname, iconPath, siteId, home) {}
    MOCK_CONST_METHOD3(openFile, std::shared_ptr<file>(const std::string& path, const std::string& filename, privilege accessMode));
    MOCK_CONST_METHOD3(accessFile, std::pair<int, std::shared_ptr<file>>(const std::string &resId, const std::string &path, const std::string &fileName));
    MOCK_CONST_METHOD3(newFile, std::shared_ptr<file>(const std::string &filename, const std::string &path, int id));
    MOCK_CONST_METHOD3(newDirectory, std::shared_ptr<directory>(const std::string &filename, const std::string &path, int id));
    MOCK_METHOD4(editPrivilege, privilege(const std::string &targetUser, const std::string &resPath, const std::string &resName,
            privilege newPrivilege));
    MOCK_CONST_METHOD3(shareResource, std::shared_ptr<filesystem>(const std::string &resPath, const std::string &resName, uri &newPrefs));
    MOCK_CONST_METHOD3(renameResource, std::shared_ptr<filesystem>(const std::string &resPath, const std::string &resName, const std::string &newName));
    MOCK_CONST_METHOD2(removeResource, std::shared_ptr<filesystem>(const std::string &path, const std::string &name));
    MOCK_CONST_METHOD1(showDir, std::string(bool rec));
    MOCK_METHOD2(editUser, user(user&, bool));
};

struct SymClientAccesser: public SymClient{
    SymClientUserMock *userMock;

    SymClientAccesser(){}

    user& getLoggedUser() override {
        if(userMock== nullptr)
            throw std::exception();
        return *userMock;
    }

    user& getLoggedUserNotMocked() {
        return loggedUser;
    }

    std::map<std::pair<int, int>, std::pair<user, MyColor>>& getUserColors(){
        return userColors;
    };
    std::pair<bool, std::shared_ptr<clientMessage>> thereIsUnansweredMex(int msgId){
        std::pair<bool, std::shared_ptr<clientMessage>> result(false, nullptr);
        auto m= std::find_if(unanswered.begin(), unanswered.end(),
                            [&](std::shared_ptr<clientMessage>& cm){return cm->getMsgId()==msgId;});
        if(m!=unanswered.end()){
            result.first=true;
            result.second=*m;
        }
        return result;
    }

    virtual void setLoggedUser(const user &loggedUser) override{
        userMock= (SymClientUserMock *) &loggedUser;
    }

    std::pair<bool, document*> docIsInActive(int id){
        std::pair<bool, document*> result(false, nullptr);
        for(auto doc:activeDoc)
            if(doc->getId()==id) {
                result.first = true;
                result.second = doc;
            }
        return result;
    }
    bool fileIsInActive(int id){
        for(auto& f:activeFile)
            if(f->getId()==id)
                return true;
         return false;
    }

    virtual ~SymClientAccesser() {
        //avoid to point to objects that don't exist anymore
        userMock=nullptr;
    }
};

struct SymClientFileMock: public file{
    SymClientFileMock(const std::string &name, const std::string &realPath) : file(name, realPath, 0) {};
    MOCK_CONST_METHOD0(getDoc, document&());
    MOCK_METHOD2(access, document&(const user& u, privilege requested));
};

struct SymClientDirMock: public directory{
    SymClientDirMock(const std::string &name) : directory(name) {};
    MOCK_METHOD5(addLink, std::shared_ptr<class symlink>(const std::string&, const std::string&, const std::string&, const std::string&, int));
};

struct SymClientDocMock: public document{
    SymClientDocMock(int id):document(id) {};
    MOCK_METHOD1(remoteInsert, void(const symbol& toInsert));
    MOCK_METHOD1(remoteRemove, void(const symbol& toRemove));
    MOCK_METHOD1(close, void(const user& noLongerActive));
    MOCK_METHOD2(access, document&(const user&, privilege));
    MOCK_METHOD2(localInsert, symbol(const std::pair<int, int>& index, symbol &toInsert));
    MOCK_METHOD1(localRemove, symbol(const std::pair<int, int> index));
};

struct SymClientTest : ::testing::Test{
    SymClientUserMock userReceived;
    static const std::string anotherUsername;
    SymClientAccesser client;
    static const std::string iconPath;
    static const std::string username;
    static const std::string pwd;
    static const std::string nickname;
    static const std::string path;
    static const std::string filename;
    static constexpr privilege aPrivilege=privilege::modify;
    static const std::string destPath;
    static uri newPreferences;
    std::shared_ptr<SymClientFileMock> fileSentByServer;
    std::shared_ptr<SymClientFileMock> fileInUserFilesystem;
    std::shared_ptr<SymClientDirMock> dirSentByServer;
    SymClientDocMock docInUserFilesystem, docSentByServer;
    static const std::pair<int, int> indexes;
    SymClientTest(): userReceived(username, pwd, nickname, iconPath, 0, std::shared_ptr<SymClientDirMock>(new SymClientDirMock(username))),
                     fileSentByServer(new SymClientFileMock(filename, "./dir1/dir2")),
                     docInUserFilesystem(120), docSentByServer(120),
                     dirSentByServer(new SymClientDirMock(filename)){};
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
    void setStageForLoggedUser(){
        client.setLoggedUser(userReceived);
    }
    void messageHasCorrectOwner(const clientMessage& mex){
        std::pair<std::string, std::string> credentials(username, "");
        EXPECT_EQ(credentials, mex.getActionOwner());
    }

    void messageHasCorrectOwnerforLogin(const clientMessage& mex){
        std::pair<std::string, std::string> credentials(username, pwd);
        EXPECT_EQ(credentials, mex.getActionOwner());
    }

    void correctInsertionOfFileAndDocumentInLists(int idOfDocActive, document *presentInUserFile, int idOfFileActive) {
        std::pair<bool, document*> docActive=client.docIsInActive(idOfDocActive);
        ASSERT_TRUE(docActive.first);
        EXPECT_TRUE(docActive.second==presentInUserFile);
        EXPECT_TRUE(client.fileIsInActive(idOfFileActive));
    }
    void setStageForOpenedDoc(){
        setStageForLoggedUser();
        EXPECT_CALL(userReceived, openFile(path, filename, aPrivilege)).WillOnce(::testing::Return(fileInUserFilesystem));
        EXPECT_CALL(*fileSentByServer, access(userReceived, uri::getDefaultPrivilege())).WillOnce(::testing::ReturnRef(docSentByServer));
        client.openSource(fileSentByServer, uri::getDefaultPrivilege());
        ASSERT_NO_FATAL_FAILURE(correctInsertionOfFileAndDocumentInLists(docSentByServer.getId(), &docInUserFilesystem, fileSentByServer->getId()));
        //tests that the document returned by openFile() has now the same content of the document sent by the server
        EXPECT_EQ(docInUserFilesystem, docSentByServer);
    }
    /*
     * Just impose that every time we generate a clientMessage the msgId is different
     */
    ::testing::AssertionResult messagesHaveDifferentIds(const char* m_expr, int previousMsgId) {
        clientMessage cm(msgType::login, {"", ""});
        if(cm.getMsgId()==previousMsgId)
            return ::testing::AssertionFailure()<<"Unable to run this test because messages are constructed with the same msgId, so messages that are actually not related can be wrongly appear related";
        else return ::testing::AssertionSuccess();
    }

    struct messageIntruder: public message{
        static void setId(message& m, int newId){
            ((messageIntruder&)(m)).msgId=newId;
        }
    };
    /*
     * It's a little workaround, sets to the same value the msgId of the copy
     * of two given messages, the compares them using standard operator==
     */
    void messageAreAnalogous(message& m1, message& m2){
        int m2PrevSiteId=m2.getMsgId();
        //change m2's siteId to make the comparison fail only
        //if there are members different from msgId that are not equal
        messageIntruder::setId(m2, m1.getMsgId());
        EXPECT_EQ(m1, m2);
        //reset the previous msgId of m2
        messageIntruder::setId(m2, m2PrevSiteId);
    }
};
const std::string SymClientTest::iconPath="./icons/icon1.jpg";
const std::string SymClientTest::username="username";
const std::string SymClientTest::pwd="AP@ssw0rd!";
const std::string SymClientTest::nickname="nickname";
const std::string SymClientTest::path="./dir1/dir2";
const std::string SymClientTest::filename="file1";
const std::string SymClientTest::destPath="./dir1/dir2";
const std::string SymClientTest::anotherUsername="anotherUsername";
uri SymClientTest::newPreferences(uriPolicy::activeAlways);
const std::pair<int, int> SymClientTest::indexes={0,0};

TEST_F(SymClientTest, setLoggedUserAssignesUserReceivedToClient){
    client.setLoggedUser(userReceived);
    EXPECT_EQ(userReceived, client.getLoggedUserNotMocked());
}

TEST_F(SymClientTest, signUpConstructsGoodMessageAndInsertInUnanswered){
    auto mex= client.signUp(username, pwd, nickname, path);
    user expected(username, pwd, nickname, iconPath, 0, nullptr);
    EXPECT_EQ(expected, mex.getNewUser());
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}
//FIXME: this won't work
TEST_F(SymClientTest, signUpAssignesLoggedUserAndRemovesFromUnanswered){
    auto mex= client.signUp(username, pwd, nickname, path);
    //just imagine that the server has answered with msgOutcome::success to client's signUpMessage, the response contain the
    //complete user data
    client.signUp(userReceived);
    EXPECT_EQ(client.getLoggedUser(), userReceived);
    //EXPECT_FALSE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, loginConstructsGoodMessageAndInsertInUnanswered){
    auto mex=client.logIn(username, pwd);
    messageHasCorrectOwnerforLogin(mex);
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, loginAssignesLoggedUserAndRemovesFromUnanswered){
    auto mex=client.logIn(username, pwd);
    //just imagine that the server has answered with msgOutcome::success to client's clientMessage, the response contain the
    //complete user data
    client.logIn(userReceived);
    EXPECT_EQ(client.getLoggedUser(), userReceived);
    //EXPECT_FALSE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, openSourceConstructsGoodMessageAndInsertInUnanswered){
    setStageForLoggedUser();
    auto mex=client.openSource(path, filename, privilege::readOnly);
    messageHasCorrectOwner(mex);
    askResMessage expected(msgType::openRes, {username, ""}, path, filename, "", uri::getDefaultPrivilege(), 0);
    messageAreAnalogous(mex, expected);
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, openSourceOpensDocAndPutInActiveAndRemovesFromUnanswered)
{
    setStageForLoggedUser();
    auto mex=client.openSource(path, filename, privilege::readOnly);
    //just imagine that the server has answered with msgOutcome::success to client's askResMessage, the response contain the
    //resource asked. sendResMessage has already been tested to call openSource on client
    //the data use to call openFile must be taken from the relative askResMessage in unanswered
    EXPECT_CALL(userReceived, openFile(path, filename, aPrivilege)).WillOnce(::testing::Return(fileInUserFilesystem));
    EXPECT_CALL(*fileSentByServer, getDoc()).WillOnce(::testing::ReturnRef(docSentByServer));
    client.openSource(fileSentByServer, privilege::readOnly);
    ASSERT_NO_FATAL_FAILURE(correctInsertionOfFileAndDocumentInLists(docSentByServer.getId(), &docInUserFilesystem, fileSentByServer->getId()));
    //tests that the document returned by openFile() has now the same content of the document sent by the server
    EXPECT_EQ(docInUserFilesystem, docSentByServer);
    //EXPECT_FALSE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, openNewSourceConstructsGoodMessageAndInsertInUnanswered){
    setStageForLoggedUser();
    auto mex=client.openNewSource(path+"/"+filename, privilege::readOnly, destPath, filename);
    messageHasCorrectOwner(mex);
    askResMessage expected(msgType::openNewRes, {username, ""}, destPath, filename, path + "/" + filename, uri::getDefaultPrivilege(), 0);
    messageAreAnalogous(mex, expected);
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, openNewSourceOpensDocAndPutInActiveAndRemovesFromUnaswered)
{
    setStageForLoggedUser();
    auto mex=client.openNewSource(path+"/"+filename, privilege::readOnly, destPath, "sym");
    //just imagine that the server has answered with msgOutcome::success to client's askResMessage, the response contain the
    //resource asked. sendResMessage has already been tested to call openNewSource on client
    //the data use to call accessFile must be taken from the relative askResMessage in unanswered
    EXPECT_CALL(*static_cast<SymClientDirMock*>(client.getLoggedUser().getHome().get()), addLink(::testing::_, ::testing::_, path, filename, ::testing::_));
    EXPECT_CALL(*fileSentByServer, getDoc()).WillOnce(::testing::ReturnRef(docSentByServer));
    client.openNewSource(path+"/"+filename, privilege::readOnly, destPath, "sym", fileSentByServer->getId(), fileSentByServer);
    ASSERT_NO_FATAL_FAILURE(correctInsertionOfFileAndDocumentInLists(docSentByServer.getId(), &docSentByServer, fileSentByServer->getId()));
    //EXPECT_FALSE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, createNewSourceConstructsGoodMessageAndInsertInUnanswered){
    setStageForLoggedUser();
    auto mex=client.createNewSource(path, filename);
    messageHasCorrectOwner(mex);
    askResMessage expected(msgType::createRes, {username, ""}, path, filename, "", uri::getDefaultPrivilege(), 0);
    messageAreAnalogous(mex, expected);
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, createNewSourceOpensDocAndPutInActiveAndRemovesFromUnaswered)
{
    setStageForLoggedUser();
    auto cmex=client.createNewSource(path, filename);
    //just imagine that the server has answered with msgOutcome::success to client's askResMessage, the response contain the
    //resource asked. sendResMessage has already been tested to call createNewSource on client
    //the data used to call newFile must be taken from the relative askResMessage in unanswered (cmex)
    EXPECT_CALL(userReceived, newFile(filename, path, fileSentByServer->getId())).WillOnce(::testing::Return(fileSentByServer));
    EXPECT_CALL(*fileSentByServer, access(userReceived, privilege::owner)).WillOnce(::testing::ReturnRef(docSentByServer));
    client.createNewSource(path, filename, fileSentByServer->getId());
    ASSERT_NO_FATAL_FAILURE(correctInsertionOfFileAndDocumentInLists(docSentByServer.getId(), &docSentByServer, fileSentByServer->getId()));
    //EXPECT_FALSE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}
 /*
TEST_F(SymClientTest, createNewSourceOpensDocAndPutInActiveAndRemovesFromUnaswered)
{
    setStageForLoggedUser();
    auto cmex=client.openNewSource(path+"/"+filename, privilege::readOnly, destPath);
    auto mex=client.createNewSource(path, filename);
    //just imagine that the server has answered with msgOutcome::success to client's askResMessage, the response contain the
    //resource asked. sendResMessage has already been tested to call createNewSource on client
    //the data used to call newFile must be taken from the relative askResMessage in unanswered (cmex)
    EXPECT_CALL(userReceived, newFile(filename, path)).WillOnce(::testing::Return(fileSentByServer));
    EXPECT_CALL(*fileSentByServer, getDoc()).WillOnce(::testing::ReturnRef(docSentByServer));
    client.openNewSource(path+"/"+filename, privilege::readOnly, destPath, "sym", fileSentByServer->getId(), fileSentByServer);
    ASSERT_NO_FATAL_FAILURE(correctInsertionOfFileAndDocumentInLists(docSentByServer.getId(), &docSentByServer, fileSentByServer->getId()));
    //EXPECT_FALSE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}
  */

TEST_F(SymClientTest, createNewDirConstructsGoodMessageAndInsertInUnanswered){
    setStageForLoggedUser();
    auto mex=client.createNewDir(path, filename);
    messageHasCorrectOwner(mex);
    askResMessage expected(msgType::createNewDir, {username, ""}, path, filename, "", uri::getDefaultPrivilege(), 0);
    messageAreAnalogous(mex, expected);
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, createNewDirOpensDocAndPutInActiveAndRemovesFromUnaswered)
{
    setStageForLoggedUser();
    auto mex=client.createNewDir(path, filename);
    //just imagine that the server has answered with msgOutcome::success to client's askResMessage, the response contain the
    //resource asked. sendResMessage has already been tested to call createNewSource on client
    //the data use to call newDirectory must be taken from the relative askResMessage in unanswered
    EXPECT_CALL(userReceived, newDirectory(filename, path, dirSentByServer->getId())).WillOnce(::testing::Return(dirSentByServer));
    client.createNewDir(path, filename, dirSentByServer->getId());
    //EXPECT_FALSE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, localInsertConstructsGoodMessageAndInsertInUnanswered){
    setStageForOpenedDoc();
    symbol toInsert('a', userReceived.getSiteId(), 0, {}, false);
    symbol inserted('a', userReceived.getSiteId(), 0, {1}, false);
    EXPECT_CALL(docSentByServer, localInsert(indexes, toInsert)).WillOnce(::testing::Return(inserted));
    auto mex= client.localInsert(docSentByServer.getId(), toInsert, indexes);
    //std::pair<bool, std::shared_ptr<clientMessage>> res=client.thereIsUnansweredMex(mex.getMsgId());
    //ASSERT_TRUE(res.first);
    //EXPECT_EQ(mex, *res.second);
}

TEST_F(SymClientTest, localRemoveConstructsGoodMessageAndInsertInUnanswered){
    setStageForOpenedDoc();
    symbol removed('a', userReceived.getSiteId(), 0, {1}, false);
    EXPECT_CALL(docSentByServer, localRemove(indexes)).WillOnce(::testing::Return(removed));
    auto mex=client.localRemove(docSentByServer.getId(), indexes);
    //std::pair<bool, std::shared_ptr<clientMessage>> res=client.thereIsUnansweredMex(mex.getMsgId());
    //ASSERT_TRUE(res.first);
    //EXPECT_EQ(mex, *res.second);
    //EXPECT_EQ(removed, std::dynamic_pointer_cast<symbolMessage>(res.second)->getSym()); //the message contains the exact symbol to remove
}

TEST_F(SymClientTest, remoteInsertCallsremoteInsertOnRightDoc){
    setStageForOpenedDoc();
    symbol arrived('a', 1, 1, {}, true);
    //resouceId of the document to insert the symbol into is contained in the symbolMessage received by the client
    EXPECT_CALL(docSentByServer, remoteInsert(arrived));
    client.remoteInsert(docSentByServer.getId(), arrived);
}

TEST_F(SymClientTest, remoteRemoveCallsremoteRemoveOnRightDoc){
    setStageForOpenedDoc();
    symbol arrived('a', 1, 1, {}, true);
    //resouceId of the document to insert the symbol into is contained in the symbolMessage received by the client
    EXPECT_CALL(docSentByServer, remoteRemove(arrived));
    client.remoteRemove(docSentByServer.getId(), arrived);
}

TEST_F(SymClientTest, editPrivilegeConstructsGoodMessageAndInsertInUnanswered){
    setStageForLoggedUser();
    auto mex=client.editPrivilege(anotherUsername, path, filename, privilege::modify);
    messageHasCorrectOwner(mex);
    privMessage expected(msgType::changePrivileges, {username, ""}, msgOutcome::success, path+"/"+filename, anotherUsername, privilege::modify);
    EXPECT_EQ(expected, mex);
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, editPrivilegeCallsEditPrivilegeOnUser){
    setStageForLoggedUser();
    SymClientUserMock& logUser= dynamic_cast<SymClientUserMock &>(client.getLoggedUser());
    EXPECT_CALL(logUser, editPrivilege(anotherUsername, path, filename, privilege::modify));
    client.editPrivilege(anotherUsername, path, filename, privilege::modify, true);
}

TEST_F(SymClientTest, shareResourceConstructsGoodMessageAndInsertInUnanswered){
    setStageForLoggedUser();
    auto mex=client.shareResource(path, filename, newPreferences);
    messageHasCorrectOwner(mex);
    uriMessage expected(msgType::shareRes, {username, ""}, msgOutcome::success, "", "",
                        newPreferences, 0);
    messageAreAnalogous(mex, expected);
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, shareResourceCallsShareResourceOnUser){
    setStageForLoggedUser();
    SymClientUserMock& logUser= dynamic_cast<SymClientUserMock &>(client.getLoggedUser());
    EXPECT_CALL(logUser, shareResource(path, filename, newPreferences));
    client.shareResource(path, filename, newPreferences, true);
}

TEST_F(SymClientTest, renameResourceConstructsGoodMessageAndInsertInUnanswered){
    setStageForLoggedUser();
    auto mex=client.renameResource(path, filename, "newName");
    messageHasCorrectOwner(mex);
    askResMessage expected(msgType::changeResName, {username, ""}, path, filename, "newName", uri::getDefaultPrivilege(), 0);
    messageAreAnalogous(mex, expected);
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, renameResourceCallsRenameResourceOnUser){
    setStageForLoggedUser();
    SymClientUserMock& logUser= dynamic_cast<SymClientUserMock &>(client.getLoggedUser());
    EXPECT_CALL(logUser, renameResource(path, filename, "newName"));
    client.shareResource(path, filename, newPreferences, true);
}

TEST_F(SymClientTest, removeResourceConstructsGoodMessageAndInsertInUnanswered){
    setStageForLoggedUser();
    auto mex=client.removeResource(path, filename);
    messageHasCorrectOwner(mex);
    askResMessage expected(msgType::removeRes, {username, ""}, path, filename, "", uri::getDefaultPrivilege(), 0);
    messageAreAnalogous(mex, expected);
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, removeResourceCallsRemoveResourceOnUser){
    setStageForLoggedUser();
    SymClientUserMock& logUser= dynamic_cast<SymClientUserMock &>(client.getLoggedUser());
    EXPECT_CALL(logUser, removeResource(path, filename));
    client.removeResource(path, filename, true);
}

TEST_F(SymClientTest, closeSourceConstructsGoodMessageAndNotInsertInUnanswered){
    setStageForOpenedDoc();
    EXPECT_CALL(docSentByServer, close(userReceived));
    auto mex=client.closeSource(docSentByServer.getId());
    messageHasCorrectOwner(mex);
    updateDocMessage expected(msgType::closeRes, {username, ""}, docSentByServer.getId());
    messageAreAnalogous(mex, expected);
    EXPECT_FALSE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, showDirCallsShowDirOnUserRecursive){
    setStageForLoggedUser();
    SymClientUserMock& logUser= dynamic_cast<SymClientUserMock &>(client.getLoggedUser());
    EXPECT_CALL(logUser, showDir(true));
    client.showDir(true);
}

TEST_F(SymClientTest, showDirCallsShowDirOnUserNotRecursive){
    setStageForLoggedUser();
    SymClientUserMock& logUser= dynamic_cast<SymClientUserMock &>(client.getLoggedUser());
    EXPECT_CALL(logUser, showDir(false));
    client.showDir(false);
}

TEST_F(SymClientTest, removeUserConstructsGoodMessageAndInsertInUnanswered){
    setStageForLoggedUser();
    auto mex=client.removeUser();
    messageHasCorrectOwner(mex);
    clientMessage expected(msgType::removeUser, {username, ""});
    messageAreAnalogous(mex, expected);
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
    EXPECT_NE(userReceived, client.getLoggedUser());
}
//FIXME: Perchè questo test controlla che il messaggio di logout venga inserito tra quelli non risposti?
// Risposta: per uniformità di protocollo, anche per poter dire che il login è andato a buon fine.
// Dobbiamo prevedere anche i casi in cui ad un certo punto si interrompa la connessione proprio prima del
// logout: lato client non è problematico ma è giusto segnalarlo, e questo puoi farlo solo se sai che ti
// deve arrivare un messaggio.
TEST_F(SymClientTest, logoutConstructsGoodMessageAndInsertInUnanswered){
    setStageForLoggedUser();
    auto mex=client.logout();
    messageHasCorrectOwner(mex);
    clientMessage expected(msgType::logout, {username, ""});
    messageAreAnalogous(mex, expected);
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
    EXPECT_NE(userReceived, client.getLoggedUser());
}

TEST_F(SymClientTest, mapSiteIdToUserConstructsGoodMessageAndInsertInUnanswered){
    auto mex=client.mapSiteIdToUser(docSentByServer);
    messageHasCorrectOwner(mex);
    updateDocMessage expected(msgType::mapChangesToUser, {username, ""}, docSentByServer.getId());
    messageAreAnalogous(mex, expected);
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, setUserColorsAssignesDifferentColorToUsers){
    //correctness of mapping has already been verified on SymServer class, here we don't care about the wrong mapping
    std::map<int, user> sampleMapping({{0, userReceived},{1, userReceived}});
    client.setUserColors(sampleMapping);
    EXPECT_TRUE(everyUserHasDifferentColor());
}

TEST_F(SymClientTest, addActiveUserCallsAccessOnDoc){
    setStageForOpenedDoc();
    SymClientUserMock anotherUser(anotherUsername, pwd, "noempty", "", 0, nullptr);
    EXPECT_CALL(docSentByServer, access(anotherUser, privilege::readOnly)).WillOnce(::testing::ReturnRef(docSentByServer));
    client.addActiveUser(docSentByServer.getId(), anotherUser);
}

TEST_F(SymClientTest, removeActiveUserCallsCloseOnDoc){
    setStageForOpenedDoc();
    EXPECT_CALL(docSentByServer, close(userReceived));
    client.addActiveUser(docSentByServer.getId(), userReceived);
}

TEST_F(SymClientTest, editUserConstructsGoodMessageAndInsertInUnanswered){
    setStageForLoggedUser();
    user newData; newData.setNickname("newNickname");
    auto mex=client.editUser(newData);
    messageHasCorrectOwner(mex);
    userDataMessage expected(msgType::changeUserData, {username, ""}, msgOutcome::success, newData);
    messageAreAnalogous(mex, expected);
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, editUserCallsSetNewDataOnLoggedUser){
    setStageForLoggedUser();
    SymClientUserMock& logUser= dynamic_cast<SymClientUserMock &>(client.getLoggedUser());
    user newData; newData.setNickname("newNickname");
    EXPECT_CALL(logUser, editUser(newData, true));
    client.removeResource(path, filename, true);
}

TEST_F(SymClientTest, retrieveRelatedMessageReturnCorrectMessage){
    auto mex=client.logIn(username, pwd);
    ASSERT_PRED_FORMAT1(messagesHaveDifferentIds, mex.getMsgId());
    ASSERT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
    serverMessage responseFromServer(msgType::login, msgOutcome::success, mex.getMsgId());
    auto retrieved=client.retrieveRelatedMessage(responseFromServer);
    messageAreAnalogous(mex, *retrieved);
}

TEST_F(SymClientTest, retrieveRelatedMessageThrowsOnMissingMessage){
    //suppose that the server send a message not related to any sent by the server,
    // the list of unanswered messages is empty at the moment
    serverMessage responseFromServer(msgType::login, msgOutcome::success);
    EXPECT_THROW(client.retrieveRelatedMessage(responseFromServer), SymClientException);
}