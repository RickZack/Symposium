#ifndef ONLINEUSERS_H
#define ONLINEUSERS_H

#include <QDialog>

#include <QMessageBox>
#include <QTreeWidgetItem>
#include <../../user.h>
#include <../../document.h>
#include "../../privilege.h"
#include "../../Symposium.h"
#include "errorconnection.h"
#include "errorlogout.h"

namespace Symposium{
class clientdispatcher;
}

namespace Ui {
class onlineusers;
}

class onlineusers : public QDialog
{
    Q_OBJECT

public:
    explicit onlineusers(QWidget *parent = nullptr, Symposium::privilege privelege=Symposium::privilege::readOnly,
                         Symposium::uint_positive_cnt::type documentID=0, Symposium::user user=Symposium::user(), std::string pathFile="");
    Symposium::uint_positive_cnt::type documentId;
    std::string pathFile;
    Symposium::user user;
    std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData>> onlineUsers;
    /**
     * @brief setting of clientdispatcher
     * @param cl clientdispatcher for reference
     */
    void setClientDispatcher(Symposium::clientdispatcher *cl);
    /**
     * @brief called by clientdispatcher when the edit of privilege for the user selected was successfully done
     */
    void successEditPrivilege();
    /**
     * @brief called by clientdispatcher when there is some error to edit the privilege for the user selected
     * @param errorMess the messagge to show
     */
    void errorEditPrivilege(std::string errorMess);
    /**
     * @brief called by clientdispatcher when there is some error with connection
     */
    void errorConnection();
    /**
     * @brief called by clientdispatcher when there is some error with connection and perform logout of the page
     * @param str the string error to visualized for user
     */
    void errorConnectionLogout(std::string str);
    ~onlineusers();

private slots:
    /**
     * @brief invoke editPrivilege on clientdispatcher
     */
    void on_button_clicked();
    /**
     * @brief selection of the user
     */
    void on_tree_itemClicked(QTreeWidgetItem *item, int column);
    /**
     * @brief choosing of the privilege owner from user to modify privilege of the selected user
     */
    void on_owner_clicked();
    /**
     * @brief choosing of the privilege writer from user to modify privilege of the selected user
     */
    void on_modify_clicked();
    /**
     * @brief choosing of the privilege readOnly from user to modify privilege of the selected user
     */
    void on_reader_clicked();
    /**
     * @brief choosing of the privilege none from user to modify privilege of the selected user
     */
    void on_none_clicked();

private:
    Ui::onlineusers *ui;
    Symposium::uint_positive_cnt::type documentID;
    Symposium::privilege privelege;
    Symposium::privilege newPrivelege;
    std::string username;
    Symposium::clientdispatcher *cl;

    void listusers();
    /**
     * @brief insert users in the tree
     */
    void insertusers();
    /**
     * @brief change tree of users after the privilege was modified
     */
    void changeList();
};

#endif // ONLINEUSERS_H
