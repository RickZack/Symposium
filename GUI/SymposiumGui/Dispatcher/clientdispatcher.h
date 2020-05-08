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

class sigin;
class notepad;
class signup;
class inserturi;
class home;
class directory;
class onlineusers;
class alluser;
class deleteAccount;
class choosedir;
class changeUserInfo;
class activecounterlink;
class activetimerlink;
class activealwayslink;
class activenonlink;
class choosepriv;
class chrono;



const int TEMPOATTESA = 180000;       //Tempo per cui si attende risposta dal server, in millisecondi

const std::string TIMERSCADUTO = "Unable to receive response from server. Please, check your connection and login again.";

const std::string IMPOSSINVIARE = "Unable to contact the server. Please, check your connection and login again.";

namespace Symposium{

    class clientdispatcher : public QObject
    {
        Q_OBJECT

    private:
        SymClient client;
        QTcpSocket socket;
        QHostAddress svAddress = QHostAddress("127.0.0.1");
        quint16 svPort = 1234;
        int currentWindow = 0;                          //variabile che dice qual è la finestra attiva
        QTimer timer;
        SymWinManager winmanager;                       //gestore finestre
        std::queue<std::pair<std::chrono::milliseconds, uint_positive_cnt::type>> attese;
        uint_positive_cnt::type ResIDofWaitingMessage;
        std::string userpwd;
        std::string username;
        document tp;
        document& requestDoc;
        // ELENCO DEI PUNTATORI A FINESTRA CHE SARRANNO POI DA ELIMINARE

        sigin* finestraLogin;                           //1
        signup* finestraSignup;                         //2
        inserturi* finestraInsertUri;                   //3
        home* finestraHome;                             //4
        deleteAccount* finestraEliminaAccount;          //5
        choosedir* finestraSceltaDir;                   //6
        changeUserInfo* finestraModificaUser;           //7
        activecounterlink* finestraActiveCounterLink;   //8
        activetimerlink* finestraActiveTimerLink;       //9
        activealwayslink* finestraActiveAlwaysLink;     //10
        ::directory* finestraDirectory;                 //12
        onlineusers* finestraOnlineUser;                //13
        alluser* finestraAllUser;                       //14
        activenonlink* finestraActiveNonLink;           //15
        choosepriv* finestraChoosePriv;                 //16

        // FINE ELENCO PUNTATORI ALLE FINESTRE

        std::vector<std::pair<uint_positive_cnt::type,notepad*>> finestreDocumenti;         //coppia resourceID-Puntatore a finestra
        //std::shared_ptr<clientMessage> message;      //Contiene il messaggio che abbiamo inviato e di cui attendiamo risposta dal server

    public:
        clientdispatcher(QObject *parent = nullptr);

        int run(int argc, char **argv);

        void TimerStart(std::chrono::milliseconds timeToSend, uint_positive_cnt::type resourceId);

        void openConnection();

        /**
         * @brief send on the socket the message to deliver it to the server
         * @param MessageToSend pointer to the message to be sent
         */
        void sendMessage(const std::shared_ptr<clientMessage> MessageToSend, uint_positive_cnt::type resourceId = 0);

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

        void autologIn();

        //bool isAutoLogin();


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
         */
        const std::forward_list<std::pair<const user *, sessionData>> onlineUser(uint_positive_cnt::type documentID);

        std::unordered_map<std::string, privilege> allUser(uint_positive_cnt::type documentID);


        void createNewSource(const std::string &path, const std::string &name);
        void createNewDir(const std::string &path, const std::string &name);
        void localInsert(uint_positive_cnt::type resourceId, const symbol &newSym, const std::pair<int, int> &index);
        void localRemove(uint_positive_cnt::type resourceId, const std::pair<int, int> indexes);
        void remoteInsert(uint_positive_cnt::type resourceId, const symbol &newSym, uint_positive_cnt::type siteId, std::pair<unsigned int, unsigned int> index);
        void remoteRemove(uint_positive_cnt::type resourceId, uint_positive_cnt::type siteId, std::pair<int, int> indexes);
        void editPrivilege(const std::string &targetUser, std::string &resPath, privilege newPrivilege, uint_positive_cnt::type documentID);
        void shareResource(const std::string &resPath, const std::string &resName, const uri &newPrefs);
        void renameResource(const std::string &resPath, const std::string &resName, const std::string &newName);
        void removeResource(const std::string &resPath, const std::string &resName);
        void closeSource(uint_positive_cnt::type resourceId);
        void moveMyCursor(uint_positive_cnt::type resId, int block, int column);
        void addUserCursor(uint_positive_cnt::type siteID, std::string username, uint_positive_cnt::type resourceID);
        void moveUserCursor(uint_positive_cnt::type resId, int block, int column, uint_positive_cnt::type siteId);
        void removeUserCursor(uint_positive_cnt::type siteID, uint_positive_cnt::type resourceID);
        void verifySymbol(uint_positive_cnt::type resId, const symbol &newSym, std::pair<int, int> indexes);
        Color getColor(uint_positive_cnt::type documentID, uint_positive_cnt::type siteID);

        /**
         * @brief method to provide the current user to the GUI
         */
        user getUser();

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


        updateDocMessage mapSiteIdToUser(const document &currentDoc);

        /**
         * @brief it provides a string with all file that in user's root directory
         */
        std::string showHome();

        /**
         * @brief it provides a string with all file of the user
         * @param recursive boolean parameter, true for obtain all files in all directory of the user
         */
        std::string showDir(bool recursive);


        //void successLogin();

        //void successLogout();

        //void successSignUp();

        //void successDeleteAccount();

        //void successEditUser();

        //void successInsertUri();

        void successEditPrivilege();

        void successShareResource(std::string path);

        void successOpenSource(document &doc);

        //void successRemoveResource();

        //void successCreateNewDir(const std::string ID);

        void successCreateNewSource(const document &doc);

        //void successRenameResource();

        void closeConnection();

        void successAction();

        std::string getStr(std::string ID_Cartella, std::string path);

        const document& getOpenDocument();


        /**
         * @brief this method assign to @ref finestraLogin the pointer to the sigin window
         * @param a pointer to the sigin window
         */
        //void setSignIn(sigin* si);

        /**
         * @brief this method insert into @ref finestreDocumenti the pair composed of the respurceID of the document and the pointer to the notepad window
         * @param resourceID the ID of the document
         * @param te a pointer to the notepad window
         */
        void setTextEdit(uint_positive_cnt::type resourceID, notepad *te);

        /**
         * @brief this method assign to @ref finestraSignup the pointer to the signup window
         * @param a pointer to the signup window
         */
        //void setSignUp(signup *su);

        /**
         * @brief this method assign to @ref finestraInsertUri the pointer to the InsertUri window
         * @param a pointer to the InsertUri window
         */
        //void setInsertUri(inserturi *iu);

        /**
         * @brief this method assign to @ref finestraHome the pointer to the Home window
         * @param a pointer to the Home window
         */
        //void setHome(home *ho);

        /**
         * @brief this method assign to @ref finestraSceltaDir the pointer to the directory window
         * @param a pointer to the directory window
         */
        //void setDirectory(::directory *dr);

        /**
         * @brief this method assign to @ref finestraEliminaAccount the pointer to the delete account window
         * @param a pointer to the deleteaccount window
         */
        //void setDeleteAccount(deleteAccount *da);

        /**
         * @brief this method assign to @ref finestraSceltaDir the pointer to the choose dir window
         * @param a pointer to the choosedir window
         */
        //void setChooseDir(choosedir *cd);

        /**
         * @brief this method assign to @ref finestraModificaUser the pointer to the change user info window
         * @param a pointer to the changeUserInfo window
         */
        //void setChangeUserInfo(changeUserInfo *cui);

        /**
         * @brief this method assign to @ref finestraActiveCounterLink the pointer to the active counter link window
         * @param a pointer to the activecounterlink window
         */
        //void setActiveCounterLink(activecounterlink *acl);

        /**
         * @brief this method assign to @ref finestraActiveTimerLink the pointer to the active timer link window
         * @param a pointer to the activetimerlink window
         */
        //void setActiveTimerLink(activetimerlink *atl);

        /**
         * @brief this method assign to @ref finestraActiveAlwaysLink the pointer to the active always link window
         * @param a pointer to the activealwayslink window
         */
        //void setActiveAlwaysLink(activealwayslink *aal);

        void setOnlineUser(onlineusers *ou);

        void setAllUser(alluser *au);

        //void setActiveNonLink(activenonlink *anl);

        //void setchoosepriv(choosepriv *pr);

        void stopTimer();


    private:
        /*int getmsgaction(const std::shared_ptr<clientMessage> Message);
        int getClassOfMessage(const clientMessage& Message);
        int getPrivilege(const privilege priv);
        int getmsgOutcome(const msgOutcome& Message);
        QString getActionOwner(const std::shared_ptr<clientMessage> Message);
        QString getnewUser(const user& utente);
        QString getSymbolserialized(const symbol& sym);
        QString getUriserialized(const uri& uri);*/
        notepad* getCorrectNotepadbyResourceID(uint_positive_cnt::type resourceID);
        void deleteActiveDocument(uint_positive_cnt::type resourceID);

        //classi per eccezioni
        class sendFailure{};

    public slots:
        void readyRead();
        /*void signinExpired();
        void signupExpired();
        void removeUserExpired();
        void editUserExpired();
        void openNewSourceExpired();
        void openSourceExpired();
        void editPrivilegeExpired();
        void shareResourceExpired();
        void removeResourceExpired();
        void createNewDirExpired();
        void createNewSourceExpired();
        void localRemoveExpired();
        void localInsertExpired();
        void closeSourceExpired();*/
        void TimerExpired();
    };
}

#endif // CLIENTDISPATCHER_H
