#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include "about.h"
#include "exit.h"
#include "sigin.h"
#include "signup.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

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
};

#endif // MAINWINDOW_H
