#include "home.h"
#include "ui_home.h"
#include "Dispatcher/clientdispatcher.h"

home::home(QWidget *parent, std::string pwd) :
    QMainWindow(parent),
    ui(new Ui::home), pwd(pwd)
{
    ui->setupUi(this);
    connect(ui->logout, SIGNAL(clicked()), this, SLOT(logout()));
    ui->waiting->hide();
    ui->gif->hide();
    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();
}

home::~home()
{
    delete ui;
}

void home::on_delete_2_clicked()
{
    deleteAccountWindow = new deleteAccount(this);
    deleteAccountWindow->setClientDispatcher(cl);
    //cl->setDeleteAccount(deleteAccountWindow);
    deleteAccountWindow->exec();
}

void home::on_InsertUri_clicked()
{
    inserturiWindow = new inserturi(this);
    //inserturiWindow->setClientDispatcher(cl);
    //cl->setInsertUri(inserturiWindow);
    inserturiWindow->exec();
}

void home::on_modify_clicked()
{
    changeWindow = new changeUserInfo(this, pwd);
    changeWindow->setClientDispatcher(cl);
    //cl->setChangeUserInfo(changeWindow);
    changeWindow->exec();
    pwd=changeWindow->pwd;
}

void home::on_directory_clicked()
{
    directoryWindow=new directory(this);
    directoryWindow->show();
    //cl->setDirectory(directoryWindow);
    this->hide();
}

void home::logout()
{
    //cl->logout();
    ui->directory->setDisabled(true);
    ui->modify->setDisabled(true);
    ui->InsertUri->setDisabled(true);
    ui->delete_2->setDisabled(true);
    ui->logout->setDisabled(true);
    ui->waiting->show();
    ui->gif->show();
}

void home::closeEvent(QCloseEvent *event)
{
    event->ignore();
    ex = new class exit(this, true);
    ex->exec();

}

void home::setClientDispatcher(Symposium::clientdispatcher *cl){
    this->cl = cl;
}

void home::successLogout()
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->directory->setDisabled(false);
    ui->modify->setDisabled(false);
    ui->InsertUri->setDisabled(false);
    ui->delete_2->setDisabled(false);
    ui->logout->setDisabled(false);
    this->hide();
    parentWidget()->show();
}

void home::errorConnection()
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->directory->setDisabled(false);
    ui->modify->setDisabled(false);
    ui->InsertUri->setDisabled(false);
    ui->delete_2->setDisabled(false);
    ui->logout->setDisabled(false);
    errorWindow = new errorconnection(this);
    errorWindow->exec();
}

void home::errorConnectionLogout(std::string str)
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->directory->setDisabled(false);
    ui->modify->setDisabled(false);
    ui->InsertUri->setDisabled(false);
    ui->delete_2->setDisabled(false);
    ui->logout->setDisabled(false);
    errorLog = new errorlogout(this, QString::fromStdString(str));
    this->hide();
    parentWidget()->close();
    errorLog->exec();
}

