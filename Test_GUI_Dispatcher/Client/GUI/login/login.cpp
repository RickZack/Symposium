#include "login.h"
#include "ui_login.h"

login::login(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    ui->error->hide();
}

void login::errorLogin()
{
    waitingWindow->close();
    ui->error->show();
}

void login::errorConnection()
{
    waitingWindow->close();
    QMessageBox::information(this,
                             tr("Connection Error"), tr("Check your internet connection, there is some problem"), QMessageBox::Ok);
}






void login::successLogin()
{
    waitingWindow->close();
    close();
    successWindow= new success(this);
    successWindow->show();
}

void login::waiting()
{
    waitingWindow= new class waiting(this);
    waitingWindow->show();
}

login::~login()
{
    delete ui;
}

void login::on_button_clicked()
{
    QString username= ui->name->text();
    QString password = ui->password->text();
    waiting();
}


