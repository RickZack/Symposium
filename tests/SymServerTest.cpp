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
#include <typeinfo>
#include "../SymServer.h"
#include "../message.h"
#include "../filesystem.h"
#include "../SymposiumException.h"
using namespace Symposium;

struct SymServerdirMock : public directory{
    SymServerdirMock(const std::string &name) : directory(name) {};
    MOCK_METHOD2(addDirectory, std::shared_ptr<directory>(const std::string &name, int idToAssign));
    MOCK_METHOD2(getFile, std::shared_ptr<file>(const std::string&, const std::string&));
    MOCK_METHOD2(getDir, std::shared_ptr<directory>(const std::string &path, const std::string &name));
};

struct SymServerUserMock: public user{
    SymServerUserMock() {};
    SymServerUserMock(const std::string &username, const std::string &pwd, const std::string &nickname,
                      const std::string &iconPath, int siteId, std::shared_ptr<directory> home) : user(username, pwd,
                                                                                                       nickname,
                                                                                                       iconPath, siteId,
                                                                                                       home) {};
    SymServerUserMock(const SymServerUserMock& other): user(other){};
    MOCK_CONST_METHOD3(accessFile,  std::shared_ptr<file>(const std::string &resId, const std::string &path,  const std::string &fileName));
    MOCK_CONST_METHOD3(openFile, std::shared_ptr<file>(const std::string &path,  const std::string &fileName, privilege accessMode));
    MOCK_CONST_METHOD3(newFile, std::shared_ptr<file>(const std::string& fileName, const std::string& pathFromHome, int));
    MOCK_CONST_METHOD3(newDirectory, std::shared_ptr<directory>(const std::string& dirName, const std::string& pathFromHome, int id));
    MOCK_CONST_METHOD4(editPrivilege, privilege(const std::string &otherUser, const std::string &resPath, const std::string &resName,
            privilege newPrivilege));
    MOCK_CONST_METHOD3(shareResource, std::shared_ptr<filesystem>(const std::string &resPath, const std::string &resName, uri& newPrefs));
    MOCK_CONST_METHOD3(renameResource, std::shared_ptr<filesystem>(const std::string& resPath, const std::string& resName, const std::string& newName));
    MOCK_CONST_METHOD2(removeResource, std::shared_ptr<filesystem>(const std::string &path, const std::string &name));
    MOCK_CONST_METHOD1(hasPwd, bool(const std::string& pwd));
    MOCK_METHOD1(setNewData, void(const user&));
};

struct SymServerFileMock: public file{
    SymServerFileMock() : file("test", "./aPath", 0) {}
    MOCK_METHOD2(access, document&(const user &targetUser, privilege accessMode));
};

struct SymServerDocMock: public document{
    SymServerDocMock():document(){};
    SymServerDocMock(const SymServerDocMock& mock){};
    MOCK_METHOD1(remoteInsert, void(const symbol &toInsert));
    MOCK_METHOD1(remoteRemove, void(const symbol &toRemove));
    MOCK_METHOD1(close, void(const user& noLongerActive));
    MOCK_METHOD0(retrieveSiteIds, std::set<int>());
};

/*
 * Used to access read-only the members of SymServer without adding getters in production class
 */
struct SymServerAccesser: public SymServer{
    std::unordered_map<std::string, SymServerUserMock> registered;
    document *d1, *d2;


    /*
     * sets a series of docs opened by two users
     */
    void setStageForDocOpenedByTwoUsers(const user &u1, const user &u2) {
        if(u1.getUsername()==u2.getUsername())
            throw std::exception();


        //user1 has d1 and d2 opened
        workingDoc[u1.getUsername()].push_front(d1);
        workingDoc[u1.getUsername()].push_front(d2);

        //user1 has d1 and d2 opened
        workingDoc[u2.getUsername()].push_front(d1);
        workingDoc[u2.getUsername()].push_front(d2);

        resIdToSiteId[10].push_front(u1.getSiteId());
        resIdToSiteId[10].push_front(u2.getSiteId());
        resIdToSiteId[20].push_front(u1.getSiteId());
        resIdToSiteId[20].push_front(u2.getSiteId());
    }

public:
    SymServerAccesser() {
        d1=new document(10);
        d2=new document(20);
    }

    virtual ~SymServerAccesser() {
        delete d1;
        delete d2;
    }

    bool userIsActive(const std::string& username, const user& toCheck){
        auto entry=active.find(username);
        bool found= entry!=active.end() &&
                *entry->second==toCheck;
        return found && userIsActive(username);
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

    /*
     * Return {true, message} if there is a message [msg] associated with siteId [siteId], {false, nullptr} otherwise
     */
    std::pair<bool, std::shared_ptr<serverMessage>> thereIsMessageForUser(int siteId, const serverMessage &msg){
        std::pair<bool, std::shared_ptr<serverMessage>> result(false, nullptr);
        auto entry=siteIdToMex.find(siteId);
        if(entry == siteIdToMex.end())
            return result;
        std::queue<std::shared_ptr<serverMessage>> messages=entry->second;
        while(!messages.empty()){
            auto m=messages.front();
            if(/*m->getMsgId()==msg.getMsgId() && */m->getAction()==msg.getAction()
               && m->getResult()==msg.getResult() && typeid(*m)== typeid(msg)){
                result.first=true; result.second=messages.front();
                //messages.pop();
                return result;
            }
            messages.pop();
        }
        return result;
    }

    void forceSiteIdForResId(const document *doc, const user &u){
        resIdToSiteId[doc->getId()].push_front(u.getSiteId());
        workingDoc[u.getUsername()].push_front(const_cast<document*>(doc));
    }

    /*
     * Used to obtain the result of a logout (e.g. the user is no longer in active)
     * without the calls to close() on all the logged's working documents
     */
    void hardWrongLogout(user& logged){
        active.erase(logged.getUsername());
    }
    /*
     * These methods allow to redirect the calls to get registered users to the mock class:
     * this permits to return objects of type SymServerUserMock, so to break the dependency
     * from user mocking the called methods
     */
    user &registerUser(user *toInsert) override {
        std::string username=toInsert->getUsername();
        SymServerUserMock* mocked= dynamic_cast<SymServerUserMock*>(toInsert);
        SymServerUserMock& toReturn=registered.insert({username, *mocked}).first->second;
        return toReturn;
    }

    user &getRegistered(const std::string &username) override {
        return registered[username];
    }
protected:
    void removeRegistered(const std::string &username) override {
        registered.erase(username);
    }

    bool userIsRegistered(const std::string &toCheck) const noexcept override {
        return registered.find(toCheck)!=registered.end();
    }

    bool userIsActive(const std::string &username) const override {
        return active.find(username)!=active.end();
    }

    /*
     * It's identical to the utility function in SymServer class:
     * here it's necessary to make the function SymServer::mapSiteIdToUser
     * take the mocked user from SymServerAccesser in tests
     */
    user findUserBySiteId(int id) const override {
        for(const auto& elem:registered)
            if(elem.second.getSiteId()==id)
                return elem.second;
        return unknownUser;
    }
};

struct SymServerTestUserFunctionality : testing::Test {
    ::testing::NiceMock<SymServerUserMock> newUser, alreadyPresent, newDifferentUser;
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
    EXPECT_CALL(*fakeDir, addDirectory(newUserUsername, ::testing::_)).WillOnce(::testing::Return(userDir));
    auto userInserted=server.addUser(newUser);
    EXPECT_TRUE(userInserted.getHome()==userDir);
}

TEST_F(SymServerTestUserFunctionality, addUserGivesDifferentSiteId){
    auto newUserComplete=server.addUser(newUser);
    auto newDifferentUserComplete=server.addUser(newDifferentUser);
    EXPECT_NE(newUserComplete.getSiteId(), newDifferentUserComplete.getSiteId());
}

TEST_F(SymServerTestUserFunctionality, loginOfRegisteredUserGenerateCorrectResponse){
    const SymServerUserMock& inserted= dynamic_cast<const SymServerUserMock&>(server.addUser(newUser));
    EXPECT_CALL(inserted, hasPwd(newUserPwd)).WillOnce(::testing::Return(true));
    auto logged=server.login(newUserUsername, newUserPwd);
    EXPECT_TRUE(server.userIsActive(newUserUsername, logged));

    loginMessage response(msgType::login, msgOutcome::success, logged);
    EXPECT_TRUE(server.thereIsMessageForUser(logged.getSiteId(), response).first);
}

TEST_F(SymServerTestUserFunctionality, loginOfRegisteredUserWithWrongPwd){
    const SymServerUserMock& inserted= dynamic_cast<const SymServerUserMock&>(server.addUser(newUser));
    EXPECT_CALL(inserted, hasPwd(wrongPwd)).WillOnce(::testing::Return(false));
    user logged;
    EXPECT_THROW(logged=server.login(newUserUsername, wrongPwd), SymServerException);
    EXPECT_FALSE(server.userIsActive(newUserUsername, logged));
    /*
     * Cases in which, for an error, the operation goes wrong, so an exception is raised, are to be handled
     * externally, in the module that controls the connection
     */
}

TEST_F(SymServerTestUserFunctionality, loginOfAlreadyLoggedUser){
    const SymServerUserMock& inserted= dynamic_cast<const SymServerUserMock&>(server.addUser(newUser));
    EXPECT_CALL(inserted, hasPwd(newUserPwd)).WillRepeatedly(::testing::Return(true));
    server.login(newUserUsername, newUserPwd);
    EXPECT_THROW(server.login(newUserUsername, newUserPwd), SymServerException);
    /*
     * Cases in which, for an error, the operation goes wrong, so an exception is raised, are to be handled
     * externally, in the module that controls the connection
     */
}

TEST_F(SymServerTestUserFunctionality, loginOfUnregisteredUser){
    std::pair<std::string, std::string> userCredentials={newUserUsername, newUserPwd};
    EXPECT_THROW(server.login(userCredentials.first, userCredentials.second), SymServerException);
    EXPECT_THROW(server.login(newUserUsername, newUserPwd), SymServerException);
    /*
     * Cases in which, for an error, the operation goes wrong, so an exception is raised, are to be handled
     * externally, in the module that controls the connection
     */
}

TEST_F(SymServerTestUserFunctionality, logoutRemovesUserFromActiveAndGenerateCorrectResponse){
    const SymServerUserMock& inserted= dynamic_cast<const SymServerUserMock&>(server.addUser(newUser));
    EXPECT_CALL(inserted, hasPwd(newUserPwd)).WillOnce(::testing::Return(true));
    server.login(newUserUsername, newUserPwd);
    ASSERT_TRUE(server.userIsActive(newUserUsername, newUser));
    server.logout(newUserUsername, newUserPwd);
    EXPECT_FALSE(server.userIsActive(newUserUsername, newUser));

    serverMessage response(msgType::logout, msgOutcome::success);
    EXPECT_TRUE(server.thereIsMessageForUser(inserted.getSiteId(), response).first);
}

TEST_F(SymServerTestUserFunctionality, removeUserRemovesUserFromRegistered){
    const SymServerUserMock& inserted= dynamic_cast<const SymServerUserMock&>(server.addUser(newUser));
    ASSERT_TRUE(server.userAlreadyRegistered(newUser));
    server.removeUser(newUserUsername, newUserPwd);
    EXPECT_FALSE(server.userAlreadyRegistered(newUser));

    serverMessage response(msgType::removeUser, msgOutcome::success);
    EXPECT_TRUE(server.thereIsMessageForUser(inserted.getSiteId(), response).first);
}

TEST_F(SymServerTestUserFunctionality, editUserCallSetUserDataAndGenerateCorrectResponse){
    SymServerUserMock& oldUser= dynamic_cast<SymServerUserMock&>(const_cast<user&>(server.addUser(newUser)));
    EXPECT_CALL(oldUser, hasPwd(newUserPwd)).WillOnce(::testing::Return(true));
    server.login(newUserUsername, newUserPwd);

    user newData("giuseppe", "123@pwd@!", "peppuccio63", validIconPath, 0, nullptr);
    EXPECT_CALL(oldUser, setNewData(newData));
    server.editUser(newUserUsername, newUserPwd, newData);

    serverMessage response(msgType::changeUserData, msgOutcome::success);
    EXPECT_TRUE(server.thereIsMessageForUser(oldUser.getSiteId(), response).first);
}

TEST_F(SymServerTestUserFunctionality, editUserChangesUserDataAndPropagateChanges){
    SymServerUserMock& oldUser= dynamic_cast<SymServerUserMock&>(const_cast<user&>(server.addUser(newUser)));
    EXPECT_CALL(oldUser, hasPwd(newUserPwd)).WillOnce(::testing::Return(true));
    server.login(newUserUsername, newUserPwd);
    server.setStageForDocOpenedByTwoUsers(newUser, newDifferentUser);
    user newData("giuseppe", "123@pwd@!", "peppuccio63", validIconPath, 0, nullptr);
    EXPECT_CALL(oldUser, setNewData(newData));

    const user& inserted=server.editUser(newUserUsername, newUserPwd, newData);

    /*
     * Change in a user's data must be propagated to clients that can be interested
     * in this user.
     * This means that all the clients that are working on documents in which the current
     * user is involved must be notified.
     */
    userDataMessage toSend(msgType::changeUserData, {newUserUsername, newUserPwd}, msgOutcome::success, newData);
    EXPECT_TRUE(server.thereIsMessageForUser(newDifferentUser.getSiteId(), toSend).first);
}

struct SymServerTestFilesystemFunctionality : testing::Test {
    ::testing::NiceMock<SymServerUserMock> loggedUser, anotherUser;
    ::testing::NiceMock<SymServerAccesser> server;
    const SymServerUserMock* inserted;
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
    SymServerUserMock* justInserted;

    SymServerTestFilesystemFunctionality():
            loggedUser(loggedUserUsername, loggedUserPwd, "m@ario", "./userIcons/test.jpg", 0, nullptr),
            anotherUser(anotherUserUsername, anotherUserPwd, "lupoLucio", "./userIcons/test.jpg", 1, nullptr),
            server(),
            userDir(new ::testing::NiceMock<SymServerdirMock>("/")),
            fakeDir(new ::testing::NiceMock<SymServerdirMock>("/")),
            fileToReturn(new ::testing::NiceMock<SymServerFileMock>()){
        server.setRoot(fakeDir);
        inserted= &dynamic_cast<const SymServerUserMock&>(server.addUser(loggedUser));
        EXPECT_CALL(*inserted, hasPwd(loggedUserPwd)).WillOnce(::testing::Return(true));
        server.login("mario", "a123@bty!!");
    };

    /*
     * Make the owner user to access the file and open the document. Used to test that closeSource actually works
     */
    void setStageForAccessedDoc(SymServerUserMock& userWhoOpens){
        SymServerUserMock& target= dynamic_cast<SymServerUserMock&>(server.getRegistered(loggedUserUsername));
        EXPECT_CALL(target, accessFile(filePath + "/" + fileName, "./", fileName)).WillOnce(::testing::Return(fileToReturn));
        EXPECT_CALL(*fileToReturn, access(target, uri::getDefaultPrivilege())).WillOnce(::testing::ReturnRef(doc));
        auto ret= server.openNewSource(target.getUsername(), filePath + "/" + fileName, "./", fileName,
                                       uri::getDefaultPrivilege());
        ASSERT_TRUE(server.userIsWorkingOnDocument(target, doc, privilege::owner));
        justInserted=&target;
    }
    void setAnotherUserActive(){
        inserted= &dynamic_cast<const SymServerUserMock&>(server.addUser(anotherUser));
        EXPECT_CALL(*inserted, hasPwd(anotherUserPwd)).WillOnce(::testing::Return(true));
        server.login(anotherUserUsername, anotherUserPwd);
        ASSERT_TRUE(server.userIsActive(anotherUserUsername, anotherUser));
    }

    void setStageForOpenedDocForLoggedUser(){
        SymServerUserMock& target= dynamic_cast<SymServerUserMock&>(server.getRegistered(loggedUserUsername));
        EXPECT_CALL(target, openFile(filePath, fileName, defaultPrivilege)).WillOnce(::testing::Return(fileToReturn));
        EXPECT_CALL(*fileToReturn, access(loggedUser, defaultPrivilege)).WillOnce(::testing::ReturnRef(doc));
        auto ret=server.openSource(loggedUserUsername, filePath, fileName, privilege::modify);
    }

    /*
     * Make anotherUser access the resource (as someone granted a privilege to him) and the close the document
     * to enable the setting of another privilege to anotherUser by loggedUser (who is the owner)
     */
    void makeAnotherUserToHavePrivilegeAndCloseSource(privilege priv){
        makeAnotherUserToHavePrivilege(priv);
        ASSERT_NO_FATAL_FAILURE(closeAfterPrivilegeAcquired(priv));
    }

    void makeAnotherUserToHavePrivilege(privilege priv){
        //anotherUser adds the file to its filesystem and closes the document
        SymServerUserMock& target= dynamic_cast<SymServerUserMock&>(server.getRegistered(anotherUserUsername));
        EXPECT_CALL(target, accessFile(filePath + "/" + fileName, "./", fileName)).WillOnce(::testing::Return(fileToReturn));
        EXPECT_CALL(*fileToReturn, access(target, uri::getDefaultPrivilege())).WillOnce(::testing::ReturnRef(doc));
        auto ret2= server.openNewSource(anotherUserUsername, filePath + "/" + fileName, "./", fileName, priv);
    }

    void closeAfterPrivilegeAcquired(privilege priv){
        SymServerUserMock& target= dynamic_cast<SymServerUserMock&>(server.getRegistered(anotherUserUsername));
        EXPECT_CALL(doc, close(anotherUser));
        server.closeSource(anotherUserUsername, doc.getId());
        ASSERT_FALSE(server.userIsWorkingOnDocument(anotherUser, doc, priv));
    }

    void makeLoggedUserNotWorkingOnDoc(){
        SymServerUserMock& target= dynamic_cast<SymServerUserMock&>(server.getRegistered(loggedUserUsername));
        EXPECT_CALL(doc, close(loggedUser));
        server.closeSource(loggedUserUsername, doc.getId());
        ASSERT_FALSE(server.userIsWorkingOnDocument(anotherUser, doc, privilege::owner));
    }

    ::testing::AssertionResult symbolMessageInQueueIsCorrect(const char* arg1, const char* arg2, std::pair<bool, std::shared_ptr<message>> res, const symbolMessage& toSend){
        if(!res.first)
            return ::testing::AssertionFailure()<<"The message is not present is the sending queue";
        if(std::dynamic_pointer_cast<symbolMessage>(res.second)->getActionOwner()!=std::pair<std::string, std::string>())
            return ::testing::AssertionFailure()<<"The actionOwner attribute is not clear"
            <<", the server should forward messages without the authentication parameters";
        if(!std::dynamic_pointer_cast<symbolMessage>(res.second)->getSym().isVerified())
            return ::testing::AssertionFailure()<<"The server should send the symbol as verified";
        return ::testing::AssertionSuccess();
    }

    void messageAssociatedWithRightUsers(const std::initializer_list<int>& siteIds, const serverMessage &toSend, const std::initializer_list<int>& siteIdsNoMex= {}){
        for(int s:siteIds) {
            EXPECT_TRUE(server.thereIsMessageForUser(s, toSend).first)
                                << "Message not associated with siteId " << s;
        }
        for(int s:siteIdsNoMex) {
            EXPECT_FALSE(server.thereIsMessageForUser(s, toSend).first)
                                << "Message is associated with siteId " << s << " while it shouldn't";
        }
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

TEST_F(SymServerTestFilesystemFunctionality, openSourceMakesTheUserWorkOnDocumentAndInsertMessagesInQueue){
    server.forceSiteIdForResId(&doc, anotherUser);
    SymServerUserMock& target= dynamic_cast<SymServerUserMock&>(server.getRegistered(loggedUserUsername));
    EXPECT_CALL(target, openFile(filePath, fileName, defaultPrivilege)).WillOnce(::testing::Return(fileToReturn));
    EXPECT_CALL(*fileToReturn, access(loggedUser, defaultPrivilege)).WillOnce(::testing::ReturnRef(doc));
    auto f=server.openSource(loggedUserUsername, filePath, fileName, defaultPrivilege);
    EXPECT_TRUE(server.userIsWorkingOnDocument(loggedUser, doc, defaultPrivilege));

    //Propagation of document opened to other clients
    updateActiveMessage toSend(msgType::addActiveUser, msgOutcome::success, loggedUser.makeCopyNoPwd(), doc.getId(), privilege::owner);
    ASSERT_NO_FATAL_FAILURE(messageAssociatedWithRightUsers({anotherUser.getSiteId()}, toSend, {loggedUser.getSiteId()}));

    //Response to the client
    sendResMessage response(msgType::openRes, msgOutcome::success, *f);
    EXPECT_TRUE(server.thereIsMessageForUser(target.getSiteId(), response).first);
}


TEST_F(SymServerTestFilesystemFunctionality, openSourceOfNotLoggedUser){
    std::shared_ptr<file> f;
    ASSERT_THROW(f=server.openSource(anotherUserUsername, filePath, fileName, defaultPrivilege), SymServerException);
    EXPECT_FALSE(server.userIsWorkingOnDocument(anotherUser, doc, defaultPrivilege));
    /*
     * Cases in which, for an error, the operation goes wrong, so an exception is raised, are to be handled
     * externally, in the module that controls the connection
     */
}

TEST_F(SymServerTestFilesystemFunctionality, logoutClosesOpenedDocumentsAndPropagateChanges){
    server.forceSiteIdForResId(&doc, anotherUser);
    SymServerUserMock& target= dynamic_cast<SymServerUserMock&>(server.getRegistered(loggedUserUsername));
    EXPECT_CALL(target, openFile(filePath, fileName, defaultPrivilege)).WillOnce(::testing::Return(fileToReturn));
    EXPECT_CALL(*fileToReturn, access(loggedUser, defaultPrivilege)).WillOnce(::testing::ReturnRef(doc));
    auto f=server.openSource(loggedUserUsername, filePath, fileName, defaultPrivilege);
    ASSERT_TRUE(server.userIsWorkingOnDocument(loggedUser, doc, defaultPrivilege));

    updateActiveMessage toSend(msgType::removeActiveUser, msgOutcome::success, loggedUser.makeCopyNoPwd(), doc.getId());


    server.logout(loggedUserUsername, loggedUserPwd);
    EXPECT_FALSE(server.userIsWorkingOnDocument(loggedUser, doc, defaultPrivilege));
    ASSERT_NO_FATAL_FAILURE(messageAssociatedWithRightUsers({anotherUser.getSiteId()}, toSend, {loggedUser.getSiteId()}));
}

TEST_F(SymServerTestFilesystemFunctionality, removeUserClosesOpenedDocuments){
    server.forceSiteIdForResId(&doc, anotherUser);
    SymServerUserMock& target= dynamic_cast<SymServerUserMock&>(server.getRegistered(loggedUserUsername));
    EXPECT_CALL(target, openFile(filePath, fileName, defaultPrivilege)).WillOnce(::testing::Return(fileToReturn));
    EXPECT_CALL(*fileToReturn, access(loggedUser, defaultPrivilege)).WillOnce(::testing::ReturnRef(doc));
    auto f=server.openSource(loggedUserUsername, filePath, fileName, defaultPrivilege);
    ASSERT_TRUE(server.userIsWorkingOnDocument(loggedUser, doc, defaultPrivilege));

    updateActiveMessage toSend(msgType::removeActiveUser, msgOutcome::success, loggedUser.makeCopyNoPwd(), doc.getId());

    server.removeUser(loggedUserUsername, loggedUserPwd);
    EXPECT_FALSE(server.userAlreadyRegistered(loggedUser));
    EXPECT_FALSE(server.userIsWorkingOnDocument(loggedUser, doc, defaultPrivilege));
    ASSERT_NO_FATAL_FAILURE(messageAssociatedWithRightUsers({anotherUser.getSiteId()}, toSend, {loggedUser.getSiteId()}));}

TEST_F(SymServerTestFilesystemFunctionality, openNewSourceAccessesTheFileAndGenerateCorrectResponse){
    server.forceSiteIdForResId(&doc, anotherUser);
    SymServerUserMock& target= dynamic_cast<SymServerUserMock&>(server.getRegistered(loggedUserUsername));
    EXPECT_CALL(target, accessFile(filePath + "/" + fileName, "./", fileName)).WillOnce(::testing::Return(fileToReturn));
    EXPECT_CALL(*fileToReturn, access(target, uri::getDefaultPrivilege())).WillOnce(::testing::ReturnRef(doc));
    auto f= server.openNewSource(loggedUserUsername, filePath + "/" + fileName, "./", fileName, defaultPrivilege);
    EXPECT_TRUE(server.userIsWorkingOnDocument(loggedUser, doc, defaultPrivilege));

    //Propagation of document opened to other clients
    updateActiveMessage toSend(msgType::addActiveUser, msgOutcome::success, loggedUser.makeCopyNoPwd(), doc.getId(), privilege::owner);
    ASSERT_NO_FATAL_FAILURE(messageAssociatedWithRightUsers({anotherUser.getSiteId()}, toSend, {loggedUser.getSiteId()}));

    //response
    sendResMessage response(msgType::openNewRes, msgOutcome::success, *f);
    EXPECT_TRUE(server.thereIsMessageForUser(target.getSiteId(), response).first);
}

TEST_F(SymServerTestFilesystemFunctionality, openNewSourceOfNotLoggedUser){
    std::shared_ptr<file> f;
    ASSERT_THROW(f= server.openNewSource(anotherUserUsername, filePath + "/" + fileName, "./", fileName, defaultPrivilege), SymServerException);
    EXPECT_FALSE(server.userIsWorkingOnDocument(anotherUser, doc, defaultPrivilege));
    /*
     * Cases in which, for an error, the operation goes wrong, so an exception is raised, are to be handled
     * externally, in the module that controls the connection
     */
}

TEST_F(SymServerTestFilesystemFunctionality, createNewSourceCallsNewFile){
    SymServerUserMock& target= dynamic_cast<SymServerUserMock&>(server.getRegistered(loggedUserUsername));
    EXPECT_CALL(target, newFile(fileName, filePath,0)).WillOnce(::testing::Return(fileToReturn));
    EXPECT_CALL(*fileToReturn, access(loggedUser, privilege::owner)).WillOnce(::testing::ReturnRef(doc));
    auto doc=server.createNewSource(loggedUserUsername, filePath, fileName);
    EXPECT_TRUE(server.userIsWorkingOnDocument(loggedUser, doc, privilege::owner));
}

TEST_F(SymServerTestFilesystemFunctionality, createNewSourceOfUnloggedUser){
    EXPECT_THROW(server.createNewSource(anotherUserUsername, filePath, fileName), SymServerException);
}

TEST_F(SymServerTestFilesystemFunctionality, createNewDirCallsNewDirectory){
    EXPECT_CALL(*inserted, newDirectory(fileName, filePath,-1));
    server.createNewDir(loggedUserUsername, filePath, fileName);
}

TEST_F(SymServerTestFilesystemFunctionality, createNewDirOfUnloggedUser){
    EXPECT_THROW(server.createNewDir(anotherUserUsername, filePath, fileName), SymServerException);
}

TEST_F(SymServerTestFilesystemFunctionality, remoteInsertCallsRemoteInsertOnDocAndPropagateChanges){
    setStageForOpenedDocForLoggedUser();
    std::initializer_list<int> siteIds{anotherUser.getSiteId()};
    server.forceSiteIdForResId(&doc, anotherUser);

    symbol toInsert('a', 0, 0, {}, false);
    symbolMessage received(msgType::insertSymbol, {loggedUserUsername, loggedUserPwd}, msgOutcome::success, 0, doc.getId(), toInsert);
    //server must send and insert a symbol that is verified, but without the authentication parameters
    symbolMessage toSend(msgType::insertSymbol, {{}, {}}, msgOutcome::success, 0, doc.getId(), toInsert.setVerified(), received.getMsgId());
    EXPECT_CALL(doc, remoteInsert(toSend.getSym()));
    server.remoteInsert(loggedUserUsername, doc.getId(), received);
    auto res= server.thereIsMessageForUser(*siteIds.begin(), toSend);
    ASSERT_NO_FATAL_FAILURE(messageAssociatedWithRightUsers(siteIds, toSend, {loggedUser.getSiteId()}));
    EXPECT_PRED_FORMAT2(symbolMessageInQueueIsCorrect, res, toSend);
}

TEST_F(SymServerTestFilesystemFunctionality, remoteInsertCallsRemoteInsertOnDocAndGenerateCorrectResponse){
    setStageForOpenedDocForLoggedUser();
    std::initializer_list<int> siteIds{anotherUser.getSiteId()};
    server.forceSiteIdForResId(&doc, anotherUser);

    symbol symSent('a', 0, 0, {}, false);
    symbol toInsert=symSent.setVerified();
    symbolMessage received(msgType::insertSymbol, {loggedUserUsername, loggedUserPwd}, msgOutcome::success, 0, doc.getId(), symSent);
    EXPECT_CALL(doc, remoteInsert(toInsert));
    server.remoteInsert(loggedUserUsername, doc.getId(), received);

    //send response (confirmation)
    serverMessage response(msgType::insertSymbol,msgOutcome::success);
    EXPECT_TRUE(server.thereIsMessageForUser(loggedUser.getSiteId(), response).first);
}

TEST_F(SymServerTestFilesystemFunctionality, remoteInsertOfUnloggedUser){
    symbol toInsert('a', 0, 0, {}, false);
    symbolMessage received(msgType::insertSymbol, {anotherUserUsername, anotherUserPwd}, msgOutcome::success, 0, doc.getId(), toInsert);
    EXPECT_THROW(server.remoteInsert(anotherUserUsername, doc.getId(), received), SymServerException);
    /*
     * Cases in which, for an error, the operation goes wrong, so an exception is raised, are to be handled
     * externally, in the module that controls the connection
     */
}

TEST_F(SymServerTestFilesystemFunctionality, remoteInsertOnDocumentNotOpened){
    symbol toInsert('a', 0, 0, {}, false);
    symbolMessage received(msgType::insertSymbol, {loggedUserUsername, loggedUserPwd}, msgOutcome::success, 0, doc.getId(), toInsert);
    EXPECT_THROW(server.remoteInsert(loggedUserUsername, doc.getId(), received), SymServerException);
    /*
     * Cases in which, for an error, the operation goes wrong, so an exception is raised, are to be handled
     * externally, in the module that controls the connection
     */
}

TEST_F(SymServerTestFilesystemFunctionality, remoteRemoveCallsRemoteRemoveOnDocAndInsertMessageInQueue){
    setStageForOpenedDocForLoggedUser();
    std::initializer_list<int> siteIds{anotherUser.getSiteId()};
    server.forceSiteIdForResId(&doc, anotherUser);

    symbol toRemove('a', 0, 0, {}, false);
    symbolMessage received(msgType::removeSymbol, {loggedUserUsername, loggedUserPwd}, msgOutcome::success, 0, doc.getId(), toRemove);
    symbolMessage toSend(msgType::removeSymbol, {{}, {}}, msgOutcome::success, 0, doc.getId(), toRemove.setVerified(), received.getMsgId());
    EXPECT_CALL(doc, remoteRemove(toSend.getSym()));
    server.remoteRemove(loggedUserUsername, doc.getId(), received);

    auto res= server.thereIsMessageForUser(*siteIds.begin(), toSend);
    ASSERT_NO_FATAL_FAILURE(messageAssociatedWithRightUsers(siteIds, toSend, {loggedUser.getSiteId()}));
    EXPECT_PRED_FORMAT2(symbolMessageInQueueIsCorrect, res, toSend);
}

TEST_F(SymServerTestFilesystemFunctionality, remoteRemoveOfUnloggedUser){
    symbol toRemove('a', 0, 0, {}, false);
    symbolMessage received(msgType::removeSymbol, {anotherUserUsername, anotherUserPwd}, msgOutcome::success, 0, doc.getId(), toRemove);
    EXPECT_THROW(server.remoteRemove(anotherUserUsername, doc.getId(), received), SymServerException);
}

TEST_F(SymServerTestFilesystemFunctionality, remoteRemoveOnDocumentNotOpened){
    symbol toRemove('a', 0, 0, {}, false);
    symbolMessage received(msgType::removeSymbol, {loggedUserUsername, loggedUserPwd}, msgOutcome::success, 0, doc.getId(), toRemove);
    EXPECT_THROW(server.remoteInsert(loggedUserUsername, doc.getId(), received), SymServerException);
}

TEST_F(SymServerTestFilesystemFunctionality, closeSourceClosesTheDocumentAndInsertMessageInQueue){
    server.forceSiteIdForResId(&doc, anotherUser);
    setStageForAccessedDoc(loggedUser);
    user loggedUserPurged=loggedUser.makeCopyNoPwd();
    updateDocMessage received(msgType::closeRes, {loggedUserUsername, loggedUserPwd}, doc.getId());
    updateActiveMessage toSend(msgType::removeActiveUser, msgOutcome::success, loggedUserPurged, doc.getId(), privilege::owner, received.getMsgId());
    EXPECT_CALL(doc, close(loggedUser));
    SymServerUserMock& target= dynamic_cast<SymServerUserMock&>(server.getRegistered(anotherUserUsername));
    server.closeSource(loggedUserUsername, received.getResourceId());
    EXPECT_FALSE(server.userIsWorkingOnDocument(target, doc, defaultPrivilege));

    ASSERT_NO_FATAL_FAILURE(messageAssociatedWithRightUsers({anotherUser.getSiteId()}, toSend, {loggedUser.getSiteId()}));
}


TEST_F(SymServerTestFilesystemFunctionality, editPrivilegeCallsEditPrivilegeOnUserAndInsertsInQueue){
    user thirdUser("ThirdUser", loggedUserPwd, "nickname", "./thisDir/a.jpg", 10, nullptr);
    setStageForAccessedDoc(loggedUser);
    setAnotherUserActive();
    makeAnotherUserToHavePrivilegeAndCloseSource(defaultPrivilege);
    std::initializer_list<int> siteIds{thirdUser.getSiteId()};
    /*
     * Let's suppose that a message like that has been received by the server
     */
    privMessage received(msgType::changePrivileges, {loggedUserUsername,loggedUserPwd}, msgOutcome::success, std::to_string(doc.getId()), anotherUserUsername, privilege::readOnly);
    server.forceSiteIdForResId(&doc, thirdUser);
    EXPECT_CALL(*justInserted, openFile(filePath, fileName, privilege::owner)).WillOnce(::testing::Return(fileToReturn));
    EXPECT_CALL(*fileToReturn, access(loggedUser, privilege::owner)).WillOnce(::testing::ReturnRef(doc));
    EXPECT_CALL(*justInserted, editPrivilege(anotherUserUsername, filePath, fileName, privilege::readOnly));

    server.editPrivilege(loggedUserUsername, anotherUserUsername, "./"+loggedUserUsername+"/"+filePath.substr(2), fileName, privilege::readOnly);
    privMessage toSend(received);
    toSend.clearAuthParam();

    /*
     * A message is supposed to be inserted in the queue corresponding to the file's document
     * This means that only the client that have the document opened will receive that message
     * This is right because the clients other than the creator have only the symlink, that doesn't
     * carry information about privileges on the file it points to.
     * What can happen is that a privilege a user has on a file (and he's not the creator) is changed,
     * so other client should be informed of this. Two ways:
     * - for every file of every active user, create something like a subscription list, e.g. a list of
     *   users that should be informed. This way we know the users associated in some way to a resource
     * - say that all the information about a file refers to the moment the file has been downloaded, and
     *   that to be updated you must open the file.
     */
    ASSERT_NO_FATAL_FAILURE(messageAssociatedWithRightUsers(siteIds, toSend, {loggedUser.getSiteId()}));
}

TEST_F(SymServerTestFilesystemFunctionality, editPrivilegeClosesDocumentIfNotPreviouslyOpened){
    setStageForAccessedDoc(loggedUser);
    setAnotherUserActive();
    makeAnotherUserToHavePrivilegeAndCloseSource(defaultPrivilege);
    makeLoggedUserNotWorkingOnDoc();
    user loggedUserPurged=loggedUser.makeCopyNoPwd();

    EXPECT_CALL(*justInserted, openFile(filePath, fileName, privilege::owner)).WillOnce(::testing::Return(fileToReturn));
    EXPECT_CALL(*fileToReturn, access(loggedUser, privilege::owner)).WillOnce(::testing::ReturnRef(doc));
    EXPECT_CALL(*justInserted, editPrivilege(anotherUserUsername, filePath, fileName, privilege::readOnly));
    EXPECT_CALL(doc, close(loggedUser));
    server.editPrivilege(loggedUserUsername, anotherUserUsername, "./"+loggedUserUsername+"/"+filePath.substr(2), fileName, privilege::readOnly);
}

TEST_F(SymServerTestFilesystemFunctionality, editPrivilegeCalledByUnloggedUser){
    setStageForAccessedDoc(loggedUser);
    setAnotherUserActive();
    makeAnotherUserToHavePrivilegeAndCloseSource(defaultPrivilege);
    server.hardWrongLogout(loggedUser);

    EXPECT_THROW(server.editPrivilege(loggedUserUsername, anotherUserUsername, "./"+loggedUserUsername+"/"+filePath.substr(2), fileName, privilege::readOnly), SymServerException);
}

TEST_F(SymServerTestFilesystemFunctionality, editPrivilegeOnUserWorkingOnDocument){
    setStageForAccessedDoc(loggedUser);
    setAnotherUserActive();
    makeAnotherUserToHavePrivilege(defaultPrivilege);
    //call expected because we need to retrieve the document id of the document named fileName (in the loggedUser space)
    //to check that anotherUser is not working on the same document
    EXPECT_CALL(*justInserted, openFile(filePath, fileName, privilege::owner)).WillOnce(::testing::Return(fileToReturn));
    EXPECT_CALL(*fileToReturn, access(loggedUser, privilege::owner)).WillOnce(::testing::ReturnRef(doc));
    EXPECT_THROW(server.editPrivilege(loggedUserUsername, anotherUserUsername, "./"+loggedUserUsername+"/"+filePath.substr(2), fileName, privilege::readOnly), SymServerException);
}

TEST_F(SymServerTestFilesystemFunctionality, shareResourceCallsShareResourceOnUserAndInsertInQueue){
    uri newPref(uriPolicy::activeAlways);
    /*
     * Let's suppose that a message like that has been received by the server
     */
    uriMessage received(msgType::shareRes, {loggedUserUsername, loggedUserPwd}, msgOutcome ::success, filePath, fileName, newPref);
    uriMessage toSend(received);
    toSend.clearAuthParam();
    server.forceSiteIdForResId(&(fileToReturn->getDoc()), anotherUser);
    EXPECT_CALL(loggedUser, shareResource(filePath, fileName, newPref)).WillOnce(::testing::Return(fileToReturn));
    auto file=server.shareResource(loggedUser, filePath, fileName, newPref);
    /*
     * A message is supposed to be inserted in the queue corresponding to the file's document
     * This means that only the client that have the document opened will receive that message
     * This is right because the client other than the creator have only the symlink, that doesn't
     * carry information about sharing preferences of the file it points to.
     */
    ASSERT_NO_FATAL_FAILURE(messageAssociatedWithRightUsers({anotherUser.getSiteId()}, toSend, {loggedUser.getSiteId()}));
}

TEST_F(SymServerTestFilesystemFunctionality, shareResourceOfUnLoggedUser){
    uri newPref(uriPolicy::activeAlways);
    EXPECT_THROW(server.shareResource(anotherUser, filePath, fileName, newPref), SymServerException);
}

TEST_F(SymServerTestFilesystemFunctionality, renameResourceCallsRenameResourceOnUser){
    setStageForAccessedDoc(loggedUser);
    EXPECT_CALL(loggedUser, renameResource(filePath, fileName, "newName")).WillOnce(::testing::Return(fileToReturn));
    auto ret=server.renameResource(loggedUserUsername, filePath, fileName, "newName");
    EXPECT_EQ(fileToReturn, ret);
}

TEST_F(SymServerTestFilesystemFunctionality, renameResourceByUnloggedUser){
    EXPECT_THROW(server.renameResource(anotherUserUsername, filePath, fileName, "newName"), SymServerException);
}

TEST_F(SymServerTestFilesystemFunctionality, removeResourceCallsResourceFileOnUser){
    setStageForAccessedDoc(loggedUser);
    EXPECT_CALL(loggedUser, removeResource(filePath, fileName)).WillOnce(::testing::Return(fileToReturn));
    auto ret=server.removeResource(loggedUserUsername, filePath, fileName);
    EXPECT_EQ(fileToReturn, ret);
}

TEST_F(SymServerTestFilesystemFunctionality, removeResourceByUnloggedUser){
    EXPECT_THROW(server.removeResource(anotherUserUsername, filePath, fileName), SymServerException);
}

TEST_F(SymServerTestFilesystemFunctionality, mapSiteIdToUserCallsRetrieveSiteIdsOnDoc){
    setStageForAccessedDoc(loggedUser);
    EXPECT_CALL(doc,retrieveSiteIds());
    server.mapSiteIdToUser(loggedUserUsername, doc.getId());
}

TEST_F(SymServerTestFilesystemFunctionality, mapSiteIdToUserOnClosedDoc){
    EXPECT_THROW(server.mapSiteIdToUser(loggedUserUsername, doc.getId()),SymServerException);
}

TEST_F(SymServerTestFilesystemFunctionality, mapSiteIdToUserCorrectMapping){
    setStageForAccessedDoc(loggedUser);
    setAnotherUserActive();
    makeAnotherUserToHavePrivilege(defaultPrivilege);
    std::set<int> siteIdsToReturn({loggedUser.getSiteId(), anotherUser.getSiteId()});
    EXPECT_CALL(doc,retrieveSiteIds()).WillOnce(::testing::Return(siteIdsToReturn));
    auto mapping=server.mapSiteIdToUser(loggedUserUsername, doc.getId());
    std::map<int, user> expected({
        std::pair<int, user>(loggedUser.getSiteId(), loggedUser),
        std::pair<int, user>(anotherUser.getSiteId(), anotherUser)});
    EXPECT_EQ(expected, mapping);
}

TEST_F(SymServerTestFilesystemFunctionality, mapSiteIdWithUnknownSiteId){
    setStageForAccessedDoc(loggedUser);
    setAnotherUserActive();
    makeAnotherUserToHavePrivilege(defaultPrivilege);
    //loggedUser and anotherUser have siteId 0 and 1,
    // so 2 and 3 are siteId without association with registered users in SymServer
    std::set<int> siteIdsToReturn({loggedUser.getSiteId(), anotherUser.getSiteId(), 2,3});
    EXPECT_CALL(doc,retrieveSiteIds()).WillOnce(::testing::Return(siteIdsToReturn));
    auto mapping=server.mapSiteIdToUser(loggedUserUsername, doc.getId());
    std::map<int, user> expected({
                                         std::pair<int, user>(loggedUser.getSiteId(), loggedUser),
                                         std::pair<int, user>(anotherUser.getSiteId(), anotherUser),
                                         std::pair<int, user>(2, SymServer::unknownUser),
                                         std::pair<int, user>(3, SymServer::unknownUser)});
    EXPECT_EQ(expected, mapping);
}