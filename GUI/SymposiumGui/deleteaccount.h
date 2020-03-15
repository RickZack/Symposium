#ifndef DELETEACCOUNT_H
#define DELETEACCOUNT_H

#include <QDialog>
#include <QMessageBox>
#include "unsuccessdeleteaccount.h"
//#include "Dispatcher/clientdispatcher.h"


namespace Ui {
class deleteAccount;
}

class deleteAccount : public QDialog
{
    Q_OBJECT

public:
    explicit deleteAccount(QWidget *parent = nullptr);
    //void deleteAccount::setClientDispatcher(clientdispatcher *cl);
    void successDeleteAccount();
    void unsuccessDeleteAccount();
    ~deleteAccount();

private slots:
    void delete_click();

private:
    Ui::deleteAccount *ui;
    unsuccessdeleteaccount *window;
    //Symposium::clientdispatcher *cl;
};

#endif // DELETEACCOUNT_H
