#include "currentusers.h"
#include "ui_currentusers.h"

currentUsers::currentUsers(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::currentUsers)
{
    ui->setupUi(this);
    listusers();
    insertusers();

}

currentUsers::~currentUsers()
{
    delete ui;
}

void currentUsers::insertusers()
{
    for(auto it: activeUsers)
    {
        std::string nick=it.first->getNickname();
        nick=nick+" with a privilege ";
        std::ostringstream priv;
        priv<<it.second;
        nick=nick+priv.str();
        std::string icon=it.first->getIconPath();
        QListWidgetItem *item=new QListWidgetItem(QIcon(QString::fromStdString(icon)), QString::fromStdString(nick));
        ui->userslist->addItem(item);

    }

}

void currentUsers::listusers()
{
    Symposium::user *u1=new Symposium::user("Mario", "AP@ssw0rd!", "Mariuz", ":/resources/avatar/beaver.png", 1, nullptr);
    Symposium::user *u2=new Symposium::user("Carlo", "AP@ssw0rd!", "Carluz", ":/resources/avatar/boar.png", 2, nullptr);
    Symposium::user *u3=new Symposium::user("Federico", "AP@ssw0rd!", "Fede", ":/resources/avatar/bull.png", 3, nullptr);
    Symposium::user *u4=new Symposium::user("Vincenzo", "AP@ssw0rd!", "Vinci", ":/resources/avatar/deer.png", 4, nullptr);
    std::pair<Symposium::user*, Symposium::privilege> p1{u1, Symposium::privilege::owner};
    std::pair<Symposium::user*, Symposium::privilege> p2{u2, Symposium::privilege::modify};
    std::pair<Symposium::user*, Symposium::privilege> p3{u3, Symposium::privilege::readOnly};
    std::pair<Symposium::user*, Symposium::privilege> p4{u4, Symposium::privilege::owner};
    activeUsers.push_front(p1);
    activeUsers.push_front(p2);
    activeUsers.push_front(p3);
    activeUsers.push_front(p4);
}
