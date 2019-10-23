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
        bool found= entry!=active.end() &&
                *entry->second==toCheck;
        return found;
    }
    void setRoot(std::shared_ptr<directory> newRoot){
        rootDir=newRoot;
    }
    bool userIsWorkingOnDocument(const user& targetUser, const document& targetDoc, privilege privilegeGranted){
        if(workingDoc.find(targetUser.getUsername())==workingDoc.end()) return false;
        std::forward_list<document*> docQueue=workingDoc[targetUser.getUsername()];
        for(auto doc:docQueue){
            if (doc->getId()==targetDoc.getId())
                return true;
        }
        return false;
    }
    bool userAlreadyRegistered(const user& toCheck){
        return registered.find(toCheck.getUsername())!=registered.end();
    }
    bool thereIsMessageForResource(int resId, const message& msg){
        auto entry=workingQueue.find(resId);
        if(entry==workingQueue.end())
            return false;
        std::queue<message> messages=entry->second;
        while(!messages.empty()){
            message m=messages.front();
            if(m.getMsgId()==msg.getMsgId()) return true;
            messages.pop();
        }
        return false;
    }
    /*
     * Used to obtain the result of a logout (e.g. the user is no longer in active)
     * without the calls to close() on all the logged's working documents
     */
    void hardWrongLogout(user& logged){
        active.erase(logged.getUsername());
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

    ~SymServerTestUserFunctionality() = default;
};
const std::string SymServerTestUserFunctionality::validIconPath="./userIcons/test.jpg";
const std::string SymServerTestUserFunctionality::newUserUsername="mario";
const std::string SymServerTestUserFunctionality::newUserPwd="a123@bty!!";
const std::string SymServerTestUserFunctionality::wrongPwd="wrong";

TEST_F(SymServerTestUserFunctionality, addUserAddingUserNotPresent){
    server.addUser(newUser);
    EXPECT_TRUE(server.userAlreadyRegistered(newUser));
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

TEST_F(SymServerTestUserFunctionality, logoutRemovesUserFromActive){
    server.addUser(newUser);
    server.login(newUserUsername, newUserPwd);
    ASSERT_TRUE(server.userIsActive(newUserUsername, newUser));
    server.logout(newUserUsername, newUserPwd);
    EXPECT_FALSE(server.userIsActive(newUserUsername, newUser));
}

TEST_F(SymServerTestUserFunctionality, removeUserRemovesUserFromRegistered){
    server.addUser(newUser);
    ASSERT_TRUE(server.userAlreadyRegistered(newUser));
    server.removeUser(newUserUsername, newUserPwd);
    EXPECT_FALSE(server.userAlreadyRegistered(newUser));
}

TEST_F(SymServerTestUserFunctionality, editUserChangesUserData){
    server.addUser(newUser);
    server.login(newUserUsername, newUserPwd);

    user newData("giuseppe", "123@pwd@!", "peppuccio63", validIconPath, 0, nullptr);
    user inserted=server.editUser(newUserUsername, newUserPwd, newData);
    EXPECT_EQ("giuseppe", inserted.getUsername());
    EXPECT_EQ("peppuccio63", inserted.getNickname());
    EXPECT_EQ("123@pwd@!", inserted.getPwdHash());
    EXPECT_EQ(newUser.getHome(), inserted.getHome());
    EXPECT_EQ(newUser.getSiteId(), inserted.getSiteId());
}

struct SymServerUserMock: public user{
    SymServerUserMock(const std::string &username, const std::string &pwd, const std::string &nickname,
                      const std::string &iconPath, int siteId, std::shared_ptr<directory> home) : user(username, pwd,
                                                                                                       nickname,
                                                                                                       iconPath, siteId,
                                                                                                       home) {};
    MOCK_CONST_METHOD3(accessFile,  std::shared_ptr<file>(const std::string &resId, const std::string &path,  const std::string &fileName));
    MOCK_CONST_METHOD3(openFile, document&(const std::string &path,  const std::string &fileName, privilege accessMode));
    MOCK_CONST_METHOD2(newFile, std::shared_ptr<file>(const std::string& fileName, const std::string& pathFromHome));
    MOCK_CONST_METHOD2(newDirectory, std::shared_ptr<directory>(const std::string& dirName, const std::string& pathFromHome));
    MOCK_CONST_METHOD4(editPrivilege, privilege(const user &otherUser, const std::string &resPath, const std::string &resName,
            privilege newPrivilege));
    MOCK_CONST_METHOD3(shareResource, uri(const std::string &resPath, const std::string &resName, uri& newPrefs));
};

struct SymServerFileMock: public file{
    SymServerFileMock() : file("test", "./aPath") {}

    MOCK_METHOD2(access, document&(const user &targetUser, privilege accessMode));
};

struct SymServerDocMock: public document{
    SymServerDocMock():document(){};

    MOCK_METHOD1(remoteInsert, void(symbol toInsert));
    MOCK_METHOD1(remoteRemove, void(const symbol& toRemove));
    MOCK_METHOD1(close, void(const user& noLongerActive));
    SymServerDocMock(const SymServerDocMock& mock){};
};

struct SymServerTestFilesystemFunctionality : testing::Test {
    ::testing::NiceMock<SymServerUserMock> loggedUser, anotherUser;
    ::testing::NiceMock<SymServerAccesser> server;
    std::shared_ptr<::testing::NiceMock<SymServerdirMock>> userDir;
    static const std::string filePath;
    static const std::string fileName;
    static const privilege defaultPrivilege;
    std::shared_ptr<::testing::NiceMock<SymServerdirMock>> fakeDir;
    static const std::string loggedUserUsername;
    static const std::string loggedUserPwd;
    static const std::string anotherUserUsername;
    static const std::string anotherUserPwd;
    ::testing::NiceMock<SymServerDocMock> doc;
    std::shared_ptr<::testing::NiceMock<SymServerFileMock>> fileToReturn;

    SymServerTestFilesystemFunctionality():
            loggedUser(loggedUserUsername, loggedUserPwd, "m@ario", "./userIcons/test.jpg", 0, nullptr),
            anotherUser(anotherUserUsername, anotherUserPwd, "lupoLucio", "./userIcons/test.jpg", 0, nullptr),
            server(),
            userDir(new ::testing::NiceMock<SymServerdirMock>("/")),
            fakeDir(new ::testing::NiceMock<SymServerdirMock>("/")),
            fileToReturn(new ::testing::NiceMock<SymServerFileMock>()){
        server.setRoot(fakeDir);
        //loggedUser.setHome(userDir);
        server.addUser(loggedUser);
        server.login("mario", "a123@bty!!");
    };
    /*
     * Make the owner user to access the file and open the document. Used to test that closeSource actually works
     */
    void setStageForHavingOpenedDoc(SymServerUserMock& userWhoOpens){
        EXPECT_CALL(userWhoOpens, accessFile(filePath + "/" + fileName, "./", "")).WillOnce(::testing::Return(fileToReturn));
        EXPECT_CALL(*fileToReturn, access(userWhoOpens, uri::getDefaultPrivilege())).WillOnce(::testing::ReturnRef(doc));
        auto ret=server.openNewSource(userWhoOpens, filePath, fileName, privilege::owner, "./");
        ASSERT_TRUE(server.userIsWorkingOnDocument(userWhoOpens, doc, privilege::owner));
    }
    void setAnotherUserActive(){
        server.addUser(anotherUser);
        server.login(anotherUserUsername, anotherUserPwd);
        ASSERT_TRUE(server.userIsActive(anotherUserUsername, anotherUser));
    }
    /*
     * Make anotherUser access the resource (as someone granted a privilege to him) and the close the document
     * to enable the setting of another privilege to anotherUser by loggedUser (who is the owner)
     */
    void makeAnotherUserToHavePrivilegeAndCloseSource(privilege priv){
        makeAnotherUserToHavePrivilege(priv);
        closeAfterPrivilegeAcquired(priv);
    }
    void makeAnotherUserToHavePrivilege(privilege priv){
        //anotherUser adds the file to its filesystem and closes the document
        EXPECT_CALL(anotherUser, accessFile(filePath + "/" + fileName, "./", "")).WillOnce(::testing::Return(fileToReturn));
        EXPECT_CALL(*fileToReturn, access(anotherUser, uri::getDefaultPrivilege())).WillOnce(::testing::ReturnRef(doc));
        document ret2=server.openNewSource(anotherUser, filePath, fileName, priv, "./");
    }
    void closeAfterPrivilegeAcquired(privilege priv){
        EXPECT_CALL(doc, close(anotherUser));
        server.closeSource(anotherUser, doc);
        ASSERT_FALSE(server.userIsWorkingOnDocument(anotherUser, doc, priv));
    }

    ~SymServerTestFilesystemFunctionality() = default;
};
const std::string SymServerTestFilesystemFunctionality::filePath="./dir1";
const std::string SymServerTestFilesystemFunctionality::fileName="testFile";
const privilege SymServerTestFilesystemFunctionality::defaultPrivilege=uri::getDefaultPrivilege();
const std::string SymServerTestFilesystemFunctionality::loggedUserUsername="mario";
const std::string SymServerTestFilesystemFunctionality::loggedUserPwd="a123@bty!!";
const std::string SymServerTestFilesystemFunctionality::anotherUserPwd="a123@bty!!";
const std::string SymServerTestFilesystemFunctionality::anotherUserUsername="lucio";

TEST_F(SymServerTestFilesystemFunctionality, openSourceMakesTheUserWorkOnDocument){
    EXPECT_CALL(loggedUser, openFile(filePath, fileName, defaultPrivilege)).WillOnce(::testing::ReturnRef(doc));
    auto doc=server.openSource(loggedUser, filePath, fileName, defaultPrivilege);
    EXPECT_TRUE(server.userIsWorkingOnDocument(loggedUser, doc, defaultPrivilege));
}


TEST_F(SymServerTestFilesystemFunctionality, openSourceOfNotLoggedUser){
    document doc;
    ASSERT_THROW(doc=server.openSource(anotherUser, filePath, fileName, defaultPrivilege), SymServerException);
    EXPECT_FALSE(server.userIsWorkingOnDocument(anotherUser, doc, defaultPrivilege));
}

TEST_F(SymServerTestFilesystemFunctionality, logoutClosesOpenedDocuments){
    EXPECT_CALL(loggedUser, openFile(filePath, fileName, defaultPrivilege)).WillOnce(::testing::ReturnRef(doc));
    auto doc=server.openSource(loggedUser, filePath, fileName, defaultPrivilege);
    ASSERT_TRUE(server.userIsWorkingOnDocument(loggedUser, doc, defaultPrivilege));

    server.logout(loggedUserUsername, loggedUserPwd);
    EXPECT_FALSE(server.userIsWorkingOnDocument(loggedUser, doc, defaultPrivilege));
}

TEST_F(SymServerTestFilesystemFunctionality, removeUserClosesOpenedDocuments){
    EXPECT_CALL(loggedUser, openFile(filePath, fileName, defaultPrivilege)).WillOnce(::testing::ReturnRef(doc));
    auto doc=server.openSource(loggedUser, filePath, fileName, defaultPrivilege);
    ASSERT_TRUE(server.userIsWorkingOnDocument(loggedUser, doc, defaultPrivilege));

    server.removeUser(loggedUserUsername, loggedUserPwd);
    EXPECT_FALSE(server.userAlreadyRegistered(loggedUser));
    EXPECT_FALSE(server.userIsWorkingOnDocument(loggedUser, doc, defaultPrivilege));
}

TEST_F(SymServerTestFilesystemFunctionality, openNewSourceAccessesTheFile){
    EXPECT_CALL(loggedUser, accessFile(filePath + "/" + fileName, "./", "")).WillOnce(::testing::Return(fileToReturn));
    EXPECT_CALL(*fileToReturn, access(loggedUser, uri::getDefaultPrivilege())).WillOnce(::testing::ReturnRef(doc));
    auto doc=server.openNewSource(loggedUser, filePath, fileName, defaultPrivilege, "./");
    EXPECT_TRUE(server.userIsWorkingOnDocument(loggedUser, doc, defaultPrivilege));
}

TEST_F(SymServerTestFilesystemFunctionality, openNewSourceOfNotLoggedUser){
    document doc;
    ASSERT_THROW(doc=server.openNewSource(anotherUser, filePath, fileName, defaultPrivilege, "./"), SymServerException);
    EXPECT_FALSE(server.userIsWorkingOnDocument(anotherUser, doc, defaultPrivilege));
}

TEST_F(SymServerTestFilesystemFunctionality, createNewSourceCallsNewFile){
    EXPECT_CALL(loggedUser, newFile(fileName, filePath)).WillOnce(::testing::Return(fileToReturn));
    EXPECT_CALL(*fileToReturn, access(loggedUser, privilege::owner)).WillOnce(::testing::ReturnRef(doc));
    auto doc=server.createNewSource(loggedUser, filePath, fileName);
    EXPECT_TRUE(server.userIsWorkingOnDocument(loggedUser, doc, privilege::owner));
}

TEST_F(SymServerTestFilesystemFunctionality, createNewSourceOfUnloggedUser){
    EXPECT_THROW(server.createNewSource(anotherUser, filePath, fileName), SymServerException);
}

TEST_F(SymServerTestFilesystemFunctionality, createNewDirCallsNewDirectory){
    EXPECT_CALL(loggedUser, newDirectory(fileName, filePath));
    server.createNewDir(loggedUser, filePath, fileName);
}

TEST_F(SymServerTestFilesystemFunctionality, createNewDirOfUnloggedUser){
    EXPECT_THROW(server.createNewDir(anotherUser, filePath, fileName), SymServerException);
}

TEST_F(SymServerTestFilesystemFunctionality, remoteInsertCallsRemoteInsertOnDoc){
    EXPECT_CALL(loggedUser, openFile(filePath, fileName, defaultPrivilege)).WillOnce(::testing::ReturnRef(doc));
    document ret=server.openSource(loggedUser, filePath, fileName, privilege::modify);
    symbol toInsert('a', 0, 0, {});
    symbolMessage received(msgType::insertSymbol, {loggedUserUsername, loggedUserPwd}, msgOutcome::success, 0, doc.getId(), toInsert);
    EXPECT_CALL(doc, remoteInsert(toInsert));
    server.remoteInsert(loggedUserUsername, doc.getId(), received);
}

TEST_F(SymServerTestFilesystemFunctionality, remoteInsertOfUnloggedUser){
    symbol toInsert('a', 0, 0, {});
    symbolMessage received(msgType::insertSymbol, {anotherUserUsername, anotherUserPwd}, msgOutcome::success, 0, doc.getId(), toInsert);
    EXPECT_THROW(server.remoteInsert(anotherUserUsername, doc.getId(), received), SymServerException);
}

TEST_F(SymServerTestFilesystemFunctionality, remoteInsertOnDocumentNotOpened){
    symbol toInsert('a', 0, 0, {});
    symbolMessage received(msgType::insertSymbol, {loggedUserUsername, loggedUserPwd}, msgOutcome::success, 0, doc.getId(), toInsert);
    EXPECT_THROW(server.remoteInsert(loggedUserUsername, doc.getId(), received), SymServerException);
}

TEST_F(SymServerTestFilesystemFunctionality, remoteInsertAppendesMessage){
    EXPECT_CALL(loggedUser, openFile(filePath, fileName, defaultPrivilege)).WillOnce(::testing::ReturnRef(doc));
    auto doc=server.openSource(loggedUser, filePath, fileName, privilege::modify);
    symbol toInsert('a', 0, 0, {});
    symbolMessage received(msgType::insertSymbol, {loggedUserUsername, loggedUserPwd}, msgOutcome::success, 0, doc.getId(), toInsert);
    server.remoteInsert(loggedUserUsername, doc.getId(), received);
    EXPECT_TRUE(server.thereIsMessageForResource(doc.getId(), received));
}

TEST_F(SymServerTestFilesystemFunctionality, remoteRemoveCallsRemoteRemoveOnDoc){
    EXPECT_CALL(loggedUser, openFile(filePath, fileName, defaultPrivilege)).WillOnce(::testing::ReturnRef(doc));
    document ret=server.openSource(loggedUser, filePath, fileName, privilege::modify);
    symbol toRemove('a', 0, 0, {});
    symbolMessage received(msgType::removeSymbol, {loggedUserUsername, loggedUserPwd}, msgOutcome::success, 0, doc.getId(), toRemove);
    EXPECT_CALL(doc, remoteRemove(toRemove));
    server.remoteRemove(loggedUserUsername, doc.getId(), received);
}

TEST_F(SymServerTestFilesystemFunctionality, remoteRemoveOfUnloggedUser){
    symbol toRemove('a', 0, 0, {});
    symbolMessage received(msgType::removeSymbol, {anotherUserUsername, anotherUserPwd}, msgOutcome::success, 0, doc.getId(), toRemove);
    EXPECT_THROW(server.remoteRemove(anotherUserUsername, doc.getId(), received), SymServerException);
}

TEST_F(SymServerTestFilesystemFunctionality, remoteRemoveOnDocumentNotOpened){
    symbol toRemove('a', 0, 0, {});
    symbolMessage received(msgType::removeSymbol, {loggedUserUsername, loggedUserPwd}, msgOutcome::success, 0, doc.getId(), toRemove);
    EXPECT_THROW(server.remoteInsert(loggedUserUsername, doc.getId(), received), SymServerException);
}

TEST_F(SymServerTestFilesystemFunctionality, closeSourceClosesTheDocument){
    setStageForHavingOpenedDoc(loggedUser);

    EXPECT_CALL(doc, close(loggedUser));
    server.closeSource(loggedUser, doc);
    EXPECT_FALSE(server.userIsWorkingOnDocument(loggedUser, doc, defaultPrivilege));
}


TEST_F(SymServerTestFilesystemFunctionality, editPrivilegeCallsEditPrivilegeOnUser){
    setStageForHavingOpenedDoc(loggedUser);
    setAnotherUserActive();
    makeAnotherUserToHavePrivilegeAndCloseSource(defaultPrivilege);

    EXPECT_CALL(loggedUser, openFile(filePath, fileName, privilege::owner)).WillOnce(::testing::ReturnRef(doc));
    EXPECT_CALL(loggedUser, editPrivilege(anotherUser, filePath, fileName, privilege::readOnly));
    server.editPrivilege(loggedUser, anotherUser, "./"+loggedUserUsername+"/"+filePath.substr(2), fileName, privilege::readOnly);
}

TEST_F(SymServerTestFilesystemFunctionality, editPrivilegeCalledByUnloggedUser){
    setStageForHavingOpenedDoc(loggedUser);
    setAnotherUserActive();
    makeAnotherUserToHavePrivilegeAndCloseSource(defaultPrivilege);
    server.hardWrongLogout(loggedUser);

    EXPECT_THROW(server.editPrivilege(loggedUser, anotherUser, "./"+loggedUserUsername+"/"+filePath.substr(2), fileName, privilege::readOnly), SymServerException);
}

TEST_F(SymServerTestFilesystemFunctionality, editPrivilegeOnUserWorkingOnDocument){
    setStageForHavingOpenedDoc(loggedUser);
    setAnotherUserActive();
    makeAnotherUserToHavePrivilege(defaultPrivilege);
    //call expected because we need to retrieve the document id of the document named fileName (in the loggedUser space)
    //to check that anotherUser is not working on the same document
    EXPECT_CALL(loggedUser, openFile(filePath, fileName, privilege::owner)).WillOnce(::testing::ReturnRef(doc));
    EXPECT_THROW(server.editPrivilege(loggedUser, anotherUser, "./"+loggedUserUsername+"/"+filePath.substr(2), fileName, privilege::readOnly), SymServerException);
}

TEST_F(SymServerTestFilesystemFunctionality, shareResourceCallsShareResourceOnUser){
    uri oldPref;
    uri newPref(uriPolicy::activeAlways);
    EXPECT_CALL(loggedUser, shareResource(filePath, fileName, newPref)).WillOnce(::testing::Return(oldPref));
    uri retPref=server.shareResource(loggedUser, filePath, fileName, newPref);
    EXPECT_EQ(oldPref, retPref);
}

TEST_F(SymServerTestFilesystemFunctionality, shareResourceOfUnLoggedUser){
    uri newPref(uriPolicy::activeAlways);
    EXPECT_THROW(server.shareResource(anotherUser, filePath, fileName, newPref), SymServerException);
}