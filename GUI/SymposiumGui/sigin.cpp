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
    ui->haveto->hide();
    ui->waiting->hide();
    ui->tryAgain->hide();
    ui->password->setEchoMode(QLineEdit::Password);
    ui->gif->hide();
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
    ui->haveto->hide();
    ui->waiting->hide();
    ui->tryAgain->hide();
    ui->gif->hide();

    QString username= ui->username->text();
    QString password = ui->password->text();
    pwd=password.toStdString();

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
        waiting();
        hide();
        homeWindow= new home(nullptr, pwd);
        homeWindow->show();
    }
    else {
        if(username=="" && password=="")
            ui->haveto->show();
        else
        {
            ui->haveto->hide();
            ui->waiting->hide();
            ui->tryAgain->show();
            ui->gif->hide();
            ui->signin->setDisabled(false);
            ui->cancel->setDisabled(false);
        }
    }
    //--------------------------------------------------


}

void sigin::closeEvent(QCloseEvent *event)
{
    QMessageBox msgBox;
    msgBox.setText("    Are you sure to quit??");
    msgBox.setWindowTitle("Exit");
    QPixmap pix(":/icon/logo1.png");
    QIcon p(pix);
    msgBox.setWindowIcon(p);
    msgBox.setStandardButtons(QMessageBox::Yes| QMessageBox::No);
    msgBox.button(QMessageBox::Yes)->setObjectName("Yes");
    msgBox.button(QMessageBox::No)->setObjectName("No");
    msgBox.setBaseSize(QSize(390, 120));
    msgBox.setStyleSheet("QMessageBox { background-color:rgb(249, 247, 241); "
                         "color: rgb(58, 80, 116);"
                         "font: 14pt 'Baskerville Old Face';} "
                         "QLabel{color: rgb(58, 80, 116);} "
                         "QPushButton#Yes { "
                         "background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, "
                         "stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116)); "
                         "color: white; font: 14pt 'Baskerville Old Face'; "
                         "border-radius:15px; width: 80px; height: 30px; "
                         "margin-left:50px; margin-right:50px;}"
                         "QPushButton#No { "
                         "background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, "
                         "stop: 0 rgb(95, 167, 175), stop: 1 grey); "
                         "color: white; font: 14pt 'Baskerville Old Face'; "
                         "border-radius:15px; width: 80px; height: 30px; "
                         "margin-left:50px; margin-right:50px;}");
    msgBox.setIcon(QMessageBox::Question);
    int ret=QMessageBox::No;
    ret=msgBox.exec();
        if (ret == QMessageBox::Yes)
                event->accept();
        else event->ignore();


    /*event->ignore();
    ex = new class exit(this, false);
    ex->exec();*/

}

void sigin::waiting()
{
    ui->waiting->show();
    ui->gif->show();
    ui->signin->setDisabled(true);
    ui->cancel->setDisabled(true);

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

void sigin::on_cancel_clicked()
{
    mw= new MainWindow();
    mw->show();
    hide();
}
