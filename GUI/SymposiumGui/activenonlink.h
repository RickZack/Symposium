#ifndef ACTIVENONLINK_H
#define ACTIVENONLINK_H

#include <QDialog>
#include <QMessageBox>
#include "../../uri.h"
#include "errorconnection.h"
#include "errorlogout.h"

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
    explicit activenonlink(QWidget *parent = nullptr, int documentId=0);
    void errorLink();
    void successLink(std::string path);
    void setClientDispatcher(Symposium::clientdispatcher *cl);
    void errorConnection();
    void errorConnectionLogout();
    ~activenonlink();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::activenonlink *ui;
    Symposium::uri u;
    Symposium::clientdispatcher *cl;
    errorconnection *errorWindow;
    errorlogout *errorLog;
    int documentId;
};

#endif // ACTIVENONLINK_H
