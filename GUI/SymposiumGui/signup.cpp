#include "signup.h"
#include "ui_signup.h"
#include "Dispatcher/clientdispatcher.h"
#include "mainwindow.h"
#include "onoff_networkinteraction.h"

signup::signup(QWidget *parent, SymWinInterface& si) :
    QDialog(parent),
    SymChildWinInterface (si, isQWidget::isQwidgetType(*this)),
    ui(new Ui::signup)
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
    QPixmap pix(":/resources/avatar/beaver.png");
    iconPath=":/resources/avatar/beaver.png";
    hideLabelsError();
    w=ui->img->width();
    h=ui->img->height();
    ui->img->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));
    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();

}

void signup::success(){
    successSignUp();
}

void signup::failure(const QString& toPrint){
    if(toPrint=="-1"){
        errorConnection();
    }else{
        errorSignUp(toPrint);
    }
}

void signup::errorConnection()
{
    enableButtons();
    hideLabelsError();
    pressed=false;
    errorconnection errorWindow(this);
    int ret=errorWindow.exec();
    if(ret==0)
        enableStyleButtons();
}

void signup::errorSignUp(const QString& errorMess)
{
    enableButtons();
    enableStyleButtons();
    hideLabelsError();
    pressed=false;
    ui->errorMess->setText(errorMess);
    ui->errorMess->show();
}


void signup::successSignUp()
{
    enableButtons();
    hideLabelsError();
    pressed=false;
    notification notWindow(nullptr, "Your account has been successfully created");
    int ret=notWindow.exec();
    if(ret==0)
    {
        home* homeWindow= new home(nullptr, pwd, *this);
        goToWindow(*homeWindow);
    }
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
    #ifdef DISPATCHER_ON
    if(username!="" && password!="" && nickname!=""){
      if(!checkPassword(password))
        {
            ui->errorMess->setText("The password does not meet the requirements");
            ui->errorMess->show();
        }
      if(!Symposium::user::noSpaceUsername(username.toStdString()))
        {
          ui->errorMess->setText("The username must not contain spaces");
          ui->errorMess->show();
        }
        else
        {
            disableStyleButtons();
            disableButtons();
            waiting();
            pressed=true;
            cl.signUp(username.toStdString(), password.toStdString(), nickname.toStdString(), iconPath);
         }
    }
    else {
        ui->haveto->show();
    }
    #endif


    //--------------------------------------------------PARTE DA CANCELLARE SUCCESSIVAMENTE
    #ifndef DISPATCHER_ON
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
            hideLabelsError();
            notification notWindow(nullptr, "Your account has been successfully created");
            int ret=notWindow.exec();
            if(ret==0)
            {
                home* homeWindow= new home(nullptr, pwd, *this);
                goToWindow(*homeWindow);
            }
        }
    }
    else {

        ui->haveto->show();
    }
    #endif
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
    event->ignore();
    if(closedByUser())
    {
        disableStyleButtons();
        event->ignore();
        class exit ex(this, false, &cl);
        int ret=ex.exec();
        if(ret==0 && !pressed)
            enableStyleButtons();
    }
    else
        event->accept();

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

void signup::on_cancel_clicked()
{
    backToParent();
}
