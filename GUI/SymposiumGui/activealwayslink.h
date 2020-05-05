#ifndef ACTIVEALWAYSLINK_H
#define ACTIVEALWAYSLINK_H

#include <QDialog>
#include "../../privilege.h"
#include "../../uri.h"
#include "../../Symposium.h"
#include <QMessageBox>

#include "errorconnection.h"
#include "errorlogout.h"
#include "successlinks.h"

namespace Symposium{
class clientdispatcher;
}

namespace Ui {
class activealwayslink;
}

class activealwayslink : public QDialog
{
    Q_OBJECT

public:
    explicit activealwayslink(QWidget *parent = nullptr,  Symposium::uint_positive_cnt::type documentId=0, std::string pathFile="");
    /**
     * @brief called by clientdispatcher when there is some error to activate link
     * @param errorMess the messagge to show
     */
    void unsuccessLink(std::string errorMess);
    /**
     * @brief called by clientdispatcher when the activation of link was successfully done
     * @param path the link to show to user
     */
    void successLink(std::string path);
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
    ~activealwayslink();

public slots:
    /**
     * @brief restore the style of buttons;
     */
    void enableButtonsAfter();

private slots:
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
    /**
     * @brief invoke shareResource on clientdispatcher
     */
    void on_ok_clicked();

private:
    Ui::activealwayslink *ui;
    Symposium::privilege privilegeToGrant;
    Symposium::uri u;
    Symposium::clientdispatcher *cl;
    Symposium::uint_positive_cnt::type documentId;
    std::string pathFile;
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
};


#endif // ACTIVEALWAYSLINK_H
