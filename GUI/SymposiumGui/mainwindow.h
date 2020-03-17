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

    void on_about_clicked();

    void on_exit_clicked();

    void on_SignIn_clicked();

    void on_SignUp_clicked();


private:
    Ui::MainWindow *ui;
    about *aboutWindow;
    class exit *exitWindow;
    sigin *signinWindow;
    signup *signupWindow;
    Symposium::clientdispatcher *cl;

    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
