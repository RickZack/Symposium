#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QCloseEvent>
#include <QFontDatabase>
#include "about.h"
#include "sigin.h"
#include "signup.h"
#include "exit.h"
#include "symwininterface.h"

namespace Symposium{
class clientdispatcher;
}

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public SymMainWinInterface
{
    Q_OBJECT

    //Not useful on this window
    void success() override{}
    void failure(const QString&) override{}

public:
    explicit MainWindow(QWidget *parent, SymWinManager& m, Symposium::clientdispatcher& p);
    ~MainWindow();
    void setClientDispatcher(Symposium::clientdispatcher *cl);
    /**
     * @brief disable the style of buttons
     */
    void disableStyleButtons();

public slots:
    /**
     * @brief restore the style of buttons;
     */
    void enableButtonsAfter();

private slots:
    /**
     * @brief show window about
     */
    void on_about_clicked();
    /**
     * @brief show window to sign in
     */
    void on_SignIn_clicked();
    /**
     * @brief show window to sign up
     */
    void on_SignUp_clicked();
    /**
     * @brief quit
     */
    void on_exitBut_clicked();

private:
    Ui::MainWindow *ui;
    //sigin *signinWindow;
    //signup *signupWindow;
    class exit *ex;
    /**
     * @brief quit from application
     */
    void closeEvent(QCloseEvent *event) override;
    /**
     * @brief enable the style of buttons
     */
    void enableStyleButtons();
};

#endif // MAINWINDOW_H
