#include "sigin.h"
#include "ui_sigin.h"
#include "mainwindow.h"
#include "Dispatcher/clientdispatcher.h"
#include "mainwindow.h"

sigin::sigin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::sigin)
{
    ui->setupUi(this);
    QPixmap pix2(":/icon/logo.png");
    int w=ui->logo->width();
    int h=ui->logo->height();
    ui->logo->setPixmap(pix2.scaled(w, h, Qt::KeepAspectRatio));
    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(hide()));
    hideLabelsError();
    ui->password->setEchoMode(QLineEdit::Password);
    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

}

sigin::~sigin()
{
    delete ui;
}

void sigin::errorConnection()
{
    enableButtons();
    hideLabelsError();
    pressed=false;
    errorWindow = new errorconnection(this);
    int ret=errorWindow->exec();
    if(ret==0)
        enableStyleButtons();
}

void sigin::errorSignIn()
{
    enableButtons();
    enableStyleButtons();
    hideLabelsError();
    pressed=false;
    ui->tryAgain->show();
}

void sigin::successSignIn()
{
    hide();
    pressed=false;
    homeWindow = new home(nullptr, pwd);
    homeWindow->setClientDispatcher(cl);
    cl->setHome(homeWindow);
    homeWindow->show();
}




void sigin::setClientDispatcher(Symposium::clientdispatcher *cl){
    this->cl = cl;
}

void sigin::enableButtonsAfter()
{
    if(!pressed)
        enableStyleButtons();
}

void sigin::on_signin_clicked()
{
    hideLabelsError();
    QString username= ui->username->text();
    QString password = ui->password->text();
    pwd=password.toStdString();

    //---------------------------------------------PARTE DA DECOMENTARE

    /*if(username!="" && password!=""){
        waiting();
        pressed=true;
        disableButtons();
        disableStyleButtons();
        this->cl->logIn(username.toStdString(), password.toStdString());
    }
    else {
        ui->haveto->show();
    }*/


    //--------------------------------------------PARTE DA CANCELLARE SUCCESSIVAMENTE
    if(username=="test" && password=="test")
    {
        hide();
        enableButtons();
        enableStyleButtons();
        pressed=false;
        homeWindow= new home(nullptr, pwd);
        homeWindow->show();
    }
    else {
        pressed=false;
        if(username=="" && password=="")
            ui->haveto->show();
        else
        {
            ui->tryAgain->show();
        }
    }
    //--------------------------------------------------


}

void sigin::closeEvent(QCloseEvent *event)
{
    disableStyleButtons();
    event->ignore();
    ex=new class exit(this);
    int ret=ex->exec();
    if(ret==0 && !pressed)
        enableStyleButtons();
}

void sigin::waiting()
{
    ui->waiting->show();
    ui->gif->show();

}

void sigin::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);

    QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setDuration(1000);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void sigin::disableButtons()
{
    ui->password->setReadOnly(true);
    ui->username->setReadOnly(true);
    ui->signin->setDisabled(true);
    ui->cancel->setDisabled(true);
}

void sigin::enableButtons()
{
    ui->password->setReadOnly(false);
    ui->username->setReadOnly(false);
    ui->waiting->hide();
    ui->gif->hide();
    ui->signin->setDisabled(false);
    ui->cancel->setDisabled(false);
}

void sigin::enableStyleButtons()
{
    ui->signin->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->cancel->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 grey);color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
}

void sigin::disableStyleButtons()
{
    ui->signin->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->cancel->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
}

void sigin::hideLabelsError()
{
    ui->haveto->hide();
    ui->tryAgain->hide();
    ui->waiting->hide();
    ui->gif->hide();
}

void sigin::on_cancel_clicked()
{
    mw= new MainWindow();
    mw->setClientDispatcher(cl);
    mw->show();
    hide();
}
