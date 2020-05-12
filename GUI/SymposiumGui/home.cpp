#include "home.h"
#include "ui_home.h"
#include "Dispatcher/clientdispatcher.h"
#include "mainwindow.h"
#include "onoff_networkinteraction.h"

home::home(QWidget *parent,const std::string pwd, SymWinInterface& si) :
    QMainWindow(parent),
    SymChildWinInterface (si, isQWidget::isQwidgetType(*this)),
    ui(new Ui::home), pwd(pwd)
{
    ui->setupUi(this);
    setFixedSize(size());
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setAttribute( Qt::WA_DeleteOnClose );

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

}

home::~home()
{
    delete ui;
}

void home::success(){
    pressed = false;
    successLogout();
}

void home::failure(const QString&){
    pressed = false;
    errorConnectionLogout();
}

void home::enableButtonsAfter()
{
    if(!pressed)
        enableStyleButtons();
}

void home::on_delete_2_clicked()
{
    disableStyleButtons();
    deleteAccount* del = new deleteAccount(this, *this);
    goToWindow(*del);
    //------------------------------------------------------------------PARTE DA DECOMENTARE
    #ifdef DISPATCHER_ON
    //cl->setDeleteAccount(deleteAccountWindow);
    #endif
    //------------------------------------------------------------------
}


void home::on_modify_clicked()
{

    changeUserInfo* changeWindow = new changeUserInfo(nullptr, pwd, *this);
    goToWindow(*changeWindow);
    //------------------------------------------------------------------PARTE DA DECOMENTARE
    #ifdef DISPATCHER_ON
    //cl.setChangeUserInfo(changeWindow);
    #endif
    //------------------------------------------------------------------
}

void home::on_directory_clicked()
{
    directory* dir = new directory(nullptr, pwd, *this);
    goToWindow(*dir);
    //------------------------------------------------------------------PARTE DA DECOMENTARE
    #ifdef DISPATCHER_ON
    //cl.setDirectory(directoryWindow);
    #endif
    //------------------------------------------------------------------
}

void home::logout()
{
    pressed=true;
    waiting();
    disableButtons();
    disableStyleButtons();
    //------------------------------------------------------------------PARTE DA DECOMENTARE
    #ifdef DISPATCHER_ON
    cl.logout();
    #endif
    //------------------------------------------------------------------
}

void home::closeEvent(QCloseEvent *event)
{
    if (closedByUser()){
        disableStyleButtons();
        event->ignore();
        class exit ex(this, true, &cl);
        int ret=ex.exec();
        if(ret==0 && !pressed)
            enableStyleButtons();
    }
    else
        event->accept();
}

void home::disableButtons()
{
    ui->directory->setDisabled(true);
    ui->modify->setDisabled(true);
    ui->delete_2->setDisabled(true);
    ui->logout->setDisabled(true);
}

void home::enableButtons()
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->directory->setDisabled(false);
    ui->modify->setDisabled(false);
    ui->delete_2->setDisabled(false);
    ui->logout->setDisabled(false);
}

void home::enableStyleButtons()
{
    ui->directory->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->modify->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->delete_2->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
}

void home::disableStyleButtons()
{
    ui->directory->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->modify->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->delete_2->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
}

void home::waiting()
{
    ui->waiting->show();
    ui->gif->show();
}

void home::successLogout()
{
    backToMainWin();
}

void home::errorConnectionLogout()
{
    enableButtons();
    enableStyleButtons();
    pressed=false;
    errorlogout errorLog(nullptr);
    int ret=errorLog.exec();
    if(ret==0)
        backToMainWin();
}

