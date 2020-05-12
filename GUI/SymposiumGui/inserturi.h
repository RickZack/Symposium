#ifndef INSERTURI_H
#define INSERTURI_H

#include <QDialog>
#include "choosedir.h"
#include "../../privilege.h"
#include "errorconnection.h"
#include "errorlogout.h"
#include "notification.h"
#include "exit.h"
#include "symwininterface.h"

class home;
class directory;

namespace Symposium{
class clientdispatcher;
}


namespace Ui {
class inserturi;
}

class inserturi : public QDialog, public SymModalWinInterface
{
    Q_OBJECT

public:
    explicit inserturi(QWidget *parent, std::string pwd, SymWinInterface& si);

    void success() override;

    void failure(const QString& toPrint) override;

    /**
     * @brief setting of clientdispatcher
     * @param cl clientdispatcher for reference
     */
    //void setClientDispatcher(Symposium::clientdispatcher *cl);
    /**
     * @brief called by clientdispatcher when there is some error during the operation
     * @param errorMess the messagge to show
     */
    void unsuccessInsert(const QString& errorMess);
    /**
     * @brief called by clientdispatcher when there is some error with connection and perform logout of the page
     * @param str the string error to visualized for user
     */
    void errorConnectionLogout();
    /**
     * @brief called by clientdispatcher when the link was successfully insert
     */
    void successInsert();
    ~inserturi() override;

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
    std::string path;
    std::string nameDir;
    Symposium::privilege privilege;
    std::string showDir;
    std::string pathId;
    std::string pwd;
    choosedir *dirWindow;
    std::string pathLink;
    std::string resourceId;
    QString title;

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

    /**
     * @brief return to directory
     */
    void closeEvent(QCloseEvent *event) override;

    std::string manipulationPath(std::string& s);
};

#endif // INSERTURI_H
