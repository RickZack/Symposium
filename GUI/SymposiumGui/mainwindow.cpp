#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Exit",
                                                                    tr("Are you sure to quit?\n"),
                                                                     QMessageBox::No | QMessageBox::Yes,
                                                                    QMessageBox::Yes);
        if (resBtn == QMessageBox::Yes)
                qApp->quit();

}

void MainWindow::on_SignIn_clicked()
{
    hide();
    signinWindow= new sigin(this);
    //cl->setSignIn(signinWindow);

    //signinWindow->setClientDispatcher(cl);

    signinWindow->show();
}

void MainWindow::on_SignUp_clicked()
{
    hide();
    signupWindow= new signup(this);
    //cl->setSignUp(signupWindow);
    //signupWindow->setClientDispatcher(cl);
    signupWindow->show();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Exit",
                                                                    tr("Are you sure to quit?\n"),
                                                                     QMessageBox::No | QMessageBox::Yes,
                                                                    QMessageBox::Yes);
        if (resBtn != QMessageBox::Yes) {
            event->ignore();
        } else {
            event->accept();
            //cl->:logout();
        }

}

/*void MainWindow::setClientDispatcher(clientdispatcher *cl){
    this->cl = cl;
}*/
