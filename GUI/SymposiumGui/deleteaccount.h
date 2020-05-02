#ifndef DELETEACCOUNT_H
#define DELETEACCOUNT_H

#include <QDialog>
#include <QMessageBox>
#include <QMovie>
#include "unsuccessdeleteaccount.h"
#include "errorconnection.h"
#include "errorlogout.h"
#include "notification.h"
#include "symwininterface.h"

class MainWindow;

namespace Symposium{
class clientdispatcher;
}

namespace Ui {
class deleteAccount;
}

class deleteAccount : public QDialog, public SymChildWinInterface
{
    Q_OBJECT

public:
    explicit deleteAccount(QWidget *parent, SymWinInterface& si);

    void success() override;

    void failure(const QString& toPrint) override;

    /**
     * @brief setting of clientdispatcher
     * @param cl clientdispatcher for reference
     */
    //void setClientDispatcher(Symposium::clientdispatcher *cl);
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
     * @brief called by clientdispatcher when the account was successfully deleted
     */
    void successDeleteAccount();
    /**
     * @brief called by clientdispatcher when there is some error during the operation
     * @param errorMess the messagge to show
     */
    void errorDeleteUser(const QString& errorMess);
    ~deleteAccount();

private slots:
    /**
     * @brief invoke removeUser on clientdispatcher
     */
    void delete_click();

    void on_cancel_clicked();

private:
    Ui::deleteAccount *ui;
    MainWindow *mw;
    unsuccessdeleteaccount *window;
    //Symposium::clientdispatcher *cl;
    errorconnection *errorWindow;
    errorlogout *errorLog;
    notification *notWindow;
    bool disc = true;
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

    void closeEvent(QCloseEvent *event);
};

#endif // DELETEACCOUNT_H
