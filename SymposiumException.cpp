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
}

const char *SymposiumException::what() const noexcept {
    return errorMsg;
}

const char* SymServerException::SymServerErrors[]={"the user already exists",
                              "the user has wrong parameters", "the user is not registered", "wrong password",
                              "user already active","the user is not logged in", "user is not working on the document",
                              "actionUser is not logged in or targetUser is not registered",
                              "user is working on the document", "user not found"};

SymServerException::SymServerException(SymServerExceptionCodes exceptionCode, const char *file, int line,
                                       const char *func) :
                                       SymposiumException(file, line, func, SymServerErrors[static_cast<int>(exceptionCode)]){};

const char* userException::userErrors[]={"An error occurred while creating a new file", "An error occurred while creating a new directory",
                                         "An error occurred while trying to access the directory where you want save the link",
                                         "A system error", "An error occurred while trying to add link", "An error occurred while trying to edit privilege",
                                         "An error occurred while trying to change privilege", "An error occurred while trying to share resource",
                                         "An error occurred while trying to rename the object", "An error occurred while trying to remove the object",
                                         "Too short password", "Too long password",
                                         "You don't use any alphabetic character for password", "You don't use any number for password",
                                         "You don't use any special character for password", "You can choose a nickname"};

userException::userException(userExceptionCodes exceptionCode, const char *file, int line,
                                       const char *func) :
        SymposiumException(file, line, func, userErrors[static_cast<int>(exceptionCode)]){};

