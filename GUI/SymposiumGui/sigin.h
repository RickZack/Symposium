#ifndef SIGIN_H
#define SIGIN_H

#include <QDialog>
#include <QCloseEvent>
#include "home.h"
#include "errorconnection.h"

namespace Symposium{
class clientdispatcher;
}

namespace Ui {
class sigin;
}

class sigin : public QDialog
{
    Q_OBJECT

public:
    explicit sigin(QWidget *parent = nullptr);
    ~sigin();
    void errorConnection();
    void errorSignIn();
    void successSignIn();
    void setClientDispatcher(Symposium::clientdispatcher *cl);


private slots:
    void on_signin_clicked();

private:

    Ui::sigin *ui;
    home *homeWindow;
    errorconnection *errorWindow;
    Symposium::clientdispatcher *cl;

    void closeEvent(QCloseEvent *event);
    void waiting();
};

#endif // SIGIN_H
