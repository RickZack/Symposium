//#define DISPATCHER_ON

#include "home.h"
#include "ui_home.h"
#include "Dispatcher/clientdispatcher.h"
#include "mainwindow.h"

home::home(QWidget *parent,const std::string pwd, SymWinInterface& si) :
    QMainWindow(parent),
    SymChildWinInterface (si, isQWidget::isQwidgetType(*this)),
    ui(new Ui::home), pwd(pwd)
{
    ui->setupUi(this);
    connect(ui->logout, SIGNAL(clicked()), this, SLOT(logout()));
    ui->waiting->hide();
    ui->gif->hide();
    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();
    QPixmap pix2(":/icon/logo.png");
    int w=ui->logo->width();
    int h=ui->logo->height();
    ui->logo->setPixmap(pix2.scaled(w, h, Qt::KeepAspectRatio));
    ButtonHoverWatcher * watcher = new ButtonHoverWatcher(this);
    ui->logout->installEventFilter(watcher);
    ui->logout->setToolTip("Logout");
    setAttribute( Qt::WA_DeleteOnClose );
}

home::~home()
{
    delete ui;
}

void home::success(){
    this->disc = false;
    this->successLogout();
}

void home::failure(const QString&){
    this->disc = false;
    this->errorConnection();
}

void home::enableButtonsAfter()
{
    if(!pressed)
        enableStyleButtons();
}

void home::on_delete_2_clicked()
{
    //disableStyleButtons();
    //deleteAccountWindow = new deleteAccount(this);
    //deleteAccountWindow->setClientDispatcher(cl);
    this->disc = false;
    deleteAccount* del = new deleteAccount(nullptr, *this);
    goToWindow(*del);
    //------------------------------------------------------------------PARTE DA DECOMENTARE
    #ifdef DISPATCHER_ON
    //cl->setDeleteAccount(deleteAccountWindow);
    #endif
    //------------------------------------------------------------------
    /*int ret=deleteAccountWindow->exec();
    if(ret==0)
        enableStyleButtons();*/
}

void home::on_InsertUri_clicked()
{
    //inserturiWindow = new inserturi(nullptr, pwd, cl);
    //------------------------------------------------------------------PARTE DA DECOMENTARE
    #ifdef DISPATCHER_ON
    //cl.setInsertUri(inserturiWindow);
    #endif
    //------------------------------------------------------------------
    inserturiWindow->show();
    this->hide();
}

void home::on_modify_clicked()
{
    //changeWindow = new changeUserInfo(nullptr, pwd, cl);
    changeUserInfo* changeWindow = new changeUserInfo(nullptr, pwd, *this);
    goToWindow(*changeWindow);
    //------------------------------------------------------------------PARTE DA DECOMENTARE
    #ifdef DISPATCHER_ON
    //cl.setChangeUserInfo(changeWindow);
    #endif
    //------------------------------------------------------------------
    //changeWindow->show();
    //this->hide();
}

void home::on_directory_clicked()
{
    //directoryWindow=new directory(nullptr, pwd, cl);
    //directoryWindow->show();
    //------------------------------------------------------------------PARTE DA DECOMENTARE
    #ifdef DISPATCHER_ON
    //cl.setDirectory(directoryWindow);
    #endif
    //------------------------------------------------------------------
    this->hide();
}

void home::logout()
{
    //------------------------------------------------------------------PARTE DA DECOMENTARE
    #ifdef DISPATCHER_ON
    cl.logout();
    #endif
    //------------------------------------------------------------------
    pressed=true;
    waiting();
    disableButtons();
    disableStyleButtons();
}

void home::closeEvent(QCloseEvent *event)
{
    if (this->disc==true){
        disableStyleButtons();
        event->ignore();
        ex=new class exit(this);
        int ret=ex->exec();
        if(ret==0 && !pressed)
            enableStyleButtons();
    }
}

void home::disableButtons()
{
    ui->directory->setDisabled(true);
    ui->modify->setDisabled(true);
    ui->InsertUri->setDisabled(true);
    ui->delete_2->setDisabled(true);
    ui->logout->setDisabled(true);
}

void home::enableButtons()
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->directory->setDisabled(false);
    ui->modify->setDisabled(false);
    ui->InsertUri->setDisabled(false);
    ui->delete_2->setDisabled(false);
    ui->logout->setDisabled(false);
}

void home::enableStyleButtons()
{
    ui->directory->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->modify->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->InsertUri->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->delete_2->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
}

void home::disableStyleButtons()
{
    ui->directory->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->modify->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->InsertUri->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->delete_2->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
}

void home::waiting()
{
    ui->waiting->show();
    ui->gif->show();
}

void home::setClientDispatcher(Symposium::clientdispatcher *cl){
    //this->cl = cl;
}

void home::successLogout()
{
    enableButtons();
    enableStyleButtons();
    //this->hide();
    backToMainWin();
    /*mw=new MainWindow();
    mw->setClientDispatcher(cl);
    mw->show();*/
}

void home::errorConnection()
{
    enableButtons();
    enableStyleButtons();
    errorWindow = new errorconnection();
    int ret=errorWindow->exec();
    if(ret==0)
        enableStyleButtons();
}

void home::errorConnectionLogout(const std::string str)
{
    enableButtons();
    enableStyleButtons();
    errorLog = new errorlogout(nullptr, QString::fromStdString(str));
    //errorLog->setClientDispatcher(cl);
    this->hide();
    errorLog->show();
}

