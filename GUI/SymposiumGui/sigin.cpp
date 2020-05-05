//#define DISPATCHER_ON

#include "sigin.h"
#include "ui_sigin.h"
#include "mainwindow.h"
#include "Dispatcher/clientdispatcher.h"

sigin::sigin(QWidget *parent, SymWinInterface& si) :
    QDialog(parent),
    SymChildWinInterface (si, isQWidget::isQwidgetType(*this)),
    ui(new Ui::sigin)
{
    ui->setupUi(this);
    setFixedSize(size());
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setAttribute( Qt::WA_DeleteOnClose );

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

}

sigin::~sigin()
{
    delete ui;
}

void sigin::success(){
    this->successSignIn();
}

void sigin::failure(const QString& toPrint){
    if(toPrint=="-1"){
        this->errorConnection();
    }else{
        this->errorSignIn();
    }
}

void sigin::errorConnection()
{
    enableButtons();
    hideLabelsError();
    pressed=false;
    errorconnection errorWindow(this);
    int ret=errorWindow.exec();
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
    pressed=false;
    home* homeWindow = new home(nullptr, pwd, *this);
    goToWindow(*homeWindow);
    #ifdef DISPATCHER_ON
    //cl->setHome(homeWindow);
    #endif
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
    #ifdef DISPATCHER_ON
    if(username!="" && password!=""){
        waiting();
        pressed=true;
        disableButtons();
        disableStyleButtons();
        cl.logIn(username.toStdString(), password.toStdString());
    }
    else {
        ui->haveto->show();
    }
    #endif


    //--------------------------------------------PARTE DA CANCELLARE SUCCESSIVAMENTE
    #ifndef DISPATCHER_ON
    if(username=="test" && password=="test")
    {
        enableButtons();
        enableStyleButtons();
        pressed=false;
        home* homeWindow= new home(nullptr, pwd, *this);
        goToWindow(*homeWindow);
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
    #endif
    //--------------------------------------------------


}

void sigin::closeEvent(QCloseEvent *event)
{
    event->ignore();
    if(closedByUser())
    {
        disableStyleButtons();
        event->ignore();
        class exit* ex=new class exit(this);
        int ret=ex->exec();
        if(ret==0 && !pressed)
            enableStyleButtons();
    }
    else
        event->accept();
}

void sigin::waiting()
{
    ui->waiting->show();
    ui->gif->show();

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
    backToParent();
}
