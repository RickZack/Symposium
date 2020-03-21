#include "sigin.h"
#include "ui_sigin.h"
#include "mainwindow.h"
#include "Dispatcher/clientdispatcher.h"

sigin::sigin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::sigin)
{
    ui->setupUi(this);
    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(hide()));
    connect(ui->cancel, SIGNAL(clicked()), parent, SLOT(show()));
    ui->haveto->hide();
    ui->waiting->hide();
    ui->tryAgain->hide();
    ui->gif->hide();
    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();

}

sigin::~sigin()
{
    delete ui;
}

void sigin::errorConnection()
{
    ui->signin->setDisabled(false);
    ui->cancel->setDisabled(false);
    errorWindow = new errorconnection(this);
    errorWindow->exec();
    ui->haveto->hide();
    ui->tryAgain->hide();
    ui->waiting->hide();
    ui->gif->hide();
}

void sigin::errorSignIn()
{
    ui->signin->setDisabled(false);
    ui->cancel->setDisabled(false);
    ui->haveto->hide();
    ui->tryAgain->show();
    ui->waiting->hide();
    ui->gif->hide();

}

void sigin::successSignIn()
{
    hide();
    homeWindow = new home(parentWidget());
    homeWindow->setClientDispatcher(cl);
    //cl->setHome(home *homeWindow);
    homeWindow->show();
}




void sigin::setClientDispatcher(Symposium::clientdispatcher *cl){
    this->cl = cl;
}

void sigin::on_signin_clicked()
{
    ui->haveto->hide();
    ui->waiting->hide();
    ui->tryAgain->hide();
    ui->gif->hide();

    QString username= ui->username->text();
    QString password = ui->password->text();

    //---------------------------------------------PARTE DA DECOMENTARE

    /*if(username!="" && password!=""){
        waiting();
        this->cl->logIn(username.toStdString(), password.toStdString());
    }
    else {
        ui->haveto->show();
    }*/


    //--------------------------------------------PARTE DA CANCELLARE SUCCESSIVAMENTE
    if(username=="test" && password=="test")
    {
        hide();
        homeWindow= new home(parentWidget());
        homeWindow->show();
    }
    else {

        ui->msg->setText("This credentials are not valid");
    }
    //--------------------------------------------------


}

void sigin::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Exit",
                                                                    tr("Are you sure to quit?\n"),
                                                                     QMessageBox::No | QMessageBox::Yes,
                                                                    QMessageBox::Yes);
        if (resBtn != QMessageBox::Yes) {
            event->ignore();
        } else {
            event->accept();
            //cl->closeConnection();
        }

}

void sigin::waiting()
{
    ui->waiting->show();
    ui->gif->show();
    ui->signin->setDisabled(true);
    ui->cancel->setDisabled(true);

}
