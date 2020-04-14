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

namespace Symposium{
class clientdispatcher;
}

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setClientDispatcher(Symposium::clientdispatcher *cl);

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
    about *aboutWindow;
    sigin *signinWindow;
    signup *signupWindow;
    Symposium::clientdispatcher *cl;
    class exit *ex;
    /**
     * @brief quit from application
     */
    void closeEvent(QCloseEvent *event);
    /**
     * @brief called when show() is invoked for this window and perform an animation
     */
    void showEvent(QShowEvent* event);
    /**
     * @brief enable the style of buttons
     */
    void enableStyleButtons();
    /**
     * @brief disable the style of buttons
     */
    void disableStyleButtons();
};

#endif // MAINWINDOW_H
