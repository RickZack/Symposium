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
 * File:   serverdispatcher.h
 * Project: Symposium
 * Authors:
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Cristian Gianetto <s265387 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 17 Dicembre 2019, 13.07
 */

#ifndef SERVERDISPATCHER_H
#define SERVERDISPATCHER_H

#include <QtNetwork>
#include <boost/archive/text_iarchive.hpp>
#include "../../../SymposiumException.h"
//#include "../message.h"
#include "../../../SymServer.h"


 namespace Symposium {
 /**
  * @brief class used to model a Network Server of Symposium system
  *
  * The task of this class is to receive messages by the clients and send the answer to them for
  * keep the system in a correct and consistance state.
  */
        class ServerDispatcher : public QTcpServer
        {
            Q_OBJECT

        private:
            SymServer server;
            QMap<int, QTcpSocket*> Connected_Clients;   //QMap<SocketDescriptor, QTcpSocket>
            QMap<int, int> Connected_SymUser;           //QMap<SiteID, SocketDescriptor>
            clientMessage temp;


        public:
            ServerDispatcher();

            /**
             * @brief constructs a server to receive connections request
             */
            void startServer();
            void sendMessage(const std::shared_ptr<serverMessage> MessageToSend, int siteID);

        protected:
            void incomingConnection(qintptr socketDescriptor) override;

        public slots:
            void readyRead();

        };
 }

#endif // SERVERDISPATCHER_H
