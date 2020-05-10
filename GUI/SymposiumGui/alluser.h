#ifndef ALLUSER_H
#define ALLUSER_H

#include <QDialog>
#include <QTreeWidgetItem>
#include <unordered_map>
#include <QMessageBox>
#include <QDebug>
#include <forward_list>
#include "symwininterface.h"

#include "../../document.h"
#include "../../privilege.h"
#include "errorconnection.h"
#include "errorlogout.h"
#include "../../user.h"
#include "../../Symposium.h"

namespace Symposium{
class clientdispatcher;
}


namespace Ui {
class alluser;
}

class alluser : public QDialog, public SymModalWinInterface
{
    Q_OBJECT

public:
    explicit alluser(QWidget *parent, Symposium::privilege privelege,
                        Symposium::uint_positive_cnt::type documentID, Symposium::user user, std::string pathFile,
                     std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData>> onlineUsers,
                     std::unordered_map<std::string, Symposium::privilege> users, SymWinInterface& si);
    Symposium::privilege privelege;
    Symposium::user us;
    std::string pathFile;
    Symposium::uint_positive_cnt::type documentID;

    void success() override;

    void failure(const QString& toPrint) override;

    /**
     * @brief setting of clientdispatcher
     * @param cl clientdispatcher for reference
     */
    //void setClientDispatcher(Symposium::clientdispatcher *cl);
    /**
     * @brief called by clientdispatcher when the edit of privilege for the user selected was successfully done
     */
    void successEditPrivilege();
    /**
     * @brief called by clientdispatcher when there is some error to edit the privilege for the user selected
     * @param errorMess the messagge to show
     */
    void errorEditPrivilege(const QString& errorMess);
    /**
     * @brief called by clientdispatcher when there is some error with connection and perform logout of the page
     * @param str the string error to visualized for user
     */
    void errorConnectionLogout();
    ~alluser();

private slots:
    /**
     * @brief selection of the user
     */
    void on_tree_itemClicked(QTreeWidgetItem *item, int column);
    /**
     * @brief invoke editPrivilege on clientdispatcher
     */
    void on_button_clicked();
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


    void on_button_2_clicked();

private:
    Ui::alluser *ui;
    Symposium::privilege newPrivelege;
    std::string username;
    Symposium::clientdispatcher *cl;
    std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData>> onlineUsers;
    std::unordered_map<std::string, Symposium::privilege> users;

    /**
     * @brief insert users in the tree
     */
    void insertusers();
    /**
     * @brief disable the style of buttons
     */
    void disableButtons();
    /**
     * @brief enable the style of buttons
     */
    void enableButtons();

};

#endif // ALLUSER_H
