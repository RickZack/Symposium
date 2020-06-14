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
 * File:   SymposiumException.h
 * Project: Symposium
  * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Cristian Gianetto <s265387 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 28 ottobre 2019, 00:47
 */
#ifndef SYMPOSIUM_SYMPOSIUMEXCEPTION_H
#define SYMPOSIUM_SYMPOSIUMEXCEPTION_H


#include <exception>

namespace Symposium{
    /**
     * @brief Unpacks the __FILE__, __LINE__ and __PRETTY_FUNCTION__ in SymposiumException subclasses
     */
    #define UnpackFileLineFunction()  __FILE__, __LINE__, __PRETTY_FUNCTION__

    /**
     * @brief class used as exception base class for classes in Symposium namespace. It composes a custom error explaining
     * message in the form "function: [functionName], in file: [filename], line: [lineNumber]: [errorMessage]", allowing
     * subclasses to specify an error message.
     * @exceptsafe no-throw
     *
     * Every specific Symposium exception class must derive from this class and use the SymposiumException constructor.
     * The exception safety is no-throw because the class is designed to encode the error explanation string starting
     * from C-style const literals composed in a static-allocated C-style string of predefined constant length.
     * Function @e snprintf is used in constructor, so buffer overflow is avoided. No memory is allocated nor de-allocated in methods.
     */
    class SymposiumException: public std::exception{
        static constexpr int msgMaxLen = 400;
        char errorMsg[msgMaxLen];
        const char *errorCodeMsg;
    protected:
        SymposiumException(const char *file, const int line, const char *func,
                           const char *errDescr);

    public:
        const char *what() const noexcept override;

        const char *getErrorCodeMsg() const noexcept ;

        ~SymposiumException() override=default;
    };


    /**
     * @brief models an exception occurred in the context of @ref SymServer class
     * @exceptsafe no-throw
     *
     * Constructor simply performs lookup on @e SymServerErrors table and pass the error message to @ref SymposiumException constructor.
     * The exception safety is no-throw because the @e SymServerErrors table is statically allocated and conversions from the underlying
     * type to @ref SymServerExceptionCodes are not allowed, so there is no way to access an illegal element of @e SymServerErrors.
     */
    class SymServerException : public SymposiumException {
        static const char* SymServerErrors[];
    public:
        /**
         * @brief Specific error codes for @ref SymServerException. They are used as indexes to the error table string
         */
        enum SymServerExceptionCodes{userAlreadyExist=0, userWrongParam, userNotRegistered, userWrongPwd,
            userAlreadyActive, userNotLogged, userNotWorkingOnDoc, actionUserNotLoggedOrTargetUserNotRegistered,
            userWorkingOnDoc, userNotFound, noUserWorkingOnRes};

        explicit SymServerException(SymServerExceptionCodes exceptionCode, const char *file, int line, const char *func);
        virtual ~SymServerException()=default;
    };

    class userException : public SymposiumException {
        static const char* userErrors[];
    public:
        /**
         * @brief Specific error codes for @ref userException. They are used as indexes to the error table string
         */
        enum userExceptionCodes{shortPwd=0, longPwd, noCharPwd, noNumPwd, noSpecialCharPwd, nickname, noPriv,
                noPermissionToChange, LinkNoCorrect, haveLink, path, pathForLink, minPriv, userSpace};

        explicit userException(userExceptionCodes exceptionCode, const char *file, int line, const char *func);
        virtual ~userException()=default;
    };

    class filesystemException : public SymposiumException {
        static const char* filesystemErrors[];
    public:
        /**
         * @brief Specific error codes for @ref filesystemException. They are used as indexes to the error table string
         */
        enum filesystemExceptionCodes{objSha=0, pathEmpty, pathNvalid, changePriv, notOwn, noPermission, noGet, noGetDir,
                noGetFile, sameName, notOnlyOwn, someoneWork, notOwnDelete};

        explicit filesystemException(filesystemExceptionCodes exceptionCode, const char *file, int line, const char *func);
        virtual ~filesystemException()=default;
    };

    class SymClientException : public SymposiumException {
        static const char* SymClientErrors[];
    public:
        /**
         * @brief Specific error codes for @ref SymClientException. They are used as indexes to the error table string
         */
        enum SymClientExceptionCodes{noRelatedMessage=0, nocolorOfUser, noActiveFile};

        explicit SymClientException(SymClientExceptionCodes exceptionCode, const char *file, int line, const char *func);
        virtual ~SymClientException()=default;
    };

    class messageException : public SymposiumException {
        static const char* messageErrors[];
    public:
        /**
         * @brief Specific error codes for @ref messageException. They are used as indexes to the error table string
         */
        enum messageExceptionCodes{action=0, notClient, notSucc, askResMes, sendResMes, symb, upAct, upDoc, userData, cursor};

        explicit messageException(messageExceptionCodes exceptionCode, const char *file, int line, const char *func);
        virtual ~messageException()=default;
    };

    class clientdispatcherException : public SymposiumException {
        static const char* clientdispatcherErrors[];
    public:
        /**
         * @brief Specific error codes for @ref clientdispatcherException. They are used as indexes to the error table string
         */
        enum clientdispatcherExceptionCodes{UnknownMessageAction=0, UnknownClassOfMessage, MsgActionNotAllowed};

        explicit clientdispatcherException(clientdispatcherExceptionCodes exceptionCode, const char *file, int line, const char *func);
        virtual ~clientdispatcherException()=default;
    };

    class documentException : public SymposiumException {
        static const char* documentErrors[];
    public:

        enum docExceptionCodes{positionNotFound=0, fixPositionSorting, outOfBounds, deletingEmptyChar, insertingAfterNewLine};

        explicit documentException(docExceptionCodes exceptionCode, const char *file, int line, const char *func);
        virtual ~documentException()=default;
    };


}


#endif //SYMPOSIUM_SYMPOSIUMEXCEPTION_H
