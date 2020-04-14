#include "signup.h"
#include "ui_signup.h"
#include "Dispatcher/clientdispatcher.h"
#include "mainwindow.h"

signup::signup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::signup)
{
    ui->setupUi(this);
    QPixmap pix2(":/icon/logo.png");
    int w=ui->logo->width();
    int h=ui->logo->height();
    ui->logo->setPixmap(pix2.scaled(w, h, Qt::KeepAspectRatio));
    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(hide()));
    QPixmap pix(":/resources/avatar/beaver.png");
    iconPath=":/resources/avatar/beaver.png";
    hideLabelsError();
    w=ui->img->width();
    h=ui->img->height();
    ui->img->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));
    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

void signup::errorConnection()
{
    enableButtons();
    hideLabelsError();
    pressed=false;
    errorWindow = new errorconnection(this);
    int ret=errorWindow->exec();
    if(ret==0)
        enableStyleButtons();
}

void signup::errorSignUp(const std::string errorMess)
{
    enableButtons();
    enableStyleButtons();
    hideLabelsError();
    pressed=false;
    ui->errorMess->setText(QString::fromStdString(errorMess));
    ui->errorMess->show();
}


void signup::successSignUp()
{
    enableButtons();
    enableStyleButtons();
    pressed=false;
    hide();
    homeWindow= new home(nullptr, pwd);
    homeWindow->setClientDispatcher(cl);
    cl->setHome(homeWindow);
    homeWindow->disableStyleButtons();
    notWindow = new notification(homeWindow, "Your account has been successfully created");
    homeWindow->show();
    int ret=notWindow->exec();
    if(ret==0)
        homeWindow->enableButtonsAfter();
}


void signup::setClientDispatcher( Symposium::clientdispatcher *cl){
    this->cl = cl;
}

signup::~signup()
{
    delete ui;
}

void signup::enableButtonsAfter()
{
    if(!pressed)
        enableStyleButtons();
}

void signup::on_signin_clicked()
{
    hideLabelsError();
    QString username= ui->username->text();
    QString password = ui->password->text();
    QString nickname =ui->nickname->text();
    pwd=password.toStdString();

    //--------------------------------------------------------------PARTE DA DECOMENTARE

    /*if(username!="" && password!="" && nickname!=""){
     * if(!checkPassword(password))
        {
            ui->errorMess->setText("The password does not meet the requirements");
            ui->errorMess->show();
        }
        else
        {
            waiting();
            disableStyleButtons();
            disableButtons();
            pressed=true;
            this->cl->signUp(username.toStdString(), password.toStdString(), nickname.toStdString(), iconPath);
         }
    }
    else {
        ui->haveto->show();
    }*/


    //--------------------------------------------------PARTE DA CANCELLARE SUCCESSIVAMENTE

    if(username!="" && password!="" && nickname!="")
    {
        waiting();
        if(!checkPassword(password))
        {
            ui->errorMess->setText("The password does not meet the requirements");
            ui->errorMess->show();
            enableButtons();
            enableStyleButtons();
        }
        else
        {
            /*hide();
            homeWindow= new home(nullptr, pwd);
            homeWindow->show();
            homeWindow->disableStyleButtons();
            notWindow = new notification(homeWindow, "Your account has been successfully created");
            int ret=notWindow->exec();
            if(ret==0)
                homeWindow->enableButtonsAfter();*/
            successSignUp();
        }
    }
    else {

        ui->haveto->show();
    }

    //----------------------------------------
}

void signup::on_iconButt_clicked()
{
    disableStyleButtons();
    iconWindow = new icon(this);
    int ret=iconWindow->exec();
    if(ret==0)
        enableStyleButtons();
}

bool signup::checkPassword(const QString passwordToCheck)
{
    std::string str=passwordToCheck.toStdString();
    if(str.length()<=5)
            return false;
    else if(str.length()>=22)
            return false;
    else if(Symposium::user::noCharPwd(str))
        return false;
    else if(Symposium::user::noNumPwd(str))
        return false;
    else if(Symposium::user::noSpecialCharPwd(str))
        return false;
    return true;
}

void signup::chooseIcon()
{
    iconPath=iconWindow->msg;
    QString msg2=QString::fromStdString(iconPath);
    QPixmap pix(msg2);
    int w=ui->img->width();
    int h=ui->img->height();
    ui->img->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));
}

void signup::closeEvent(QCloseEvent *event)
{
    disableStyleButtons();
    event->ignore();
    ex=new class exit(this);
    int ret=ex->exec();
    if(ret==0 && !pressed)
        enableStyleButtons();

}

void signup::waiting()
{
    ui->waiting->show();
    ui->gif->show();
}

void signup::disableButtons()
{
    ui->nickname->setReadOnly(true);
    ui->password->setReadOnly(true);
    ui->username->setReadOnly(true);
    ui->iconButt->setDisabled(true);
    ui->signin->setDisabled(true);
    ui->cancel->setDisabled(true);
}

void signup::enableButtons()
{
    ui->nickname->setReadOnly(false);
    ui->password->setReadOnly(false);
    ui->username->setReadOnly(false);
    ui->waiting->hide();
    ui->gif->hide();
    ui->iconButt->setDisabled(false);
    ui->signin->setDisabled(false);
    ui->cancel->setDisabled(false);
}

void signup::enableStyleButtons()
{
    ui->signin->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->cancel->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 grey);color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
}

void signup::disableStyleButtons()
{
    ui->signin->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->cancel->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
}

void signup::hideLabelsError()
{
    ui->haveto->hide();
    ui->errorMess->hide();
    ui->msg->hide();
    ui->waiting->hide();
    ui->gif->hide();
}

void signup::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);

    QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setDuration(1000);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void signup::on_cancel_clicked()
{
    mw= new MainWindow();
    mw->setClientDispatcher(cl);
    mw->show();
    hide();
}
