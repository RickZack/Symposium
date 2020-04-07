#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QCloseEvent>
#include "about.h"
#include "exit.h"
#include "sigin.h"
#include "signup.h"

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

private slots:
    /**
     * @brief show window about
     */
    void on_about_clicked();
    /**
     * @brief quit
     */
    void on_exit_clicked();
    /**
     * @brief show window to sign in
     */
    void on_SignIn_clicked();
    /**
     * @brief show window to sign up
     */
    void on_SignUp_clicked();


private:
    Ui::MainWindow *ui;
    about *aboutWindow;
    class exit *ex;
    sigin *signinWindow;
    signup *signupWindow;
    Symposium::clientdispatcher *cl;
    /**
     * @brief quit from application
     */
    void closeEvent(QCloseEvent *event);
    /**
     * @brief called when show() is invoked for this window and perform an animation
     */
    void showEvent(QShowEvent* event);
};

#endif // MAINWINDOW_H
