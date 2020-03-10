#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
#include <QMessageBox>
#include <QMovie>
#include "success.h"
#include "clientdispatcher.h"

class Clientdispatcher;


namespace Ui {
class login;
}

class login : public QMainWindow
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    void errorLogin();
    void errorConnection();
    void successLogin();
    void waiting();
    void setClientDispatcher(Clientdispatcher *cl);
    ~login();

private slots:
    void on_button_clicked();

private:
    Ui::login *ui;
    success *successWindow;
    class waiting *waitingWindow;
    Clientdispatcher *cl;
};

#endif // LOGIN_H
