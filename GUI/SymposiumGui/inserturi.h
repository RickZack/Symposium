#ifndef INSERTURI_H
#define INSERTURI_H

#include <QDialog>
#include "choosedir.h"
#include "../../privilege.h"
#include "errorconnection.h"
#include "errorlogout.h"



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
    explicit inserturi(QWidget *parent = nullptr);
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

public slots:
    /**
     * @brief set the selected folder
     */
    void change_text();
    /**
     * @brief cancel the folder selected
     */
    void reset_text();
    /**
     * @brief quit from application
     */
    void closeEvent(QCloseEvent *event);

private:
    Ui::inserturi *ui;
    choosedir *dirWindow;
    std::string path;
    std::string nameDir;
    Symposium::privilege privilege;
    std::string showDir;
    std::string pathId;
    Symposium::clientdispatcher *cl;
    errorconnection *errorWindow;
    errorlogout *errorLog;
};

#endif // INSERTURI_H
