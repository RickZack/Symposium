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
    /**
     * @brief setting of clientdispatcher
     * @param cl clientdispatcher for reference
     */
    void setClientDispatcher( Symposium::clientdispatcher *cl);
    /**
     * @brief called by clientdispatcher when there is some error with connection
     */
    void errorConnection();
    /**
     * @brief called by clientdispatcher when there is some error to signUp
     * @param errorMess the messagge to show
     */
    void errorSignUp(std::string errorMess);
    /**
     * @brief called by clientdispatcher when the signup was successfully done
     */
    void successSignUp();
    ~signup();

private slots:
    /**
     * @brief invoke signUp on clientdispatcher
     */
    void on_signin_clicked();
    /**
     * @brief show the icon the user choose
     */
    void chooseIcon();
    /**
     * @brief show icon window
     */
    void on_iconButt_clicked();

private:
    Ui::signup *ui;
    std::string pwd;
    home *homeWindow;
    icon *iconWindow;
    errorconnection *errorWindow;
    std::string iconPath="";
    Symposium::clientdispatcher *cl;
    /**
     * @brief quit from application
     */
    void reject();
    /**
     * @brief waiting of the conclusion of the operation by clientdispatcher
     */
    void waiting();
};

#endif // SIGNUP_H
