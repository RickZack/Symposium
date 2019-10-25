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
#include "../filesystem.h"
#include <cstdlib>
#include "../user.h"
#include "../document.h"
#include "../uri.h"

struct documentMock: public document {
    documentMock() : document(0) {};

    MOCK_METHOD2(access, document(const user &newActive, privilege accessPriv));

};

struct fileMock: public file{
    fileMock(): file("dummy", "."){};
    MOCK_METHOD1(getUserPrivilege, privilege(const user&));
};

struct FileSystemTestT: ::testing::Test{
    file *f;
    ::testing::NiceMock<documentMock> *document;
    FileSystemTestT(){
        document=new ::testing::NiceMock<documentMock>();
        f=new file("f", ".");
    }
    ~FileSystemTestT(){
        delete document;
    }
};

TEST_F(FileSystemTestT, accessTest)
{

    user u("user", "", "", "", 0, std::shared_ptr<directory>());
    fileMock *f=new fileMock();
    /*EXPECT_CALL(*f, getUserPrivilege(u)).WillOnce(::testing::Return(privilege::none));
    EXPECT_THROW(f->access(u, privilege::owner), filesystemException);
    EXPECT_CALL(*f, getUserPrivilege(u)).WillOnce(::testing::Return(privilege::modify));
    EXPECT_THROW(f->access(u, privilege::owner), filesystemException);*/
    EXPECT_CALL(*f, getUserPrivilege(u)).WillOnce(::testing::Return(privilege::owner));
    EXPECT_CALL(*document, access(u, privilege::modify));
    f->access(u, privilege::modify);
}

TEST(FileSystemTest, getSetFileTest)
{
    directory d("d");
    class symlink sym("sym", ".", "f");
    user u("user", "", "", "", 0, std::shared_ptr<directory>());
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

TEST(FileSystemTest, printFileTest)
{
    directory *d= new directory("root");
    std::shared_ptr<directory> home(d);
    user u1("", "", "", "", 0, home);
    fileMock *f=new fileMock();
    EXPECT_CALL(*f, getUserPrivilege(u1)).WillOnce(::testing::Return(privilege::none));
    EXPECT_EQ("You no longer have the possibility to access the file in any mode", f->print(u1));
}

TEST(FileSystemTest, getDirectoryGetFileTest)
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

TEST(FileSystemTest, addDirAddFileAddSymPrintTest)
{
    directory *d=new directory("root");
    std::shared_ptr<directory> home(d);
    user u1("", "", "", "", 0, home);
    std::shared_ptr<directory> dir=d->addDirectory("cart1");
    ASSERT_FALSE(dir==nullptr);
    EXPECT_EQ("root\r\n-cart1\r\n", d->print(u1));
    std::shared_ptr<file> f=d->addFile(".", "file1");
    ASSERT_FALSE(f==nullptr);
    EXPECT_EQ("root\r\n-cart1\r\n-file1\r\n", d->print(u1));
    std::shared_ptr<class symlink> sym=d->addLink(".", "sym");
    ASSERT_FALSE(sym==nullptr);
    EXPECT_EQ("root\r\n-cart1\r\n-file1\r\n-sym\r\n", d->print(u1));
}

TEST(FileSystemTest, removeTest)
{
    directory *d=new directory("root");
    std::shared_ptr<directory> home(d);
    user u1("", "", "", "", 0, home);
    std::shared_ptr<directory>cart1;
    cart1=d->addDirectory("cart1");
    std::shared_ptr<filesystem> cart2;
    cart2=d->remove(u1, "/root", "cart1");
    EXPECT_EQ("root\r\n", d->print(u1));
}

TEST(FileSystemTest, printSymTest)
{
    directory *d=new directory("root");
    std::shared_ptr<directory> home(d);
    user u1("", "", "", "", 0, home);
    std::shared_ptr<file> f1;
    f1=d->addFile("file1", "/root");
    class symlink sym("sym", "/root", "file1");
    EXPECT_EQ("sym", sym.print(u1));
}
