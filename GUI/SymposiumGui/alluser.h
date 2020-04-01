#ifndef ALLUSER_H
#define ALLUSER_H

#include <QDialog>
#include <QTreeWidgetItem>
#include <unordered_map>
#include <QMessageBox>
#include <QDebug>
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

class alluser : public QDialog
{
    Q_OBJECT

public:
    Symposium::privilege privelege;
    Symposium::user us;
    std::string pathFile;
    std::unordered_map<std::string, Symposium::privilege> users;
    Symposium::uint_positive_cnt::type documentID;
    explicit alluser(QWidget *parent = nullptr, Symposium::privilege privelege=Symposium::privilege::readOnly,
                     Symposium::uint_positive_cnt::type documentID=0, Symposium::user user=Symposium::user(), std::string pathFile="");
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


private:
    Ui::alluser *ui;
    Symposium::privilege newPrivelege;
    std::string username;
    Symposium::clientdispatcher *cl;
    errorconnection *errorWindow;
    errorlogout *errorLog;
    void listusers();
    /**
     * @brief insert users in the tree
     */
    void insertusers();
};

#endif // ALLUSER_H
