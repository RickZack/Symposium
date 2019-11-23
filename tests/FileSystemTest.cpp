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


#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdlib>
#include "../filesystem.h"
#include "../user.h"
#include "../document.h"
#include "../uri.h"
#include "../AccessStrategy.h"
#include "../symbol.h"

using namespace Symposium;

struct documentMock: public document {
    documentMock() : document(0) {};

    MOCK_METHOD2(access, document&(const user &newActive, privilege accessPriv));

};

struct RMOAccessMock: public RMOAccess{
    RMOAccessMock(): RMOAccess(){};
    MOCK_METHOD2(validateAction, bool(const std::string &targetUser, privilege requested));
    MOCK_METHOD2(setPrivilege, privilege(const std::string &targetUser, privilege toGrant));
    MOCK_METHOD1(getPrivilege, privilege(const std::string &targetUser));

};



struct FileSystemTestT: ::testing::Test{
    file *f;
    ::testing::NiceMock<documentMock> *document;
    ::testing::NiceMock<RMOAccessMock> *rmo;
    FileSystemTestT(){
        document=new ::testing::NiceMock<documentMock>();
        rmo=new ::testing::NiceMock<RMOAccessMock>();
        f=new file("f", "./somedir");
    }
    ~FileSystemTestT() override{
        delete f;
        ::testing::Mock::AllowLeak(document);
        ::testing::Mock::AllowLeak(rmo);
    }
};

TEST_F(FileSystemTestT, DISABLED_FileSetGetPrivilegeTest)
{
    std::string u="username";
    user aUser(u, "AP@ssw0rd!", "noempty", "", 0, nullptr);
    EXPECT_CALL(*rmo, setPrivilege(u, privilege::modify));
    f->setUserPrivilege(u, privilege::modify);
    EXPECT_CALL(*rmo, getPrivilege(u));
    f->getUserPrivilege(u);
}



TEST_F(FileSystemTestT, DISABLED_accessTest)
{
    user u("username", "AP@ssw0rd!", "noempty", "", 0, nullptr);
    f->setUserPrivilege(u.getUsername(), privilege::owner);
    EXPECT_CALL(*document, access(u, privilege::modify)).WillOnce(::testing::ReturnRef(*document));
    f->access(u, privilege::modify);
    f->setUserPrivilege(u.getUsername(), privilege::modify);
    EXPECT_CALL(*document, access(u, privilege::modify)).WillOnce(::testing::ReturnRef(*document));
    f->access(u, privilege::modify);
    f->setUserPrivilege(u.getUsername(), privilege::readOnly);
    EXPECT_THROW(f->access(u, privilege::owner), filesystemException);
    f->setUserPrivilege(u.getUsername(), privilege::none);
    EXPECT_THROW(f->access(u, privilege::modify), filesystemException);
}

TEST(FileSystemTest, getSetDirSymTest)
{
    directory d("d");
    class symlink sym("sym", ".", "f");
    std::string u="username";
    user aUser(u, "AP@ssw0rd!", "noempty", "", 0, nullptr);
    uri u1(uriPolicy::activeAlways);
    EXPECT_THROW(sym.getSharingPolicy(), filesystemException);
    std::cout << "SymLinkError "<< std::endl;
    EXPECT_THROW(sym.getUserPrivilege(u), filesystemException);
    std::cout << "SymLinkError "<< std::endl;
    EXPECT_THROW(sym.setUserPrivilege(u, privilege::owner), filesystemException);
    std::cout << "SymLinkError "<< std::endl;
    EXPECT_THROW(sym.setSharingPolicy(u, u1), filesystemException);
    std::cout << "SymLinkError "<< std::endl;

    EXPECT_THROW(d.getSharingPolicy(), filesystemException);
    std::cout << "DirectoryError "<< std::endl;
    EXPECT_THROW(d.getUserPrivilege(u), filesystemException);
    std::cout << "DirectoryError "<< std::endl;
    EXPECT_THROW(d.setUserPrivilege(u, privilege::owner), filesystemException);
    std::cout << "DirectoryError "<< std::endl;
    EXPECT_THROW(d.setSharingPolicy(u, u1), filesystemException);
    std::cout << "DirectoryError "<< std::endl;
}

TEST(FileSystemTest, DISABLED_printFileTest)
{
    std::string u="username";
    user aUser(u, "AP@ssw0rd!", "noempty", "", 0, nullptr);
    file f("file", "./somedir");
    f.setUserPrivilege(u, privilege::owner);
    EXPECT_EQ("file owner", f.print(u));
    f.setUserPrivilege(u, privilege::none);
    EXPECT_EQ("file You no longer have the possibility to access the file in any mode", f.print(u));
}

TEST(FileSystemTest, DISABLED_getDirectoryGetFileTest)
{
    directory *d=new directory("root");
    std::shared_ptr<directory> home(d);
    std::shared_ptr<directory>cart1;
    cart1=d->addDirectory("cart1");
    std::shared_ptr<file> f1;
    f1=d->addFile("file1", "/root");
    EXPECT_THROW(d->getDir("/root", "file1"), filesystemException);
    EXPECT_THROW(d->getFile("/root", "cart1"), filesystemException);
}
/*
TEST(FileSystemTest, addDirAddFileAddSymPrintTest)
{
    directory *d=new directory("root");
    std::shared_ptr<directory> home(d);
    user u1("username", "AP@ssw0rd!", "noempty", "", 0, home);
    std::shared_ptr<directory> dir=d->addDirectory("cart1");
    ASSERT_FALSE(dir==nullptr);
    EXPECT_EQ("root\r\n-cart1\r\n", d->print(u1.getUsername()));
    std::shared_ptr<file> f=d->addFile(".", "file1");
    ASSERT_FALSE(f==nullptr);
    EXPECT_EQ("root\r\n-cart1\r\n-file1\r\n", d->print(u1.getUsername()));
    std::shared_ptr<class symlink> sym= d->addLink(".", "sym", <#initializer#>, <#initializer#>);
    ASSERT_FALSE(sym==nullptr);
    EXPECT_EQ("root\r\n-cart1\r\n-file1\r\n-sym\r\n", d->print(u1.getUsername()));
}
 */

TEST(FileSystemTest, removeTest)
{
    directory *d=new directory("root");
    std::shared_ptr<directory> home(d);
    user u1("username", "AP@ssw0rd!", "noempty", "", 0, nullptr);
    std::shared_ptr<directory>cart1;
    cart1=d->addDirectory("cart1");
    std::shared_ptr<filesystem> cart2;
    cart2=d->remove(u1, "/root", "cart1");
    EXPECT_EQ("root\r\n", d->print(u1.getUsername()));
}

TEST(FileSystemTest, DISABLED_printSymTest)
{
    directory *d=new directory("root");
    std::shared_ptr<directory> home(d);
    user u1("username", "AP@ssw0rd!", "noempty", "", 0, nullptr);
    std::shared_ptr<file> f1;
    f1=d->addFile("file1", "/root");
    class symlink sym("sym", "/root", "file1");
    EXPECT_EQ("sym", sym.print(u1.getUsername()));
}

/*
 * More tests to prove robustness
 */
struct directoryAccesser: public directory{ //only to access the protected members of directory from tests
    directoryAccesser(const std::string &name) : directory(name) {};
    AccessStrategy* getStrategy(){
        return strategy.get();
    }
    MOCK_METHOD2(getFile, std::shared_ptr<file>(const std::string&, const std::string&));
};

struct fileAccesser: public file{ //only to access the protected members of directory from tests
    fileAccesser(const std::string &name) : file(name, "./somedir") {};
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
    symlinkAccesser(const std::string &name) : symlink(name, "", "") {};
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

TEST_F(FileSystemTestRobust, FileThrowsOnMalformedRealPath){
    file *f;
    std::string someWrongFormats[]={"../", "..", ".", "./", "path", "./dir/file.jpg"};
    for (auto& path:someWrongFormats)
        EXPECT_THROW(f=new file("fileName", path), filesystemException);
}

TEST_F(FileSystemTestRobust, FileAcceptsWellFormedRealPath) {
    std::string aGoodPath{"./dir1/dir2/dir3"};
    file f("fileName", "./dir1/dir2/dir3");
}

struct FileSystemTestSharing: ::testing::Test{
    directoryAccesser dir;
    symlinkAccesser sym;
    fileAccesser f;
    uri expected;
    static const user u;

    static const std::string username;

    FileSystemTestSharing():dir("dir"), f("file"), sym("link"){};

    ::testing::AssertionResult getRootIsImplemented(const char* m_expr, std::shared_ptr<directory> root){
        if(root.get()== nullptr || directory::getRoot()!=root)
            return ::testing::AssertionFailure()<<"Unable to run this test since directory::getRoot() is not correctly implemented yet";
        return ::testing::AssertionSuccess();
    }

    void verifySetPrivilegeOnFile(privilege toAssign=privilege::owner){
        RMOAccessMock* dummyStrategy=new RMOAccessMock();
        f.setDummyStrategy(dummyStrategy);
        EXPECT_CALL(*dummyStrategy, setPrivilege(username, toAssign)).WillOnce(::testing::Return(toAssign));
        f.setUserPrivilege(username, toAssign);
    }

    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -anotherUsername (another user's directory)
     *     -file1
     *   -someUser (user's directory)
     *     -dir1
     *       -sym1
     *       -file2
     * (expected is dir1)
     */
    void constructTree(){
        ASSERT_PRED_FORMAT1(getRootIsImplemented, directory::getRoot());
        auto expected=directory::getRoot()->addDirectory(username, 1)->addDirectory("dir1", 7);
        directory::getRoot()->addDirectory("anotherUsername", 2);
        auto inserted=directory::getRoot()->addFile("./1/2", "file1");
        /*
         * Simulate the fact that the user obtains a [defaultPrivilege] on [file1]
         */
        inserted->setUserPrivilege(username, uri::getDefaultPrivilege());
        directory::getRoot()->addFile("./1/7", "file2");
        auto linkToInserted= directory::getRoot()->addLink("./1/7", "sym1", "/1/2", "/1/7/"+std::to_string(inserted->getId()));
        std::shared_ptr<file> ret=std::dynamic_pointer_cast<file>(directory::getRoot()->get("./1/2/", std::to_string(inserted->getId())));
    }
};
const std::string FileSystemTestSharing::username="someUser";
const user FileSystemTestSharing::u(username, "P@assW0rd!", "noempty", "", 0, directory::getRoot());

TEST_F(FileSystemTestSharing, setUserPrivilegeOnFileCallsStrategy) {
    RMOAccessMock* dummyStrategy=new RMOAccessMock();
    f.setDummyStrategy(dummyStrategy);
    EXPECT_CALL(*dummyStrategy, setPrivilege(username, privilege::owner));
    f.setUserPrivilege(username, privilege::owner);
}

TEST_F(FileSystemTestSharing, setSharingPolicyOnFile) {
    //Just to verify that doesn't return a default constructed uri
    expected.activateCount(2);
    ASSERT_NO_FATAL_FAILURE(verifySetPrivilegeOnFile());
    EXPECT_CALL(*f.getStrategyMocked(), getPrivilege(username)).WillOnce(::testing::Return(privilege::owner));
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
    EXPECT_CALL(*f.getStrategyMocked(), getPrivilege(username));
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
    expected<<resourceType::file<<" "<<f.getName()<<" "<<privilege::readOnly;
    unsigned  spaces=rand()%10+1; //expect n spaces
    std::string ret=f.print(username, false, spaces);
    EXPECT_EQ(expected.str().insert(0, spaces, ' '), ret);
}

TEST_F(FileSystemTestSharing, getSearchesRecursive){
    ASSERT_PRED_FORMAT1(getRootIsImplemented, directory::getRoot());
    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -someUser (user's directory)
     *     -dir1
     * (expected is dir1)
     */
    auto expected=directory::getRoot()->addDirectory(username, 1)->addDirectory("dir1", 7);
    auto ret=directory::getRoot()->get("./1/", "7");
    auto ret2=directory::getRoot()->getDir("./", "1")->get("./", "7");
    EXPECT_EQ(ret2, ret);
    EXPECT_EQ(expected, ret);
    directory::getRoot()->remove(u, "./", "1");
}

TEST_F(FileSystemTestSharing, setNameSearchesRecursive){
    ASSERT_PRED_FORMAT1(getRootIsImplemented, directory::getRoot());
    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -someUser (user's directory)
     *     -dir1
     * (expected is dir1)
     */
    std::string newName="NewName";
    auto expected=directory::getRoot()->addDirectory(username, 1)->addDirectory("dir1", 7);
    std::string oldName=expected->getName();
    std::string ret=directory::getRoot()->setName("./1/", "7", newName);
    EXPECT_EQ(newName, expected->getName());
    EXPECT_EQ(oldName, ret);
    directory::getRoot()->remove(u, "./", "1");
}

TEST_F(FileSystemTestSharing, AddFileInsertsRecursive){
    ASSERT_PRED_FORMAT1(getRootIsImplemented, directory::getRoot());
    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -someUser (user's directory)
     *     -dir1
     *       -file1
     * (expected is dir1)
     */
    auto expected=directory::getRoot()->addDirectory(username, 1)->addDirectory("dir1", 7);
    auto inserted=directory::getRoot()->addFile("./1/7", "file1");
    auto ret=directory::getRoot()->get("./1/7/", std::to_string(inserted->getId()));
    EXPECT_EQ(inserted, ret);
    directory::getRoot()->remove(u, "./", "1");
}

TEST_F(FileSystemTestSharing, AddLinkInsertsRecursive){
    ASSERT_PRED_FORMAT1(getRootIsImplemented, directory::getRoot());
    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -anotherUsername (another user's directory)
     *     -sym1
     *   -someUser (user's directory)
     *     -dir1
     *       -file1
     * (expected is dir1)
     */
    auto expected=directory::getRoot()->addDirectory(username, 1)->addDirectory("dir1", 7);
    directory::getRoot()->addDirectory("anotherUsername", 2);
    auto inserted=directory::getRoot()->addFile("./1/7", "file1");
    auto linkToInserted= directory::getRoot()->addLink("./2", "sym1", "./1/7/", std::to_string(inserted->getId()));
    std::shared_ptr<file> ret=directory::getRoot()->getFile("./2/", std::to_string(linkToInserted->getId()));
    EXPECT_EQ(inserted, ret);
    directory::getRoot()->remove(u, "./", "1");
    //directory::getRoot()->remove(u, "./", "2");
}

TEST_F(FileSystemTestSharing, printSymLink){
    ASSERT_PRED_FORMAT1(getRootIsImplemented, directory::getRoot());
    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -anotherUsername (another user's directory)
     *     -sym1
     *   -someUser (user's directory)
     *     -dir1
     *       -file1
     * (expected is dir1)
     */
    auto expected=directory::getRoot()->addDirectory(username, 1)->addDirectory("dir1", 7);
    directory::getRoot()->addDirectory("anotherUsername", 2);
    auto inserted=directory::getRoot()->addFile("./1/7", "file1");

    /*
     * Fake authorization, the normal stack is:
     * from user::accessFile call getFile()
     * from user::accessFile call getShare() on the uri contained into the retrieved file
     * from user::accessFile call setUserPrivilege() on the retrieved file with the privilege that is possible to grant
     * from user::accessFile call addLink()
     */
    inserted->setUserPrivilege(username, privilege::readOnly);

    auto linkToInserted= directory::getRoot()->addLink("./2", "sym1", "./1/7/", std::to_string(inserted->getId()));
    std::ostringstream out; int n_space=rand()%100+1;
    std::string spaces; spaces.insert(0, n_space, ' ');
    out<<spaces<<resourceType::symlink<<" "<<linkToInserted->getName()<<" "<<privilege::readOnly;
    EXPECT_EQ( out.str(), linkToInserted->print(username, false, n_space));
}

TEST_F(FileSystemTestSharing, accessOnFile){
    ASSERT_PRED_FORMAT1(getRootIsImplemented, directory::getRoot());
    documentMock d;
    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -someUser (user's directory)
     *     -dir1
     *       -file1
     * (expected is dir1)
     */
    //These two following lines are only explanatory, not useful for the test but to understand this structure is used
    auto expected=dir.addDirectory(username, 1)->addDirectory("dir1", 7);
    auto inserted=dir.addFile("./1/7", "file1");
    fileAccesser* dummy=new fileAccesser("dummyFile");
    EXPECT_CALL(dir, getFile("./1/7/", std::to_string(inserted->getId()))).WillOnce(::testing::Return(std::shared_ptr<file>(dummy)));
    EXPECT_CALL(*dummy, access(u,privilege::readOnly)).WillOnce(::testing::ReturnRef(d));
    //EXPECT_CALL(d, access(u, privilege::readOnly));
    dir.access(u, "./1/7/", std::to_string(inserted->getId()), privilege::readOnly);
}

TEST_F(FileSystemTestSharing, printDirectoryRecursive){
    ASSERT_NO_FATAL_FAILURE(constructTree()); //set the following tree
    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -anotherUsername (another user's directory)
     *     -file1
     *   -someUser (user's directory)
     *     -dir1
     *       -sym1
     *       -file2
     * (expected is dir1)
     */

    std::ostringstream res; //will contain the print of someUser's directory
    /*
     * Explaining on why we expect this string as output:
     * addLink() is usually called by user::accessFile, whose job is to obtain a privilege on the file whose resId
     * (concatenation of ids) is given and create a symlink in its home directory. In constructTree() we simulated
     * this behaviour setting the privilege for [username] to uri::getDefaultPrivilege() for file [file1]. In this
     * way, we expect sym1 (that should point to file1) to return its name and the privilege that the user holds
     * on [file1] (not on the symlink itself, that doesn't make much sense).
     */
    res<<"someUser"<<" "<<"dir1\r\n"<<"  "<<"sym1 "<<uri::getDefaultPrivilege()<<"\r\n"<<"  "<<"file2 "<<privilege::owner<<"\r\n";
    EXPECT_EQ(res.str(), directory::getRoot()->getDir("./", "1")->print(username, true, 0));
}

TEST_F(FileSystemTestSharing, removeDirectoryRecursive){
    ASSERT_PRED_FORMAT1(getRootIsImplemented, directory::getRoot());
    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -anotherUsername (another user's directory)
     *     -file1
     *   -someUser (user's directory)
     *     -dir1
     *       -sym1
     *       -file2
     * (expected is dir1)
     */
    auto expected=directory::getRoot()->addDirectory(username, 1)->addDirectory("dir1", 7);
    directory::getRoot()->addDirectory("anotherUsername", 2);
    auto inserted=directory::getRoot()->addFile("./2", "file1");
    auto file2=directory::getRoot()->addFile("./1/7", "file2");
    auto linkToInserted= directory::getRoot()->addLink("./1/7", "sym1", "/2", "/1/7/"+std::to_string(inserted->getId()));
    std::shared_ptr<file> ret=std::dynamic_pointer_cast<file>(directory::getRoot()->get("./2/", std::to_string(inserted->getId())));
    //Remove file2, should not throw
    //EXPECT_CALL(file2, deleteFromStrategy("username"));
    directory::getRoot()->remove(u, "/1/7", std::to_string(file2->getId()));
    //Try to remove again file2, should throw
    EXPECT_THROW(directory::getRoot()->remove(u, "/1/7", std::to_string(file2->getId())), filesystemException);
    directory::getRoot()->remove(u, "./", "2");
}

TEST_F(FileSystemTestSharing, accessOnSymlink){
    ASSERT_PRED_FORMAT1(getRootIsImplemented, directory::getRoot());
    /*
     * Create a tree like this:
     * -/ (root directory)
     *   -anotherUsername (another user's directory)
     *     -file1
     *   -someUser (user's directory)
     *     -dir1
     *       -sym1
     * (expected is dir1)
     */
    auto expected=directory::getRoot()->addDirectory(username, 1)->addDirectory("dir1", 7);
    directory::getRoot()->addDirectory("anotherUsername", 2);
    auto file1=directory::getRoot()->addFile("./2", "file1");
    auto sym1= directory::getRoot()->addLink("./1/7", "sym1", "./2", std::to_string(file1->getId()));
    std::shared_ptr<file> ret=directory::getRoot()->getFile("./1/7", std::to_string(sym1->getId()));
    //if symlink::access (and file::access) are correctly implemented, they should return the same document
    document& fromFile1=file1->access(u, uri::getDefaultPrivilege());
    document& fromSym1= sym1->access(u, uri::getDefaultPrivilege());
    //test against the pointers, to avoid that, if document is not implemented yet, two different document can appear to be the same
    EXPECT_EQ(&fromFile1, &fromSym1);
}