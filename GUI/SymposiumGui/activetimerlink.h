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
#include "../../Symposium.h"
#include "errorconnection.h"
#include "errorlogout.h"
#include <QMovie>

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
    explicit activetimerlink(QWidget *parent = nullptr, Symposium::uint_positive_cnt::type documentId=0, std::string pathFile="");
    void unsuccessLink(std::string errorMess);
    void successLink(std::string path);
    void setClientDispatcher(Symposium::clientdispatcher *cl);
    void errorConnection();
    void errorConnectionLogout(std::string str);
    ~activetimerlink();

private slots:
    void on_ok_clicked();

    void on_owner_clicked();

    void on_writer_clicked();

    void on_reader_clicked();

    void on_cancel_clicked();

private:
    Ui::activetimerlink *ui;
    std::string pathFile;
    Symposium::privilege privilegeToGrant;
    Symposium::uri u;
    Symposium::clientdispatcher *cl;
    errorconnection *errorWindow;
    errorlogout *errorLog;
    std::string time;
    Symposium::uint_positive_cnt::type documentId;

};

#endif // ACTIVETIMERLINK_H
