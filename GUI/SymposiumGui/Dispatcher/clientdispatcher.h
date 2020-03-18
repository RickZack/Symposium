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
 * File:   serverdispatcher.cpp
 * Project: Symposium
 * Authors:
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Cristian Gianetto <s265387 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 9 Febbraio 2020, 16.23
 */


#ifndef CLIENTDISPATCHER_H
#define CLIENTDISPATCHER_H

#include <QObject>
#include <QWidget>
#include <QtNetwork>
#include <iostream>
#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "../../../SymposiumException.h"
#include "../../../message.h"
#include "../../../SymClient.h"


class sigin;

namespace Symposium {

    class clientdispatcher : public QObject
    {
        Q_OBJECT

    private:
        SymClient client;
        QTcpSocket socket;
        QHostAddress svAddress = QHostAddress("127.0.0.1");
        quint16 svPort = 1234;
        void *currentWindow;

    public:
        clientdispatcher(QObject *parent = nullptr);
        void sendMessage(const std::shared_ptr<clientMessage> MessageToSend);
        //void sendMessage_withBoost(const std::shared_ptr<clientMessage> MessageToSend);
        signUpMessage signUp(const std::string &username, const std::string &pwd, const std::string &nickname, const std::string &iconPath);
        void logIn(const std::string &username, const std::string &pwd);
        askResMessage openSource(const std::string &path, const std::string &name, privilege reqPriv);
        askResMessage openNewSource(const std::string &resourceId, privilege reqPriv, const std::string &destPath, const std::string& destName);
        askResMessage createNewSource(const std::string &path, const std::string &name);
        askResMessage createNewDir(const std::string &path, const std::string &name);
        symbolMessage localInsert(int resourceId, const symbol &newSym, const std::pair<int, int> &index);
        symbolMessage localRemove(int resourceId, const std::pair<int, int> indexes);
        privMessage editPrivilege(const std::string &targetUser, const std::string &resPath, const std::string &resName, privilege newPrivilege);
        uriMessage shareResource(const std::string &resPath, const std::string &resName, const uri &newPrefs);
        askResMessage renameResource(const std::string &resPath, const std::string &resName, const std::string &newName);
        askResMessage removeResource(const std::string &resPath, const std::string &resName);
        updateDocMessage closeSource(int resourceId);
        userDataMessage editUser(user &newUserData);
        clientMessage removeUser();
        clientMessage logout();
        updateDocMessage mapSiteIdToUser(const document &currentDoc);
        void test_sendMessage();

        /**
         * @brief constructs a @ref signUpMessage to send to the server to ask for registration
         * @param a pointer to a sigin window
         */
        //void setSignIn(sigin *si);


    private:
        /*int getmsgaction(const std::shared_ptr<clientMessage> Message);
        int getClassOfMessage(const clientMessage& Message);
        int getPrivilege(const privilege priv);
        int getmsgOutcome(const msgOutcome& Message);
        QString getActionOwner(const std::shared_ptr<clientMessage> Message);
        QString getnewUser(const user& utente);
        QString getSymbolserialized(const symbol& sym);
        QString getUriserialized(const uri& uri);*/
        //classi per eccezioni
        class sendFailure{};

    public slots:
        void readyRead();
    };
}

#endif // CLIENTDISPATCHER_H
