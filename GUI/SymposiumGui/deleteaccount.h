#ifndef DELETEACCOUNT_H
#define DELETEACCOUNT_H

#include <QDialog>
#include <QMessageBox>
#include "unsuccessdeleteaccount.h"
#include "errorconnection.h"
#include "errorlogout.h"




namespace Symposium{
class clientdispatcher;
}

namespace Ui {
class deleteAccount;
}

class deleteAccount : public QDialog
{
    Q_OBJECT

public:
    explicit deleteAccount(QWidget *parent = nullptr);
    void setClientDispatcher(Symposium::clientdispatcher *cl);
    void errorConnection();
    void errorConnectionLogout();
    void successDeleteAccount();
    void unsuccessDeleteAccount();
    ~deleteAccount();

private slots:
    void delete_click();

private:
    Ui::deleteAccount *ui;
    unsuccessdeleteaccount *window;
    Symposium::clientdispatcher *cl;
    errorconnection *errorWindow;
    errorlogout *errorLog;
};

#endif // DELETEACCOUNT_H
