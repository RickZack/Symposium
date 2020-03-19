#ifndef ONLINEUSERS_H
#define ONLINEUSERS_H

#include <QDialog>
#include <../../user.h>
#include <../../document.h>
#include "../../privilege.h"

namespace Ui {
class onlineusers;
}

class onlineusers : public QDialog
{
    Q_OBJECT

public:
    explicit onlineusers(QWidget *parent = nullptr);
    int documentId;
    std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData>> onlineUsers;
    ~onlineusers();

private:
    Ui::onlineusers *ui;
    Symposium::privilege privelege;
    Symposium::user *user;
    void listusers();
    void insertusers();
};

#endif // ONLINEUSERS_H
