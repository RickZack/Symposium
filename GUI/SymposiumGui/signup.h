#ifndef SIGNUP_H
#define SIGNUP_H

#include <QDialog>
#include "home.h"
#include "icon.h"

namespace Ui {
class signup;
}

class signup : public QDialog
{
    Q_OBJECT

public:
    explicit signup(QWidget *parent = nullptr);
    ~signup();

private slots:
    void on_signin_clicked();

    void chooseIcon();
    void on_iconButt_clicked();

private:
    Ui::signup *ui;
    home *homeWindow;
    icon *iconWindow;
};

#endif // SIGNUP_H
