#ifndef CURRENTUSERS_H
#define CURRENTUSERS_H

#include <QDialog>
#include <forward_list>
#include <sstream>

#include "../../user.h"
#include "../../privilege.h"


namespace Ui {
class currentUsers;
}

class currentUsers : public QDialog
{
    Q_OBJECT

public:
    explicit currentUsers(QWidget *parent = nullptr);
    ~currentUsers();

private:
    Ui::currentUsers *ui;
    std::forward_list<std::pair<Symposium::user *, Symposium::privilege>> activeUsers;

    void listusers();
    void insertusers();
};

#endif // CURRENTUSERS_H
