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
    enableStyleButtons();
    hideLabelsError();
    pressed=false;
    errorWindow = new errorconnection();
    errorWindow->exec();
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
    //cl->setHome(home *homeWindow);
    homeWindow->show();
}




void sigin::setClientDispatcher(Symposium::clientdispatcher *cl){
    this->cl = cl;
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
    disableButtons();
    disableStyleButtons();
    QMessageBox msgBox;
    msgBox.setText("<p align='center'>Are you sure to quit?</p>");
    msgBox.setWindowTitle("Exit");
    QPixmap pix(":/icon/logo1.png");
    QIcon p(pix);
    msgBox.setWindowIcon(p);
    msgBox.setStandardButtons(QMessageBox::Yes| QMessageBox::No);
    msgBox.button(QMessageBox::Yes)->setObjectName("Yes");
    msgBox.button(QMessageBox::Yes)->setText("Quit");
    msgBox.button(QMessageBox::No)->setObjectName("No");
    msgBox.button(QMessageBox::No)->setText("Remain");
    msgBox.setBaseSize(QSize(390, 120));
    msgBox.setStyleSheet("QMessageBox { background-color:rgb(249, 247, 241); "
                         "color: rgb(58, 80, 116);"
                         "font: 14pt 'Baskerville Old Face';} "
                         "QLabel{color: rgb(58, 80, 116);} "
                         "QPushButton#Yes { "
                         "background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, "
                         "stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116)); "
                         "color: white; font: 14pt 'Baskerville Old Face'; "
                         "border-radius:15px; width: 100px; height: 30px; "
                         "margin-right:50px;}"
                         "QPushButton#No { "
                         "background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, "
                         "stop: 0 rgb(95, 167, 175), stop: 1 grey); "
                         "color: white; font: 14pt 'Baskerville Old Face'; "
                         "border-radius:15px; width: 80px; height: 30px; "
                         "}");
    msgBox.setIcon(QMessageBox::Question);
    int ret=msgBox.exec();
    if (ret == QMessageBox::Yes)
        event->accept();
    else
    {
        if(!pressed)
        {
            enableButtons();
            enableStyleButtons();
        }
        event->ignore();
    }

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
    ui->signin->setDisabled(true);
    ui->cancel->setDisabled(true);
}

void sigin::enableButtons()
{
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
