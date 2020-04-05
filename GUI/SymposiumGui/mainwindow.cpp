#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Dispatcher/clientdispatcher.h"

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
    aboutWindow =new about(this);
    aboutWindow->show();
}

void MainWindow::on_exit_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("    Are you sure to quit??");
    msgBox.setStandardButtons(QMessageBox::Yes| QMessageBox::No);
    msgBox.setStyleSheet("QMessageBox { background-color: white }");
    int ret=msgBox.exec();

        if (ret == QMessageBox::Yes)
                qApp->quit();

}

void MainWindow::on_SignIn_clicked()
{
    hide();
    signinWindow= new sigin(this);
    //cl->setSignIn(signinWindow);

    signinWindow->setClientDispatcher(cl);

    signinWindow->show();
}

void MainWindow::on_SignUp_clicked()
{
    hide();
    signupWindow= new signup(this);
    //cl->setSignUp(signupWindow);
    signupWindow->setClientDispatcher(cl);
    signupWindow->show();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox msgBox;
    msgBox.setText("Are you sure to quit??");
    msgBox.setStandardButtons(QMessageBox::Yes| QMessageBox::No);
    msgBox.setBaseSize(QSize(390, 120));
    msgBox.setStyleSheet("QMessageBox { background-color:rgb(249, 247, 241); color: rgb(58, 80, 116);font: 14pt 'Baskerville Old Face';} QLabel{color: rgb(58, 80, 116);} QPushButton { background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116)); color: white; font: 14pt 'Baskerville Old Face'; border-radius:15px; width: 80px; height: 30px; margin-left:50px; margin-right:50px;}");
    msgBox.setIcon(QMessageBox::Question);
    int ret=msgBox.exec();

        if (ret == QMessageBox::Yes)
                event->accept();
        else event->ignore();

}

void MainWindow::setClientDispatcher(Symposium::clientdispatcher *cl){
    this->cl = cl;
}
