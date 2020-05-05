#ifndef SIGNUP_H
#define SIGNUP_H

#include <QDialog>
#include <QCloseEvent>
#include "home.h"
#include "icon.h"
#include "errorconnection.h"
#include "notification.h"
#include "exit.h"
#include "../../user.h"
#include"symwininterface.h"

class MainWindow;


namespace Symposium{
class clientdispatcher;
}

namespace Ui {
class signup;
}

class signup : public QDialog, public SymChildWinInterface
{
    Q_OBJECT

public:
    explicit signup(QWidget *parent, SymWinInterface& si);

    void success() override;
    void failure(const QString& toPrint) override;

    /**
     * @brief called by clientdispatcher when there is some error with connection
     */
    void errorConnection();
    /**
     * @brief called by clientdispatcher when there is some error to signUp
     * @param errorMess the messagge to show
     */
    void errorSignUp(const QString& errorMess);
    /**
     * @brief called by clientdispatcher when the signup was successfully done
     */
    void successSignUp();

    ~signup() override;

private slots:
    /**
     * @brief restore the style of buttons;
     */
    void enableButtonsAfter();
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
    /**
     * @brief check the correctness of the password
     */
    bool checkPassword(const QString passwordToCheck);
    /**
     * @brief go back to mainwindow
     */
    void on_cancel_clicked();

private:
    Ui::signup *ui;
    std::string pwd;
    icon *iconWindow;
    std::string iconPath="";
    class exit *ex;
    bool pressed=false;
    /**
     * @brief quit from application
     */
    void closeEvent(QCloseEvent *event) override;
    /**
     * @brief waiting of the conclusion of the operation by clientdispatcher
     */
    void waiting();
    /**
     * @brief disable all buttons present so user cannot perform any operation
     */
    void disableButtons();
    /**
     * @brief enable all buttons present
     */
    void enableButtons();
    /**
     * @brief enable the style of buttons
     */
    void enableStyleButtons();
    /**
     * @brief disable the style of buttons
     */
    void disableStyleButtons();
    /**
     * @brief hide the labels
     */
    void hideLabelsError();
};

#endif // SIGNUP_H
