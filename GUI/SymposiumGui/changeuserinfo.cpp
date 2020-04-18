//#define DISPATCHER_ON

#include "changeuserinfo.h"
#include "ui_changeuserinfo.h"
#include "Dispatcher/clientdispatcher.h"
#include "home.h"
#include <QMovie>

changeUserInfo::changeUserInfo(QWidget *parent, std::string pwd, Symposium::clientdispatcher *cl) :
    QDialog(parent),
    pwd(pwd), ui(new Ui::changeUserInfo), cl(cl)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QPixmap pix2(":/icon/logo.png");
    int w=ui->logo->width();
    int h=ui->logo->height();
    ui->logo->setPixmap(pix2.scaled(w, h, Qt::KeepAspectRatio));
    hideLabelsError();
    hiddenpwd();

    //-----------------------------------------------------PARTE DA DECOMENTARE

    #ifdef DISPATCHER_ON
    us=cl->getUser();
    ui->username->setText(QString::fromStdString(us.getUsername()));
    ui->nickname->setText(QString::fromStdString(us.getNickname()));
    img=us.getIconPath();
    #endif


    //-----------------------------------------------------PARTE DA CANCELLARE
    #ifndef DISPATCHER_ON
    ui->username->setText("Mario");
    ui->nickname->setText("Rossi");
    img=":/resources/avatar/beaver.png";
    #endif

    //---------------------------------------------------------------------------

    QPixmap pix(QString::fromStdString(img));
    w=ui->img->width();
    h=ui->img->height();
    ui->img->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));

    ui->waiting->hide();
    ui->gif->hide();
    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();

    connect(ui->confirm, SIGNAL(clicked()), this, SLOT(confirm_click()));

}

changeUserInfo::~changeUserInfo()
{
    delete ui;
}

void changeUserInfo::enableButtonsAfter()
{
    if(!pressed)
        enableStyleButtons();
}

void changeUserInfo::on_iconButt_clicked()
{
    disableStyleButtons();
    iconWindow = new icon(this);
    int ret=iconWindow->exec();
    if(ret==0)
        enableStyleButtons();
}

void changeUserInfo::chooseIcon()
{
    img=iconWindow->msg;
    QString msg2=QString::fromStdString(img);
    QPixmap pix(msg2);
    int w=ui->img->width();
    int h=ui->img->height();
    ui->img->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));
}


void changeUserInfo::confirm_click()
{
    hideLabelsError();
    newpass=pwd;
    QString username = ui->username->text();
    QString nickname = ui->nickname->text();
    disableButtons();
    disableStyleButtons();
    waiting();
    pressed=true;
    //------------------------------------------------------------------PARTE DA DECOMMENTARE
    #ifdef DISPATCHER_ON
    QString imagine = QString::fromStdString(img);
    Symposium::user usNew(username.toStdString(), pwd, nickname.toStdString(), img, us.getSiteId(), us.getHome());
    cl->editUser(usNew);
    #endif
    //------------------------------------------------------------------

    //------------------------------------------------------------------PARTE DA CANCELLARE
    #ifndef DISPATCHER_ON
    QString stringa="Your information has been successfully modified";
    hideLabelsError();
    enableButtons();
    notWindow = new notification(this, stringa);
    int ret=notWindow->exec();
    if(ret==0)
        enableStyleButtons();
    pressed=false;
    #endif
    //--------------------------------------------------------------------------
}


void changeUserInfo::setClientDispatcher(Symposium::clientdispatcher *cl){
    this->cl = cl;
}

void changeUserInfo::errorConnection()
{
    hideLabelsError();
    pressed=false;
    enableButtons();
    enableStyleButtons();
    errorWindow = new errorconnection(this);
    errorWindow->show();
}

void changeUserInfo::errorConnectionLogout(const std::string str)
{
    hideLabelsError();
    pressed=false;
    enableButtons();
    enableStyleButtons();
    errorLog = new errorlogout(this, QString::fromStdString(str));
    errorLog->setClientDispatcher(cl);
    this->close();
    errorLog->show();
}

void changeUserInfo::errorEditUser(const std::string errorMess)
{
    hideLabelsError();
    pressed=false;
    enableButtons();
    enableStyleButtons();
    ui->error->setText(QString::fromStdString(errorMess));
    ui->error->show();
}



void changeUserInfo::successEditUser()
{
    pwd=newpass;
    this->hide();
    hideLabelsError();
    pressed=false;
    enableButtons();
    QString stringa="Your information has been successfully modified";
    notWindow = new notification(this, stringa);
    int ret=notWindow->exec();
    if(ret==0)
        enableStyleButtons();
    h=new home(nullptr, pwd);
    h->setClientDispatcher(cl);
    h->show();
}

void changeUserInfo::disableStyleButtons()
{
    if(!passwordView)
    {
        ui->confirm->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
        ui->cancel->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");

    }
    else
    {
        ui->confirm2->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
        ui->cancel2->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");

    }
}

void changeUserInfo::on_confirm2_clicked()
{
    hideLabelsError();
    QString newp1=ui->newpwd1->text();
    QString newp2=ui->newpwd2->text();
    if(ui->oldpwd->text()=="")
    {
        ui->error->setText("First you have to write your actual password in \"old password\" field");
        ui->error->show();
    }
    else if(ui->oldpwd->text()!=QString::fromStdString(pwd))
    {
        ui->error->setText("Your actual password is wrong");
        ui->error->show();
    }
    else if(newp1=="")
    {
        ui->error->setText("You have to write the new password in \"new password\" field");
        ui->error->show();
    }
    else if(newp2=="")
    {
        ui->error->setText("You have to confirm the new password in \"repeat new password\" field");
        ui->error->show();
    }
    else if(!checkPassword(newp1))
    {
        ui->error->setText("The password does not meet the requirements");
        ui->error->show();
    }
    else if(newp1==newp2)
    {
        newpass=newp1.toStdString();
        waiting();
        disableButtons();
        disableStyleButtons();
        pressed=true;
        //------------------------------------------------------------------PARTE DA DECOMENTARE
        #ifdef DISPATCHER_ON
        QString imagine = QString::fromStdString(img);
        Symposium::user usNew(us.getUsername(), newp1.toStdString(), us.getNickname(), img, us.getSiteId(), us.getHome());
        cl->editUser(usNew);
        pressed=true;
        #endif
        //------------------------------------------------------------------

        //------------------------------------------------------------------PARTE DA CANCELLARE
        #ifndef DISPATCHER_ON
        hideLabelsError();
        enableButtons();
        QString stringa="Your information has been successfully modified";
        notWindow = new notification(this, stringa);
        int ret=notWindow->exec();
        if(ret==0)
            enableStyleButtons();
        pwd=newp1.toStdString();
        pressed=false;
        #endif
        //--------------------------------------------------------------------------
    }
    else
    {
       enableButtons();
       enableStyleButtons();
       hideLabelsError();
       ui->error->setText("Password in the \"new password\" field has to be equal\nto the password in the \"repeat new password\" field!");
       ui->error->show();
    }
}

void changeUserInfo::on_cancel2_clicked()
{
    hiddenpwd();
    showinformation();
    passwordView=false;
}

void changeUserInfo::on_changepwd_clicked()
{
    hiddeninformation();
    showpwd();
    passwordView=true;

}

void changeUserInfo::hiddeninformation()
{
    ui->label->hide();
    ui->label_5->hide();
    ui->label_4->hide();
    ui->username->hide();
    ui->nickname->hide();
    ui->img->hide();
    ui->iconButt->hide();
    ui->changepwd->hide();
    ui->confirm->hide();
    ui->cancel->hide();
    ui->error->setText("");
    ui->confirm->setDisabled(true);
    ui->cancel->setDisabled(true);
    ui->changepwd->setDisabled(true);
}

void changeUserInfo::showinformation()
{
    ui->label->show();
    ui->label_5->show();
    ui->label_4->show();
    ui->username->show();
    ui->nickname->show();
    ui->img->show();
    ui->iconButt->show();
    ui->changepwd->show();
    ui->confirm->show();
    ui->cancel->show();
    ui->error->setText("");
    ui->confirm->setDisabled(false);
    ui->cancel->setDisabled(false);
    ui->changepwd->setDisabled(false);
}

void changeUserInfo::hiddenpwd()
{
    ui->labelpwd1->hide();
    ui->labelpwd2->hide();
    ui->labelpwd3->hide();
    ui->confirm2->hide();
    ui->cancel2->hide();
    ui->oldpwd->hide();
    ui->newpwd1->hide();
    ui->newpwd2->hide();
    ui->veroldpwd->hide();
    ui->vernewpwd->hide();
    ui->vernewpwd2->hide();
    ui->label_6->hide();
    ui->label_7->hide();
    ui->error->setText("");
    ui->confirm2->setDisabled(true);
    ui->cancel2->setDisabled(true);
}

void changeUserInfo::showpwd()
{
    ui->labelpwd1->show();
    ui->labelpwd2->show();
    ui->labelpwd3->show();
    ui->confirm2->show();
    ui->cancel2->show();
    ui->oldpwd->show();
    ui->newpwd1->show();
    ui->newpwd2->show();
    ui->label_6->show();
    ui->label_7->show();
    ui->veroldpwd->show();
    ui->veroldpwd->clear();
    ui->vernewpwd->show();
    ui->vernewpwd->clear();
    ui->vernewpwd2->show();
    ui->vernewpwd2->clear();
    ui->error->setText("");
    ui->confirm2->setDisabled(false);
    ui->cancel2->setDisabled(false);
}

void changeUserInfo::closeEvent(QCloseEvent *event)
{
    disableStyleButtons();
    event->ignore();
    ex=new class exit(this);
    int ret=ex->exec();
    if(ret==0 && !pressed)
        enableStyleButtons();
}

void changeUserInfo::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setDuration(1000);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void changeUserInfo::disableButtons()
{
   if(!passwordView)
   {
       ui->confirm->setDisabled(true);
       ui->cancel->setDisabled(true);
       ui->changepwd->setDisabled(true);
       ui->iconButt->setDisabled(true);
       ui->nickname->setReadOnly(true);
       ui->username->setReadOnly(true);
   }
   else
   {
       ui->confirm2->setDisabled(true);
       ui->cancel2->setDisabled(true);
   }
}

void changeUserInfo::enableButtons()
{
    if(!passwordView)
    {
        ui->confirm->setDisabled(false);
        ui->cancel->setDisabled(false);
        ui->changepwd->setDisabled(false);
        ui->iconButt->setDisabled(false);
        ui->nickname->setReadOnly(false);
        ui->username->setReadOnly(false);
    }
    else
    {
        ui->confirm2->setDisabled(false);
        ui->cancel2->setDisabled(false);
    }
}

void changeUserInfo::enableStyleButtons()
{
    if(!passwordView)
    {
        ui->confirm->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
        ui->cancel->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 grey);color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");

    }
    else
    {
        ui->confirm2->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
        ui->cancel2->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 grey);color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");

    }
}

void changeUserInfo::waiting()
{
    ui->waiting->show();
    ui->gif->show();
}

void changeUserInfo::hideLabelsError()
{
    ui->error->hide();
    ui->waiting->hide();
    ui->gif->hide();
}

bool changeUserInfo::checkPassword(const QString passwordToCheck)
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

void changeUserInfo::on_oldpwd_textChanged(const QString &arg1)
{
    if(arg1.toStdString()!=pwd)
    {
        QPixmap pix(":/icon/no.png");
        int w=ui->veroldpwd->width();
        int h=ui->veroldpwd->height();
        ui->veroldpwd->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));
    }
    else
    {
        QPixmap pix(":/icon/ok.png");
        int w=ui->veroldpwd->width();
        int h=ui->veroldpwd->height();
        ui->veroldpwd->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));
    }
}

void changeUserInfo::on_newpwd2_textChanged(const QString &arg1)
{
    QString newp=ui->newpwd1->text();
    if(arg1!=newp)
    {
        QPixmap pix(":/icon/no.png");
        int w=ui->veroldpwd->width();
        int h=ui->veroldpwd->height();
        ui->vernewpwd->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));
    }
    else
    {
        QPixmap pix(":/icon/ok.png");
        int w=ui->veroldpwd->width();
        int h=ui->veroldpwd->height();
        ui->vernewpwd->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));
    }
}

void changeUserInfo::on_cancel_clicked()
{
    h=new home(nullptr, pwd);
    h->setClientDispatcher(cl);
    //------------------------------------------------------------------PARTE DA DECOMENTARE
    #ifdef DISPATCHER_ON
    cl->setHome(h);
    #endif
    //------------------------------------------------------------------
    this->hide();
    h->show();
}

void changeUserInfo::on_newpwd1_textChanged(const QString &arg1)
{
    hideLabelsError();
    if(!checkPassword(arg1))
    {
        QPixmap pix(":/icon/no.png");
        int w=ui->vernewpwd2->width();
        int h=ui->vernewpwd2->height();
        ui->vernewpwd2->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));
        ui->error->setText("The password does not meet the requirements");
        ui->error->show();
    }
    else
    {
        QPixmap pix(":/icon/ok.png");
        int w=ui->vernewpwd2->width();
        int h=ui->vernewpwd2->height();
        ui->vernewpwd2->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));
    }
}
