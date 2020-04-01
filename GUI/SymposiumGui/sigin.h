#ifndef SIGIN_H
#define SIGIN_H

#include <QDialog>
#include <QCloseEvent>
#include "home.h"
#include "errorconnection.h"

namespace Symposium{
class clientdispatcher;
}

namespace Ui {
class sigin;
}

class sigin : public QDialog
{
    Q_OBJECT

public:
    explicit sigin(QWidget *parent = nullptr);
    ~sigin();
    /**
     * @brief called by clientdispatcher when there is some error with connection
     */
    void errorConnection();
    /**
     * @brief called by clientdispatcher when there is some error during the operation
     */
    void errorSignIn();
    /**
     * @brief called by clientdispatcher when the log-in was successfully done
     */
    void successSignIn();
    /**
     * @brief setting of clientdispatcher
     * @param cl clientdispatcher for reference
     */
    void setClientDispatcher(Symposium::clientdispatcher *cl);


private slots:
    /**
     * @brief invoke logIn on clientdispatcher
     */
    void on_signin_clicked();

private:

    Ui::sigin *ui;
    home *homeWindow;
    errorconnection *errorWindow;
    Symposium::clientdispatcher *cl;
    /**
     * @brief quit from application
     */
    void closeEvent(QCloseEvent *event);
    /**
     * @brief waiting of the conclusion of the operation by clientdispatcher
     */
    void waiting();
};

#endif // SIGIN_H
