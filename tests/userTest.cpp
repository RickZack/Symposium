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
using namespace Symposium;


struct dirMock: public directory{
    dirMock(): directory("dummy"){};

    MOCK_METHOD2(getFile, std::shared_ptr<file>(const std::string&, const std::string&));
    MOCK_METHOD2(addFile, std::shared_ptr<file>(const std::string&, const std::string&));
    MOCK_METHOD2(get, std::shared_ptr<filesystem>(const std::string &path, const std::string &name));
    MOCK_METHOD2(getDir, std::shared_ptr<directory>(const std::string &path, const std::string &name));
    MOCK_METHOD0(getRoot, std::shared_ptr<directory>());
    MOCK_METHOD2(addLink, std::shared_ptr<class symlink>(const std::string &, const std::string &));

    MOCK_METHOD1(addDirectory, std::shared_ptr<directory>(const std::string &name));

    MOCK_METHOD3(access, document(const user &targetUser, const std::string &path, const std::string &resName));
    MOCK_METHOD3(remove, std::shared_ptr<filesystem>(const user &, const std::string &, const std::string &));

    MOCK_CONST_METHOD3(print, std::string(const user &targetUser, bool recursive, int indent));
};

struct fileMock: public file{
    fileMock(): file("dummy", "./somedir"){};
    MOCK_METHOD2(setUserPrivilege, privilege(const user&, privilege));
    MOCK_METHOD2(setSharingPolicy, uri(const user&, uri& newSharingPrefs));
};

struct dummyFunctional{
    bool operator()(){
        return true;
    }
};

struct UserTest: ::testing::Test{
    user *u;
    ::testing::NiceMock<dirMock> *homeDir;
    ::testing::NiceMock<dirMock> *Dir;
    ::testing::NiceMock<dirMock> *Root;
    ::testing::NiceMock<fileMock> *dummyFile;
    static dummyFunctional dummyFunc;

    UserTest(){
        homeDir=new ::testing::NiceMock<dirMock>();
        dummyFile= new ::testing::NiceMock<fileMock>();
        Dir=new ::testing::NiceMock<dirMock>();
        Root=new ::testing::NiceMock<dirMock>();
        u=new user("username", "AP@ssw0rd!", "noempty", "", 0, std::shared_ptr<directory>(homeDir));
    }
    ~UserTest(){
        delete u;
        ::testing::Mock::AllowLeak(homeDir);
        ::testing::Mock::AllowLeak(Root);
        ::testing::Mock::AllowLeak(Dir);
        ::testing::Mock::AllowLeak(dummyFile);
    }
};

TEST_F(UserTest, DISABLED_callAccessFile){
    EXPECT_CALL(*homeDir, getDir("h", "h")).WillOnce(::testing::Return(std::shared_ptr<directory>(Dir)));
    EXPECT_CALL(*Dir, getRoot()).WillOnce(::testing::Return(std::shared_ptr<directory>(Root)));
    EXPECT_CALL(*homeDir, addLink("h/h", "sym"));//aggiungere valore di ritorno
    EXPECT_CALL(*Root, getFile("f", "f"));
    u->accessFile("f/f", "h/h", "sym");

}

TEST(userTest, makeNewFileMock){
    dirMock *dir=new dirMock();
    std::shared_ptr<directory> home(dir);
    user u1("username", "AP@ssw0rd!", "noempty", "", 0, home);
    file *created=new file("ciao", "./somedir");
    EXPECT_CALL(*dir, addFile(".", "ciao")).WillOnce(::testing::Return(std::shared_ptr<file>(created)));
    u1.newFile("ciao");
}

TEST(userTest, makeNewDirMock){
    dirMock *dir=new dirMock();
    std::shared_ptr<directory> home(dir);
    user u1("username", "AP@ssw0rd!", "noempty", "", 0, home);
    directory *created=new directory("ciao");
    EXPECT_CALL(*dir, addDirectory("ciao")).WillOnce(::testing::Return(std::shared_ptr<directory>(created)));
    u1.newDirectory("ciao");
}

TEST_F(UserTest, callEditPrivilege){
    testing::InSequence forceOrder; //force function call order in EXPECT_CALL
    //This user is not useful, no function will be called on it, a mock is not necessary for this
    user otherUser("otherUser", "AP@ssw0rd!", "noempty", "", 0, nullptr);
    //homeDir is a mock for a directory object: the user in the fixture is initialized with this object and not with a
    //directory object, so any call to the methods overriden by dirMock is handled by the test suite
    EXPECT_CALL(*homeDir, getFile(".", "dummyFile")).WillOnce(::testing::Return(std::shared_ptr<file>(dummyFile)));
    EXPECT_CALL(*dummyFile, setUserPrivilege(otherUser, privilege::owner));
    u->editPrivilege(otherUser, ".", "dummyFile", privilege::owner);
}

TEST_F(UserTest, callChangePrivilege){
    testing::InSequence forceOrder; //force function call order in EXPECT_CALL
    //homeDir is a mock for a directory object: the user in the fixture is initialized with this object and not with a
    //directory object, so any call to the methods overriden by dirMock is handled by the test suite
    //user otherUser("otherUser", "", "", "", 0, std::shared_ptr<directory>());
    EXPECT_CALL(*homeDir, getFile(".", "dummyFile")).WillOnce(::testing::Return(std::shared_ptr<file>(dummyFile)));
    EXPECT_CALL(*dummyFile, setUserPrivilege(*u, privilege::owner));
    u->changePrivilege(".", "dummyFile", privilege::owner);
}

TEST_F(UserTest, callRemoveResource)
{
    EXPECT_CALL(*homeDir, remove(*u, ".", "dir"));
    u->removeResource(".", "dir");
}

TEST_F(UserTest, callShowDir)
{
    EXPECT_CALL(*homeDir, print(*u, true, 0));
    u->showDir(false);
}

TEST_F(UserTest, callOpenFile)
{
    EXPECT_CALL(*homeDir, access(*u, ".", "dummyFile"));
    u->openFile(".", "dummyFile", uri::getDefaultPrivilege());
}

TEST_F(UserTest, callShareResource){
    testing::InSequence forceOrder; //force function call order in EXPECT_CALL
    uri ur;
    //homeDir is a mock for a directory object: the user in the fixture is initialized with this object and not with a
    //directory object, so any call to the methods overriden by dirMock is handled by the test suite
    EXPECT_CALL(*homeDir, getFile(".", "dummyFile")).WillOnce(::testing::Return(std::shared_ptr<file>(dummyFile)));
    EXPECT_CALL(*dummyFile, setSharingPolicy(*u, ur));
    u->shareResource(".", "dummyFile", ur);
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