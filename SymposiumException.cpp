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
 * File:   SymposiumException.cpp
 * Project: Symposium
  * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Cristian Gianetto <s265387 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 28 ottobre 2019, 00:47
 */
#include "SymposiumException.h"
#include <cstdio>
#include <cstring>
using namespace Symposium;

//char SymposiumException::genericError[msgMaxLen]="Generic Symposium error";

SymposiumException::SymposiumException(const char *file, const int line, const char *func,
                                       const char *errDescr) {
    snprintf(errorMsg, msgMaxLen, "function: %s, in file: %s, line %d: %s", func, file, line, errDescr);
    errorCodeMsg=errDescr;
}

const char *SymposiumException::what() const noexcept {
    return errorMsg;
}

const char *SymposiumException::getErrorCodeMsg() const noexcept {
    return errorCodeMsg;
}

const char* SymServerException::SymServerErrors[]={"the user already exists",
                              "the user has wrong parameters", "the user is not registered", "wrong password",
                              "user already active","the user is not logged in", "user is not working on the document",
                              "actionUser is not logged in or targetUser is not registered",
                              "user is working on the document", "user not found",
                              "There are no users working on this resource"};

SymServerException::SymServerException(SymServerExceptionCodes exceptionCode, const char *file, int line,
                                       const char *func) :
                                       SymposiumException(file, line, func, SymServerErrors[static_cast<int>(exceptionCode)]){};

const char* userException::userErrors[]={"Too short password", "Too long password",
                                         "You don't use any alphabetic character for password", "You don't use any number for password",
                                         "You don't use any special character for password", "You can choose a nickname",
                                         "You don't have any privilege to this file anymore", "You cannot upgrade this privilege",
                                         "Inserted link is not correct", "You already have the access to this file",
                                         "The path for new resource is not correct", "The link isn't correct, check it and try again"};

userException::userException(userExceptionCodes exceptionCode, const char *file, int line,
                                       const char *func) :
        SymposiumException(file, line, func, userErrors[static_cast<int>(exceptionCode)]){};



const char* filesystemException::filesystemErrors[]={"Object is not shareable", "The path is empty", "The format of path is not valid",
                                                     "You are an owner, so you cannot change your privilege", "You are not an owner to share this file",
                                                     "You have not permission to access this file in this mode", "The element has not been found with get",
                                                     "Directory are you searching for, is not present", "File are you searching for, is not present",
                                                     "You already have an element with the same name", "You are not the only owner of this file, so you cannot delete it",
                                                     "You have some error while you trying to delete your symlink"};

filesystemException::filesystemException(filesystemExceptionCodes exceptionCode, const char *file, int line,
                             const char *func) :
        SymposiumException(file, line, func, filesystemErrors[static_cast<int>(exceptionCode)]){};

const char* SymClientException::SymClientErrors[]={"No active document with that ID", "No relative message found", "User not found on that document"};

SymClientException::SymClientException(SymClientExceptionCodes exceptionCode, const char *file, int line,
                                       const char *func) :
        SymposiumException(file, line, func, SymClientErrors[static_cast<int>(exceptionCode)]){};

const char* messageException::messageErrors[]={"Action is not consistent with the message type", "This is not a client message",
                                               "The action had not succeded", "This is not an askRes message",
                                               "This is not a sendRes  message", "This is not a symbol message",
                                               "This is not an updateActive message", "This is not an updateDoc message",
                                               "This is not an userData message", "This is not a cursor message"};

messageException::messageException(messageExceptionCodes exceptionCode, const char *file, int line,
                                       const char *func) :
        SymposiumException(file, line, func, messageErrors[static_cast<int>(exceptionCode)]){};


const char* clientdispatcherException::clientdispatcherErrors[]={"The action of this message is not valid", "Unrecognized class of message", "The action of this message is not allowed"};

clientdispatcherException::clientdispatcherException(clientdispatcherExceptionCodes exceptionCode, const char *file, int line,
                                       const char *func) :
        SymposiumException(file, line, func, clientdispatcherErrors[static_cast<int>(exceptionCode)]){};
