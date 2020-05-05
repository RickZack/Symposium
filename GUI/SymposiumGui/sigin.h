#ifndef SIGIN_H
#define SIGIN_H

#include <QDialog>
#include <QCloseEvent>
#include "home.h"
#include "errorconnection.h"
#include "exit.h"
#include "symwininterface.h"

namespace Ui {
class sigin;
}

class sigin : public QDialog, public SymChildWinInterface
{
    Q_OBJECT

public:
    explicit sigin(QWidget *parent, SymWinInterface& si);
    ~sigin() override;

    void success() override;
    void failure(const QString& toPrint) override;

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

public slots:
    /**
     * @brief restore the style of buttons;
     */
    void enableButtonsAfter();

private slots:
    /**
     * @brief invoke logIn on clientdispatcher
     */
    void on_signin_clicked();
    /**
     * @brief go back to main window
     */
    void on_cancel_clicked();

private:

    Ui::sigin *ui;
    std::string pwd;
    class exit *ex;
    bool pressed=false;
    /**
     * @brief quit from application
     */
    void closeEvent(QCloseEvent *event) override;
    /**
     * @brief waiting of the conclusion of the operation by clientdispatcher
     */
    void waiting();
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
};

#endif // SIGIN_H
