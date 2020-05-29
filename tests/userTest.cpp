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
 * File:   userTest.cpp
 * Project: Symposium
 * Authors:
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Alice Morano <s259158 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 20 agosto 2019, 13.40
 */
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../user.h"
#include "../filesystem.h"
#include "../SymposiumException.h"
using namespace Symposium;


struct dirMock: public directory{
    dirMock(const std::string& name="dummy"): directory(name){};

    MOCK_METHOD2(getFile, std::shared_ptr<file>(const std::string&, const std::string&));
    MOCK_METHOD3(addFile, std::shared_ptr<file>(const std::string&, const std::string&, uint_positive_cnt::type));
    MOCK_METHOD2(get, std::shared_ptr<filesystem>(const std::string &path, const std::string &name));
    MOCK_METHOD2(getDir, std::shared_ptr<directory>(const std::string &path, const std::string &name));
    MOCK_METHOD0(getRoot, std::shared_ptr<directory>());
    MOCK_METHOD5(addLink, std::shared_ptr<Symposium::symlink>(const std::string &path, const std::string &name, const std::string &filePath,
            const std::string &fileName, uint_positive_cnt::type));
    MOCK_METHOD2(addDirectory, std::shared_ptr<directory>(const std::string &name, uint_positive_cnt::type idToAssign));

    MOCK_METHOD4(access, document&(const user &targetUser, const std::string &path, const std::string &resName, privilege accessMode));
    MOCK_METHOD3(remove, std::shared_ptr<filesystem>(const user &, const std::string &, const std::string &));
    MOCK_METHOD3(setName, std::string(const std::string &, const std::string &, const std::string &));

    MOCK_CONST_METHOD3(print, std::string(const std::string &targetUser, bool recursive, unsigned int indent));

    void setRootOfHome(std::shared_ptr<dirMock> root){
        this->root=root;
    }
};

struct uriMock: public uri{
    uriMock(privilege p){
        uri::activateAlways(p);
    }

    MOCK_METHOD1(getShare, privilege(privilege));

    virtual ~uriMock() override = default;
};

struct StrategyMock: public RMOAccess{
    MOCK_CONST_METHOD1(getPrivilege, privilege(const std::string&));
};


struct fileMock: public file{
    uriMock policyMocked;

    fileMock(): file("dummy", 0), policyMocked(uri::getDefaultPrivilege()) {
        strategy=std::make_unique<StrategyMock>();
    };
    MOCK_METHOD2(setUserPrivilege, privilege(const std::string&, privilege));
    MOCK_METHOD2(setSharingPolicy, uri(const std::string&, const uri& newSharingPrefs));
    MOCK_CONST_METHOD1(getUserPrivilege, privilege(const std::string&));
    MOCK_METHOD0(getSharingPolicy, uri&());
    MOCK_METHOD2(validateAction, bool(const std::string&, privilege));
    std::unique_ptr<AccessStrategy>& getStrategy(){
        return strategy;
    }

    virtual ~fileMock() override= default;
};

struct UserTest: ::testing::Test{
    user *u;
    std::shared_ptr<::testing::NiceMock<dirMock>> homeDir;
    std::shared_ptr<::testing::NiceMock<dirMock>> Dir;
    std::shared_ptr<::testing::NiceMock<dirMock>> Root;
    std::shared_ptr<::testing::NiceMock<fileMock>> dummyFile;

    UserTest(){
        homeDir=std::make_shared<::testing::NiceMock<dirMock>>();
        dummyFile=std::make_shared<::testing::NiceMock<fileMock>>();
        Dir=std::make_shared<::testing::NiceMock<dirMock>>();
        Root=std::make_shared<::testing::NiceMock<dirMock>>();
        homeDir->setRootOfHome(Root);
        u=new user("username", "AP@ssw0rd!", "noempty", "", 0, homeDir);

    }
    ~UserTest(){
        if(u!= nullptr) {
            delete u;
            u=nullptr;
        }
        ::testing::Mock::AllowLeak(homeDir.get());
        ::testing::Mock::AllowLeak(dummyFile.get());
        ::testing::Mock::AllowLeak(Dir.get());
        ::testing::Mock::AllowLeak(Root.get());
        ::testing::Mock::AllowLeak(dummyFile->getStrategy().get());
    }
};

TEST_F(UserTest, callAccessFile){
    //this will make the call to getShare to return the requested privilege
    uri um; um.activateAlways();
    /*
     * -anotherDir (has id=1)
     *   -file1 (has id=4)
     * -homeDir (has id=0)
     *   -dir1 (has id=2)
     *     -sym1 (has id=decided internally)
     */
    std::shared_ptr<class symlink> returned(new Symposium::symlink("sym1", "./1", "4"));
    EXPECT_CALL(*Root, getFile("./1", "4")).WillOnce(::testing::Return(dummyFile));
    EXPECT_CALL(*dummyFile, getSharingPolicy()).WillOnce(::testing::ReturnRef(um));
    EXPECT_CALL(*dummyFile, setUserPrivilege(u->getUsername(), privilege::modify));
    EXPECT_CALL(*homeDir, addLink("./2", "sym1", "./1", "4",0)).WillOnce(::testing::Return(returned));

    u->accessFile("./1/4", "./2", "sym1", privilege::modify);
}

TEST(userTest, makeNewFileMock){
    dirMock *dir=new dirMock();
    std::shared_ptr<directory> home(dir);
    user u1("username", "AP@ssw0rd!", "noempty", "", 0, home);
    file *created= new file("ciao", 0);
    EXPECT_CALL(*dir, addFile("./", "ciao",0)).WillOnce(::testing::Return(std::shared_ptr<file>(created)));
    u1.newFile("ciao");
}

TEST(userTest, makeNewDirMock){
    dirMock *dir=new dirMock();
    std::shared_ptr<directory> home(dir);
    user u1("username", "AP@ssw0rd!", "noempty", "", 0, home);
    directory *created=new dirMock("ciao");
    EXPECT_CALL(*dir, addDirectory("ciao",0)).WillOnce(::testing::Return(std::shared_ptr<directory>(created)));
    u1.newDirectory("ciao", "./");
}


ACTION(throwEx){
    throw std::exception();
}

TEST_F(UserTest, accessFileStandardCase){
    privilege requested=privilege::readOnly;
    std::shared_ptr<Symposium::symlink> dummySym=std::make_shared<Symposium::symlink>("link", "./1", "2", 0);
    EXPECT_CALL(*Root, getFile("./1", "2")).WillOnce(::testing::Return(dummyFile));
    EXPECT_CALL(*dummyFile, getSharingPolicy()).WillOnce(::testing::ReturnRef(dummyFile->policyMocked));
    EXPECT_CALL(dummyFile->policyMocked, getShare(requested)).WillOnce(::testing::Return(requested));
    EXPECT_CALL(*dummyFile, setUserPrivilege(u->getUsername(), requested));
    EXPECT_CALL(*homeDir, addLink("./3", "link", "./1", "2", 0)).WillOnce(::testing::Return(dummySym));
    u->accessFile("./1/2", "./3", "link", requested);
}


TEST_F(UserTest, accessFileAskingForNoPrivilege){
    privilege requested=privilege::none;
    ON_CALL(*Root, getFile("./1", "2")).WillByDefault(throwEx());
    EXPECT_THROW(u->accessFile("./1/2", "./3", "link", requested), userException);
}

TEST_F(UserTest, accessFileIllegalAbsolutePathSyntaxError){
    privilege requested=privilege::readOnly;
    ON_CALL(*Root, getFile(::testing::_, ::testing::_)).WillByDefault(throwEx());

    EXPECT_THROW(u->accessFile("./1/2/", "./3", "link", requested), userException); //just syntax error
}

TEST_F(UserTest, accessFileIllegalAbsolutePathForbiddenDirectory){
    privilege requested=privilege::readOnly;
    ON_CALL(*Root, getFile(::testing::_, ::testing::_)).WillByDefault(throwEx());
    EXPECT_THROW(u->accessFile("./1", "./3", "link", requested), userException); //forbidden directory
}

TEST_F(UserTest, accessFileIllegalFile){
    privilege requested=privilege::readOnly;
    EXPECT_CALL(*Root, getFile("./"+std::to_string(homeDir->getId()), "5")).WillOnce(::testing::Return(dummyFile));
    EXPECT_CALL(*dummyFile, getSharingPolicy()).WillOnce(::testing::ReturnRef(dummyFile->policyMocked));
    EXPECT_CALL(dummyFile->policyMocked, getShare(requested)).WillOnce(::testing::Return(requested));
    EXPECT_CALL(*dummyFile, setUserPrivilege(u->getUsername(), requested));
    EXPECT_CALL(*dynamic_cast<StrategyMock*>((dummyFile->getStrategy().get())), getPrivilege(u->getUsername())).WillOnce(::testing::Return(requested));
    ON_CALL(*homeDir, addLink(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).WillByDefault(throwEx());
    EXPECT_THROW(u->accessFile("./"+std::to_string(homeDir->getId())+"/5", "./3", "link", requested), userException); //object in user's filesystem
}

TEST_F(UserTest, editPrivilegeLegalCase){
    privilege requested=privilege::modify;
    EXPECT_CALL(*homeDir, getFile("./", "2")).WillOnce(::testing::Return(dummyFile));
    EXPECT_CALL(*dummyFile, validateAction(u->getUsername(), privilege::owner)).WillOnce(::testing::Return(true));

    u->editPrivilege("anotherUser", "./", "2", requested);
}

TEST_F(UserTest, editPrivilegeFromNotOwner){
    privilege requested=privilege::modify;
    EXPECT_CALL(*homeDir, getFile("./", "2")).WillOnce(::testing::Return(dummyFile));
    EXPECT_CALL(*dummyFile, validateAction(u->getUsername(), privilege::owner)).WillOnce(::testing::Return(false));

    EXPECT_THROW(u->editPrivilege("anotherUser", "./", "2", requested), userException);
}

TEST_F(UserTest, editPrivilegeFromNotOwnerUpgradePrivilege){
    privilege requested=privilege::owner;
    EXPECT_CALL(*homeDir, getFile("./", "2")).WillOnce(::testing::Return(dummyFile));
    EXPECT_CALL(*dummyFile, validateAction(u->getUsername(), privilege::owner)).WillOnce(::testing::Return(false));

    EXPECT_THROW(u->editPrivilege(u->getUsername(), "./", "2", requested), userException);
}

TEST_F(UserTest, callRemoveResource)
{
    EXPECT_CALL(*homeDir, remove(*u, "./", "1"));
    u->removeResource("./", "1");
}

//Test added on 20/05/2020, because was called a wrong version of setName, that is now deleted
TEST_F(UserTest, callSetName){
    EXPECT_CALL(*homeDir, setName("./1", "5", "newName"));
    u->renameResource("./1", "5", "newName");
}

TEST_F(UserTest, callShowDir)
{
    EXPECT_CALL(*homeDir, print(u->getUsername(), false, 0));
    u->showDir(false);
}

TEST_F(UserTest, callOpenFile)
{
    document expected;
    EXPECT_CALL(*homeDir, getFile("./", "dummyFile"));
    u->openFile("./", "dummyFile", uri::getDefaultPrivilege());
}

TEST_F(UserTest, callShareResource){
    testing::InSequence forceOrder; //force function call order in EXPECT_CALL
    uri ur;
    //homeDir is a mock for a directory object: the user in the fixture is initialized with this object and not with a
    //directory object, so any call to the methods overriden by dirMock is handled by the test suite
    EXPECT_CALL(*homeDir, getFile("./", "dummyFile")).WillOnce(::testing::Return(dummyFile));
    EXPECT_CALL(*dummyFile, setSharingPolicy(u->getUsername(), ur));
    u->shareResource("./", "dummyFile", ur);
}

/*
 * More tests to prove robustness
 */
struct UserTestRobust: ::testing::Test{
    user* u=nullptr;
    /*
     * Add to this list to give example of NOT good passwords
     */
    static const std::string tooShortPwd;
    static const std::string tooLongPwd;
    static const std::string noLettersPwd;
    static const std::string noNumbersPwd;
    static const std::string noSpecialCharPwd;
    static const std::string specialCharacters;

    /*
     * Change these parameters to define what is good for a password
     */
    static constexpr int minPwdLen=6;
    static constexpr int maxPwdLen=12;

    /*
     * Example of good password
     */
    static const std::string goodPwd;

    /*
     * These functions return true if pwd is NOT good
     */
    static bool testPwdIfShort(const std::string& pwd){
        return pwd.size()<minPwdLen;
    }
    static bool testPwdIfLong(const std::string& pwd){
        return pwd.size()>maxPwdLen;
    }
    static bool testPwdIfNoLetter(const std::string& pwd){
        return std::find_if(pwd.begin(), pwd.end(), [](char c){return std::isalpha(c);})==pwd.end();
    }
    static bool testPwdIfNoNumber(const std::string& pwd){
        return std::find_if(pwd.begin(), pwd.end(), [](char c){return std::isdigit(c);})==pwd.end();
    }
    static bool testPwdIfNoSpecialChar(const std::string& pwd){
        for (char sc:specialCharacters)
            if(pwd.find(sc)!=std::string::npos)
                return false;
        return true;
    }
    static void setPwdExpectationAndTest(bool notValid(const std::string&), const std::string& pwd){
        user *u;
        //assert that this pwd shouldn't be good, so it shouldn't be accepted by user's constructor
        ASSERT_TRUE(notValid(pwd));
        EXPECT_THROW(u=new user("", pwd, "nickname", "", 0, nullptr), userException);
    }

    /*
     * Return true if password is good
     */
    static bool isAGoodPwd(const std::string& pwd){
        return !testPwdIfShort(pwd) && !testPwdIfLong(pwd) &&
               !testPwdIfNoLetter(pwd) && !testPwdIfNoNumber(pwd)
               && !testPwdIfNoSpecialChar(pwd);
    }

    ::testing::AssertionResult newDataChangesUserParameters(const char* arg0, const char* arg1, const char* arg3, const user& userModified, const user& oldData, const user& newData){
        if(userModified.getUsername()!=oldData.getUsername())
            return ::testing::AssertionFailure() << "Username of an user cannot be changed";
        if(userModified.getSiteId()!=oldData.getSiteId())
            return ::testing::AssertionFailure() << "SiteId of an user cannot be changed";
        if(newData.getNickname().empty() && userModified.getNickname().empty())
            return ::testing::AssertionFailure() << "Nickname cannot be changed to the empty string";
        if(newData.getIconPath().empty() && userModified.getNickname().empty())
            return ::testing::AssertionFailure() << "IconPath cannot be changed to the empty string";
        if(oldData.getHome()!=userModified.getHome())
            return ::testing::AssertionFailure() << "User's home cannot be changed";
        return ::testing::AssertionSuccess();
    }

    virtual ~UserTestRobust() {
        delete u;
        u=nullptr;
    }

};
const std::string UserTestRobust::tooShortPwd="ab12!";
const std::string UserTestRobust::tooLongPwd="abcd1234!!!!WayTooLong";
const std::string UserTestRobust::noLettersPwd="12345!";
const std::string UserTestRobust::noNumbersPwd="abcde!";
const std::string UserTestRobust::noSpecialCharPwd="abcde1";
const std::string UserTestRobust::specialCharacters="?!$+-/.,@ˆ_";
const std::string UserTestRobust::goodPwd="GoodP@ssw0rd";

/*
 * Let's impose some constraints on the password the user can be given:
 * length between 6 and 12 characters, must contain at least one letter,
 * one number and one special character.
 */
TEST_F(UserTestRobust, DontAcceptTooShortPwd){
    setPwdExpectationAndTest(testPwdIfShort, tooShortPwd);
}

TEST_F(UserTestRobust, DontAcceptTooLongPwd){
    setPwdExpectationAndTest(testPwdIfLong, tooLongPwd);
}

TEST_F(UserTestRobust, DontAcceptNoLettersPwd){
    setPwdExpectationAndTest(testPwdIfNoLetter, noLettersPwd);
}

TEST_F(UserTestRobust, DontAcceptNoNumberPwd){
    setPwdExpectationAndTest(testPwdIfNoNumber, noNumbersPwd);
}

TEST_F(UserTestRobust, DontAcceptNoSpecialCharPwd){
    setPwdExpectationAndTest(testPwdIfNoSpecialChar, noSpecialCharPwd);
}

TEST_F(UserTestRobust, nicknameCantBeEmpty){
    EXPECT_THROW(u=new user("username", goodPwd, "", "", 0, nullptr), userException);
}

TEST_F(UserTestRobust, applyTransformationOnGivenPwd){
    ASSERT_TRUE(isAGoodPwd(goodPwd));
    u=new user("username", goodPwd, "nickname", "", 0, nullptr);
    EXPECT_FALSE(u->getPwdHash()==goodPwd);
}

TEST_F(UserTestRobust, setNewDataDoesntChangeUsername){
    u=new user("username", goodPwd, "nickname", "", 0, nullptr);
    user newData("anotherUsername", goodPwd, "nickname", "", 0, nullptr);
    user modified(*u);
    modified.setNewData(newData);
    EXPECT_PRED_FORMAT3(newDataChangesUserParameters, modified, *u, newData);
}

TEST_F(UserTestRobust, setNewDataDoesntChangeSiteId){
    u=new user("username", goodPwd, "nickname", "", 0, nullptr);
    user newData("username", goodPwd, "nickname", "", 1, nullptr);
    user modified(*u);
    modified.setNewData(newData);
    EXPECT_PRED_FORMAT3(newDataChangesUserParameters, modified, *u, newData);
}

TEST_F(UserTestRobust, setNewDataDoesntChangeHome){
    u=new user("username", goodPwd, "nickname", "", 0, directory::getRoot());
    user newData("username", goodPwd, "nickname", "", 1, nullptr);
    user modified(*u);
    modified.setNewData(newData);
    EXPECT_PRED_FORMAT3(newDataChangesUserParameters, modified, *u, newData);
}

TEST_F(UserTestRobust, setNewDataDoesntChangeWithEmptyStrings){
    u=new user("username", goodPwd, "nickname", "", 0, nullptr);
    user newData;
    user modified(*u);
    modified.setNewData(newData);
    EXPECT_PRED_FORMAT3(newDataChangesUserParameters, modified, *u, newData);
}

//Added on 12/04/2020 to prove correctness after bug found in logIn flow
TEST_F(UserTestRobust, hasPwdReturnCorrectResult){
    u=new user("username", goodPwd, "nickname", "", 0, nullptr);
    EXPECT_TRUE(u->hasPwd(goodPwd));
    EXPECT_FALSE(u->hasPwd(goodPwd+"something"));
}

/*
 * Introduced after the bug has been found by inspection, it stands
 * here as documentation
 */
TEST_F(UserTestRobust, newDirUsesCorrectlyIdToAssign){
    int id=rand()%100+1; //num in [1,100]
    dirMock *dir=new dirMock();
    std::shared_ptr<directory> home(dir);
    user u1("username", "AP@ssw0rd!", "noempty", "", 0, home);
    directory *created=new dirMock("ciao");
    EXPECT_CALL(*dir, addDirectory("ciao", id)).WillOnce(::testing::Return(std::shared_ptr<directory>(created)));
    u1.newDirectory("ciao", "./", id);
}

struct userSerialization: ::testing::Test{
    std::stringstream stream;

    void storeUser(const user& u){
        boost::archive::text_oarchive oa(stream);
        oa<<u;
    }
    void loadUser(user& u){
        boost::archive::text_iarchive ia(stream);
        ia>>u;
    }
};

TEST_F(userSerialization, serialize){
    user toStore("username", UserTestRobust::goodPwd, "nickname", "", 0, nullptr);
    user toLoad("username2", UserTestRobust::goodPwd, "nickname", "", 1, nullptr);
    ASSERT_NE(toStore, toLoad);
    storeUser(toStore);
    loadUser(toLoad);
    EXPECT_EQ(toLoad, toStore);
}