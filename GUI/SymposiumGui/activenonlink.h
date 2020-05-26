#ifndef ACTIVENONLINK_H
#define ACTIVENONLINK_H

#include <QDialog>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QCloseEvent>
#include "../../uri.h"
#include "../../Symposium.h"
#include "errorconnection.h"
#include "errorlogout.h"
#include "successlinks.h"
#include <QMovie>
#include "symwininterface.h"

namespace Symposium{
class clientdispatcher;
}

namespace Ui {
class activenonlink;
}

class activenonlink : public QDialog, public SymModalWinInterface
{
    Q_OBJECT

public:
    explicit activenonlink(QWidget *parent, Symposium::uint_positive_cnt::type fileId, std::string pathFile, std::string pathAssoluto, SymWinInterface& si);
    /**
     * @brief called by clientdispatcher when there is some error to deactivate link
     * @param errorMess the messagge to show
     */

    void success() override;
    void failure(const QString& toPrint) override;

    void unsuccessLink(const QString& errorMess);
    /**
     * @brief called by clientdispatcher when the deactivation of link was successfully done
     */
    void successLink();
    /**
     * @brief called by clientdispatcher when there is some error with connection and perform logout of the page
     * @param str the string error to visualized for user
     */
    void errorConnectionLogout();
    ~activenonlink() override;

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
     * @brief close the window
     */
    void on_cancel_clicked();

private:
    Ui::activenonlink *ui;
    Symposium::uri u;
    std::string pathFile;
    std::string pathAssoluto;
    Symposium::uint_positive_cnt::type fileId;
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

#endif // ACTIVENONLINK_H
