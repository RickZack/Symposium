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

#include <QtNetwork>
#include <iostream>
#include <sstream>
#include <forward_list>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include "../../../SymposiumException.h"
#include "../../../document.h"
#include "../../../message.h"
#include "../../../SymClient.h"
#include "../winmanager.h"

class notepad;
class inserturi;
class onlineusers;
class alluser;
class choosedir;
class activecounterlink;
class activetimerlink;
class activealwayslink;
class activenonlink;
class choosepriv;
class chrono;



const int TEMPOATTESA = 180000;                                 /**< Time the client waits for a response from the server, in milliseconds  */
const int TEMPOMAXREADYREAD = 250;                              /**< Waiting time after processing 20 consecutive messages, in milliseconds  */

namespace Symposium{

    class clientdispatcher : public QObject
    {
        Q_OBJECT

    private:
        SymClient client;                                       /**< variable that model this client  */
        QTcpSocket socket;                                      /**< socket used to communicated with server  */
        QHostAddress svAddress = QHostAddress("127.0.0.1");     /**< variable to contain the IP address of the server  */
        quint16 svPort = 1234;                                  /**< variable to contain the port of the server  */
        QTimer timer;                                           /**< timer used to wait a response from the server  */
        QTimer timer_to_read;                                   /**< timer used to call the readyRead method in case of many packages to manage   */
        SymWinManager winmanager;                               /**< GUI windows manager  */
        std::queue<std::chrono::milliseconds> attese;           /**< queue to contain the time of sending messages  */
        std::string userpwd;                                    /**< variable to contain the password that user inserted in signin or signup window  */
        std::string username;                                   /**< variable to contain the username that user inserted in signin or signup window  */
        uint_positive_cnt::type openDocumentID;                 /**< variable to contain the id of document that client want to open or has just created  */
        uint_positive_cnt::type openFileID;                     /**< variable to contain the id of file that client want to open or has just created  */
        bool appIsClosing;                                      /**< this variable is true if the application will close, false otherwise  */
        bool userIsLogged;                                      /**< this variable is true if the user is logged in, false otherwise  */
        bool connectionClosed;
    public:
        clientdispatcher(QObject *parent = nullptr);

        /**
         * @brief this method is invoke from the main to started the application
         * @param argc passed from the main function
         * @param argv passed from the main function
         */
        int run(int argc, char **argv);

        void setServerAddress(std::string address, std::string port);

        std::pair<std::string, std::string> getServerAddress();

        /**
         * @brief start the @ref timer if it is not already start, otherwise insert the @ref timeToSend into queue @ref attese
         * @param timeToSend
         */
        void TimerStart(std::chrono::milliseconds timeToSend);

        /**
         * @brief it establish a connection between this client and Symposium server, if there isn't one yet
         */
        void openConnection();

        /**
         * @brief send on the socket the message to deliver it to the server
         * @param MessageToSend pointer to the message to be sent
         */
        void sendMessage(const std::shared_ptr<clientMessage> MessageToSend);

        /**
         * @brief invoke the corrisponding method in Symclient and send to the server the message that @ref SymClient's method create
         * @param username the username chosen by the user
         * @param pwd the password chosen by the user
         * @param nickname the nickname chosen by the user
         * @param iconPath the path of the icon chosen by the user
         */
        void signUp(const std::string &username, const std::string &pwd, const std::string &nickname, const std::string &iconPath);

        /**
         * @brief invoke the corrisponding method in Symclient and send to the server the message that @ref SymClient's method create
         * @param username the username of the user to login
         * @param pwd the password of the user to login
         */
        void logIn(const std::string &username, const std::string &pwd);

        /**
         * @brief when this client do a signup that end succesfully, the system do an automatic login thanks this method
         */
        void autologIn();

        /**
         * @brief invoke the corrisponding method in Symclient and send to the server the message that @ref SymClient's method create
         * @param path the path of the file to open, relative to user's home directory
         * @param name the id of the file to open
         * @param reqPriv the privilege requested opening the file
         */
        void openSource(const std::string &path, const std::string &name, privilege reqPriv);

        /**
         * @brief invoke the corrisponding method in Symclient and send to the server the message that @ref SymClient's method create
         * @param resourceId the universal resource identifier for the resource to open
         * @param reqPriv the privilege requested opening the file
         * @param destPath the path inside user's home directory where to put a symlink to the file
         */
        void openNewSource(const std::string &resourceId, privilege reqPriv, const std::string &destPath, const std::string& destName);

        /**
         * @brief it provides the list of online users on the current document
         * @param documentID the document's ID which on you want online users
         * @return forward list to contain a pair with user reference and @ref sessionData, contained into document
         */
        const std::forward_list<std::pair<const user *, sessionData>> onlineUser(uint_positive_cnt::type documentID);

        /**
         * @brief it provides the list of all users on the document that has @ref documentID
         * @param documentID the document's ID which on you want all users
         * @return the list of all users
         */
        std::unordered_map<std::string, privilege> allUser(uint_positive_cnt::type documentID);

        /**
         * @brief invoke the corrisponding method in Symclient and send to the server the message that @ref SymClient's method create
         * @param path the path of the file to create, relative to user's home directory
         * @param name the name of the file to create
         */
        void createNewSource(const std::string &path, const std::string &name);

        /**
         * @brief invoke the corrisponding method in Symclient and send to the server the message that @ref SymClient's method create
         * @param path the path of the directory to create, relative to user's home directory
         * @param name the name of the directory to create
         */
        void createNewDir(const std::string &path, const std::string &name);

        /**
         * @brief invoke the corrisponding method in Symclient and send to the server the message that @ref SymClient's method create
         * @param resourceId the document ID of the deletion refers to
         * @param newSym the symbol to insert
         * @param index the indexes of the symbol to insert
         */
        void localInsert(uint_positive_cnt::type resourceId, const symbol &newSym, const std::pair<int, int> &index);

        /**
         * @brief invoke the corrisponding method in Symclient and send to the server the message that @ref SymClient's method create
         * @param resourceId the document ID of the deletion refers to
         * @param index the indexes of the symbol to delete
         */
        void localRemove(uint_positive_cnt::type resourceId, const std::pair<int, int> indexes);

        /**
         * @brief it used to insert the symbol located at the @ref index position from the correct notepad GUI window
         * @param resourceId the document ID the insertion refers to
         * @param siteId the site id of the user performing the insertion
         * @param index the indexes of the symbol to insert
         */
        void remoteInsert(uint_positive_cnt::type resourceId, const symbol &newSym, uint_positive_cnt::type siteId, std::pair<unsigned int, unsigned int> index);

        /**
         * @brief it used to remove the symbol located at the @ref indexes position from the correct notepad GUI window
         * @param resourceId the document ID the deletion refers to
         * @param siteId the site id of the user performing the insertion
         * @param index the indexes of the symbol to insert
         */
        void remoteRemove(uint_positive_cnt::type resourceId, uint_positive_cnt::type siteId, std::pair<int, int> indexes);

        /**
         * @brief invoke the corrisponding method in Symclient and send to the server the message that @ref SymClient's method create
         * @param targetUser the user whose privilege has to be modified
         * @param resPath the relative path of the resource
         * @param newPrivilege the new privilege to be granted to @e targetUser
         * @param documentID the id of the document
         */
        void editPrivilege(const std::string &targetUser, std::string &resPath, privilege newPrivilege, uint_positive_cnt::type documentID);

        /**
         * @brief invoke the corrisponding method in Symclient and send to the server the message that @ref SymClient's method create
         * @param resPath the relative path of the resource
         * @param resName the id of the resource
         * @param newPrefs new sharing preferences to set the resource to
         */
        void shareResource(const std::string &resPath, const std::string &resName, const uri &newPrefs);

        /**
         * @brief invoke the corrisponding method in Symclient and send to the server the message that @ref SymClient's method create
         * @param resPath the relative path to the user's @e home directory where to create the file
         * @param resName the resource's id
         * @param newName the new resource's name
         */
        void renameResource(const std::string &resPath, const std::string &resName, const std::string &newName);

        /**
         * @brief invoke the corrisponding method in Symclient and send to the server the message that @ref SymClient's method create
         * @param resPath the relative path to the user's @e home directory where to delete the file
         * @param resName the resource's id
         */
        void removeResource(const std::string &resPath, const std::string &resName);

        /**
         * @brief invoke the corrisponding method in Symclient and send to the server the message that @ref SymClient's method create
         * @param resourceId document ID that to be closed
         */
        void closeSource(uint_positive_cnt::type resourceId);

        /**
         * @brief invoke the method @ref updateCursorPos in Symclient and send to the server the message that @ref SymClient's method create
         * @param resId the id of the document
         * @param block X coordinate of the new position of cursor
         * @param column Y coordinate of the new position of cursor
         */
        void moveMyCursor(uint_positive_cnt::type resId, int block, int column);

        /**
         * @brief invoke the corrisponding method on the correct notepad GUI to add a new cursor
         * @param siteID the id of the user to be add his cursor
         * @param username the username of the user to be add his cursor
         * @param resourceID the ID of the document to add the cursor to
         */
        void addUserCursor(uint_positive_cnt::type siteID, std::string username, uint_positive_cnt::type resourceID);

        /**
         * @brief invoke the corrisponding method on the correct notepad GUI to move the cursor
         * @param resId the ID of the document to move the cursor to
         * @param block X coordinate of the new position of cursor
         * @param column Y coordinate of the new position of cursor
         * @param siteId the id of the user to be move his cursor
         */
        void moveUserCursor(uint_positive_cnt::type resId, int block, int column, uint_positive_cnt::type siteId);

        /**
         * @brief invoke the corrisponding method on the correct notepad GUI to remove the cursor
         * @param resourceID the ID of the document to remove the cursor to
         * @param siteId the id of the user to remove his cursor
         */
        void removeUserCursor(uint_positive_cnt::type siteID, uint_positive_cnt::type resourceID);

        /**
         * @brief invoke the corrisponding method on the correct notepad GUI to verify the symbol
         * @param resId the ID of the document to verify symbol
         * @param newSym the symbol to verify
         * @param indexes the indexes of the symbol to verify
         */
        void verifySymbol(uint_positive_cnt::type resId, const symbol &newSym, std::pair<int, int> indexes);

        /**
         * @brief it provides the color to assign to user
         * @param documentID the ID of the document
         * @param siteId the id of the user
         */
        Color getColor(uint_positive_cnt::type documentID, uint_positive_cnt::type siteID);

        Color getColorbyUsername(uint_positive_cnt::type documentID, const std::string& username);

        /**
         * @brief method to provide the ID of logged user's home directory
         */
        uint_positive_cnt::type getHomeIDofCurrentUser();

        /**
         * @brief method to provide the current user to the GUI
         */
        const user& getUser();

        /**
         * @brief invoke the corrisponding method in Symclient and send to the server the message that @ref SymClient's method create
         * @param newUserData the user with new data
         */
        void editUser(user &newUserData);

        /**
         * @brief invoke the corrisponding method in Symclient and send to the server the message that @ref SymClient's method create
         */
        void removeUser(const std::string &pwd);

        /**
         * @brief invoke the corrisponding method in Symclient and send to the server the message that @ref SymClient's method create
         */
        void logout();

        /**
         * @brief invoke the corrisponding method in Symclient and send to the server the message that @ref SymClient's method create
         */
        void mapSiteIdToUser(const document &currentDoc);

        void localEditLineStyle(uint_positive_cnt::type docId, const std::pair<alignType, unsigned>& oldLineStyle, const std::pair<alignType, unsigned>& newLineStyle, unsigned row);

        void remoteEditLineStyle(uint_positive_cnt::type docId, const std::pair<alignType, unsigned>& newLineStyle, unsigned row);
        /**
         * @brief it provides a string with all file that in user's root directory
         * @return the string of content of the user's home
         */
        std::string showHome();

        /**
         * @brief it provides a string with all file of the user
         * @param recursive boolean parameter, true for obtain all files in all directory of the user
         * @return the string of the content
         */
        std::string showDir(bool recursive);

        /**
         * @brief closes all the notepads, set the state to closing and closes the Qt application
         */
        void closeApp();

        /**
         * @brief signal the success of operation to the GUI
         */
        void successAction();

        /**
         * @brief it provides the content of requested directory
         * @param ID_Cartella the id of directory
         * @param path the path of the directory
         * @return the string of directory's content
         */
        std::string getStr(std::string ID_Cartella, std::string path);

        /**
         * @brief it provides the GUI with the document that has the same ID of @ref openDocumentID
         * @return the document to open
         */
        const document& getOpenDocument();

        /**
         * @brief it provides the GUI the ID of file that contain the document that has the same ID of openDocumentID
         * @return the id of the file that contain the document to be open
         */
        uint_positive_cnt::type getOpenFileID();

        /**
         * @brief it provides the GUI the user's privilege on the file to be open
         * @return the user's privilege on the file that contain the document to be open
         */
        privilege getMyPrivilegeOnFileOpen();

        /**
         * @brief it stop the timer when server has reply. If the queue @ref attese is not empty, it start a new timer
         */
        void stopTimer();

        /**
         * @brief it sets @ref openFileID and @ref openDocumentID of requested document from the user and signal the success to the GUI
         * @param docID the id of requested document
         * @param fileID the id of file that contain the requested document
         */
        void updateRequestDocFileandSuccess(uint_positive_cnt::type docID, uint_positive_cnt::type fileID);

        /**
         * @brief it signals to the GUI that it is not possible to remove a resource because it is open
         */
        void failureRemovedResource();

        /**
         * @brief it used to notify the success of setUserColors to the GUI
         */
        void successSetUserColors(uint_positive_cnt::type docID);

        ~clientdispatcher() override;

    private:

        //classe per eccezione
    class sendFailure : public std::exception {};

    public slots:

        /**
         * @brief method that manages the reception of messages from the server
         */
        void readyRead();

        /**
         * @brief method invoked when the response from the server did not arrive within the @ref TEMPOATTESA
         */
        void TimerExpired();

        /**
         * @brief disconnects when the connection to the server is lost
         */
        void connectionLost();

        /**
         * @brief calls the readyRead method when the timer @ref timer_to_read expires
         */
        void continueReadyRead();
    };
}

#endif // CLIENTDISPATCHER_H
