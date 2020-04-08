#include "signupmw.h"
#include "ui_signupmw.h"
#include "mainwindow.h"

signupmw::signupmw(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::signupmw)
{
    ui->setupUi(this);
    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(hide()));

    QPixmap pix2(":/icon/logo.png");
    int w=ui->logo->width();
    int h=ui->logo->height();
    ui->logo->setPixmap(pix2.scaled(w, h, Qt::KeepAspectRatio));

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
}

signupmw::~signupmw()
{
    delete ui;
}

void signupmw::setClientDispatcher(Symposium::clientdispatcher *cl)
{
    this->cl=cl;
}

void signupmw::on_cancel_clicked()
{
    mw= new MainWindow();
    mw->show();
    hide();
}

void signupmw::on_ok_clicked()
{
    ui->haveto->hide();
    ui->errorMess->hide();
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
            ui->ok->setDisabled(false);
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
            ui->ok->setDisabled(false);
            ui->cancel->setDisabled(false);
            ui->gif->hide();
            ui->waiting->hide();
        }
        else
        {
            hide();
            homeWindow= new home(nullptr, pwd);
            homeWindow->show();
            notWindow = new notification(homeWindow, "Your account was successfully created");
            notWindow->exec();
        }
    }
    else {

        ui->haveto->show();
    }

    //----------------------------------------
}

void signupmw::chooseIcon()
{
    iconPath=iconWindow->msg;
    QString msg2=QString::fromStdString(iconPath);
    QPixmap pix(msg2);
    int w=ui->img->width();
    int h=ui->img->height();
    ui->img->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));
}

void signupmw::on_iconButt_clicked()
{
    iconWindow = new icon(this);
    iconWindow->show();
}

bool signupmw::checkPassword(QString passwordToCheck)
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

void signupmw::waiting()
{
    ui->waiting->show();
    ui->gif->show();
    ui->iconButt->setDisabled(true);
    ui->ok->setDisabled(true);
    ui->cancel->setDisabled(true);
}
