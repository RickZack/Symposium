#include "signup.h"
#include "ui_signup.h"
#include "Dispatcher/clientdispatcher.h"

signup::signup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::signup)
{
    ui->setupUi(this);
    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(hide()));
    connect(ui->cancel, SIGNAL(clicked()), parent, SLOT(show()));
    QPixmap pix(":/resources/avatar/beaver.png");
    iconPath=":/resources/avatar/beaver.png";
    ui->haveto->hide();
    ui->usernameExist->hide();
    ui->passwordNotCorrect->hide();
    int w=ui->img->width();
    int h=ui->img->height();
    ui->img->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));
}

void signup::errorConnection()
{
    errorWindow = new errorconnection(this);
    errorWindow->show();
}

void signup::errorUsernameSignUp()
{
    ui->usernameExist->show();
}

void signup::errorPasswordSignUp()
{
    ui->passwordNotCorrect->show();
}

void signup::successSignUp()
{
    hide();
    homeWindow= new home(parentWidget());
    homeWindow->setClientDispatcher(cl);
    //cl->setHome(homeWindow);
    homeWindow->show();
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
    ui->usernameExist->hide();
    ui->passwordNotCorrect->hide();
    QString username= ui->username->text();
    QString password = ui->password->text();
    QString nickname =ui->nickname->text();

    //--------------------------------------------------------------PARTE DA DECOMENTARE

    /*if(username!="" && password!="" && nickname!=""){
        waiting();
        this->cl->signUp(username.toStdString(), password.toStdString(), nickname.toStdString(), iconPath);
    }
    else {
        ui->haveto->show();
    }*/


    //--------------------------------------------------PARTE DA CANCELLARE SUCCESSIVAMENTE

    if(username!="" && password!="" && nickname!="")
    {
        hide();
        homeWindow= new home(parentWidget());
        homeWindow->show();
    }
    else {

        ui->msg->setText("This credentials are not valid");
    }

    //----------------------------------------
}

void signup::on_iconButt_clicked()
{
    iconWindow = new icon(this);
    iconWindow->show();
}

void signup::chooseIcon()
{
    std::string msg=iconWindow->msg;
    iconPath=msg;
    QString msg2=QString::fromStdString(msg);
    QPixmap pix(msg2);
    int w=ui->img->width();
    int h=ui->img->height();
    ui->img->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));
}

void signup::reject()
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Exit",
                                                                    tr("Are you sure to quit?\n"),
                                                                     QMessageBox::No | QMessageBox::Yes,
                                                                    QMessageBox::Yes);
        if (resBtn == QMessageBox::Yes)
        {
            QDialog::reject();
            cl->logout();
        }
}
