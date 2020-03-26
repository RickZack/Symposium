#ifndef ACTIVENONLINK_H
#define ACTIVENONLINK_H

#include <QDialog>
#include <QMessageBox>
#include "../../uri.h"
#include "../../Symposium.h"
#include "errorconnection.h"
#include "errorlogout.h"
#include <QMovie>

namespace Symposium{
class clientdispatcher;
}

namespace Ui {
class activenonlink;
}

class activenonlink : public QDialog
{
    Q_OBJECT

public:
    explicit activenonlink(QWidget *parent = nullptr, Symposium::uint_positive_cnt::type documentId=0, std::string pathFile="");
    void unsuccessLink(std::string errorMess);
    void successLink(std::string path);
    void setClientDispatcher(Symposium::clientdispatcher *cl);
    void errorConnection();
    void errorConnectionLogout(std::string str);
    ~activenonlink();

private slots:

    void on_ok_clicked();

    void on_cancel_clicked();

private:
    Ui::activenonlink *ui;
    Symposium::uri u;
    std::string pathFile;
    Symposium::clientdispatcher *cl;
    errorconnection *errorWindow;
    errorlogout *errorLog;
    Symposium::uint_positive_cnt::type documentId;
};

#endif // ACTIVENONLINK_H
