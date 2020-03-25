#ifndef ACTIVEALWAYSLINK_H
#define ACTIVEALWAYSLINK_H

#include <QDialog>
#include "../../privilege.h"
#include "../../uri.h"
#include <QMessageBox>
#include "errorconnection.h"
#include "errorlogout.h"

namespace Symposium{
class clientdispatcher;
}

namespace Ui {
class activealwayslink;
}

class activealwayslink : public QDialog
{
    Q_OBJECT

public:
    explicit activealwayslink(QWidget *parent = nullptr,  int documentId=0, std::string pathFile="");
    void unsuccessLink(std::string errorMess);
    void successLink(std::string path);
    void setClientDispatcher(Symposium::clientdispatcher *cl);
    void errorConnection();
    void errorConnectionLogout(std::string str);
    ~activealwayslink();

private slots:
    void on_owner_clicked();

    void on_writer_clicked();

    void on_reader_clicked();

    void on_cancel_clicked();

    void on_ok_clicked();

private:
    Ui::activealwayslink *ui;
    std::string pathToFile;
    Symposium::privilege privilegeToGrant;
    Symposium::uri u;
    Symposium::clientdispatcher *cl;
    errorconnection *errorWindow;
    errorlogout *errorLog;
    int documentId;
    std::string pathFile;
};


#endif // ACTIVEALWAYSLINK_H
