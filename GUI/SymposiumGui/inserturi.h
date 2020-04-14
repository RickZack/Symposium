#ifndef INSERTURI_H
#define INSERTURI_H

#include <QDialog>
#include "choosedir.h"
#include "../../privilege.h"
#include "errorconnection.h"
#include "errorlogout.h"
#include "notification.h"
#include "exit.h"

class home;
class directory;

namespace Symposium{
class clientdispatcher;
}


namespace Ui {
class inserturi;
}

class inserturi : public QDialog
{
    Q_OBJECT

public:
    explicit inserturi(QWidget *parent = nullptr, std::string pwd="", bool homeWindow=true);
    /**
     * @brief setting of clientdispatcher
     * @param cl clientdispatcher for reference
     */
    void setClientDispatcher(Symposium::clientdispatcher *cl);
    /**
     * @brief called by clientdispatcher when there is some error during the operation
     * @param errorMess the messagge to show
     */
    void unsuccessInsert(std::string errorMess);
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
     * @brief called by clientdispatcher when the link was successfully insert
     */
    void successInsert();
    ~inserturi();

private slots:
    /**
     * @brief show window to choose directory
     */
    void on_dir_clicked();
    /**
     * @brief choosing of the privilege readOnly from user
     */
    void on_reader_clicked();
    /**
     * @brief choosing of the privilege modify from user
     */
    void on_writer_clicked();
    /**
     * @brief choosing of the privilege owner from user
     */
    void on_owner_clicked();
    /**
     * @brief invoke openNewSource on clientdispatcher
     */
    void on_add_clicked();

    void on_cancel_clicked();

public slots:
    /**
     * @brief restore the style of buttons;
     */
    void enableButtonsAfter();
    /**
     * @brief set the selected folder
     */
    void change_text();
    /**
     * @brief cancel the folder selected
     */
    void reset_text();

private:
    Ui::inserturi *ui;
    choosedir *dirWindow;
    std::string path;
    std::string nameDir;
    Symposium::privilege privilege;
    std::string showDir;
    std::string pathId;
    bool pressed=false;
    Symposium::clientdispatcher *cl;
    home *h;
    directory *d;
    notification *notWindow;
    std::string pwd;
    bool homeWindow;
    class exit *ex;
    errorconnection *errorWindow;
    errorlogout *errorLog;
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
     * @brief disable the style of buttons
     */
    void disableStyleButtons();
    /**
     * @brief hide the labels
     */
    void hideLabelsError();
    /**
     * @brief waiting of the conclusion of the operation by clientdispatcher
     */
    void waiting();
};

#endif // INSERTURI_H
