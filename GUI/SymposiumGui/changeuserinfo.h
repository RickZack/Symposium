#ifndef CHANGEUSERINFO_H
#define CHANGEUSERINFO_H

#include <QDialog>
#include "icon.h"
#include <QMessageBox>
#include "../../user.h"
#include "errorconnection.h"
#include "errorlogout.h"
#include "exit.h"
#include "notification.h"



class home;

namespace Symposium{
class clientdispatcher;
}


namespace Ui {
class changeUserInfo;
}

class changeUserInfo : public QDialog
{
    Q_OBJECT

public:
    explicit changeUserInfo(QWidget *parent = nullptr, std::string pwd="");
    std::string pwd;
    /**
     * @brief setting of clientdispatcher
     * @param cl clientdispatcher for reference
     */
    void setClientDispatcher(Symposium::clientdispatcher *cl);
    /**
     * @brief called by clientdispatcher when there is some error with connection
     */
    void errorConnection();
    /**
     * @brief called by clientdispatcher when there is some error with connection and perform logout of the page
     * @param str the string error to visualized for user
     */
    void errorConnectionLogout(const std::string str);
    /**
     * @brief called by clientdispatcher when there is some error to edit the information of the current user
     * @param errorMess the messagge to show
     */
    void errorEditUser(const std::string errorMess);
    /**
     * @brief called by clientdispatcher when the edit of the information of the current user was successfully done
     */
    void successEditUser();
    /**
     * @brief disable the style of buttons
     */
    void disableStyleButtons();
    ~changeUserInfo();

private slots:
    /**
     * @brief restore the style of buttons;
     */
    void enableButtonsAfter();
    /**
     * @brief show the icon the user choose
     */
    void chooseIcon();
    /**
     * @brief show window to choose icon
     */
    void on_iconButt_clicked();
    /**
     * @brief invoke editUser on clientdispatcher
     */
    void confirm_click();


    //void on_username_textEdited(const QString &arg1);

    void on_confirm2_clicked();

    void on_cancel2_clicked();

    void on_changepwd_clicked();

    void on_oldpwd_textChanged(const QString &arg1);

    void on_newpwd2_textChanged(const QString &arg1);

    void on_cancel_clicked();

    void on_newpwd1_textChanged(const QString &arg1);

private:
    Ui::changeUserInfo *ui;
    icon *iconWindow;
    std::string img;
    std::string newpass;
    Symposium::user us;
    Symposium::clientdispatcher *cl;
    errorconnection *errorWindow;
    errorlogout *errorLog;
    notification *notWindow;
    bool pressed=false;
    bool passwordView=false;
    home *h;
    class exit *ex;

    void hiddeninformation();
    void showinformation();
    void hiddenpwd();
    void showpwd();
    /**
     * @brief quit from application
     */
    void closeEvent(QCloseEvent *event);
    /**
     * @brief called when show() is invoked for this window and perform an animation
     */
    void showEvent(QShowEvent* event);
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
     * @brief waiting of the conclusion of the operation by clientdispatcher
     */
    void waiting();
    /**
     * @brief hide the labels
     */
    void hideLabelsError();
    /**
     * @brief check the correctness of the password
     */
    bool checkPassword(const QString passwordToCheck);
};

#endif // CHANGEUSERINFO_H
