#ifndef HOME_H
#define HOME_H

#include <QMainWindow>
#include <QCloseEvent>
#include "buttonhoverwatcher.h"
#include "deleteaccount.h"
#include "inserturi.h"
#include "changeuserinfo.h"
#include "directory.h"
#include "errorconnection.h"
#include "errorlogout.h"
#include "exit.h"
#include "symwininterface.h"

class MainWindow;

namespace Symposium{
class clientdispatcher;
}

namespace Ui {
class home;
}

class home : public QMainWindow, public SymChildWinInterface
{
    Q_OBJECT

public:
    explicit home(QWidget *parent, const std::string pwd, SymWinInterface& si);


    void success() override;

    void failure(const QString&) override;

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
    void errorConnectionLogout(const std::string str);
    /**
     * @brief disable the style of buttons
     */
    void disableStyleButtons();
    ~home();

public slots:
    /**
     * @brief restore the style of buttons;
     */
    void enableButtonsAfter();


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
    //deleteAccount *deleteAccountWindow;
    std::string pwd;
    inserturi *inserturiWindow;
    //changeUserInfo *changeWindow;
    directory *directoryWindow;
    //MainWindow *mw;
    class exit *ex;
    bool pressed=false;
    //Symposium::clientdispatcher *cl;
    errorconnection *errorWindow;
    errorlogout *errorLog;
    bool disc = true;
    /**
     * @brief quit from application
     */
    void closeEvent(QCloseEvent *event);
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
};

#endif // HOME_H
