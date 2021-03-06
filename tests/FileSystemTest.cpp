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
 * File:   FileSystemTest.cpp
 * Project: Symposium
 * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Alice Morano <s259158 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 22 agosto 2019, 13.38
 */

#include <memory>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdlib>
#include "../filesystem.h"
#include "../user.h"

#include <boost/serialization/shared_ptr.hpp> //NOLINT

#pragma clang diagnostic ignored "cert-err58-cpp" //NOLINT
#pragma clang diagnostic ignored "cert-msc50-cpp" //NOLINT

using namespace Symposium;


struct documentMock: public document {
    documentMock() : document(0) {};

    MOCK_METHOD2(access, document&(const user &newActive, privilege accessPriv));

};

struct RMOAccessMock: public RMOAccess{
    RMOAccessMock(): RMOAccess(){};
    MOCK_CONST_METHOD2(validateAction, bool(const std::string &targetUser, privilege requested));
    MOCK_METHOD2(setPrivilege, privilege(const std::string &targetUser, privilege toGrant));
    MOCK_CONST_METHOD1(getPrivilege, privilege(const std::string &targetUser));

};

struct directoryAccesser: public directory{ //only to access the protected members of directory from tests
    explicit directoryAccesser(const std::string &name) : directory(name) {};
    AccessStrategy* getStrategy(){
        return strategy.get();
    }
    /*
     * Used to achieve independence of tests while directory is a singleton class
     * resets all the parameters to the ones predefined when the filesystem is originally created
     */
    void resetFreshFilesystem(){
        auto r= static_cast<directoryAccesser*>(this->root.get()); //NOLINT
        if(r){
            //r->idCounter=0;
            r->contained.clear();
            this->root.reset(); //the static object is now free (root removed) NOLINT
            //Note:: r is not valid at this point
        }
    }
    MOCK_METHOD2(getFile, std::shared_ptr<file>(const std::string&, const std::string&));
};

struct fileAccesser: public file{ //only to access the protected members of directory from tests
    explicit fileAccesser(const std::string &name) : file(name, 0) {};
    /*
     * Used in tests when we want to set expectations on the strategy object, that normally
     * is an internal detail of file
     */
    RMOAccessMock* getStrategyMocked(){
        auto ptr=dynamic_cast<RMOAccessMock*>(strategy.get());
        if(ptr==nullptr)
            throw std::exception();
        return ptr;
    }

    /*
     * Used when we want to retrieve the strategy object set and hold by file
     */
    AccessStrategy* getStrategy(){
        return strategy.get();
    }

    void setDummyStrategy(RMOAccessMock *pMock) {
        strategy.reset(pMock);
    }
    MOCK_METHOD2(access, document&(const user &targetUser, privilege accessMode));

    /*
     * Used when we are testing not that a function calls file::access, but when we want to test
     * if file::access does what we expect
     */
    document& accessNotMocked(const user &targetUser, privilege accessMode){
        return file::access(targetUser, accessMode);
    }
};

struct symlinkAccesser: public symlink{ //only to access the protected members of directory from tests
    explicit symlinkAccesser(const std::string &name) : symlink(name, "./dir/dir2", "fakesym") {};
    AccessStrategy* getStrategy(){
        return strategy.get();
    }
};

struct FileSystemTestRobust: ::testing::Test{
    directoryAccesser dir;
    symlinkAccesser sym;
    fileAccesser f;

    FileSystemTestRobust():dir("dir"), f("file"), sym("link"){};
};

TEST_F(FileSystemTestRobust, directoryUsesTrivialAccess){
    AccessStrategy* used=dir.getStrategy();
    ASSERT_FALSE(used== nullptr);
    EXPECT_TRUE(dynamic_cast<RMOAccess*>(used)==nullptr);
}

TEST_F(FileSystemTestRobust, symlinkUsesTrivialAccess){
    AccessStrategy* used=sym.getStrategy();
    ASSERT_FALSE(used== nullptr);
    EXPECT_TRUE(dynamic_cast<RMOAccess*>(used)==nullptr);
}

TEST_F(FileSystemTestRobust, fileUsesRMOAccess){
    AccessStrategy* used=f.getStrategy();
    ASSERT_FALSE(used== nullptr);
    EXPECT_TRUE(dynamic_cast<TrivialAccess*>(used)==nullptr);
}

TEST_F(FileSystemTestRobust, returnCorrectResourceType){
    EXPECT_EQ(resourceType::directory, dir.resType());
    EXPECT_EQ(resourceType::file, f.resType());
    EXPECT_EQ(resourceType::symlink, sym.resType());
}

TEST_F(FileSystemTestRobust, differentFilesystemObjectHaveDifferentId){
    EXPECT_NE(dir.getId(), f.getId());
}
/*
TEST_F(FileSystemTestRobust, FileThrowsOnMalformedRealPath){
    file *f;
    std::string someWrongFormats[]={"../", "..", ".", "./", "path", "./dir/file.jpg"};
    for (auto& path:someWrongFormats)
        EXPECT_THROW(f= new file("resId", 0), filesystemException);
}
*/
TEST_F(FileSystemTestRobust, FileAcceptsWellFormedRealPath) {
    std::string aGoodPath{"./dir1/dir2/dir3"};
    file f("resId", 0);
}

struct FileSystemTestSharing: ::testing::Test{
    directoryAccesser dir;
    symlinkAccesser sym;
    fileAccesser f;
    uri expected;

    //users
    static const user u;
    static const user anotherUser;
    [[maybe_unused]] static const user anotherUser2;

    //data used for user initialization
    static const std::string iconPath;
    static const std::string username;
    static const std::string anotherUsername;
    static const std::string anotherUsername2;
    static const std::string pwd;
    static const std::string nickname;
    static const std::string path;
    static const std::string filename;

    //pointers to filesystem objects created in the tests
    std::shared_ptr<directory> anotherUser2_d; //NOLINT
    std::shared_ptr<directory> anotherUser_d; //NOLINT
    std::shared_ptr<directory> aUser_d;
    std::shared_ptr<directory> dir1;
    std::shared_ptr<file> file1;
    std::shared_ptr<file> file2;
    std::shared_ptr<class symlink> sym1;
    std::shared_ptr<class symlink> sym2;


    FileSystemTestSharing():dir("dir"), f(filename), sym("link"){};


    static ::testing::AssertionResult getRootIsImplemented(const char*, const std::shared_ptr<directory>& root){
        if(root== nullptr || directory::getRoot()!=root)
            return ::testing::AssertionFailure()<<"Unable to run this test since directory::getRoot() is not correctly implemented yet";
        return ::testing::AssertionSuccess();
    }

    void verifySetPrivilegeOnFile(privilege toAssign=privilege::owner){
        auto* dummyStrategy=new RMOAccessMock();
        f.setDummyStrategy(dummyStrategy);
        EXPECT_CALL(*dummyStrategy, setPrivilege(username, toAssign)).WillOnce(::testing::Return(toAssign));
        f.setUserPrivilege(username, toAssign);
    }

    /*
     * ConstructTreeN functions create sample filesystem tree, setting also fake privileges on files
     * These authorizations are fake, the normal stack is:
     * from user::accessFile call getFile()
     * from user::accessFile call getShare() on the uri contained into the retrieved file
     * from user::accessFile call setUserPrivilege() on the retrieved file with the privilege that is possible to grant
     * from user::accessFile call addLink()
     */

    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -aUser (user's directory) (id=1)
     *     -dir1 (id=7)
     */
    void constructTree0(){
        ASSERT_PRED_FORMAT1(getRootIsImplemented, directory::getRoot());
        aUser_d=directory::getRoot()->addDirectory(username, 1);
        dir1=aUser_d->addDirectory("dir1", 7);
    }

    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -aUser (user's directory) (id=1)
     *     -dir1 (id=7)
     *       -file1
     */
    void constructTree1(){
        ASSERT_NO_FATAL_FAILURE(constructTree0());
        file1= directory::getRoot()->addFile("./1/7", "file1");
        /*
         * Simulate the fact that the user created the file, so he has [privilege::owner] on [file1]
         */
        file1->setUserPrivilege(username, privilege::owner);
    }

    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -anotherUser (another user's directory) (id=2)
     *     -sym1
     *   -aUser (user's directory) (id=1)
     *     -dir1 (id=7)
     *       -file1
     */
    void constructTree2(){
        ASSERT_NO_FATAL_FAILURE(constructTree1());
        anotherUser_d=directory::getRoot()->addDirectory("anotherUsername", 2);
        sym1= directory::getRoot()->addLink("./2", "sym1", "./1/7", std::to_string(file1->getId()));
        /*
         * Simulate the fact that the privilege acquired adding a symlink
         */
        file1->setUserPrivilege(anotherUsername, uri::getDefaultPrivilege());
    }

    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -anotherUser (another user's directory) (id=2)
     *     -file2
     *     -sym1
     *   -aUser (user's directory) (id=1)
     *     -dir1 (id=7)
     *       -sym2
     *       -file1
     */
    void constructTree3(){
        ASSERT_NO_FATAL_FAILURE(constructTree2());
        file2= directory::getRoot()->addFile("./2", "file2");
        sym2= directory::getRoot()->addLink("./1/7", "sym2", "./2", std::to_string(file2->getId()));
        /*
         * Simulate the fact that the user created the file, so he has [privilege::owner] on [file2]
         * Simulate the fact that the privilege acquired adding a symlink
         */
        file2->setUserPrivilege(anotherUsername, privilege::owner);
        file2->setUserPrivilege(username, uri::getDefaultPrivilege());
        //std::shared_ptr<file> ret=std::dynamic_pointer_cast<file>(directory::getRoot()->get("./1/2/", std::to_string(file1->getId())));
    }

    ~FileSystemTestSharing() override{
        //release the content of root directory and the root itself, so all the trees constructed are destroyed
        dir.resetFreshFilesystem();
    }


};
const std::string FileSystemTestSharing::iconPath="./icons/icon1.jpg";
const std::string FileSystemTestSharing::username="username";
const std::string FileSystemTestSharing::anotherUsername="anotherUsername";
const std::string FileSystemTestSharing::anotherUsername2="anotherUsername2";
const std::string FileSystemTestSharing::pwd="AP@ssw0rd!";
const std::string FileSystemTestSharing::nickname="nickname";
const std::string FileSystemTestSharing::path="./dir1/dir2";
const std::string FileSystemTestSharing::filename="file1";
const user FileSystemTestSharing::u(username, pwd, nickname, iconPath, 0, nullptr);
[[maybe_unused]] const user FileSystemTestSharing::anotherUser2(anotherUsername2, pwd, nickname, iconPath, 0, nullptr);
const user FileSystemTestSharing::anotherUser(anotherUsername, pwd, nickname, iconPath, 0, nullptr);


TEST_F(FileSystemTestSharing, setUserPrivilegeOnFileCallsStrategy) {
    auto* dummyStrategy=new RMOAccessMock();
    f.setDummyStrategy(dummyStrategy);
    EXPECT_CALL(*dummyStrategy, setPrivilege(username, privilege::owner));
    f.setUserPrivilege(username, privilege::owner);
}

TEST_F(FileSystemTestSharing, setSharingPolicyOnFile) {
    //Just to verify that doesn't return a default constructed uri
    expected.activateCount(2);
    ASSERT_NO_FATAL_FAILURE(verifySetPrivilegeOnFile());
    EXPECT_CALL(*f.getStrategyMocked(), validateAction(username, privilege::owner)).WillOnce(::testing::Return(true));
    uri returned=f.setSharingPolicy(username, expected);
    EXPECT_EQ(expected, returned);
}

/*
 * What happens if the strategy says that user has no right?
 * setSharingPolicy should throw so we don't need an error return value to propagate up
 * to the caller of the function
 */
TEST_F(FileSystemTestSharing, setSharingPolicyOnFileThrowsOnInsufficientPrivilege) {
    //Just to verify that doesn't return a default constructed uri
    expected.activateCount(2);
    ASSERT_NO_FATAL_FAILURE(verifySetPrivilegeOnFile(privilege::modify));
    EXPECT_THROW(f.setSharingPolicy(username, expected), filesystemException);
}

TEST_F(FileSystemTestSharing, setSharingPolicyOnSymlinkThrows) {
    EXPECT_THROW(sym.setSharingPolicy(username, expected), filesystemException);
}

TEST_F(FileSystemTestSharing, setSharingPolicyOnDirThrows) {
    EXPECT_THROW(dir.setSharingPolicy(username, expected), filesystemException);
}

TEST_F(FileSystemTestSharing, accessOnFileCallsValidateAction){
    ASSERT_NO_FATAL_FAILURE(verifySetPrivilegeOnFile(privilege::modify));
    EXPECT_CALL(*f.getStrategyMocked(), validateAction(username, privilege::readOnly)).WillOnce(::testing::Return(true));
    f.accessNotMocked(u,privilege::readOnly);
}

TEST_F(FileSystemTestSharing, accessOnFileThrowsWithAnInsufficientPrivilege){
    ASSERT_NO_FATAL_FAILURE(verifySetPrivilegeOnFile(privilege::readOnly));
    EXPECT_CALL(*f.getStrategyMocked(), validateAction(username, privilege::modify)).WillOnce(::testing::Return(false));
    EXPECT_THROW(f.accessNotMocked(u,privilege::modify), filesystemException);
}

TEST_F(FileSystemTestSharing, printFileWithIndent){
    ASSERT_NO_FATAL_FAILURE(verifySetPrivilegeOnFile(privilege::readOnly));
    EXPECT_CALL(*f.getStrategyMocked(), getPrivilege(username)).WillRepeatedly(::testing::Return(privilege::readOnly));
    std::ostringstream expected;
    expected<<resourceType::file<<" "<<f.getId();
    unsigned  spaces=rand()%10+1; //expect n spaces
    for(int i=0; i<spaces; i++)
        expected<<" ";
    expected<<" "<<f.getName()<<" "<<privilege::readOnly;
    std::string ret=f.print(username, false, spaces);
    EXPECT_EQ(expected.str(), ret);
}

TEST_F(FileSystemTestSharing, getSearchesRecursive){
    ASSERT_NO_FATAL_FAILURE(constructTree0());
    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -aUser (user's directory) (id=1)
     *     -dir1 (id=7)
     */
    auto ret_rec=directory::getRoot()->get("./1", "7");
    auto ret_nrec=directory::getRoot()->getDir("./", "1")->get("./", "7");
    EXPECT_EQ(ret_nrec, ret_rec);
    EXPECT_EQ(dir1, ret_rec);
}

TEST_F(FileSystemTestSharing, setNameSearchesRecursive){
    ASSERT_NO_FATAL_FAILURE(constructTree0());
    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -aUser (user's directory) (id=1)
     *     -dir1 (id=7)
     */
    std::string newName="NewName";
    std::string oldName=dir1->getName();
    std::string ret=directory::getRoot()->setName("./1", "7", newName);
    EXPECT_EQ(newName, dir1->getName());
    EXPECT_EQ(oldName, ret);
}

TEST_F(FileSystemTestSharing, AddFileInsertsRecursive){
    ASSERT_NO_FATAL_FAILURE(constructTree1());
    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -aUser (user's directory) (id=1)
     *     -dir1 (id=7)
     *       -file1
     */
    auto ret=directory::getRoot()->get("./1/7", std::to_string(file1->getId()));
    EXPECT_EQ(file1, ret);
}

TEST_F(FileSystemTestSharing, AddLinkInsertsRecursive){
    ASSERT_NO_FATAL_FAILURE(constructTree2());
    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -anotherUser (another user's directory) (id=2)
     *     -sym1
     *   -aUser (user's directory) (id=1)
     *     -dir1 (id=7)
     *       -file1
     */
    std::shared_ptr<file> ret=directory::getRoot()->getFile("./2", std::to_string(sym1->getId()));
    EXPECT_EQ(file1, ret);
    directory::getRoot()->remove(u, "./", "1");

}

TEST_F(FileSystemTestSharing, printSymLink){
    ASSERT_NO_FATAL_FAILURE(constructTree2());
    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -anotherUser (another user's directory) (id=2)
     *     -sym1
     *   -aUser (user's directory) (id=1)
     *     -dir1 (id=7)
     *       -file1
     */

    std::ostringstream out;
    int n_space=rand()%100+1;
    std::string spaces;
    spaces.insert(0, n_space, ' ');
    out<<resourceType::symlink<<" "<<sym1->getId()<<" "<<spaces<<sym1->getName()<<" "<<uri::getDefaultPrivilege();
    EXPECT_EQ( out.str(), sym1->print(anotherUsername, false, n_space));
}

TEST_F(FileSystemTestSharing, accessOnFile){
    ASSERT_PRED_FORMAT1(getRootIsImplemented, directory::getRoot());
    documentMock d;
    /*
    * Create a tree like this:
    * -/ (root directory)
    *   -aUser (user's directory) (id=1)
    *     -dir1 (id=7)
    *       -file1
    */
    ASSERT_NO_FATAL_FAILURE(constructTree1());
    auto* dummy=new fileAccesser("dummyFile");
    EXPECT_CALL(dir, getFile("./1/7", std::to_string(file1->getId()))).WillOnce(::testing::Return(std::shared_ptr<file>(dummy)));
    EXPECT_CALL(*dummy, access(u,privilege::readOnly)).WillOnce(::testing::ReturnRef(d));
    //EXPECT_CALL(d, access(u, privilege::readOnly));
    dir.access(u, "./1/7", std::to_string(file1->getId()), privilege::readOnly);
}

TEST_F(FileSystemTestSharing, printDirectoryRecursive){
    ASSERT_NO_FATAL_FAILURE(constructTree3()); //set the following tree
    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -anotherUser (another user's directory) (id=2)
     *     -file2
     *     -sym1
     *   -aUser (user's directory) (id=1)
     *     -dir1 (id=7)
     *       -sym2
     *       -file1
     */

    std::ostringstream res; //will contain the print of someUser's directory
    /*
     * Explaining on why we expect this string as output:
     * addLink() is usually called by user::accessFile, whose job is to obtain a privilege on the file whose resId
     * (concatenation of ids) is given and create a symlink in its home directory. In constructTree3() we simulated
     * this behaviour setting the privilege for [username] to uri::getDefaultPrivilege() for file [file1]. In this
     * way, we expect sym1 (that should point to file1) to return its name and the privilege that the user holds
     * on [file1] (not on the symlink itself, that doesn't make much sense).
     */
    res<<username<<" "<<"directory "<<dir1->getId()<<" "<<"dir1\n"<<"file "<<file1->getId()<<"  file1 "<<privilege::owner<<"\n "<<"symlink "<<sym2->getId()<<"  sym2 "<<uri::getDefaultPrivilege()<<"\n ";
    EXPECT_EQ(res.str(), directory::getRoot()->getDir("./", "1")->print(username, true, 0));
}

/*
 * Deletion policies
 * The creator of the file hasn't to remain owner of the file for its entire life.
 * A call to remove on a symlink deletes the symlink and the file pointed if:
 *          + the user is the only owner of the file and no user is active on it;
 * A call to remove on a symlink deletes the symlink but not the file pointed if:
 *          + the file pointed has more owners and the user is one of them;
 *                                         or
 *          + the user is not an owner of the file pointed;
 * A call to remove on a symlink fails without removing the symlink if:
 *          + the user is the only owner but some other user is active on the file pointed;
 * A file can be removed if and only if:
 *          + It has only an owner, and it is the user who is removing the file;
 *          + At the moment of deletion, no user is active on it;
 * A directory can be removed if and only if:
 *          + All of the contained elements fulfill the preceding conditions;
 * An operation on a any element either succeeds or fails without any effect (special care for directories)
 */
TEST_F(FileSystemTestSharing, removeFileLegalCase){
    ASSERT_NO_FATAL_FAILURE(constructTree3()); //set the following tree
    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -anotherUser (another user's directory) (id=2)
     *     -file2
     *     -sym1
     *   -aUser (user's directory) (id=1)
     *     -dir1 (id=7)
     *       -sym2
     *       -file1
     */
    //Remove file1, should not throw
    EXPECT_NO_THROW(directory::getRoot()->remove(u, "./1/7", std::to_string(file1->getId())));
    //Try to remove again file1, should throw
    EXPECT_THROW(directory::getRoot()->remove(u, "./1/7", std::to_string(file1->getId())), filesystemException);
}

TEST_F(FileSystemTestSharing, removeFileNotRemoveFileWithMoreOwners){
    ASSERT_NO_FATAL_FAILURE(constructTree3()); //set the following tree
    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -anotherUser (another user's directory) (id=2)
     *     -file2
     *     -sym1
     *   -aUser (user's directory) (id=1)
     *     -dir1 (id=7)
     *       -sym2
     *       -file1
     */

    //Fake assignment of privileges, they are set into user's class
    file2->setUserPrivilege(username, privilege::owner);

    //file2 has more than one owner, can't be deleted
    EXPECT_THROW(directory::getRoot()->remove(u, "./2", std::to_string(file2->getId())), filesystemException);
}

TEST_F(FileSystemTestSharing, removeFileNotRemoveFileIfUserActiveOnIt){
    ASSERT_NO_FATAL_FAILURE(constructTree3()); //set the following tree
    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -anotherUser (another user's directory) (id=2)
     *     -file2
     *     -sym1
     *   -aUser (user's directory) (id=1)
     *     -dir1 (id=7)
     *       -sym2
     *       -file1
     */

    //file1 has one owner (username) and one writer (anotherUsername). Just set "anotherUser" active and make "u" delete the file
    file1->access(anotherUser, privilege::modify);
    //Remove file1, should throw because "anotherUser" is active on it
    EXPECT_THROW(directory::getRoot()->remove(u, "./1/7", std::to_string(file1->getId())), filesystemException);
    //file1 must be still there
    EXPECT_NO_THROW(directory::getRoot()->get("./1/7", std::to_string(file1->getId())));
}

TEST_F(FileSystemTestSharing, removeFileRemoveIfNoMoreOwners){
    ASSERT_NO_FATAL_FAILURE(constructTree3()); //set the following tree
    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -anotherUser (another user's directory) (id=2)
     *     -file2
     *     -sym1
     *   -aUser (user's directory) (id=1)
     *     -dir1 (id=7)
     *       -sym2
     *       -file1
     */

    //Fake assignment of privileges, they are set into user's class
    file2->setUserPrivilege(username, privilege::owner);

    directory::getRoot()->remove(u, "./1/7", std::to_string(sym2->getId()));
    //If there are no more owners, the file can be deleted
    EXPECT_NO_THROW(directory::getRoot()->remove(anotherUser, "./2", std::to_string(file2->getId())));
}

TEST_F(FileSystemTestSharing, removeSymlinkNotRemoveFilePointedIfHasMoreOwners){
    ASSERT_NO_FATAL_FAILURE(constructTree3()); //set the following tree
    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -anotherUser (another user's directory) (id=2)
     *     -file2
     *     -sym1
     *   -aUser (user's directory) (id=1)
     *     -dir1 (id=7)
     *       -sym2
     *       -file1
     */

    //Fake assignment of privileges, they are set into user's class
    file2->setUserPrivilege(username, privilege::owner);

    //Now a call from user "aUser" should delete the symlink but not the file itself (because anotherUser is also an owner)
    directory::getRoot()->remove(u, "./1/7", std::to_string(sym2->getId()));
    EXPECT_THROW(directory::getRoot()->get("./1/7", std::to_string(sym2->getId())), filesystemException); //sym2 removed
    EXPECT_NO_THROW(directory::getRoot()->get("./2", std::to_string(file2->getId()))); //file2 still there
    EXPECT_EQ(privilege::none,file2->getUserPrivilege(username));
}

TEST_F(FileSystemTestSharing, removeSymlinkNotRemoveSymlinkAndFilePointedIfHasUsersActive){
    ASSERT_NO_FATAL_FAILURE(constructTree3()); //set the following tree
    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -anotherUser (another user's directory) (id=2)
     *     -file2
     *     -sym1
     *   -aUser (user's directory) (id=1)
     *     -dir1 (id=7)
     *       -sym2
     *       -file1
     */

    //Fake assignment of privileges, they are set into user's class. After these instructions, "u" is the only owner
    //and "anotherUser" is active on the file
    file2->setUserPrivilege(username, privilege::owner);
    file2->setUserPrivilege(anotherUsername, privilege::modify);
    file2->access(anotherUser, uri::getDefaultPrivilege());

    //Now a call from user "aUser" should delete the symlink but not the file itself (because anotherUser is active on it)
    EXPECT_THROW(directory::getRoot()->remove(u, "./1/7", std::to_string(sym2->getId())), filesystemException);
    EXPECT_NO_THROW(directory::getRoot()->get("./1/7", std::to_string(sym2->getId()))); //sym2 removed
    EXPECT_NO_THROW(directory::getRoot()->get("./2", std::to_string(file2->getId()))); //file2 still there
    EXPECT_EQ(privilege::owner,file2->getUserPrivilege(username));
}

TEST_F(FileSystemTestSharing, removeSymlinkRemoveSymlinkAndFilePointedIfHasThisOnlyOwner){
    ASSERT_NO_FATAL_FAILURE(constructTree3()); //set the following tree
    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -anotherUser (another user's directory) (id=2)
     *     -file2
     *     -sym1
     *   -aUser (user's directory) (id=1)
     *     -dir1 (id=7)
     *       -sym2
     *       -file1
     */

    //Fake assignment of privileges, they are set into user's class
    file2->setUserPrivilege(username, privilege::owner);
    file2->setUserPrivilege(anotherUsername, privilege::modify);

    //Now a call from user "aUser" should delete the symlink and the file itself
    //(because anotherUser is no more an owner and no one is active on the file)
    directory::getRoot()->remove(u, "./1/7", std::to_string(sym2->getId()));
    EXPECT_THROW(directory::getRoot()->get("./1/7", std::to_string(sym2->getId())), filesystemException); //sym2 not found
    EXPECT_THROW(directory::getRoot()->get("./2", std::to_string(file2->getId())), filesystemException); //file2 not found
}

TEST_F(FileSystemTestSharing, removeSymlinkRemoveSymlinkIfFilePointedHasBeenDeleted){
    ASSERT_NO_FATAL_FAILURE(constructTree3()); //set the following tree
    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -anotherUser (another user's directory) (id=2)
     *     -file2
     *     -sym1
     *   -aUser (user's directory) (id=1)
     *     -dir1 (id=7)
     *       -sym2
     *       -file1
     */

    // "anotherUser" deletes his file, this is legal
    EXPECT_NO_THROW(directory::getRoot()->remove(anotherUser, "./2", std::to_string(file2->getId())));
    //Now a call from user "aUser" should delete the symlink and not throw if file pointed has not been found
    //(because already deleted by "anotherUser)
    EXPECT_NO_THROW(directory::getRoot()->remove(u, "./1/7", std::to_string(sym2->getId())));
    EXPECT_THROW(directory::getRoot()->get("./2", std::to_string(sym2->getId())), filesystemException); //sym2 must be deleted by remove
}

TEST_F(FileSystemTestSharing, removeDirectoryRemovesIfNoOneActiveOnContainedElements){
    ASSERT_NO_FATAL_FAILURE(constructTree3()); //set the following tree
    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -anotherUser (another user's directory) (id=2)
     *     -file2
     *     -sym1
     *   -aUser (user's directory) (id=1)
     *     -dir1 (id=7)
     *       -sym2
     *       -file1
     */
    directory::getRoot()->remove(u, "./1", "7");
    EXPECT_THROW(directory::getRoot()->get("./1", "7"), filesystemException); //dir1 not found
}

TEST_F(FileSystemTestSharing, removeDirectoryNotRemovesIfSomeoneActiveOnContainedElement){
    ASSERT_NO_FATAL_FAILURE(constructTree3()); //set the following tree
    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -anotherUser (another user's directory) (id=2)
     *     -file2
     *     -sym1
     *   -aUser (user's directory) (id=1)
     *     -dir1 (id=7)
     *       -sym2
     *       -file1
     */
    //make someone active on file2
    file1->access(anotherUser, privilege::modify);
    EXPECT_THROW(directory::getRoot()->remove(u, "./1", "7"), filesystemException);

    //dir1 and its content must be still there
    EXPECT_NO_THROW(directory::getRoot()->get("./1", "7"));
    EXPECT_NO_THROW(directory::getRoot()->get("./1/7", std::to_string(sym2->getId())));
    EXPECT_NO_THROW(directory::getRoot()->get("./1/7", std::to_string(file1->getId())));
}

TEST_F(FileSystemTestSharing, removeDirectoryNotRemovesIfMoreOwnersOnContainedElement){
    ASSERT_NO_FATAL_FAILURE(constructTree3()); //set the following tree
    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -anotherUser (another user's directory) (id=2)
     *     -file2
     *     -sym1
     *   -aUser (user's directory) (id=1)
     *     -dir1 (id=7)
     *       -sym2
     *       -file1
     */
    //make "u" an owner fo file2
    file2->setUserPrivilege(username, privilege::owner);
    EXPECT_THROW(directory::getRoot()->remove(anotherUser, "./", "2"), filesystemException);

    //anotherUser dir and contained elements should not be deleted, because deletion of file2 caused an error
    EXPECT_NO_THROW(directory::getRoot()->get("./", "2"));
    EXPECT_NO_THROW(directory::getRoot()->get("./2", std::to_string(file2->getId())));
    EXPECT_NO_THROW(directory::getRoot()->get("./2", std::to_string(sym1->getId())));
}


TEST_F(FileSystemTestSharing, accessOnSymlink){
    ASSERT_NO_FATAL_FAILURE(constructTree2());
    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -anotherUser (another user's directory) (id=2)
     *     -sym1
     *   -aUser (user's directory) (id=1)
     *     -dir1 (id=7)
     *       -file1
     */
    //if symlink::access (and file::access) are correctly implemented, they should return the same document
    document& fromFile1=file1->access(u, uri::getDefaultPrivilege());
    document& fromSym1= sym1->access(anotherUser, uri::getDefaultPrivilege());
    //test against the pointers, to avoid that, if document is not implemented yet, two different document can appear to be the same
    EXPECT_EQ(&fromFile1, &fromSym1);
}

struct filesystemSerialization: ::testing::Test{
    std::shared_ptr<filesystem> toStore, toLoad;
    std::stringstream stream;

    filesystemSerialization()= default;
    void storeFilesystemObj(std::shared_ptr<filesystem> &as){
        boost::archive::text_oarchive oa(stream);
        oa<<as;
    }

    void loadFilesystemObj(std::shared_ptr<filesystem> &as){
        boost::archive::text_iarchive ia(stream);
        ia>>as;
    }
    ~filesystemSerialization() override= default;
};

TEST_F(filesystemSerialization, file){
    toStore=std::make_shared<file>("filename", 0);
    toLoad=std::make_shared<file>("filename", 0);
    ASSERT_NE(*dynamic_cast<file*>(toStore.get()), *dynamic_cast<file*>(toLoad.get()));
    storeFilesystemObj(toStore);
    loadFilesystemObj(toLoad);
    EXPECT_EQ(*dynamic_cast<file*>(toStore.get()), *dynamic_cast<file*>(toLoad.get()));
}

TEST_F(filesystemSerialization, directory){
    toStore=directory::getRoot();
    toLoad=directory::emptyDir();
    ASSERT_NE(*dynamic_cast<directory*>(toStore.get()), *dynamic_cast<directory*>(toLoad.get()));
    storeFilesystemObj(toStore);
    loadFilesystemObj(toLoad);
    EXPECT_EQ(*dynamic_cast<directory*>(toStore.get()), *dynamic_cast<directory*>(toLoad.get()));
}

TEST_F(filesystemSerialization, symlink){
    toStore=std::make_shared<Symposium::symlink>("name", "./path", "filename");
    toLoad=std::make_shared<Symposium::symlink>("name2", "./path", "filename");
    ASSERT_NE(*dynamic_cast<Symposium::symlink*>(toStore.get()), *dynamic_cast<Symposium::symlink*>(toLoad.get()));
    storeFilesystemObj(toStore);
    loadFilesystemObj(toLoad);
    EXPECT_EQ(*dynamic_cast<Symposium::symlink*>(toStore.get()), *dynamic_cast<Symposium::symlink*>(toLoad.get()));
}