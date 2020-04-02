#ifndef CHANGEUSERINFO_H
#define CHANGEUSERINFO_H

#include <QDialog>
#include "icon.h"
#include <QMessageBox>
#include "../../user.h"
#include "errorconnection.h"
#include "errorlogout.h"



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
    void errorConnectionLogout(std::string str);
    /**
     * @brief called by clientdispatcher when there is some error to edit the information of the current user
     * @param errorMess the messagge to show
     */
    void errorEditUser(std::string errorMess);
    /**
     * @brief called by clientdispatcher when the edit of the information of the current user was successfully done
     */
    void successEditUser();
    ~changeUserInfo();

private slots:
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

private:
    Ui::changeUserInfo *ui;
    icon *iconWindow;
    std::string img;
    std::string newpass;
    Symposium::user us;
    Symposium::clientdispatcher *cl;
    errorconnection *errorWindow;
    errorlogout *errorLog;

    void hiddeninformation();
    void showinformation();
    void hiddenpwd();
    void showpwd();
};

#endif // CHANGEUSERINFO_H
