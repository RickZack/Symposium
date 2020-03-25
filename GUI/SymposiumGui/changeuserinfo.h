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
    explicit changeUserInfo(QWidget *parent = nullptr);
    void setClientDispatcher(Symposium::clientdispatcher *cl);
    void errorConnection();
    void errorConnectionLogout(std::string str);
    void errorEditUser(std::string errorMess);
    void successEditUser();
    ~changeUserInfo();

private slots:
    void chooseIcon();
    void on_iconButt_clicked();
    void confirm_click();


    //void on_username_textEdited(const QString &arg1);

private:
    Ui::changeUserInfo *ui;
    icon *iconWindow;
    std::string img;
    Symposium::user us;
    Symposium::clientdispatcher *cl;
    errorconnection *errorWindow;
    errorlogout *errorLog;
};

#endif // CHANGEUSERINFO_H
