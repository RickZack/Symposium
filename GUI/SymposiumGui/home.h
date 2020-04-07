#ifndef HOME_H
#define HOME_H

#include <QMainWindow>
#include <QCloseEvent>
#include "deleteaccount.h"
#include "inserturi.h"
#include "changeuserinfo.h"
#include "directory.h"
#include "errorconnection.h"
#include "errorlogout.h"
#include "exit.h"


namespace Symposium{
class clientdispatcher;
}

namespace Ui {
class home;
}

class home : public QMainWindow
{
    Q_OBJECT

public:
    explicit home(QWidget *parent = nullptr, std::string pwd="");
    /**
     * @brief setting of clientdispatcher
     * @param cl clientdispatcher for reference
     */
    void setClientDispatcher(Symposium::clientdispatcher *cl);
    /**
     * @brief called by clientdispatcher when the logout was successfully done
     */
    void successLogout();
    /**
     * @brief called by clientdispatcher when there is some error with connection
     */
    void errorConnection();
    /**
     * @brief called by clientdispatcher when there is some error with connection and perform logout of the page
     * @param str the string error to visualized for user
     */
    void errorConnectionLogout(std::string str);
    ~home();

private slots:
    /**
     * @brief show deleteaccount window
     */
    void on_delete_2_clicked();
    /**
     * @brief show inserturi window
     */
    void on_InsertUri_clicked();
    /**
     * @brief show changeuserinfo window
     */
    void on_modify_clicked();
    /**
     * @brief show directory window
     */
    void on_directory_clicked();
    /**
     * @brief perform logout action
     */
    void logout();

private:
    Ui::home *ui;
    deleteAccount *deleteAccountWindow;
    std::string pwd;
    inserturi *inserturiWindow;
    changeUserInfo *changeWindow;
    directory *directoryWindow;
    Symposium::clientdispatcher *cl;
    errorconnection *errorWindow;
    errorlogout *errorLog;
    class exit *ex;
    /**
     * @brief quit from application
     */
    void closeEvent(QCloseEvent *event);
};

#endif // HOME_H
