#ifndef ACTIVECOUNTERLINK_H
#define ACTIVECOUNTERLINK_H

#include <QDialog>
#include <QMessageBox>
#include "../../privilege.h"
#include "../../uri.h"
#include "errorconnection.h"
#include "errorlogout.h"

namespace Symposium{
class clientdispatcher;
}

namespace Ui {
class activecounterlink;
}

class activecounterlink : public QDialog
{
    Q_OBJECT

public:
    explicit activecounterlink(QWidget *parent = nullptr, int documentId=0, std::string pathFile="");
    void unsuccessLink(std::string errorMess);
    void successLink(std::string path);
    void setClientDispatcher(Symposium::clientdispatcher *cl);
    void errorConnection();
    void errorConnectionLogout(std::string str);
    ~activecounterlink();

private slots:
    void on_ok_clicked();

    void on_owner_clicked();

    void on_writer_clicked();

    void on_reader_clicked();

    void on_cancel_clicked();

private:
    Ui::activecounterlink *ui;
    std::string pathFile;
    Symposium::privilege privilegeToGrant;
    unsigned int numCounter;
    Symposium::uri u;
    Symposium::clientdispatcher *cl;
    errorconnection *errorWindow;
    errorlogout *errorLog;
    int documentId;
};

#endif // ACTIVECOUNTERLINK_H
