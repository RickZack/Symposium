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

#pragma clang diagnostic ignored "cert-err58-cpp" //NOLINT
#pragma clang diagnostic ignored "cert-msc50-cpp" //NOLINT

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <typeinfo>
#include "../SymServer.h"
#include "../filesystem.h"

using namespace Symposium;

struct SymServerdirMock : public directory{
    explicit SymServerdirMock(const std::string &name) : directory(name) {};
    MOCK_METHOD2(addDirectory, std::shared_ptr<directory>(const std::string &name, uint_positive_cnt::type idToAssign));
    MOCK_METHOD2(get, std::shared_ptr<filesystem>(const std::string&, const std::string&));
    MOCK_METHOD2(getFile, std::shared_ptr<file>(const std::string&, const std::string&));
    MOCK_METHOD2(getDir, std::shared_ptr<directory>(const std::string &path, const std::string &name));
    MOCK_METHOD3(remove, std::shared_ptr<filesystem>(const user &targetUser, const std::string &path, const std::string &resName));
};

struct SymServerUserMock: public user{
    SymServerUserMock() = default;
    SymServerUserMock(const std::string &username, const std::string &pwd, const std::string &nickname,
                      const std::string &iconPath, int siteId, const std::shared_ptr<directory>& home) : user(username, pwd,
                                                                                                       nickname,
                                                                                                       iconPath, siteId,
                                                                                                       home) {};
    SymServerUserMock(const SymServerUserMock& other): user(other){};
    MOCK_CONST_METHOD4(accessFile,  std::pair<int, std::shared_ptr<file>>(const std::string &resId, const std::string &path,  const std::string &fileName, privilege));
    MOCK_CONST_METHOD3(openFile, std::shared_ptr<file>(const std::string &path,  const std::string &fileName, privilege accessMode));
    MOCK_CONST_METHOD3(newFile, std::shared_ptr<file>(const std::string& fileName, const std::string& pathFromHome, uint_positive_cnt::type));
    MOCK_CONST_METHOD3(newDirectory, std::shared_ptr<directory>(const std::string& dirName, const std::string& pathFromHome, uint_positive_cnt::type id));
    MOCK_CONST_METHOD4(editPrivilege, privilege(const std::string &otherUser, const std::string &resPath, const std::string &resName,
            privilege newPrivilege));
    MOCK_CONST_METHOD3(shareResource, std::shared_ptr<filesystem>(const std::string &resPath, const std::string &resName, const uri& newPrefs));
    MOCK_CONST_METHOD3(renameResource, std::shared_ptr<filesystem>(const std::string& resPath, const std::string& resName, const std::string& newName));
    MOCK_CONST_METHOD2(removeResource, std::shared_ptr<filesystem>(const std::string &path, const std::string &name));
    MOCK_CONST_METHOD1(hasPwd, bool(const std::string& pwd));
    MOCK_METHOD1(setNewData, void(const user&));
};

struct SymServerFileMock: public file{
    SymServerFileMock() : file("test", 0) {}
    void forceContent(const document& d){
        const_cast<document&>(this->getDoc())=d;
    }
    MOCK_METHOD2(access, document&(const user &targetUser, privilege accessMode));
    MOCK_CONST_METHOD1(getUserPrivilege, privilege(const std::string&));
};

struct SymServerDocMock: public document{
    SymServerDocMock():document(){};
    SymServerDocMock(const SymServerDocMock& mock) : document(mock) {};
    MOCK_METHOD2(remoteInsert, std::pair<unsigned int, unsigned int>(uint_positive_cnt::type siteId, const symbol &toInsert));
    MOCK_METHOD2(remoteRemove, std::pair<unsigned int, unsigned int>(uint_positive_cnt::type siteId, const symbol &toRemove));
    MOCK_METHOD1(close, void(const user& noLongerActive));
    MOCK_CONST_METHOD0(retrieveSiteIds, std::set<uint_positive_cnt::type>());

    MOCK_METHOD3(updateCursorPos, void(unsigned, unsigned, unsigned));
    MOCK_METHOD2(editLineStyle, void(const std::pair<alignType, unsigned>&, unsigned));
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
    SymServerAccesser() : SymServer(false, false){
        d1=new document(10);
        d2=new document(20);
    }

    ~SymServerAccesser() override {
        delete d1;
        delete d2;
    }

    bool userIsActive(const std::string& username, const user& toCheck){
        auto entry=active.find(username);
        bool found= entry!=active.end() &&
                *entry->second==toCheck;
        return found && userIsActive(username);
    }
    void setRoot(const std::shared_ptr<directory>& newRoot){
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
            auto m=messages.front().get();
            if(m->getMsgId()==msg.getMsgId() && m->getAction()==msg.getAction()
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
    user &registerUser(const user &toInsert) override {
        std::string username=toInsert.getUsername();
        const auto* mocked= dynamic_cast<const SymServerUserMock*>(&toInsert);
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
    const user& findUserBySiteId(int id) const noexcept override {
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
    uint_positive_cnt::type msId;

    SymServerTestUserFunctionality():
    fakeDir(new ::testing::NiceMock<SymServerdirMock>("/")),
    userDir(new ::testing::NiceMock<SymServerdirMock>(newUserUsername)),
    newUser(newUserUsername, newUserPwd, "m@ario", validIconPath, 0, nullptr),
    alreadyPresent(newUser),
    newDifferentUser("lucio", "a123@bty!!", "lupoLucio", validIconPath, 0, nullptr),
    server(){
        server.setRoot(fakeDir);
        msId=rand()+1;
    };
    ~SymServerTestUserFunctionality() override = default;
};
const std::string SymServerTestUserFunctionality::validIconPath=":/resources/avatar/unknown.png";
const std::string SymServerTestUserFunctionality::newUserUsername="mario";
const std::string SymServerTestUserFunctionality::newUserPwd="a123@bty!!";
const std::string SymServerTestUserFunctionality::wrongPwd="wrong";

TEST_F(SymServerTestUserFunctionality, addUserAddingUserNotPresent){
    server.addUser(newUser, msId);
    EXPECT_TRUE(server.userAlreadyRegistered(newUser));
}

TEST_F(SymServerTestUserFunctionality, addUserAddingUserAlreadyPresent){
    server.addUser(newUser, msId);
    EXPECT_THROW(server.addUser(alreadyPresent, msId), SymServerException);
}

TEST_F(SymServerTestUserFunctionality, addUserAddingUserHavingWrongIconPath){
    newUser.setIconPath("");
    EXPECT_THROW(server.addUser(newUser, msId), SymServerException);
}

TEST_F(SymServerTestUserFunctionality, addUserAddingUserHavingWrongNickname){
    newUser.setNickname("");
    EXPECT_THROW(server.addUser(newUser, msId), SymServerException);
}

TEST_F(SymServerTestUserFunctionality, addUserAssignesHomeToUser){
    EXPECT_CALL(*fakeDir, addDirectory(newUserUsername, ::testing::_)).WillOnce(::testing::Return(userDir));
    auto userInserted= server.addUser(newUser, msId);
    EXPECT_TRUE(userInserted.getHome()==userDir);
}

TEST_F(SymServerTestUserFunctionality, addUserGivesDifferentSiteId){
    auto newUserComplete= server.addUser(newUser, msId);
    auto newDifferentUserComplete= server.addUser(newDifferentUser, msId);
    EXPECT_NE(newUserComplete.getSiteId(), newDifferentUserComplete.getSiteId());
}

//Added in 8/04/2020, after check on the flow and noticing that adding in queue was missing
TEST_F(SymServerTestUserFunctionality, addUserGenerateCorrectResponse){
    auto userInserted= server.addUser(newUser, msId);
    loginMessage response(msgType::registration, msgOutcome::success, userInserted, msId);
    EXPECT_TRUE(server.thereIsMessageForUser(userInserted.getSiteId(), response).first);
}

TEST_F(SymServerTestUserFunctionality, loginOfRegisteredUserGenerateCorrectResponse){
    const auto& inserted= dynamic_cast<const SymServerUserMock&>(server.addUser(newUser, 0));
    EXPECT_CALL(inserted, hasPwd(newUserPwd)).WillOnce(::testing::Return(true));
    auto logged= server.login(newUserUsername, newUserPwd, msId);
    EXPECT_TRUE(server.userIsActive(newUserUsername, logged));

    loginMessage response(msgType::login, msgOutcome::success, logged, msId);
    EXPECT_TRUE(server.thereIsMessageForUser(logged.getSiteId(), response).first);
}

TEST_F(SymServerTestUserFunctionality, loginOfRegisteredUserWithWrongPwd){
    const auto& inserted= dynamic_cast<const SymServerUserMock&>(server.addUser(newUser, 0));
    EXPECT_CALL(inserted, hasPwd(wrongPwd)).WillOnce(::testing::Return(false));
    user logged;
    EXPECT_THROW(logged= server.login(newUserUsername, wrongPwd, 0), SymServerException);
    EXPECT_FALSE(server.userIsActive(newUserUsername, logged));
    /*
     * Cases in which, for an error, the operation goes wrong, so an exception is raised, are to be handled
     * externally, in the module that controls the connection
     */
}

TEST_F(SymServerTestUserFunctionality, loginOfAlreadyLoggedUser){
    const auto& inserted= dynamic_cast<const SymServerUserMock&>(server.addUser(newUser, 0));
    EXPECT_CALL(inserted, hasPwd(newUserPwd)).WillRepeatedly(::testing::Return(true));
    server.login(newUserUsername, newUserPwd, 0);
    EXPECT_THROW(server.login(newUserUsername, newUserPwd, 0), SymServerException);
    /*
     * Cases in which, for an error, the operation goes wrong, so an exception is raised, are to be handled
     * externally, in the module that controls the connection
     */
}

TEST_F(SymServerTestUserFunctionality, loginOfUnregisteredUser){
    std::pair<std::string, std::string> userCredentials={newUserUsername, newUserPwd};
    EXPECT_THROW(server.login(userCredentials.first, userCredentials.second, 0), SymServerException);
    EXPECT_THROW(server.login(newUserUsername, newUserPwd, 0), SymServerException);
    /*
     * Cases in which, for an error, the operation goes wrong, so an exception is raised, are to be handled
     * externally, in the module that controls the connection
     */
}

TEST_F(SymServerTestUserFunctionality, logoutRemovesUserFromActiveAndGenerateCorrectResponse){
    const auto& inserted= dynamic_cast<const SymServerUserMock&>(server.addUser(newUser, 0));
    EXPECT_CALL(inserted, hasPwd(newUserPwd)).WillOnce(::testing::Return(true));
    server.login(newUserUsername, newUserPwd, 0);
    ASSERT_TRUE(server.userIsActive(newUserUsername, newUser));
    server.logout(newUserUsername, msId);
    EXPECT_FALSE(server.userIsActive(newUserUsername, newUser));

    serverMessage response(msgType::logout, msgOutcome::success, msId);
    EXPECT_TRUE(server.thereIsMessageForUser(inserted.getSiteId(), response).first);
}

TEST_F(SymServerTestUserFunctionality, removeUserRemovesUserFromRegistered){
    EXPECT_CALL(*fakeDir, addDirectory(newUserUsername, 0)).WillOnce(::testing::Return(userDir));
    const auto& inserted= dynamic_cast<const SymServerUserMock&>(server.addUser(newUser, 0));
    ASSERT_TRUE(server.userAlreadyRegistered(newUser));
    EXPECT_CALL(inserted, hasPwd(newUserPwd)).WillOnce(::testing::Return(true));
    EXPECT_CALL(*fakeDir, remove(inserted, "./", std::to_string(userDir->getId())));
    server.removeUser(newUserUsername, newUserPwd, msId);
    EXPECT_FALSE(server.userAlreadyRegistered(newUser));

    serverMessage response(msgType::removeUser, msgOutcome::success, msId);
    EXPECT_TRUE(server.thereIsMessageForUser(inserted.getSiteId(), response).first);
}

TEST_F(SymServerTestUserFunctionality, editUserCallSetUserDataAndGenerateCorrectResponse){
    auto& oldUser= dynamic_cast<SymServerUserMock&>(const_cast<user&>(server.addUser(newUser, 0)));
    EXPECT_CALL(oldUser, hasPwd(newUserPwd)).WillOnce(::testing::Return(true));
    server.login(newUserUsername, newUserPwd, 0);

    user newData("giuseppe", "123@pwd@!", "peppuccio63", validIconPath, 0, nullptr);
    EXPECT_CALL(oldUser, setNewData(newData));
    server.editUser(newUserUsername, newData, msId);

    serverMessage response(msgType::changeUserData, msgOutcome::success, msId);
    EXPECT_TRUE(server.thereIsMessageForUser(oldUser.getSiteId(), response).first);
}

TEST_F(SymServerTestUserFunctionality, editUserChangesUserDataAndPropagateChanges){
    auto& oldUser= dynamic_cast<SymServerUserMock&>(const_cast<user&>(server.addUser(newUser, msId)));
    EXPECT_CALL(oldUser, hasPwd(newUserPwd)).WillOnce(::testing::Return(true));
    server.login(newUserUsername, newUserPwd, 0);
    server.setStageForDocOpenedByTwoUsers(newUser, newDifferentUser);
    user newData("giuseppe", "123@pwd@!", "peppuccio63", validIconPath, 0, nullptr);
    EXPECT_CALL(oldUser, setNewData(newData));

    const user& inserted= server.editUser(newUserUsername, newData, msId);

    /*
     * Change in a user's data must be propagated to clients that can be interested
     * in this user.
     * This means that all the clients that are working on documents in which the current
     * user is involved must be notified.
     */
    userDataMessage toSend(msgType::changeUserData, {newUserUsername, newUserPwd}, msgOutcome::success, newData, msId);
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
    uint_positive_cnt::type msId;

    SymServerTestFilesystemFunctionality():
            loggedUser(loggedUserUsername, loggedUserPwd, "m@ario", ":/resources/avatar/test.jpg", 1, nullptr),
            anotherUser(anotherUserUsername, anotherUserPwd, "lupoLucio", ":/resources/avatar/test.jpg", 2, nullptr),
            server(),
            userDir(new ::testing::NiceMock<SymServerdirMock>("/")),
            fakeDir(new ::testing::NiceMock<SymServerdirMock>("/")),
            fileToReturn(new ::testing::NiceMock<SymServerFileMock>()){

        server.setRoot(fakeDir);
        EXPECT_CALL(*fakeDir, addDirectory(loggedUserUsername, 0)).WillOnce(::testing::Return(userDir));
        inserted= &dynamic_cast<const SymServerUserMock&>(server.addUser(loggedUser, 0));
        EXPECT_CALL(*inserted, hasPwd(loggedUserPwd)).WillOnce(::testing::Return(true));
        server.login("mario", "a123@bty!!", 0);
        msId=rand()+1;
        justInserted=nullptr;
        fileToReturn->forceContent(doc);
    };

    /*
     * Make the owner user to access the file and open the document. Used to test that closeSource actually works
     */
    void setStageForAccessedDoc(SymServerUserMock& userWhoOpens){
        auto& target= dynamic_cast<SymServerUserMock&>(server.getRegistered(loggedUserUsername));
        EXPECT_CALL(target, accessFile(filePath + "/" + fileName, "./", fileName, privilege::owner)).WillOnce(::testing::Return(std::pair<int, std::shared_ptr<file>>(0, fileToReturn)));
        EXPECT_CALL(*fileToReturn, getUserPrivilege(target.getUsername())).WillOnce(::testing::Return(privilege::owner));
        EXPECT_CALL(*fileToReturn, access(target, privilege::owner)).WillOnce(::testing::ReturnRef(doc));
        auto ret= server.openNewSource(target.getUsername(), filePath + "/" + fileName, "./", fileName,
                                       privilege::owner, 0);
        ASSERT_TRUE(server.userIsWorkingOnDocument(target, doc, privilege::owner));
        justInserted=&target;
    }
    void setAnotherUserActive(){
        EXPECT_CALL(*fakeDir, addDirectory(anotherUserUsername, 0)).WillOnce(::testing::Return(userDir));
        inserted= &dynamic_cast<const SymServerUserMock&>(server.addUser(anotherUser, 0));
        EXPECT_CALL(*inserted, hasPwd(anotherUserPwd)).WillOnce(::testing::Return(true));
        server.login(anotherUserUsername, anotherUserPwd, 0);
        ASSERT_TRUE(server.userIsActive(anotherUserUsername, anotherUser));
    }

    void setStageForOpenedDocForLoggedUser(){
        auto& target= dynamic_cast<SymServerUserMock&>(server.getRegistered(loggedUserUsername));
        EXPECT_CALL(target, openFile(filePath, fileName, defaultPrivilege)).WillOnce(::testing::Return(fileToReturn));
        EXPECT_CALL(*fileToReturn, access(loggedUser, defaultPrivilege)).WillOnce(::testing::ReturnRef(doc));
        auto ret= server.openSource(loggedUserUsername, filePath, fileName, privilege::modify, 0);
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
        auto& target= dynamic_cast<SymServerUserMock&>(server.getRegistered(anotherUserUsername));
        EXPECT_CALL(target, accessFile(filePath + "/" + fileName, "./", fileName, priv)).WillOnce(::testing::Return(std::pair<int, std::shared_ptr<file>>(0, fileToReturn)));
        EXPECT_CALL(*fileToReturn, getUserPrivilege(target.getUsername())).WillOnce(::testing::Return(priv));
        EXPECT_CALL(*fileToReturn, access(target, uri::getDefaultPrivilege())).WillOnce(::testing::ReturnRef(doc));
        auto ret2= server.openNewSource(anotherUserUsername, filePath + "/" + fileName, "./", fileName, priv, 0);
    }

    void closeAfterPrivilegeAcquired(privilege priv){
        auto& target= dynamic_cast<SymServerUserMock&>(server.getRegistered(anotherUserUsername));
        EXPECT_CALL(doc, close(anotherUser));
        server.closeSource(anotherUserUsername, doc.getId(), 0);
        ASSERT_FALSE(server.userIsWorkingOnDocument(anotherUser, doc, priv));
    }

    void makeLoggedUserNotWorkingOnDoc(){
        auto& target= dynamic_cast<SymServerUserMock&>(server.getRegistered(loggedUserUsername));
        EXPECT_CALL(doc, close(loggedUser));
        server.closeSource(loggedUserUsername, doc.getId(), 0);
        ASSERT_FALSE(server.userIsWorkingOnDocument(anotherUser, doc, privilege::owner));
    }

    static ::testing::AssertionResult symbolMessageInQueueIsCorrect(const char* arg1, const char* arg2, const std::pair<bool, std::shared_ptr<message>>& res, const symbolMessage& toSend){
        if(!res.first)
            return ::testing::AssertionFailure()<<"The message is not present is the sending queue";
        if(std::dynamic_pointer_cast<symbolMessage>(res.second)->getActionOwner()!=std::pair<std::string, std::string>())
            return ::testing::AssertionFailure()<<"The actionOwner attribute is not clear"
            <<", the server should forward messages without the authentication parameters";
        if(!std::dynamic_pointer_cast<symbolMessage>(res.second)->getSym().isVerified())
            return ::testing::AssertionFailure()<<"The server should send the symbol as verified";
        return ::testing::AssertionSuccess();
    }

    void messageAssociatedWithRightUsers(const std::initializer_list<uint_positive_cnt::type>& siteIds, const serverMessage &toSend, const std::initializer_list<uint_positive_cnt::type>& siteIdsNoMex= {}){
        for(uint_positive_cnt::type s:siteIds) {
            EXPECT_TRUE(server.thereIsMessageForUser(s, toSend).first)
                                << "Message not associated with siteId " << s;
        }
        for(uint_positive_cnt::type s:siteIdsNoMex) {
            EXPECT_FALSE(server.thereIsMessageForUser(s, toSend).first)
                                << "Message is associated with siteId " << s << " while it shouldn't";
        }
    }

    ~SymServerTestFilesystemFunctionality() override = default;
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
    auto& target= dynamic_cast<SymServerUserMock&>(server.getRegistered(loggedUserUsername));
    EXPECT_CALL(target, openFile(filePath, fileName, defaultPrivilege)).WillOnce(::testing::Return(fileToReturn));
    EXPECT_CALL(*fileToReturn, access(loggedUser, defaultPrivilege)).WillOnce(::testing::ReturnRef(doc));
    auto f= server.openSource(loggedUserUsername, filePath, fileName, defaultPrivilege, msId);
    EXPECT_TRUE(server.userIsWorkingOnDocument(loggedUser, doc, defaultPrivilege));

    //Propagation of document opened to other clients
    updateActiveMessage toSend(msgType::addActiveUser, msgOutcome::success, loggedUser.makeCopyNoPwd(), doc.getId(), privilege::owner, msId);
    ASSERT_NO_FATAL_FAILURE(messageAssociatedWithRightUsers({anotherUser.getSiteId()}, toSend, {loggedUser.getSiteId()}));

    //Response to the client
    sendResMessage response(msgType::openRes, msgOutcome::success, f, 0, msId);
    EXPECT_TRUE(server.thereIsMessageForUser(target.getSiteId(), response).first);
}


TEST_F(SymServerTestFilesystemFunctionality, openSourceOfNotLoggedUser){
    std::shared_ptr<file> f;
    ASSERT_THROW(f= server.openSource(anotherUserUsername, filePath, fileName, defaultPrivilege, 0), SymServerException);
    EXPECT_FALSE(server.userIsWorkingOnDocument(anotherUser, doc, defaultPrivilege));
    /*
     * Cases in which, for an error, the operation goes wrong, so an exception is raised, are to be handled
     * externally, in the module that controls the connection
     */
}

TEST_F(SymServerTestFilesystemFunctionality, logoutClosesOpenedDocumentsAndPropagateChanges){
    server.forceSiteIdForResId(&doc, anotherUser);
    auto& target= dynamic_cast<SymServerUserMock&>(server.getRegistered(loggedUserUsername));
    EXPECT_CALL(target, openFile(filePath, fileName, defaultPrivilege)).WillOnce(::testing::Return(fileToReturn));
    EXPECT_CALL(*fileToReturn, access(loggedUser, defaultPrivilege)).WillOnce(::testing::ReturnRef(doc));
    auto f= server.openSource(loggedUserUsername, filePath, fileName, defaultPrivilege, 0);
    ASSERT_TRUE(server.userIsWorkingOnDocument(loggedUser, doc, defaultPrivilege));

    updateActiveMessage toSend(msgType::removeActiveUser, msgOutcome::success, loggedUser.makeCopyNoPwd(), doc.getId(), privilege::readOnly, msId);


    server.logout(loggedUserUsername, msId);
    EXPECT_FALSE(server.userIsWorkingOnDocument(loggedUser, doc, defaultPrivilege));
    ASSERT_NO_FATAL_FAILURE(messageAssociatedWithRightUsers({anotherUser.getSiteId()}, toSend, {loggedUser.getSiteId()}));
}

// Added on 15/04/2020 after introduction of hardLogout() method
TEST_F(SymServerTestFilesystemFunctionality, hardLogoutClosesOpenedDocumentsAndPropagateChanges){
    server.forceSiteIdForResId(&doc, anotherUser);
    auto& target= dynamic_cast<SymServerUserMock&>(server.getRegistered(loggedUserUsername));
    EXPECT_CALL(target, openFile(filePath, fileName, defaultPrivilege)).WillOnce(::testing::Return(fileToReturn));
    EXPECT_CALL(*fileToReturn, access(loggedUser, defaultPrivilege)).WillOnce(::testing::ReturnRef(doc));
    auto f= server.openSource(loggedUserUsername, filePath, fileName, defaultPrivilege, 0);
    ASSERT_TRUE(server.userIsWorkingOnDocument(loggedUser, doc, defaultPrivilege));

    updateActiveMessage toSend(msgType::removeActiveUser, msgOutcome::success, loggedUser.makeCopyNoPwd(), doc.getId(), privilege::readOnly, 1);
    serverMessage response(msgType::logout, msgOutcome::success, 1);

    server.hardLogout(loggedUser.getSiteId());
    EXPECT_FALSE(server.userIsWorkingOnDocument(loggedUser, doc, defaultPrivilege));
    EXPECT_FALSE(server.thereIsMessageForUser(loggedUser.getSiteId(), response).first);
    ASSERT_NO_FATAL_FAILURE(messageAssociatedWithRightUsers({anotherUser.getSiteId()}, toSend, {loggedUser.getSiteId()}));
}

TEST_F(SymServerTestFilesystemFunctionality, removeUserClosesOpenedDocuments){
    server.forceSiteIdForResId(&doc, anotherUser);
    auto& target= dynamic_cast<SymServerUserMock&>(server.getRegistered(loggedUserUsername));
    EXPECT_CALL(target, openFile(filePath, fileName, defaultPrivilege)).WillOnce(::testing::Return(fileToReturn));
    EXPECT_CALL(*fileToReturn, access(loggedUser, defaultPrivilege)).WillOnce(::testing::ReturnRef(doc));
    auto f= server.openSource(loggedUserUsername, filePath, fileName, defaultPrivilege, 0);
    ASSERT_TRUE(server.userIsWorkingOnDocument(loggedUser, doc, defaultPrivilege));

    updateActiveMessage toSend(msgType::removeActiveUser, msgOutcome::success, loggedUser.makeCopyNoPwd(), doc.getId(), privilege::readOnly, msId);
    EXPECT_CALL(target, hasPwd(loggedUserPwd)).WillOnce(::testing::Return(true));
    EXPECT_CALL(*fakeDir, remove(target, "./", std::to_string(userDir->getId())));
    server.removeUser(loggedUserUsername, loggedUserPwd, msId);
    EXPECT_FALSE(server.userAlreadyRegistered(loggedUser));
    EXPECT_FALSE(server.userIsWorkingOnDocument(loggedUser, doc, defaultPrivilege));
    ASSERT_NO_FATAL_FAILURE(messageAssociatedWithRightUsers({anotherUser.getSiteId()}, toSend, {loggedUser.getSiteId()}));}

TEST_F(SymServerTestFilesystemFunctionality, openNewSourceAccessesTheFileAndGenerateCorrectResponse){
    server.forceSiteIdForResId(&doc, anotherUser);
    auto& target= dynamic_cast<SymServerUserMock&>(server.getRegistered(loggedUserUsername));
    EXPECT_CALL(target, accessFile(filePath + "/" + fileName, "./", fileName, defaultPrivilege)).WillOnce(::testing::Return(std::pair<int, std::shared_ptr<file>>(0, fileToReturn)));
    EXPECT_CALL(*fileToReturn, getUserPrivilege(loggedUserUsername)).WillOnce(::testing::Return(defaultPrivilege));
    EXPECT_CALL(*fileToReturn, access(target, defaultPrivilege)).WillOnce(::testing::ReturnRef(doc));
    auto f= server.openNewSource(loggedUserUsername, filePath + "/" + fileName, "./", fileName, defaultPrivilege, msId);
    EXPECT_TRUE(server.userIsWorkingOnDocument(loggedUser, doc, defaultPrivilege));

    //Propagation of document opened to other clients
    updateActiveMessage toSend(msgType::addActiveUser, msgOutcome::success, loggedUser.makeCopyNoPwd(), doc.getId(), privilege::owner, msId);
    ASSERT_NO_FATAL_FAILURE(messageAssociatedWithRightUsers({anotherUser.getSiteId()}, toSend, {loggedUser.getSiteId()}));

    //response
    sendResMessage response(msgType::openNewRes, msgOutcome::success, f, 0, msId);
    EXPECT_TRUE(server.thereIsMessageForUser(target.getSiteId(), response).first);
}

TEST_F(SymServerTestFilesystemFunctionality, openNewSourceOfNotLoggedUser){
    std::shared_ptr<file> f;
    ASSERT_THROW(f= server.openNewSource(anotherUserUsername, filePath + "/" + fileName, "./", fileName,
                                         defaultPrivilege, 0), SymServerException);
    EXPECT_FALSE(server.userIsWorkingOnDocument(anotherUser, doc, defaultPrivilege));
    /*
     * Cases in which, for an error, the operation goes wrong, so an exception is raised, are to be handled
     * externally, in the module that controls the connection
     */
}

TEST_F(SymServerTestFilesystemFunctionality, createNewSourceCallsNewFile){
    auto& target= dynamic_cast<SymServerUserMock&>(server.getRegistered(loggedUserUsername));
    EXPECT_CALL(target, newFile(fileName, filePath,0)).WillOnce(::testing::Return(fileToReturn));
    EXPECT_CALL(*fileToReturn, access(loggedUser, privilege::owner)).WillOnce(::testing::ReturnRef(doc));
    auto doc= server.createNewSource(loggedUserUsername, filePath, fileName, msId);
    EXPECT_TRUE(server.userIsWorkingOnDocument(loggedUser, doc, privilege::owner));

    //response
    sendResMessage response(msgType::createRes, msgOutcome::success, fileToReturn, 0, msId);
    EXPECT_TRUE(server.thereIsMessageForUser(target.getSiteId(), response).first);
}

TEST_F(SymServerTestFilesystemFunctionality, createNewSourceOfUnloggedUser){
    EXPECT_THROW(server.createNewSource(anotherUserUsername, filePath, fileName, 0), SymServerException);
}

TEST_F(SymServerTestFilesystemFunctionality, createNewDirCallsNewDirectory){
    EXPECT_CALL(*inserted, newDirectory(fileName, filePath, 0));
    server.createNewDir(loggedUserUsername, filePath, fileName, msId);

    //response
    sendResMessage response(msgType::createNewDir, msgOutcome::success, fileToReturn, 0, msId);
    EXPECT_TRUE(server.thereIsMessageForUser(inserted->getSiteId(), response).first);
}

TEST_F(SymServerTestFilesystemFunctionality, createNewDirOfUnloggedUser){
    EXPECT_THROW(server.createNewDir(anotherUserUsername, filePath, fileName, 0), SymServerException);
}

TEST_F(SymServerTestFilesystemFunctionality, remoteInsertCallsRemoteInsertOnDocAndPropagateChanges){
    setStageForOpenedDocForLoggedUser();
    std::initializer_list<uint_positive_cnt::type> siteIds{anotherUser.getSiteId()};
    server.forceSiteIdForResId(&doc, anotherUser);

    symbol toInsert('a', 0, 0, {}, false);
    symbolMessage received(msgType::insertSymbol, {loggedUserUsername, loggedUserPwd}, msgOutcome::success, loggedUser.getSiteId(), doc.getId(), toInsert);
    //server must send and insert a symbol that is verified, but without the authentication parameters
    symbolMessage toSend(msgType::insertSymbol, {{}, {}}, msgOutcome::success, received.getSiteId(), doc.getId(), toInsert.setVerified(), received.getMsgId());
    EXPECT_CALL(doc, remoteInsert(received.getSiteId(), toSend.getSym()));
    server.remoteInsert(loggedUserUsername, doc.getId(), received);
    auto res= server.thereIsMessageForUser(*siteIds.begin(), toSend);
    ASSERT_NO_FATAL_FAILURE(messageAssociatedWithRightUsers(siteIds, toSend, {loggedUser.getSiteId()}));
    EXPECT_PRED_FORMAT2(symbolMessageInQueueIsCorrect, res, toSend);
}

TEST_F(SymServerTestFilesystemFunctionality, remoteInsertCallsRemoteInsertOnDocAndGenerateCorrectResponse){
    setStageForOpenedDocForLoggedUser();
    std::initializer_list<uint_positive_cnt::type> siteIds{anotherUser.getSiteId()};
    server.forceSiteIdForResId(&doc, anotherUser);

    symbol symSent('a', 0, 0, {}, false);
    symbol toInsert=symSent.setVerified();
    symbolMessage received(msgType::insertSymbol, {loggedUserUsername, loggedUserPwd}, msgOutcome::success, loggedUser.getSiteId(), doc.getId(), symSent);
    EXPECT_CALL(doc, remoteInsert(received.getSiteId(), toInsert));
    server.remoteInsert(loggedUserUsername, doc.getId(), received);

    //send response (confirmation)
    serverMessage response(msgType::insertSymbol,msgOutcome::success, received.getMsgId());
    EXPECT_TRUE(server.thereIsMessageForUser(loggedUser.getSiteId(), response).first);
}

TEST_F(SymServerTestFilesystemFunctionality, remoteInsertOfUnloggedUser){
    symbol toInsert('a', 0, 0, {}, false);
    symbolMessage received(msgType::insertSymbol, {anotherUserUsername, anotherUserPwd}, msgOutcome::success, anotherUser.getSiteId(), doc.getId(), toInsert);
    EXPECT_THROW(server.remoteInsert(anotherUserUsername, doc.getId(), received), SymServerException);
    /*
     * Cases in which, for an error, the operation goes wrong, so an exception is raised, are to be handled
     * externally, in the module that controls the connection
     */
}

TEST_F(SymServerTestFilesystemFunctionality, remoteInsertOnDocumentNotOpened){
    setStageForAccessedDoc(loggedUser);
    makeLoggedUserNotWorkingOnDoc();
    symbol toInsert('a', 0, 0, {}, false);
    symbolMessage received(msgType::insertSymbol, {loggedUserUsername, loggedUserPwd}, msgOutcome::success, loggedUser.getSiteId(), doc.getId(), toInsert);
    EXPECT_THROW(server.remoteInsert(loggedUserUsername, doc.getId(), received), SymServerException);
    /*
     * Cases in which, for an error, the operation goes wrong, so an exception is raised, are to be handled
     * externally, in the module that controls the connection
     */
}

TEST_F(SymServerTestFilesystemFunctionality, remoteRemoveCallsRemoteRemoveOnDocAndPropagateChanges){
    setStageForOpenedDocForLoggedUser();
    std::initializer_list<uint_positive_cnt::type> siteIds{anotherUser.getSiteId()};
    server.forceSiteIdForResId(&doc, anotherUser);

    symbol toRemove('a', 0, 0, {}, false);
    symbolMessage received(msgType::removeSymbol, {loggedUserUsername, loggedUserPwd}, msgOutcome::success, loggedUser.getSiteId(), doc.getId(), toRemove, rand());
    symbolMessage toSend(msgType::removeSymbol, {{}, {}}, msgOutcome::success, loggedUser.getSiteId(), doc.getId(), toRemove.setVerified(), received.getMsgId());
    EXPECT_CALL(doc, remoteRemove(received.getSiteId(), toSend.getSym()));
    server.remoteRemove(loggedUserUsername, doc.getId(), received);

    auto res= server.thereIsMessageForUser(*siteIds.begin(), toSend);
    ASSERT_NO_FATAL_FAILURE(messageAssociatedWithRightUsers(siteIds, toSend, {loggedUser.getSiteId()}));
    EXPECT_PRED_FORMAT2(symbolMessageInQueueIsCorrect, res, toSend);
}

TEST_F(SymServerTestFilesystemFunctionality, remoteRemoveCallsRemoteRemoveOnDocAndGenerateCorrectResponse){
    setStageForOpenedDocForLoggedUser();
    std::initializer_list<uint_positive_cnt::type> siteIds{anotherUser.getSiteId()};
    server.forceSiteIdForResId(&doc, anotherUser);
    uint_positive_cnt::type msId=rand()+1;

    symbol toRemove('a', 0, 0, {}, false);
    symbolMessage received(msgType::removeSymbol, {loggedUserUsername, loggedUserPwd}, msgOutcome::success, 0, doc.getId(), toRemove, msId);
    symbolMessage toSend(msgType::removeSymbol, {{}, {}}, msgOutcome::success, 0, doc.getId(), toRemove.setVerified(), received.getMsgId());
    EXPECT_CALL(doc, remoteRemove(received.getSiteId(), toSend.getSym()));
    server.remoteRemove(loggedUserUsername, doc.getId(), received);

    //send response (confirmation)
    serverMessage response(msgType::removeSymbol,msgOutcome::success, msId);
    EXPECT_TRUE(server.thereIsMessageForUser(loggedUser.getSiteId(), response).first);
}

TEST_F(SymServerTestFilesystemFunctionality, remoteRemoveOfUnloggedUser){
    symbol toRemove('a', 0, 0, {}, false);
    symbolMessage received(msgType::removeSymbol, {anotherUserUsername, anotherUserPwd}, msgOutcome::success, 0, doc.getId(), toRemove);
    EXPECT_THROW(server.remoteRemove(anotherUserUsername, doc.getId(), received), SymServerException);
    /*
     * Cases in which, for an error, the operation goes wrong, so an exception is raised, are to be handled
     * externally, in the module that controls the connection
     */
}

TEST_F(SymServerTestFilesystemFunctionality, remoteRemoveOnDocumentNotOpened){
    setStageForAccessedDoc(loggedUser);
    makeLoggedUserNotWorkingOnDoc();
    symbol toRemove('a', 0, 0, {}, false);
    symbolMessage received(msgType::removeSymbol, {loggedUserUsername, loggedUserPwd}, msgOutcome::success, 0, doc.getId(), toRemove);
    EXPECT_THROW(server.remoteInsert(loggedUserUsername, doc.getId(), received), SymServerException);
    /*
     * Cases in which, for an error, the operation goes wrong, so an exception is raised, are to be handled
     * externally, in the module that controls the connection
     */
}

TEST_F(SymServerTestFilesystemFunctionality, closeSourceClosesTheDocumentAndPropagateChanges){
    server.forceSiteIdForResId(&doc, anotherUser);
    setStageForAccessedDoc(loggedUser);
    user loggedUserPurged=loggedUser.makeCopyNoPwd();
    updateDocMessage received(msgType::closeRes, {loggedUserUsername, loggedUserPwd}, doc.getId(), msId);
    updateActiveMessage toSend(msgType::removeActiveUser, msgOutcome::success, loggedUserPurged, doc.getId(), privilege::owner, received.getMsgId());
    EXPECT_CALL(doc, close(loggedUser));
    auto& target= dynamic_cast<SymServerUserMock&>(server.getRegistered(anotherUserUsername));
    server.closeSource(loggedUserUsername, received.getResourceId(), msId);
    EXPECT_FALSE(server.userIsWorkingOnDocument(target, doc, defaultPrivilege));

    ASSERT_NO_FATAL_FAILURE(messageAssociatedWithRightUsers({anotherUser.getSiteId()}, toSend, {loggedUser.getSiteId()}));
}

TEST_F(SymServerTestFilesystemFunctionality, closeSourceClosesTheDocumentAndGenerateCorrectResponse){
    server.forceSiteIdForResId(&doc, anotherUser);
    setStageForAccessedDoc(loggedUser);
    user loggedUserPurged=loggedUser.makeCopyNoPwd();
    updateDocMessage received(msgType::closeRes, {loggedUserUsername, loggedUserPwd}, doc.getId());
    updateActiveMessage toSend(msgType::removeActiveUser, msgOutcome::success, loggedUserPurged, doc.getId(), privilege::owner, received.getMsgId());
    EXPECT_CALL(doc, close(loggedUser));
    auto& target= dynamic_cast<SymServerUserMock&>(server.getRegistered(anotherUserUsername));
    server.closeSource(loggedUserUsername, received.getResourceId(), msId);

    //send response (confirmation)
    serverMessage response(msgType::closeRes,msgOutcome::success, msId);
    EXPECT_TRUE(server.thereIsMessageForUser(loggedUser.getSiteId(), response).first);
}


TEST_F(SymServerTestFilesystemFunctionality, editPrivilegeCallsEditPrivilegeOnUserAndPropagateChanges){
    user thirdUser("ThirdUser", loggedUserPwd, "nickname", "./thisDir/a.jpg", 10, userDir);
    setStageForAccessedDoc(loggedUser);
    setAnotherUserActive();
    makeAnotherUserToHavePrivilegeAndCloseSource(defaultPrivilege);
    std::initializer_list<uint_positive_cnt::type> siteIds{thirdUser.getSiteId()};
    /*
     * Let's suppose that a message like that has been received by the server
     */
    privMessage received(msgType::changePrivileges, {loggedUserUsername,loggedUserPwd}, msgOutcome::success, std::to_string(doc.getId()), anotherUserUsername, privilege::readOnly, msId);
    server.forceSiteIdForResId(&doc, thirdUser);
    EXPECT_CALL(*justInserted, openFile(filePath, fileName, privilege::owner)).WillOnce(::testing::Return(fileToReturn));
    EXPECT_CALL(*fileToReturn, access(loggedUser, privilege::owner)).WillOnce(::testing::ReturnRef(doc));
    EXPECT_CALL(*justInserted, editPrivilege(anotherUserUsername, filePath, fileName, privilege::readOnly));

    //For transforming the path from local to global
    EXPECT_CALL(*userDir, get(filePath, fileName)).WillOnce(::testing::Return(fileToReturn));

    server.editPrivilege(loggedUserUsername, anotherUserUsername, filePath,
                         fileName, privilege::readOnly, msId);
    privMessage toSend(received);
    toSend.clearAuthParam();

    /*
     * A message is supposed to be inserted in the queue corresponding to the file's document
     * This means that only the client that have the document opened will receive that message
     * This is right because the clients other than the creator have only the symlink, that doesn't
     * carry information about privileges on the file it points to.
     */
    ASSERT_NO_FATAL_FAILURE(messageAssociatedWithRightUsers(siteIds, toSend, {loggedUser.getSiteId()}));
}

TEST_F(SymServerTestFilesystemFunctionality, editPrivilegeCallsEditPrivilegeOnUserAndGenerateCorrectResponse){
    setStageForAccessedDoc(loggedUser);
    setAnotherUserActive();
    makeAnotherUserToHavePrivilegeAndCloseSource(defaultPrivilege);
    /*
     * Let's suppose that a message like that has been received by the server
     */
    privMessage received(msgType::changePrivileges, {loggedUserUsername,loggedUserPwd}, msgOutcome::success, std::to_string(doc.getId()), anotherUserUsername, privilege::readOnly, msId);
    EXPECT_CALL(*justInserted, openFile(filePath, fileName, privilege::owner)).WillOnce(::testing::Return(fileToReturn));
    EXPECT_CALL(*fileToReturn, access(loggedUser, privilege::owner)).WillOnce(::testing::ReturnRef(doc));
    EXPECT_CALL(*justInserted, editPrivilege(anotherUserUsername, filePath, fileName, privilege::readOnly));

    //For transforming the path from local to global
    EXPECT_CALL(*userDir, get(filePath, fileName)).WillOnce(::testing::Return(fileToReturn));
    server.editPrivilege(loggedUserUsername, anotherUserUsername, filePath,
                         fileName, privilege::readOnly, msId);

    //send response (confirmation)
    serverMessage response(msgType::changePrivileges,msgOutcome::success, msId);
    EXPECT_TRUE(server.thereIsMessageForUser(loggedUser.getSiteId(), response).first);
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

    //For transforming the path from local to global
    EXPECT_CALL(*userDir, get(filePath, fileName)).WillOnce(::testing::Return(fileToReturn));

    server.editPrivilege(loggedUserUsername, anotherUserUsername, filePath,
                         fileName, privilege::readOnly, 0);
}

TEST_F(SymServerTestFilesystemFunctionality, editPrivilegeCalledByUnloggedUser){
    setStageForAccessedDoc(loggedUser);
    setAnotherUserActive();
    makeAnotherUserToHavePrivilegeAndCloseSource(defaultPrivilege);
    server.hardWrongLogout(loggedUser);

    EXPECT_THROW(server.editPrivilege(loggedUserUsername, anotherUserUsername,
                                      filePath, fileName,
                                      privilege::readOnly, 0), SymServerException);
    /*
     * Cases in which, for an error, the operation goes wrong, so an exception is raised, are to be handled
     * externally, in the module that controls the connection
     */
}

TEST_F(SymServerTestFilesystemFunctionality, editPrivilegeOnUserWorkingOnDocument){
    setStageForAccessedDoc(loggedUser);
    setAnotherUserActive();
    makeAnotherUserToHavePrivilege(defaultPrivilege);
    //call expected because we need to retrieve the document id of the document named resId (in the loggedUser space)
    //to check that anotherUser is not working on the same document
    EXPECT_CALL(*justInserted, openFile(filePath, fileName, privilege::owner)).WillOnce(::testing::Return(fileToReturn));
    EXPECT_CALL(*fileToReturn, access(loggedUser, privilege::owner)).WillOnce(::testing::ReturnRef(doc));
    EXPECT_THROW(server.editPrivilege(loggedUserUsername, anotherUserUsername,
                                      filePath, fileName,
                                      privilege::readOnly, 0), SymServerException);
    /*
     * Cases in which, for an error, the operation goes wrong, so an exception is raised, are to be handled
     * externally, in the module that controls the connection
     */
}

TEST_F(SymServerTestFilesystemFunctionality, shareResourceCallsShareResourceOnUserAndPropagateChanges){
    setStageForAccessedDoc(loggedUser);
    uri newPref(uriPolicy::activeAlways);
    /*
     * Let's suppose that a message like that has been received by the server
     */
    uriMessage received(msgType::shareRes, {loggedUserUsername, loggedUserPwd}, msgOutcome ::success, filePath, fileName, newPref, msId);
    uriMessage toSend(received);
    toSend.clearAuthParam();
    server.forceSiteIdForResId(&(fileToReturn->getDoc()), anotherUser);
    EXPECT_CALL(*justInserted, shareResource(filePath, fileName, newPref)).WillOnce(::testing::Return(fileToReturn));
    //For transforming the path from local to global
    EXPECT_CALL(*userDir, get(filePath, fileName)).WillOnce(::testing::Return(fileToReturn));
    auto file= server.shareResource(loggedUserUsername, filePath, fileName, newPref, msId);
    /*
     * A message is supposed to be inserted in the queue corresponding to the file's document
     * This means that only the client that have the document opened will receive that message
     * This is right because the client other than the creator have only the symlink, that doesn't
     * carry information about sharing preferences of the file it points to.
     */
    ASSERT_NO_FATAL_FAILURE(messageAssociatedWithRightUsers({anotherUser.getSiteId()}, toSend, {loggedUser.getSiteId()}));
}

TEST_F(SymServerTestFilesystemFunctionality, shareResourceCallsShareResourceOnUserAndGenerateCorrectResponse){
    setStageForAccessedDoc(loggedUser);
    uri newPref(uriPolicy::activeAlways);
    /*
     * Let's suppose that a message like that has been received by the server
     */
    uriMessage received(msgType::shareRes, {loggedUserUsername, loggedUserPwd}, msgOutcome ::success, filePath, fileName, newPref, msId);
    server.forceSiteIdForResId(&(fileToReturn->getDoc()), anotherUser);
    EXPECT_CALL(*justInserted, shareResource(filePath, fileName, newPref)).WillOnce(::testing::Return(fileToReturn));
    //For transforming the path from local to global
    EXPECT_CALL(*userDir, get(filePath, fileName)).WillOnce(::testing::Return(fileToReturn));
    auto file= server.shareResource(loggedUserUsername, filePath, fileName, newPref, msId);

    //send response (confirmation)
    serverMessage response(msgType::shareRes,msgOutcome::success, msId);
    EXPECT_TRUE(server.thereIsMessageForUser(loggedUser.getSiteId(), response).first);
}

TEST_F(SymServerTestFilesystemFunctionality, shareResourceOfUnLoggedUser){
    uri newPref(uriPolicy::activeAlways);
    EXPECT_THROW(server.shareResource(anotherUserUsername, filePath, fileName, newPref, 0), SymServerException);
    /*
     * Cases in which, for an error, the operation goes wrong, so an exception is raised, are to be handled
     * externally, in the module that controls the connection
     */
}

TEST_F(SymServerTestFilesystemFunctionality, renameResourceCallsRenameResourceOnUserAndGenerateCorrectResponse){
    setStageForAccessedDoc(loggedUser);
    EXPECT_CALL(*justInserted, renameResource(filePath, fileName, "newName")).WillOnce(::testing::Return(fileToReturn));
    std::shared_ptr<filesystem> ret= server.renameResource(loggedUserUsername, filePath, fileName, "newName", msId);
    EXPECT_EQ(fileToReturn, ret);

    //send response (confirmation)
    serverMessage response(msgType::changeResName,msgOutcome::success, msId);
    EXPECT_TRUE(server.thereIsMessageForUser(loggedUser.getSiteId(), response).first);
}

TEST_F(SymServerTestFilesystemFunctionality, renameResourceByUnloggedUser){
    EXPECT_THROW(server.renameResource(anotherUserUsername, filePath, fileName, "newName", 0), SymServerException);
    /*
     * Cases in which, for an error, the operation goes wrong, so an exception is raised, are to be handled
     * externally, in the module that controls the connection
     */
}

TEST_F(SymServerTestFilesystemFunctionality, removeResourceCallsResourceFileOnUserAndGenerateCorrectResponse){
    setStageForAccessedDoc(loggedUser);
    makeLoggedUserNotWorkingOnDoc();
    EXPECT_CALL(*justInserted, removeResource(filePath, fileName)).WillOnce(::testing::Return(fileToReturn));
    auto ret= server.removeResource(loggedUserUsername, filePath, fileName, msId);
    EXPECT_EQ(fileToReturn, ret);

    //send response (confirmation)
    serverMessage response(msgType::removeRes,msgOutcome::success, msId);
    EXPECT_TRUE(server.thereIsMessageForUser(loggedUser.getSiteId(), response).first);
}

TEST_F(SymServerTestFilesystemFunctionality, removeResourceByUnloggedUser){
    EXPECT_THROW(server.removeResource(anotherUserUsername, filePath, fileName, 0), SymServerException);
    /*
     * Cases in which, for an error, the operation goes wrong, so an exception is raised, are to be handled
     * externally, in the module that controls the connection
     */
}

TEST_F(SymServerTestFilesystemFunctionality, mapSiteIdToUserCallsRetrieveSiteIdsOnDocAndGenerateCorrectResponse){
    setStageForAccessedDoc(loggedUser);
    EXPECT_CALL(doc,retrieveSiteIds());
    auto mappedIds= server.mapSiteIdToUser(loggedUserUsername, doc.getId(), msId);

    //send response (confirmation)
    mapMessage response(msgType::mapChangesToUser, msgOutcome::success, mappedIds, msId);
    EXPECT_TRUE(server.thereIsMessageForUser(loggedUser.getSiteId(), response).first);
}

TEST_F(SymServerTestFilesystemFunctionality, mapSiteIdToUserOnClosedDoc){
    EXPECT_THROW(server.mapSiteIdToUser(loggedUserUsername, doc.getId(), 0), SymServerException);
    /*
     * Cases in which, for an error, the operation goes wrong, so an exception is raised, are to be handled
     * externally, in the module that controls the connection
     */
}

TEST_F(SymServerTestFilesystemFunctionality, mapSiteIdToUserCorrectMapping){
    setStageForAccessedDoc(loggedUser);
    setAnotherUserActive();
    makeAnotherUserToHavePrivilege(defaultPrivilege);
    std::set<uint_positive_cnt::type> siteIdsToReturn({loggedUser.getSiteId(), anotherUser.getSiteId()});
    EXPECT_CALL(doc,retrieveSiteIds()).WillOnce(::testing::Return(siteIdsToReturn));
    auto mapping= server.mapSiteIdToUser(loggedUserUsername, doc.getId(), 0);
    std::map<uint_positive_cnt::type, user> expected({
        std::pair<uint_positive_cnt::type, user>(loggedUser.getSiteId(), loggedUser),
        std::pair<uint_positive_cnt::type, user>(anotherUser.getSiteId(), anotherUser)});
    EXPECT_EQ(expected, mapping);
}

TEST_F(SymServerTestFilesystemFunctionality, mapSiteIdWithUnknownSiteId){
    setStageForAccessedDoc(loggedUser);
    //loggedUser and anotherUser have siteId 0 and 1,
    // so 2 and 3 are siteId without association with registered users in SymServer
    std::set<uint_positive_cnt::type> siteIdsToReturn({2,3});
    EXPECT_CALL(doc,retrieveSiteIds()).WillOnce(::testing::Return(siteIdsToReturn));
    auto mapping= server.mapSiteIdToUser(loggedUserUsername, doc.getId(), 0);
    std::map<uint_positive_cnt::type, user> expected({
                                         std::pair<uint_positive_cnt::type, user>(2, SymServer::unknownUser),
                                         std::pair<uint_positive_cnt::type, user>(3, SymServer::unknownUser)});
    EXPECT_EQ(expected, mapping);
}

TEST_F(SymServerTestFilesystemFunctionality, updateCursorPosCallsupdateCursorPosOnDocAndGenerateCorrectResponse){
    setStageForOpenedDocForLoggedUser();
    int row=rand()%1000, col=rand()%1000;


    cursorMessage received(msgType::updateCursor, {loggedUserUsername, {}}, msgOutcome::success, loggedUser.getSiteId(), doc.getId(), row, col);
    serverMessage response(msgType::updateCursor, msgOutcome::success, received.getMsgId());
    EXPECT_CALL(doc, updateCursorPos(loggedUser.getSiteId(), row, col));

    server.updateCursorPos(loggedUserUsername, doc.getId(), received);
    EXPECT_TRUE(server.thereIsMessageForUser(loggedUser.getSiteId(), response).first);
}

TEST_F(SymServerTestFilesystemFunctionality, updateCursorPosCallsupdateCursorPosOnDocAndPropagateChanges){
    setStageForOpenedDocForLoggedUser();
    std::initializer_list<uint_positive_cnt::type> siteIds{anotherUser.getSiteId()};
    server.forceSiteIdForResId(&doc, anotherUser);
    int row=rand()%1000, col=rand()%1000;

    cursorMessage received(msgType::updateCursor, {loggedUserUsername, {}}, msgOutcome::success, loggedUser.getSiteId(), doc.getId(), row, col, msId);

    cursorMessage toSend(msgType::updateCursor, {{}, {}}, msgOutcome::success, loggedUser.getSiteId(), doc.getId(), row, col, received.getMsgId());
    EXPECT_CALL(doc, updateCursorPos(loggedUser.getSiteId(), row, col));
    server.updateCursorPos(loggedUserUsername, doc.getId(), received);


    ASSERT_NO_FATAL_FAILURE(messageAssociatedWithRightUsers(siteIds, toSend, {loggedUser.getSiteId()}));
}



TEST_F(SymServerTestFilesystemFunctionality, updateCursorOfUnloggedUser){
    unsigned int row=rand()%1000, col=rand()%1000;
    cursorMessage received(msgType::updateCursor, {loggedUserUsername, {}}, msgOutcome::success, loggedUser.getSiteId(), doc.getId(), row, col);
    EXPECT_THROW(server.updateCursorPos(loggedUserUsername, doc.getId(), received), SymServerException);
    /*
     * Cases in which, for an error, the operation goes wrong, so an exception is raised, are to be handled
     * externally, in the module that controls the connection
     */
}

TEST_F(SymServerTestFilesystemFunctionality, updateCursorOnDocumentNotOpened){
    setStageForAccessedDoc(loggedUser);
    makeLoggedUserNotWorkingOnDoc();
    unsigned int row=rand()%1000, col=rand()%1000;
    cursorMessage received(msgType::updateCursor, {loggedUserUsername, {}}, msgOutcome::success, loggedUser.getSiteId(), doc.getId(), row, col);
    EXPECT_THROW(server.updateCursorPos(loggedUserUsername, doc.getId(), received), SymServerException);
    /*
     * Cases in which, for an error, the operation goes wrong, so an exception is raised, are to be handled
     * externally, in the module that controls the connection
     */
}

TEST_F(SymServerTestFilesystemFunctionality, editLineStyleCallsEditLineStyleOnDocAndPropagateChanges){
    setStageForOpenedDocForLoggedUser();
    std::initializer_list<uint_positive_cnt::type> siteIds{anotherUser.getSiteId()};
    server.forceSiteIdForResId(&doc, anotherUser);
    int row=rand()%1000;
    std::pair<alignType, unsigned> oldStyle{alignType::left, 0}, newStyle{alignType::right, 1};

    editLineStyleMessage received(msgType::editLineStyle, {{loggedUserUsername}, {}}, msgOutcome::success, oldStyle, newStyle, doc.getId(), row, msId);

    editLineStyleMessage toSend(msgType::editLineStyle, {{}, {}}, msgOutcome::success, oldStyle, newStyle, doc.getId(), row, msId);
    EXPECT_CALL(doc, editLineStyle(newStyle, row));
    server.editLineStyle(loggedUserUsername, doc.getId(), newStyle, row, received);


    ASSERT_NO_FATAL_FAILURE(messageAssociatedWithRightUsers(siteIds, toSend, {loggedUser.getSiteId()}));
}

TEST_F(SymServerTestFilesystemFunctionality, editLineStyleCallsEditLineStyleOfUnloggedUser){
    int row=rand()%1000;
    std::pair<alignType, unsigned> oldStyle{alignType::left, 0}, newStyle{alignType::right, 1};
    editLineStyleMessage received(msgType::editLineStyle, {{loggedUserUsername}, {}}, msgOutcome::success, oldStyle, newStyle, doc.getId(), row, msId);
    EXPECT_THROW(server.editLineStyle(loggedUserUsername, doc.getId(), newStyle, row, received), SymServerException);
    /*
     * Cases in which, for an error, the operation goes wrong, so an exception is raised, are to be handled
     * externally, in the module that controls the connection
     */
}

TEST_F(SymServerTestFilesystemFunctionality, editLineStyleOnDocumentNotOpened){
    setStageForAccessedDoc(loggedUser);
    makeLoggedUserNotWorkingOnDoc();
    int row=rand()%1000;
    std::pair<alignType, unsigned> oldStyle{alignType::left, 0}, newStyle{alignType::right, 1};
    editLineStyleMessage received(msgType::editLineStyle, {{loggedUserUsername}, {}}, msgOutcome::success, oldStyle, newStyle, doc.getId(), row, msId);
    EXPECT_THROW(server.editLineStyle(loggedUserUsername, doc.getId(), newStyle, row, received), SymServerException);
    /*
     * Cases in which, for an error, the operation goes wrong, so an exception is raised, are to be handled
     * externally, in the module that controls the connection
     */
}

TEST_F(SymServerTestFilesystemFunctionality, extractNextMessagePopsMessageFromQueue){
    /*
     * In the fixture constructor, login is called on server, so there must be two messages in the queue (registration and login)
     */
    auto m1=server.extractNextMessage();
    ASSERT_TRUE(m1.first!=0);
    auto m2=server.extractNextMessage();
    ASSERT_TRUE(m2.first!=0);
    auto m3=server.extractNextMessage();
    EXPECT_TRUE(m3.first==0);
}



struct SymServerSerialization: ::testing::Test{
    SymServer toStore, toLoad;
    std::stringstream stream;

    SymServerSerialization() :toStore(false, false), toLoad(false, false){};

    void store(const SymServer& u){
        boost::archive::text_oarchive oa(stream);
        oa<<u;
    }
    void load(SymServer& u){
        boost::archive::text_iarchive ia(stream);
        ia>>u;
    }
};

TEST_F(SymServerSerialization, serialize){
    toStore.addUser(const_cast<user &>(SymServer::unknownUser), 0);
    EXPECT_NE(toStore, toLoad);
    store(toStore);
    load(toLoad);
    EXPECT_EQ(toStore, toLoad);
}