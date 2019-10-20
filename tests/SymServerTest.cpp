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

struct SymServerdirMock : public directory{
    SymServerdirMock(const std::string &name) : directory(name) {};
    MOCK_METHOD1(addDirectory, std::shared_ptr<directory>(const std::string &name));
    MOCK_METHOD2(getFile, std::shared_ptr<file>(const std::string&, const std::string&));
    MOCK_METHOD2(getDir, std::shared_ptr<directory>(const std::string &path, const std::string &name));
};

/*
 * Used to access read-only the members of SymServer without adding getters in production class
 */
struct SymServerAccesser: public SymServer{
    std::unordered_map<std::string, user> getRegistered(){
        return registered;
    }
    bool userIsActive(const std::string& username, const user& toCheck){
        auto entry=active.find(username);
        return entry!=active.end() &&
                *entry->second==toCheck;
    }
    void setRoot(std::shared_ptr<directory> newRoot){
        rootDir=newRoot;
    }
    bool userIsWorkingOnDocument(const user& targetUser, const document& targetDoc, privilege privilegeGranted){
        if(workingDoc.find(targetUser.getUsername())==workingDoc.end()) return false;
        std::forward_list<std::pair<privilege, document*>> docQueue=workingDoc[targetUser.getUsername()];
        for(auto element:docQueue){
            auto priv=element.first;
            auto doc=element.second;
            if (doc->getId()==targetDoc.getId() && priv==privilegeGranted)
                return true;
        }
        return false;
    }
};

struct SymServerTestUserFunctionality : testing::Test {
    user newUser, alreadyPresent, newDifferentUser;
    SymServerAccesser server;
    static const std::string validIconPath;
    static const std::string newUserUsername, newUserPwd, wrongPwd;
    std::shared_ptr<::testing::NiceMock<SymServerdirMock>> fakeDir;
    std::shared_ptr<::testing::NiceMock<SymServerdirMock>> userDir;

    SymServerTestUserFunctionality():
    fakeDir(new ::testing::NiceMock<SymServerdirMock>("/")),
    userDir(new ::testing::NiceMock<SymServerdirMock>(newUserUsername)),
    newUser(newUserUsername, newUserPwd, "m@ario", validIconPath, 0, nullptr),
    alreadyPresent(newUser),
    newDifferentUser("lucio", "a123@bty!!", "lupoLucio", validIconPath, 0, nullptr),
    server(){
        server.setRoot(fakeDir);
    };
    bool userAlreadyPresent(user& toCheck){
        auto users=server.getRegistered();
        return users.find(toCheck.getUsername())!=users.end();
    }

    ~SymServerTestUserFunctionality() = default;
};
const std::string SymServerTestUserFunctionality::validIconPath="./userIcons/test.jpg";
const std::string SymServerTestUserFunctionality::newUserUsername="mario";
const std::string SymServerTestUserFunctionality::newUserPwd="a123@bty!!";
const std::string SymServerTestUserFunctionality::wrongPwd="wrong";

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

TEST_F(SymServerTestUserFunctionality, addUserAssignesHomeToUser){
    EXPECT_CALL(*fakeDir, addDirectory(newUserUsername)).WillOnce(::testing::Return(userDir));
    auto userInserted=server.addUser(newUser);
    EXPECT_TRUE(userInserted.getHome()==userDir);
}

TEST_F(SymServerTestUserFunctionality, addUserGivesDifferentSiteId){
    auto newUserComplete=server.addUser(newUser);
    auto newDifferentUserComplete=server.addUser(newDifferentUser);
    EXPECT_NE(newUserComplete.getSiteId(), newDifferentUserComplete.getSiteId());
}

TEST_F(SymServerTestUserFunctionality, loginOfRegisteredUser){
    server.addUser(newUser);
    auto logged=server.login(newUserUsername, newUserPwd);
    EXPECT_TRUE(server.userIsActive(newUserUsername, logged));
}

TEST_F(SymServerTestUserFunctionality, loginOfRegisteredUserWithWrongPwd){
    server.addUser(newUser);
    user logged;
    EXPECT_THROW(logged=server.login(newUserUsername, wrongPwd), SymServerException);
    EXPECT_FALSE(server.userIsActive(newUserUsername, logged));
}

TEST_F(SymServerTestUserFunctionality, loginOfAlreadyLoggedUser){
    server.addUser(newUser);
    server.login(newUserUsername, newUserPwd);
    EXPECT_THROW(server.login(newUserUsername, newUserPwd), SymServerException);
}

TEST_F(SymServerTestUserFunctionality, loginOfUnregisteredUser){
    std::pair<std::string, std::string> userCredentials={newUserUsername, newUserPwd};
    EXPECT_THROW(server.login(userCredentials.first, userCredentials.second), SymServerException);
}

struct SymServerUserMock: public user{
    SymServerUserMock(const std::string &username, const std::string &pwd, const std::string &nickname,
                      const std::string &iconPath, int siteId, std::shared_ptr<directory> home) : user(username, pwd,
                                                                                                       nickname,
                                                                                                       iconPath, siteId,
                                                                                                       home) {};
    MOCK_CONST_METHOD3(accessFile,  std::shared_ptr<file>(const std::string &resId, const std::string &path,  const std::string &fileName));
    MOCK_CONST_METHOD3(openFile, document(const std::string &path,  const std::string &fileName, privilege accessMode));
};

struct SymServerFileMock: public file{
    SymServerFileMock() : file("test", "./aPath") {}

    MOCK_METHOD2(access, document(const user &targetUser, privilege accessMode));
};

struct SymServerTestFilesystemFunctionality : testing::Test {
    ::testing::NiceMock<SymServerUserMock> aUser, anotherUser;
    SymServerAccesser server;
    std::shared_ptr<::testing::NiceMock<SymServerdirMock>> userDir;
    static const std::string filePath;
    static const std::string fileName;
    static const privilege defaultPrivilege;
    std::shared_ptr<::testing::NiceMock<SymServerdirMock>> fakeDir;


    SymServerTestFilesystemFunctionality():
            aUser("mario", "a123@bty!!", "m@ario", "./userIcons/test.jpg", 0, nullptr),
            anotherUser("lucio", "a123@bty!!", "lupoLucio", "./userIcons/test.jpg", 0, nullptr),
            server(),
            userDir(new ::testing::NiceMock<SymServerdirMock>("/")),
            fakeDir(new ::testing::NiceMock<SymServerdirMock>("/")){
        server.setRoot(fakeDir);
        //aUser.setHome(userDir);
        server.addUser(aUser);
        server.login("mario", "a123@bty!!");
    };

    ~SymServerTestFilesystemFunctionality() = default;
};
const std::string SymServerTestFilesystemFunctionality::filePath="./dir1";
const std::string SymServerTestFilesystemFunctionality::fileName="testFile";
const privilege SymServerTestFilesystemFunctionality::defaultPrivilege=uri::getDefaultPrivilege();

TEST_F(SymServerTestFilesystemFunctionality, openSourceMakesTheUserWorkOnDocument){
    EXPECT_CALL(aUser, openFile(filePath, fileName, defaultPrivilege));
    auto doc=server.openSource(aUser, filePath, fileName, defaultPrivilege);
    EXPECT_TRUE(server.userIsWorkingOnDocument(aUser, doc, defaultPrivilege));
}


TEST_F(SymServerTestFilesystemFunctionality, openSourceOfNotLoggedUser){
    document doc;
    ASSERT_THROW(doc=server.openSource(anotherUser, filePath, fileName, defaultPrivilege), SymServerException);
    EXPECT_FALSE(server.userIsWorkingOnDocument(anotherUser, doc, defaultPrivilege));
}

TEST_F(SymServerTestFilesystemFunctionality, openNewSourceAccessesTheFile){
    std::shared_ptr<::testing::NiceMock<SymServerFileMock>> fileToReturn(new ::testing::NiceMock<SymServerFileMock>());

    EXPECT_CALL(aUser, accessFile(filePath+"/"+fileName, "./", "")).WillOnce(::testing::Return(fileToReturn));
    EXPECT_CALL(*fileToReturn, access(aUser, uri::getDefaultPrivilege()));
    auto doc=server.openNewSource(aUser, filePath, fileName, defaultPrivilege, "./");
    EXPECT_TRUE(server.userIsWorkingOnDocument(aUser, doc, defaultPrivilege));
}

TEST_F(SymServerTestFilesystemFunctionality, openNewSourceOfNotLoggedUser){
    document doc;
    ASSERT_THROW(doc=server.openNewSource(anotherUser, filePath, fileName, defaultPrivilege, "./"), SymServerException);
    EXPECT_FALSE(server.userIsWorkingOnDocument(anotherUser, doc, defaultPrivilege));
}