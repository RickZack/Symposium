#ifndef ONLINEUSERS_H
#define ONLINEUSERS_H

#include <QDialog>
#include <QMessageBox>
#include <QTreeWidgetItem>
#include <../../user.h>
#include <../../document.h>
#include "../../privilege.h"
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
    explicit onlineusers(QWidget *parent = nullptr);
    int documentId;
    Symposium::user user;
    std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData>> onlineUsers;

    void setClientDispatcher(Symposium::clientdispatcher *cl);
    void successEditPrivilege();
    void errorEditPrivilege(std::string errorMess);
    void errorConnection();
    void errorConnectionLogout();
    ~onlineusers();

private slots:
    void on_button_clicked();

    void on_tree_itemClicked(QTreeWidgetItem *item, int column);

    void on_owner_clicked();

    void on_modify_clicked();

    void on_reader_clicked();

    void on_none_clicked();

private:
    Ui::onlineusers *ui;
    int documentID;
    Symposium::privilege privelege;
    Symposium::privilege newPrivelege;
    std::string username;
    Symposium::clientdispatcher *cl;
    errorconnection *errorWindow;
    errorlogout *errorLog;

    void listusers();
    void insertusers();
    void changeList();
};

#endif // ONLINEUSERS_H
