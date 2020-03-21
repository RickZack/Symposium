#ifndef SIGNUP_H
#define SIGNUP_H

#include <QDialog>
#include <QCloseEvent>
#include "home.h"
#include "icon.h"
#include "errorconnection.h"


namespace Symposium{
class clientdispatcher;
}

namespace Ui {
class signup;
}

class signup : public QDialog
{
    Q_OBJECT

public:
    explicit signup(QWidget *parent = nullptr);
    void setClientDispatcher( Symposium::clientdispatcher *cl);
    void errorConnection();
    void errorSignUp(std::string errorMess);
    void successSignUp();
    ~signup();

private slots:
    void on_signin_clicked();

    void chooseIcon();
    void on_iconButt_clicked();

private:
    Ui::signup *ui;
    home *homeWindow;
    icon *iconWindow;
    errorconnection *errorWindow;
    std::string iconPath="";
    Symposium::clientdispatcher *cl;
    void reject();
    void waiting();
};

#endif // SIGNUP_H
