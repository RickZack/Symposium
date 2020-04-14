#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Dispatcher/clientdispatcher.h"
#include "QDebug"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QPixmap pix(":/icon/logo.png");
    int w=ui->label->width();
    int h=ui->label->height();
    ui->label->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_about_clicked()
{
    disableStyleButtons();
    aboutWindow =new about(this);
    int ret=aboutWindow->exec();
    if(ret==0)
        enableStyleButtons();
}



void MainWindow::on_SignIn_clicked()
{
    signinWindow= new sigin();
    cl->setSignIn(signinWindow);

    signinWindow->setClientDispatcher(cl);
    signinWindow->show();
    hide();

}

void MainWindow::on_SignUp_clicked()
{
    signupWindow= new signup();
    cl->setSignUp(signupWindow);
    signupWindow->setClientDispatcher(cl);
    signupWindow->show();
    hide();

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    disableStyleButtons();
    event->ignore();
    ex=new class exit(this);
    int ret=ex->exec();
    if(ret==0)
        enableStyleButtons();

}

void MainWindow::setClientDispatcher(Symposium::clientdispatcher *cl){
    this->cl = cl;
}

void MainWindow::enableButtonsAfter()
{
    enableStyleButtons();
}


void MainWindow::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);
    QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setDuration(1000);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::enableStyleButtons()
{
    ui->about->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->SignIn->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->SignUp->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->exitBut->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");

}

void MainWindow::disableStyleButtons()
{
    ui->about->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->SignIn->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->SignUp->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->exitBut->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
}


void MainWindow::on_exitBut_clicked()
{
    disableStyleButtons();
    ex=new class exit(this);
    int ret=ex->exec();
    if(ret==0)
        enableStyleButtons();
}
