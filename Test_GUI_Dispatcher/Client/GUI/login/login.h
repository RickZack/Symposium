#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
#include <QMessageBox>
#include <QMovie>
#include "success.h"


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
    ~login();

private slots:
    void on_button_clicked();

private:
    Ui::login *ui;
    success *successWindow;
    class waiting *waitingWindow;
};

#endif // LOGIN_H
