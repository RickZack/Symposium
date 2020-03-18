#ifndef CURRENTUSERS_H
#define CURRENTUSERS_H

#include <QDialog>
#include <QListWidget>
#include <QMessageBox>
#include <forward_list>
#include <sstream>

#include "../../user.h"
#include "../../privilege.h"
#include "../../document.h"
#include "errorconnection.h"
#include "errorlogout.h"

namespace Symposium{
class clientdispatcher;
}


namespace Ui {
class currentUsers;
}

class currentUsers : public QDialog
{
    Q_OBJECT

public:
    int documentId;
    std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData>> onlineUsers;

    explicit currentUsers(QWidget *parent = nullptr, bool modify=false);
    void setClientDispatcher(Symposium::clientdispatcher *cl);
    void successEditPrivilege();
    void errorEditPrivilege(std::string errorMess);
    void errorConnection();
    void errorConnectionLogout();
    ~currentUsers();

private slots:
    void on_userslist_itemClicked(QListWidgetItem *item);

    void on_owner_clicked();

    void on_modify_clicked();

    void on_reader_clicked();

    void on_button_clicked();

    void on_none_clicked();

private:
    Ui::currentUsers *ui;
    bool modify;
    int number;
    std::forward_list<std::pair<Symposium::user *, Symposium::privilege>> activeUsers;
    Symposium::privilege privilegeUser;
    Symposium::privilege newPrivelege;
    Symposium::user *user=new Symposium::user("Vincenzo", "AP@ssw0rd!", "Vinci", ":/resources/avatar/deer.png", 4, nullptr);
    int userToChangeSiteId;
    Symposium::user userToChange;
    Symposium::clientdispatcher *cl;
    errorconnection *errorWindow;
    errorlogout *errorLog;

    void listusers();
    void insertusers();
    void userPrivilege();
    void changeUserPrivilege();
    void changeList();
};

#endif // CURRENTUSERS_H
