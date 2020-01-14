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
    exitWindow =new class exit(this);
    exitWindow->show();
}

void MainWindow::on_SignIn_clicked()
{
    hide();
    signinWindow= new sigin(this);
    signinWindow->show();
}

void MainWindow::on_SignUp_clicked()
{
    hide();
    signupWindow= new signup(this);
    signupWindow->show();
}
