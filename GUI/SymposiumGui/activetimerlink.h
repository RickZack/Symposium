#ifndef ACTIVETIMERLINK_H
#define ACTIVETIMERLINK_H

#include <QDialog>
#include <iostream>
#include <sstream>
#include <locale>
#include <iomanip>
#include <QMessageBox>
#include "../../privilege.h"
#include "../../uri.h"
#include "errorconnection.h"
#include "errorlogout.h"

namespace Symposium{
class clientdispatcher;
}

namespace Ui {
class activetimerlink;
}

class activetimerlink : public QDialog
{
    Q_OBJECT

public:
    explicit activetimerlink(QWidget *parent = nullptr, int documentId=0);
    void errorLink();
    void successLink(std::string path);
    void setClientDispatcher(Symposium::clientdispatcher *cl);
    void errorConnection();
    void errorConnectionLogout();
    ~activetimerlink();

private slots:
    void on_ok_clicked();

    void on_owner_clicked();

    void on_writer_clicked();

    void on_reader_clicked();

    void on_cancel_clicked();

private:
    Ui::activetimerlink *ui;
    std::string pathToFile;
    Symposium::privilege privilegeToGrant;
    Symposium::uri u;
    Symposium::clientdispatcher *cl;
    errorconnection *errorWindow;
    errorlogout *errorLog;
    std::string time;
    int documentId;

};

#endif // ACTIVETIMERLINK_H
