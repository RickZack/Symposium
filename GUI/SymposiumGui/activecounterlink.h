#ifndef ACTIVECOUNTERLINK_H
#define ACTIVECOUNTERLINK_H

#include <QDialog>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QCloseEvent>
#include "../../privilege.h"
#include "../../uri.h"
#include "../../Symposium.h"
#include "errorconnection.h"
#include "errorlogout.h"
#include "successlinks.h"
#include "symwininterface.h"

namespace Symposium{
class clientdispatcher;
}

namespace Ui {
class activecounterlink;
}

class activecounterlink : public QDialog, public SymModalWinInterface
{
    Q_OBJECT

public:
    explicit activecounterlink(QWidget *parent, Symposium::uint_positive_cnt::type documentId, std::string pathFile, Symposium::user us, SymWinInterface& si);

    void success() override;
    void failure(const QString& toPrint) override;

    /**
     * @brief called by clientdispatcher when there is some error to activate link
     * @param errorMess the messagge to show
     */
    void unsuccessLink(const QString& errorMess);
    /**
     * @brief called by clientdispatcher when the activation of link was successfully done
     */
    void successLink();
    /**
     * @brief called by clientdispatcher when there is some error with connection and perform logout of the page
     * @param str the string error to visualized for user
     */
    void errorConnectionLogout();
    ~activecounterlink() override;

public slots:
    /**
     * @brief restore the style of buttons;
     */
    void enableButtonsAfter();

private slots:
    /**
     * @brief invoke shareResource on clientdispatcher
     */
    void on_ok_clicked();
    /**
     * @brief choosing of the privilege owner from user for sharing the link
     */
    void on_owner_clicked();
    /**
     * @brief choosing of the privilege writer from user for sharing the link
     */
    void on_writer_clicked();
    /**
     * @brief choosing of the privilege readOnly from user for sharing the link
     */
    void on_reader_clicked();
    /**
     * @brief close the window
     */
    void on_cancel_clicked();

private:
    Ui::activecounterlink *ui;
    std::string pathFile;
    Symposium::privilege privilegeToGrant;
    unsigned int numCounter;
    Symposium::uri u;
    Symposium::uint_positive_cnt::type documentId;
    Symposium::user us;

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
     * @brief waiting of the conclusion of the operation by clientdispatcher
     */
    void waiting();
    /**
     * @brief return to directory
     */
    void closeEvent(QCloseEvent *event) override;
};

#endif // ACTIVECOUNTERLINK_H
