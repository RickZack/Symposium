#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Dispatcher/clientdispatcher.h"
#include "QDebug"

MainWindow::MainWindow(QWidget *parent, SymWinManager& m, Symposium::clientdispatcher& p) :
    QMainWindow(parent),
    SymMainWinInterface(m, p, isQWidget::isQwidgetType(*this)),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(size());
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

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
    about aboutWindow(this);
    aboutWindow.exec();
}



void MainWindow::on_SignIn_clicked()
{
    sigin* signinWindow= new sigin(nullptr, *this);
    goToWindow(*signinWindow);
}

void MainWindow::on_SignUp_clicked()
{
    signup* signupWindow= new signup(nullptr, *this);
    goToWindow(*signupWindow);
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


void MainWindow::enableButtonsAfter()
{
    enableStyleButtons();
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
