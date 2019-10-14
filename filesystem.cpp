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
 * File:   filesystem.cpp
 * Project: Symposium
 * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Alice Morano <s259158 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 18 Giugno 2019, 22.02
 */
#include "filesystem.h"

int filesystem::idCounter=0;

filesystem::filesystem(const std::string &name) : name(name) {
    //TODO: implement
}

int filesystem::getId() const {
    return id;
}

const std::string &filesystem::getName() const {
    return name;
}

privilege filesystem::getUserPrivilege(const user &targetUser) const {
    throw filesystemException("Object"+name+"is not shareable");
}

const uri &filesystem::getSharingPolicy() const {
    return sharingPolicy;
}

privilege filesystem::setUserPrivilege(const user &targetUser, privilege newPrivilege) {
    throw filesystemException("Object"+name+"is not shareable");
}

std::string filesystem::setName(const std::string &newName) {
    //TODO: implement
    return name;
}

uri filesystem::setSharingPolicy(const user &actionUser, uri &newSharingPrefs) {
    throw filesystemException("Object"+name+"is not shareable");
}

file::file(const std::string &name, const std::string &realPath) : filesystem(name), realPath(realPath), doc(0) {
    //TODO: implement
}

resourceType file::resType() const {
    return resourceType::file;
}

privilege file::getUserPrivilege(const user &targetUser) const {
    //TODO: implement
    return privilege::none;
}

privilege file::setUserPrivilege(const user &targetUser, privilege newPrivilege) {
    //TODO: implement
    return privilege::none;
}

uri file::setSharingPolicy(const user &actionUser, uri &newSharingPrefs) {
    return uri();
}

document file::access(const user &targetUser) {
    //TODO: implement
    return document(0);
}

void file::store(const std::string &storePath) const {
    //TODO: implement
}

void file::load(const std::string &loadPath) {
    //TODO: implement
}

void file::send() const {
    //TODO: implement
}

std::string file::print(const user &targetUser, bool recursive, int indent) const {
    //TODO: implement
    return "";
}

directory::directory(const std::string &name) : filesystem(name) {
    //TODO: implement
}

std::shared_ptr<directory> directory::nullDir() {
    return std::shared_ptr<directory>();
}

std::shared_ptr<directory> directory::getRoot() {
    //TODO: implement
    return std::shared_ptr<directory>();
}

std::shared_ptr<filesystem> directory::get(const std::string &path, const std::string &name) {
    //TODO: implement
    return std::shared_ptr<filesystem>();
}

std::shared_ptr<directory> directory::getDir(const std::string &path, const std::string &name) {
    //TODO: implement
    return nullptr;
}

std::shared_ptr<file> directory::getFile(const std::string &path, const std::string &name) {
    //TODO: implement
    return nullptr;
}

std::string& directory::setName(const std::string &path, const std::string &fileName, const std::string& newName) {
    //TODO: implement
    return name;
}

std::shared_ptr<directory> directory::addDirectory(const std::string &name) {
    //TODO: implement
    return std::shared_ptr<directory>();
}

std::shared_ptr<file> directory::addFile(const std::string &path, const std::string &name) {
    //TODO: implement
    return nullptr;
}
std::shared_ptr<symlink> directory::addLink(const std::string &path, const std::string &name)
{
    //TODO: implement
    return nullptr;
}

resourceType directory::resType() const {
    return resourceType::directory;
}

document directory::access(const user &targetUser, const std::string &path, const std::string &resName) {
    //TODO: implement
    return document(0);
}

std::shared_ptr<filesystem> directory::remove(const user &targetUser, const std::string &path, const std::string &resName) {
    //TODO: implement
    return std::shared_ptr<filesystem>();
}

void directory::store(const std::string &storePath) const {
    //TODO: implement
}

void directory::load(const std::string &loadPath) {
    //TODO: implement
}

void directory::send() const {
    //TODO: implement
}

std::string directory::print(const user &targetUser, bool recursive, int indent) const {
    //TODO: implement
    return "";
}

symlink::symlink(const std::string &name, const std::string &pathToFile, const std::string &fileName) : filesystem(name), pathToFile(pathToFile), fileName{fileName} {
    //TODO: implement
}

resourceType symlink::resType() const {
    return resourceType::symlink;
}

document symlink::access() {
    //TODO: implement
    return document(0);
}

void symlink::store(const std::string &storePath) const {
    //TODO: implement
}

void symlink::load(const std::string &loadPath) {
    //TODO: implement
}

void symlink::send() const {
    //TODO: implement
}

std::string symlink::print(const user &targetUser, bool recursive, int indent) const {
    //TODO: implement
    return "";
}