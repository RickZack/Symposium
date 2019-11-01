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


struct SymClientAccesser: public SymClient{
    user& getLoggedUser(){
        return loggedUser;
    }
    std::map<std::pair<int, int>, std::pair<user, MyColor>>& getUserColors(){
        return userColors;
    };
    bool thereIsUnansweredMex(int msgId){
        while(!unanswered.empty()){
            auto mex=unanswered.front();
            if(mex.getMsgId()==msgId)
                return true;
            unanswered.pop();
        }
    }
    void setUser(user& logged){
        loggedUser=logged;
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
};

struct SymClientUserMock: public user{
    SymClientUserMock(const std::string &username, const std::string &pwd, const std::string &nickname,
                      const std::string &iconPath, int siteId, const std::shared_ptr<directory> &home) :
                      user(username, pwd, nickname, iconPath, siteId, home) {}
    MOCK_CONST_METHOD3(openFile, document&(const std::string& path, const std::string& filename, privilege accessMode));
};

struct SymClientFileMock: public file{
    SymClientFileMock(const std::string &name, const std::string &realPath) : file(name, realPath) {};
    MOCK_CONST_METHOD0(getDoc, document&());
};


struct SymClientTest : ::testing::Test{
    SymClientUserMock userReceived;
    SymClientAccesser client;
    static const std::string username;
    static const std::string pwd;
    static const std::string nickname;
    static const std::string path;
    static const std::string filename;
    static constexpr privilege aPrivilege=privilege::modify;
    SymClientTest():userReceived(username, pwd, nickname, "", 0, nullptr){};
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
        client.setUser(userReceived);
    }
    void messageHasCorrectOwner(const clientMessage& mex){
        std::pair<std::string, std::string> credentials(username, pwd);
        EXPECT_EQ(credentials, mex.getActionOwner());
    }
    void correctInsertionOfFileAndDocumentInLists(int idOfDocActive, document *presentInUserFile, int idOfFileActive) {
        std::pair<bool, document*> docActive=client.docIsInActive(idOfDocActive);
        ASSERT_TRUE(docActive.first);
        EXPECT_TRUE(docActive.second==presentInUserFile);
        EXPECT_TRUE(client.fileIsInActive(idOfFileActive));
    }
};
const std::string SymClientTest::username="username";
const std::string SymClientTest::pwd="123abc!!";
const std::string SymClientTest::nickname="nickname";
const std::string SymClientTest::path="./dir1/dir2";
const std::string SymClientTest::filename="file1";

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

TEST_F(SymClientTest, signUpConstructsGoodMessageAndInsertInUnanswered){
    auto mex= client.signUp(username, pwd, nickname);
    user expected(username, pwd, nickname, "", 0, nullptr);
    EXPECT_EQ(expected, mex.getNewUser());
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()));
}

TEST_F(SymClientTest, signUpAssignesLoggedUser){
    //just imagine that the server has answered with msgOutcome::success to client's signUpMessage, the response contain the
    //complete user data
    client.signUp(userReceived);
    EXPECT_EQ(client.getLoggedUser(), userReceived);
}

TEST_F(SymClientTest, loginConstructsGoodMessageAndInsertInUnanswered){
    auto mex=client.logIn(username, pwd);
    messageHasCorrectOwner(mex);
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()));
}

TEST_F(SymClientTest, loginAssignesLoggedUser){
    //just imagine that the server has answered with msgOutcome::success to client's clientMessage, the response contain the
    //complete user data
    client.logIn(userReceived);
    EXPECT_EQ(client.getLoggedUser(), userReceived);
}

TEST_F(SymClientTest, openSourceConstructsGoodMessageAndInsertInUnanswered){
    setStageForLoggedUser();
    auto mex=client.openSource(path, filename, privilege::readOnly);
    messageHasCorrectOwner(mex);
    askResMessage expected(msgType::openRes, {username, pwd}, path, filename);
    EXPECT_EQ(expected, mex);
    EXPECT_TRUE(client.thereIsUnansweredMex(mex.getMsgId()));
}

TEST_F(SymClientTest, openSourceOpensDocAndPutInActive)
{
    setStageForLoggedUser();
    //just imagine that the server has answered with msgOutcome::success to client's askResMessage, the response contain the
    //resource asked. sendResMessage has already been tested to call openSource on client
    std::shared_ptr<SymClientFileMock> f(new SymClientFileMock(filename, "realPath"));
    document docInUserFilesystem(0), docSentByServer(120);
    //the data use to call openFile must be taken from the relative askResMessage in unanswered
    EXPECT_CALL(userReceived, openFile(path, filename, aPrivilege)).WillOnce(::testing::ReturnRef(docInUserFilesystem));
    EXPECT_CALL(*f, getDoc()).WillOnce(::testing::ReturnRef(docSentByServer));
    client.openSource(f);
    correctInsertionOfFileAndDocumentInLists(120, &docInUserFilesystem, f->getId());
    //tests that the document returned by openFile() has now the same content of the document sent by the server
    EXPECT_EQ(docInUserFilesystem, docSentByServer);
}