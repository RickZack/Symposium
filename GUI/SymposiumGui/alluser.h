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
    int documentID;
    explicit alluser(QWidget *parent = nullptr, Symposium::privilege privelege=Symposium::privilege::readOnly,
                     int documentID=0, Symposium::user user=Symposium::user(), std::string pathFile="");
    void setClientDispatcher(Symposium::clientdispatcher *cl);
    void successEditPrivilege();
    void errorEditPrivilege(std::string errorMess);
    void errorConnection();
    void errorConnectionLogout(std::string str);
    ~alluser();

private slots:
    void on_tree_itemClicked(QTreeWidgetItem *item, int column);

    void on_button_clicked();
    void on_owner_clicked();
    void on_modify_clicked();
    void on_reader_clicked();
    void on_none_clicked();


private:
    Ui::alluser *ui;
    Symposium::privilege newPrivelege;
    std::string username;
    Symposium::clientdispatcher *cl;
    errorconnection *errorWindow;
    errorlogout *errorLog;
    void listusers();
    void insertusers();
};

#endif // ALLUSER_H
