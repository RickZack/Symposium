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

using namespace Symposium;


struct SymClientUserMock: public user{
    SymClientUserMock(const std::string &username, const std::string &pwd, const std::string &nickname,
                      const std::string &iconPath, int siteId, const std::shared_ptr<directory> &home) :
            user(username, pwd, nickname, iconPath, siteId, home) {}
    MOCK_CONST_METHOD3(openFile, std::shared_ptr<file>(const std::string& path, const std::string& filename, privilege accessMode));
    MOCK_CONST_METHOD3(accessFile, std::pair<int, std::shared_ptr<file>>(const std::string &resId, const std::string &path, const std::string &fileName));
    MOCK_CONST_METHOD3(newFile, std::shared_ptr<file>(const std::string &filename, const std::string &path, uint_positive_cnt::type id));
    MOCK_CONST_METHOD3(newDirectory, std::shared_ptr<directory>(const std::string &filename, const std::string &path, uint_positive_cnt::type id));
    MOCK_CONST_METHOD4(editPrivilege, privilege(const std::string &targetUser, const std::string &resPath, const std::string &resName,
            privilege newPrivilege));
    MOCK_CONST_METHOD3(shareResource, std::shared_ptr<filesystem>(const std::string &resPath, const std::string &resName, const uri &newPrefs));
    MOCK_CONST_METHOD3(renameResource, std::shared_ptr<filesystem>(const std::string &resPath, const std::string &resName, const std::string &newName));
    MOCK_CONST_METHOD2(removeResource, std::shared_ptr<filesystem>(const std::string &path, const std::string &name));
    MOCK_CONST_METHOD1(showDir, std::string(bool rec));
    MOCK_METHOD1(setNewData, void(const user&));
};

struct SymClientAccesser: public SymClient{
    SymClientUserMock *userMock;

    SymClientAccesser(){}

    user& getLoggedUser() const override {
        if(userMock== nullptr)
            throw std::exception();
        return *userMock;
    }

    user& getLoggedUserNotMocked() {
        return loggedUser;
    }

    std::map<std::pair<uint_positive_cnt::type, uint_positive_cnt::type>, std::pair<user, Color>>& getUserColors(){
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

    std::pair<bool, document* > docIsInActive(int id){
        std::pair<bool, document*> result(false, nullptr);
        for(auto entry:activeDoc)
            if(entry.first->getId()==id) {
                result.first = true;
                result.second = entry.first;
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
    SymClientFileMock(const std::string &name, const std::string &realPath) : file(name, 0) {};
    MOCK_CONST_METHOD0(getDoc, document&());
    MOCK_METHOD2(access, document&(const user& u, privilege requested));
    MOCK_METHOD1(getUserPrivilege, privilege(const std::string&));
};

struct SymClientDirMock: public directory{
    SymClientDirMock(const std::string &name) : directory(name) {};
    MOCK_METHOD5(addLink, std::shared_ptr<class symlink>(const std::string&, const std::string&, const std::string&, const std::string&, uint_positive_cnt::type));
};

struct SymClientDocMock: public document{
    SymClientDocMock(int id):document(id) {};
    MOCK_METHOD2(remoteInsert, std::pair<unsigned int, unsigned int>(uint_positive_cnt::type siteId, const symbol& toInsert));
    MOCK_METHOD2(remoteRemove, std::pair<unsigned int, unsigned int>(uint_positive_cnt::type siteId, const symbol& toRemove));
    MOCK_METHOD1(close, void(const user& noLongerActive));
    MOCK_METHOD2(access, document&(const user&, privilege));
    MOCK_METHOD2(localInsert, symbol(const std::pair<unsigned, unsigned>& index, symbol &toInsert));
    MOCK_METHOD2(localRemove, symbol(const std::pair<unsigned, unsigned>& index, uint_positive_cnt::type siteId));
    MOCK_METHOD3(updateCursorPos, void(unsigned int, unsigned int, unsigned int));
    MOCK_METHOD1(verifySymbol, std::pair<unsigned int, unsigned int>(const symbol &sym));
};

struct SymClientTest : ::testing::Test{
    SymClientUserMock userReceived;
    static const std::string anotherUsername;
    SymClientAccesser client;
    static const std::string iconPath;
    static const std::string username;
    static const std::string pwd;
    static const std::string nickname;
    static const std::string resPath;
    static const std::string resId;
    static constexpr privilege aPrivilege=privilege::modify;
    static const std::string destPath;
    static uri newPreferences;
    std::shared_ptr<SymClientFileMock> fileSentByServer;
    std::shared_ptr<SymClientFileMock> fileInUserFilesystem;
    std::shared_ptr<SymClientDirMock> dirSentByServer;
    SymClientDocMock docInUserFilesystem, docSentByServer;
    static const std::pair<unsigned, unsigned> indexes;
    SymClientTest(): userReceived(username, pwd, nickname, iconPath, 0, std::shared_ptr<SymClientDirMock>(new SymClientDirMock(username))),
                     fileSentByServer(new SymClientFileMock(resId, "./dir1/dir2")),
                     fileInUserFilesystem(new SymClientFileMock(resId, "./dir1/dir2")),
                     docInUserFilesystem(120), docSentByServer(120),
                     dirSentByServer(new SymClientDirMock(resId)){};
    bool everyUserHasDifferentColor(){
        auto mapped=client.getUserColors();
        std::set<std::tuple<int, int, Color>> colors; //{siteId, documentId, color}
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
        EXPECT_TRUE(*docActive.second==*presentInUserFile);
        EXPECT_TRUE(client.fileIsInActive(idOfFileActive));
    }
    void setStageForOpenedDoc(){
        setStageForLoggedUser();
        EXPECT_CALL(userReceived, openFile(resPath, resId, aPrivilege)).WillOnce(::testing::Return(fileInUserFilesystem));
        EXPECT_CALL(*fileInUserFilesystem, access(userReceived, uri::getDefaultPrivilege())).WillOnce(::testing::ReturnRef(docInUserFilesystem));
        client.openSource(resPath, resId, fileSentByServer, uri::getDefaultPrivilege());
        ASSERT_NO_FATAL_FAILURE(correctInsertionOfFileAndDocumentInLists(docSentByServer.getId(), &docInUserFilesystem, fileInUserFilesystem->getId()));
        //tests that the document returned by openFile() has now the same content of the document sent by the server
        EXPECT_EQ(docInUserFilesystem, docSentByServer);
    }

    bool userHasAssignedColor(uint_positive_cnt::type siteId, uint_positive_cnt::type thisResId){
        auto colorMap=client.getUserColors();
        return colorMap.find({siteId, thisResId}) != colorMap.end();
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
    template<class T>
    void messageAreAnalogous(T& toTest, const T &expected){
        int m2PrevSiteId=toTest.getMsgId();
        //change m2's siteId to make the comparison fail only
        //if there are members different from msgId that are not equal
        messageIntruder::setId(toTest, expected.getMsgId());
        EXPECT_EQ(toTest, expected);
        //reset the previous msgId of m2
        messageIntruder::setId(toTest, m2PrevSiteId);
    }
};
const std::string SymClientTest::iconPath="./icons/icon1.jpg";
const std::string SymClientTest::username="username";
const std::string SymClientTest::pwd="AP@ssw0rd!";
const std::string SymClientTest::nickname="nickname";
const std::string SymClientTest::resPath="./1/2";
const std::string SymClientTest::resId="3";
const std::string SymClientTest::destPath="./4/5";
const std::string SymClientTest::anotherUsername="anotherUsername";
uri SymClientTest::newPreferences(uriPolicy::activeAlways);
const std::pair<unsigned, unsigned> SymClientTest::indexes={0,0};

TEST_F(SymClientTest, setLoggedUserAssignesUserReceivedToClient){
    client.setLoggedUser(userReceived);
    EXPECT_EQ(userReceived, client.getLoggedUserNotMocked());
}

TEST_F(SymClientTest, signUpConstructsGoodMessageAndInsertInUnanswered){
    auto mex= client.signUp(username, pwd, nickname, resPath);
    user expected(username, pwd, nickname, iconPath, 0, nullptr);
    EXPECT_EQ(expected, mex.getNewUser());
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}
TEST_F(SymClientTest, signUpAssignesLoggedUser){
    auto mex= client.signUp(username, pwd, nickname, resPath);
    //just imagine that the server has answered with msgOutcome::success to client's signUpMessage, the response contain the
    //complete user data
    client.signUp(userReceived);
    EXPECT_EQ(client.getLoggedUser(), userReceived);
}

TEST_F(SymClientTest, loginConstructsGoodMessageAndInsertInUnanswered){
    auto mex=client.logIn(username, pwd);
    messageHasCorrectOwnerforLogin(mex);
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, loginAssignesLoggedUser){
    auto mex=client.logIn(username, pwd);
    //just imagine that the server has answered with msgOutcome::success to client's clientMessage, the response contain the
    //complete user data
    client.logIn(userReceived);
    EXPECT_EQ(client.getLoggedUser(), userReceived);
}

TEST_F(SymClientTest, openSourceConstructsGoodMessageAndInsertInUnanswered){
    setStageForLoggedUser();
    auto mex=client.openSource(resPath, resId, privilege::readOnly);
    messageHasCorrectOwner(mex);
    askResMessage expected(msgType::openRes, {username, ""}, resPath, resId, "", uri::getDefaultPrivilege(), 0);
    messageAreAnalogous(mex, expected);
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, openSourceOpensDocAndPutInActive)
{
    setStageForLoggedUser();
    auto mex=client.openSource(resPath, resId, privilege::readOnly);
    //just imagine that the server has answered with msgOutcome::success to client's askResMessage, the response contain the
    //resource asked. sendResMessage has already been tested to call openSource on client
    //the data use to call openFile must be taken from the relative askResMessage in unanswered
    EXPECT_CALL(userReceived, openFile(resPath, resId, privilege::readOnly)).WillOnce(::testing::Return(fileInUserFilesystem));
    EXPECT_CALL(*fileInUserFilesystem, access(userReceived, privilege::readOnly)).WillOnce(::testing::ReturnRef(docSentByServer));
    client.openSource(resPath, resId, fileSentByServer, privilege::readOnly);
    ASSERT_NO_FATAL_FAILURE(correctInsertionOfFileAndDocumentInLists(docSentByServer.getId(), &docInUserFilesystem, fileInUserFilesystem->getId()));
    //tests that the document returned by openFile() has now the same content of the document sent by the server
    EXPECT_EQ(docInUserFilesystem, docSentByServer);
}

TEST_F(SymClientTest, openSourceGenerateColorForUser)
{
    setStageForLoggedUser();
    auto mex=client.openSource(resPath, resId, privilege::readOnly);

    EXPECT_CALL(userReceived, openFile(resPath, resId, privilege::readOnly)).WillOnce(::testing::Return(fileInUserFilesystem));
    EXPECT_CALL(*fileInUserFilesystem, access(userReceived, privilege::readOnly)).WillOnce(::testing::ReturnRef(docSentByServer));
    client.openSource(resPath, resId, fileSentByServer, privilege::readOnly);

    EXPECT_TRUE(userHasAssignedColor(userReceived.getSiteId(), docSentByServer.getId()));
}

TEST_F(SymClientTest, openNewSourceConstructsGoodMessageAndInsertInUnanswered){
    setStageForLoggedUser();
    auto mex=client.openNewSource(resPath + "/" + resId, privilege::readOnly, destPath, resId);
    messageHasCorrectOwner(mex);
    askResMessage expected(msgType::openNewRes, {username, ""}, destPath, resId, resPath + "/" + resId, uri::getDefaultPrivilege(), 0);
    messageAreAnalogous(mex, expected);
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, openNewSourceOpensDocAndPutInActive)
{
    setStageForLoggedUser();
    auto mex=client.openNewSource(resPath + "/" + resId, privilege::readOnly, "./", "sym");
    //just imagine that the server has answered with msgOutcome::success to client's askResMessage, the response contain the
    //resource asked. sendResMessage has already been tested to call openNewSource on client
    //the data use to call accessFile must be taken from the relative askResMessage in unanswered
    EXPECT_CALL(*static_cast<SymClientDirMock*>(client.getLoggedUser().getHome().get()), addLink("./", "sym", resPath, resId, fileSentByServer->getId()));
    EXPECT_CALL(*fileSentByServer, access(client.getLoggedUser(), privilege::readOnly)).WillOnce(::testing::ReturnRef(docSentByServer));
    client.openNewSource(resPath + "/" + resId, privilege::readOnly, "./", "sym", fileSentByServer->getId(), fileSentByServer);
    ASSERT_NO_FATAL_FAILURE(correctInsertionOfFileAndDocumentInLists(docSentByServer.getId(), &docSentByServer, fileSentByServer->getId()));
}

TEST_F(SymClientTest, openNewSourceGenerateColorForUser)
{
    setStageForLoggedUser();
    auto mex=client.openNewSource(resPath + "/" + resId, privilege::readOnly, "./", "sym");

    EXPECT_CALL(*static_cast<SymClientDirMock*>(client.getLoggedUser().getHome().get()), addLink("./", "sym", resPath, resId, fileSentByServer->getId()));
    EXPECT_CALL(*fileSentByServer, access(client.getLoggedUser(), privilege::readOnly)).WillOnce(::testing::ReturnRef(docSentByServer));
    client.openNewSource(resPath + "/" + resId, privilege::readOnly, "./", "sym", fileSentByServer->getId(), fileSentByServer);

    EXPECT_TRUE(userHasAssignedColor(userReceived.getSiteId(), docSentByServer.getId()));
}

TEST_F(SymClientTest, createNewSourceConstructsGoodMessageAndInsertInUnanswered){
    setStageForLoggedUser();
    auto mex=client.createNewSource(resPath, resId);
    messageHasCorrectOwner(mex);
    askResMessage expected(msgType::createRes, {username, ""}, resPath, resId, "", uri::getDefaultPrivilege(), 0);
    messageAreAnalogous(mex, expected);
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, createNewSourceOpensDocAndPutInActive)
{
    setStageForLoggedUser();
    auto cmex=client.createNewSource(resPath, resId);
    //just imagine that the server has answered with msgOutcome::success to client's askResMessage, the response contain the
    //resource asked. sendResMessage has already been tested to call createNewSource on client
    //the data used to call newFile must be taken from the relative askResMessage in unanswered (cmex)
    EXPECT_CALL(userReceived, newFile(resId, resPath, fileSentByServer->getId())).WillOnce(::testing::Return(fileSentByServer));
    EXPECT_CALL(*fileSentByServer, access(userReceived, privilege::owner)).WillOnce(::testing::ReturnRef(docSentByServer));
    client.createNewSource(resPath, resId, fileSentByServer->getId(), fileSentByServer);
    ASSERT_NO_FATAL_FAILURE(correctInsertionOfFileAndDocumentInLists(docSentByServer.getId(), &docSentByServer, fileSentByServer->getId()));
}

TEST_F(SymClientTest, createNewSourceGenerateColorForUser)
{
    setStageForLoggedUser();
    auto cmex=client.createNewSource(resPath, resId);


    EXPECT_CALL(userReceived, newFile(resId, resPath, fileSentByServer->getId())).WillOnce(::testing::Return(fileSentByServer));
    EXPECT_CALL(*fileSentByServer, access(userReceived, privilege::owner)).WillOnce(::testing::ReturnRef(docSentByServer));
    client.createNewSource(resPath, resId, fileSentByServer->getId(), fileSentByServer);

    EXPECT_TRUE(userHasAssignedColor(userReceived.getSiteId(), docSentByServer.getId()));
}

TEST_F(SymClientTest, createNewDirConstructsGoodMessageAndInsertInUnanswered){
    setStageForLoggedUser();
    auto mex=client.createNewDir(resPath, resId);
    messageHasCorrectOwner(mex);
    askResMessage expected(msgType::createNewDir, {username, ""}, resPath, resId, "", uri::getDefaultPrivilege(), 0);
    messageAreAnalogous(mex, expected);
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, createNewDirOpensDocAndPutInActive)
{
    setStageForLoggedUser();
    auto mex=client.createNewDir(resPath, resId);
    //just imagine that the server has answered with msgOutcome::success to client's askResMessage, the response contain the
    //resource asked. sendResMessage has already been tested to call createNewSource on client
    //the data use to call newDirectory must be taken from the relative askResMessage in unanswered
    EXPECT_CALL(userReceived, newDirectory(resId, resPath, dirSentByServer->getId())).WillOnce(::testing::Return(dirSentByServer));
    client.createNewDir(resPath, resId, dirSentByServer->getId());
}

TEST_F(SymClientTest, localInsertConstructsGoodMessageAndInsertInUnanswered){
    setStageForOpenedDoc();
    symbol toInsert('a', userReceived.getSiteId(), 0, {}, false);
    symbol inserted('a', userReceived.getSiteId(), 0, {1}, false);
    EXPECT_CALL(docInUserFilesystem, localInsert(indexes, toInsert)).WillOnce(::testing::Return(inserted));
    auto mex= client.localInsert(docInUserFilesystem.getId(), toInsert, indexes);
    messageHasCorrectOwner(mex);
    symbolMessage expected(msgType::insertSymbol, {username, ""}, msgOutcome::success, userReceived.getSiteId(), docInUserFilesystem.getId(), inserted);
    messageAreAnalogous(mex, expected);

    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, localRemoveConstructsGoodMessageAndInsertInUnanswered){
    setStageForOpenedDoc();
    symbol removed('a', userReceived.getSiteId(), 0, {1}, false);
    EXPECT_CALL(docInUserFilesystem, localRemove(indexes, userReceived.getSiteId())).WillOnce(::testing::Return(removed));
    auto mex=client.localRemove(docInUserFilesystem.getId(), indexes);

    messageHasCorrectOwner(mex);
    symbolMessage expected(msgType::removeSymbol, {username, ""}, msgOutcome::success, userReceived.getSiteId(), docInUserFilesystem.getId(), removed);
    messageAreAnalogous(mex, expected);

    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, remoteInsertCallsremoteInsertOnRightDoc){
    setStageForOpenedDoc();
    symbol arrived('a', 1, 1, {}, true);
    //siteId of the user performing the action is contained in the symbolMessage received by the client
    //resouceId of the document to insert the symbol into is contained in the symbolMessage received by the client
    EXPECT_CALL(docInUserFilesystem, remoteInsert(0, arrived));
    client.remoteInsert(0, docInUserFilesystem.getId(), arrived);
}

TEST_F(SymClientTest, remoteRemoveCallsremoteRemoveOnRightDoc){
    setStageForOpenedDoc();
    symbol arrived('a', 1, 1, {}, true);
    //siteId of the user performing the action is contained in the symbolMessage received by the client
    //resouceId of the document to insert the symbol into is contained in the symbolMessage received by the client
    EXPECT_CALL(docInUserFilesystem, remoteRemove(0, arrived));
    client.remoteRemove(0, docInUserFilesystem.getId(), arrived);
}

TEST_F(SymClientTest, verifySymbolCallsVerifySymOnDoc){
    setStageForOpenedDoc();
    symbol arrived('a', 1, 1, {}, true);
    EXPECT_CALL(docInUserFilesystem, verifySymbol(arrived));
    client.verifySymbol(docInUserFilesystem.getId(), arrived);
}

TEST_F(SymClientTest, editPrivilegeConstructsGoodMessageAndInsertInUnanswered){
    setStageForLoggedUser();
    auto mex=client.editPrivilege(anotherUsername, resPath, resId, privilege::modify);
    messageHasCorrectOwner(mex);
    privMessage expected(msgType::changePrivileges, {username, ""}, msgOutcome::success, resPath + "/" + resId, anotherUsername, privilege::modify);
    messageAreAnalogous(mex, expected);
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, editPrivilegeCallsEditPrivilegeOnUser){
    setStageForLoggedUser();
    SymClientUserMock& logUser= dynamic_cast<SymClientUserMock &>(client.getLoggedUser());
    EXPECT_CALL(logUser, editPrivilege(anotherUsername, resPath, resId, privilege::modify));
    client.editPrivilege(anotherUsername, resPath, resId, privilege::modify, true);
}

TEST_F(SymClientTest, shareResourceConstructsGoodMessageAndInsertInUnanswered){
    setStageForLoggedUser();
    auto mex=client.shareResource(resPath, resId, newPreferences);
    messageHasCorrectOwner(mex);
    uriMessage expected(msgType::shareRes, {username, ""}, msgOutcome::success, resPath, resId,
                        newPreferences);
    messageAreAnalogous(mex, expected);
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, shareResourceCallsShareResourceOnUser){
    setStageForLoggedUser();
    SymClientUserMock& logUser= dynamic_cast<SymClientUserMock &>(client.getLoggedUser());
    EXPECT_CALL(logUser, shareResource(resPath, resId, newPreferences));
    client.shareResource(std::string(), resPath, resId, newPreferences, true);
}

TEST_F(SymClientTest, renameResourceConstructsGoodMessageAndInsertInUnanswered){
    setStageForLoggedUser();
    auto mex=client.renameResource(resPath, resId, "newName");
    messageHasCorrectOwner(mex);
    askResMessage expected(msgType::changeResName, {username, ""}, resPath, resId, "newName", uri::getDefaultPrivilege(), 0);
    messageAreAnalogous(mex, expected);
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, renameResourceCallsRenameResourceOnUser){
    setStageForLoggedUser();
    SymClientUserMock& logUser= dynamic_cast<SymClientUserMock &>(client.getLoggedUser());
    EXPECT_CALL(logUser, renameResource(resPath, resId, "newName"));
    client.renameResource(resPath, resId, "newName", true);
}

TEST_F(SymClientTest, removeResourceConstructsGoodMessageAndInsertInUnanswered){
    setStageForLoggedUser();
    auto mex=client.removeResource(resPath, resId);
    messageHasCorrectOwner(mex);
    askResMessage expected(msgType::removeRes, {username, ""}, resPath, resId, "", uri::getDefaultPrivilege(), 0);
    messageAreAnalogous(mex, expected);
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, removeResourceCallsRemoveResourceOnUser){
    setStageForLoggedUser();
    SymClientUserMock& logUser= dynamic_cast<SymClientUserMock &>(client.getLoggedUser());
    EXPECT_CALL(logUser, removeResource(resPath, resId));
    client.removeResource(resPath, resId, true);
}

TEST_F(SymClientTest, closeSourceConstructsGoodMessageAndInsertInUnanswered){
    setStageForOpenedDoc();
    EXPECT_CALL(docInUserFilesystem, close(userReceived));
    auto mex=client.closeSource(docInUserFilesystem.getId());
    messageHasCorrectOwner(mex);
    updateDocMessage expected(msgType::closeRes, {username, ""}, docInUserFilesystem.getId());
    messageAreAnalogous(mex, expected);
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
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
    auto mex= client.removeUser(pwd);
    messageHasCorrectOwnerforLogin(mex);
    clientMessage expected(msgType::removeUser, {username, pwd});
    messageAreAnalogous(mex, expected);
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, removeUserCleansLoggedUserAndMapping){
    setStageForLoggedUser();
    client.removeUser(true);

    EXPECT_NE(userReceived, client.getLoggedUser());
    EXPECT_TRUE(client.getUserColors().empty());
}

TEST_F(SymClientTest, logoutConstructsGoodMessageAndInsertInUnanswered){
    setStageForLoggedUser();
    auto mex=client.logout();
    messageHasCorrectOwner(mex);
    clientMessage expected(msgType::logout, {username, ""});
    messageAreAnalogous(mex, expected);
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, logoutCleansMapping){
    setStageForLoggedUser();
    client.logout(true);

    EXPECT_TRUE(client.getUserColors().empty());
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
    setStageForOpenedDoc();
    std::map<uint_positive_cnt::type, user> sampleMapping({{0, userReceived},{1, userReceived}});
    client.setUserColors(docInUserFilesystem.getId(), sampleMapping);
    EXPECT_TRUE(everyUserHasDifferentColor());
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
    EXPECT_CALL(logUser, setNewData(newData));
    client.editUser(newData, true);
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

TEST_F(SymClientTest, updateCursorPosConstructsGoodMessageAndInsertInUnanswered){
    setStageForOpenedDoc();
    srand(time(NULL)); unsigned int row=rand()%1000, col=rand()%1000;
    auto mex=client.updateCursorPos(docSentByServer.getId(), row, col);
    messageHasCorrectOwner(mex);

    cursorMessage expected(msgType::updateCursor, {username, ""}, msgOutcome::success, userReceived.getSiteId(), docSentByServer.getId(), row, col, mex.getMsgId());
    messageAreAnalogous(mex, expected);
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()).first);
}

TEST_F(SymClientTest, updateCursorPosCallsUpdateCursorPosOnDoc){
    setStageForOpenedDoc();
    srand(time(NULL)); unsigned int row=rand()%1000, col=rand()%1000;
    int anotherUserSiteId=userReceived.getSiteId()+1;

    EXPECT_CALL(docInUserFilesystem, updateCursorPos(anotherUserSiteId, row, col));
    client.updateCursorPos(anotherUserSiteId, docInUserFilesystem.getId(), row, col);
}