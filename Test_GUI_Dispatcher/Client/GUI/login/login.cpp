#include "login.h"
#include "ui_login.h"

login::login(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    ui->error->hide();
    ui->gif->hide();
    ui->wait->hide();
    ui->haveto->hide();
    QMovie *movie = new QMovie(":/img/loader.gif");
    ui->gif->setMovie(movie);
    movie->start();
}

void login::errorLogin()
{
    ui->haveto->hide();
    ui->gif->hide();
    ui->wait->hide();
    ui->error->show();
}

void login::errorConnection()
{
    ui->haveto->hide();
    ui->gif->hide();
    ui->wait->hide();
    QMessageBox::information(this,
                             tr("Connection Error"), tr("Check your internet connection, there is some problem"), QMessageBox::Ok);
}






void login::successLogin()
{
    ui->haveto->hide();
    ui->gif->hide();
    ui->wait->hide();
    close();
    successWindow= new success(this);
    successWindow->show();
}

void login::waiting()
{
    ui->haveto->hide();
    ui->gif->show();
    ui->wait->show();
}

login::~login()
{
    delete ui;
}

void login::on_button_clicked()
{
    QString username= ui->name->text();
    QString password = ui->password->text();
    if(username!="" && password!="")
        waiting();
    else {
        ui->haveto->show();
    }
}


