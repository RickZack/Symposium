#include "signup.h"
#include "ui_signup.h"
#include "Dispatcher/clientdispatcher.h"

signup::signup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::signup)
{
    ui->setupUi(this);
    QPixmap pix2(":/icon/logo.png");
    int w=ui->logo->width();
    int h=ui->logo->height();
    ui->logo->setPixmap(pix2.scaled(w, h, Qt::KeepAspectRatio));
    connect(ui->cancel, SIGNAL(clicked()), parent, SLOT(show()));
    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(hide()));
    QPixmap pix(":/resources/avatar/beaver.png");
    iconPath=":/resources/avatar/beaver.png";
    ui->haveto->hide();
    ui->errorMess->hide();
    w=ui->img->width();
    h=ui->img->height();
    ui->img->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));
    ui->waiting->hide();
    ui->gif->hide();
    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

void signup::errorConnection()
{
    ui->iconButt->setDisabled(false);
    ui->signin->setDisabled(false);
    ui->cancel->setDisabled(false);
    ui->haveto->hide();
    ui->errorMess->hide();
    ui->msg->hide();
    errorWindow = new errorconnection(this);
    errorWindow->exec();
}

void signup::errorSignUp(std::string errorMess)
{
    ui->iconButt->setDisabled(false);
    ui->signin->setDisabled(false);
    ui->cancel->setDisabled(false);
    ui->haveto->hide();
    ui->msg->hide();
    ui->errorMess->setText(QString::fromStdString(errorMess));
    ui->errorMess->show();
}


void signup::successSignUp()
{
    hide();
    ui->iconButt->setDisabled(false);
    ui->signin->setDisabled(false);
    ui->cancel->setDisabled(false);

    homeWindow= new home(parentWidget(), pwd);
    homeWindow->setClientDispatcher(cl);
    //cl->setHome(homeWindow);
    homeWindow->show();
    notWindow = new notification(parentWidget(), "The account was successfully created");
    notWindow->exec();
}


void signup::setClientDispatcher( Symposium::clientdispatcher *cl){
    this->cl = cl;
}

signup::~signup()
{
    delete ui;
}

void signup::on_signin_clicked()
{
    ui->haveto->hide();
    ui->errorMess->hide();
    ui->msg->hide();
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
            ui->signin->setDisabled(false);
            ui->cancel->setDisabled(false);
            ui->gif->hide();
            ui->waiting->hide();
        }
        else
        {
            waiting();
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
            ui->signin->setDisabled(false);
            ui->cancel->setDisabled(false);
            ui->gif->hide();
            ui->waiting->hide();
        }
        else
        {
            hide();
            homeWindow= new home(parentWidget(), pwd);
            homeWindow->show();
            notWindow = new notification(parentWidget(), "Your account was successfully created");
            notWindow->exec();
        }
    }
    else {

        ui->haveto->show();
    }

    //----------------------------------------
}

void signup::on_iconButt_clicked()
{
    iconWindow = new icon(this);
    iconWindow->exec();
    iconPath=iconWindow->msg;
    QString msg2=QString::fromStdString(iconPath);
    QPixmap pix(msg2);
    int w=ui->img->width();
    int h=ui->img->height();
    ui->img->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));
}

bool signup::checkPassword(QString passwordToCheck)
{
    std::string str=passwordToCheck.toStdString();
    if(str.length()<=5)
            return false;
    else if(str.length()>=22)
            return false;
    else
    {
        static constexpr char noChar[] ="1234567890?!$+-/.,@ˆ_ ";
        std::size_t found = str.find_first_not_of(noChar);
        if(found == std::string::npos)
            return false;
        else {
            static constexpr char noNum[] ="abcdefghijklmnopqrstuvwxyz?!$+-/.,@ˆ_ ";
            std::size_t found = str.find_first_not_of(noNum);
            if(found == std::string::npos)
                return false;
            else {
                static constexpr char noSpecialChar[] ="abcdefghijklmnopqrstuvwxyz1234567890 ";
                std::size_t found = str.find_first_not_of(noSpecialChar);
                if(found == std::string::npos)
                    return false;
            }
        }
    }
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
   ex = new class exit(this, false);
   ex->exec();

}

void signup::waiting()
{
    ui->waiting->show();
    ui->gif->show();
    ui->iconButt->setDisabled(true);
    ui->signin->setDisabled(true);
    ui->cancel->setDisabled(true);
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
