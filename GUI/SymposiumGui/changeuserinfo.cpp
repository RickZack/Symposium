#include "changeuserinfo.h"
#include "ui_changeuserinfo.h"
#include "Dispatcher/clientdispatcher.h"
#include <QMovie>

changeUserInfo::changeUserInfo(QWidget *parent, std::string pwd) :
    QDialog(parent),
    ui(new Ui::changeUserInfo), pwd(pwd)
{
    ui->setupUi(this);
    ui->error->setText("");
    hiddenpwd();

    //-----------------------------------------------------PARTE DA DECOMENTARE

    /*
     us=cl->getUser();
    ui->username->setText(QString::fromStdString(us.getUsername()));
    ui->nickname->setText(QString::fromStdString(us.getNickname()));
    img=us.getIconPath();
     */


    //-----------------------------------------------------PARTE DA CANCELLARE
    ui->username->setText("Mario");
    ui->nickname->setText("Rossi");
    img=":/resources/avatar/beaver.png";


    //---------------------------------------------------------------------------

    QPixmap pix(QString::fromStdString(img));
    int w=ui->img->width();
    int h=ui->img->height();
    ui->img->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));

    ui->waiting->hide();
    ui->gif->hide();
    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();


    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->confirm, SIGNAL(clicked()), this, SLOT(confirm_click()));

}

changeUserInfo::~changeUserInfo()
{
    delete ui;
}

void changeUserInfo::on_iconButt_clicked()
{
    iconWindow = new icon(this);
    iconWindow->exec();
    img=iconWindow->msg;
    QString msg2=QString::fromStdString(img);
    QPixmap pix(msg2);
    int w=ui->img->width();
    int h=ui->img->height();
    ui->img->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));
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
    ui->waiting->show();
    ui->gif->show();
    ui->confirm->setDisabled(true);
    ui->cancel->setDisabled(true);
    ui->error->setText("");
    newpass=pwd;
    QString username = ui->username->text();
    QString nickname = ui->nickname->text();
    //QString imagine = QString::fromStdString(img);
    //Symposium::user usNew(username.toStdString(), pwd, nickname.toStdString(), img, us.getSiteId(), us.getHome());
    //cl->editUser(usNew);

    //------------------------------------------------------------------PARTE DA CANCELLARE
    QString stringa="Your information has been successfully modified";
    QMessageBox::information(parentWidget(),
                             tr("Confirm"), stringa, QMessageBox::Ok);
    //--------------------------------------------------------------------------
}


void changeUserInfo::setClientDispatcher(Symposium::clientdispatcher *cl){
    this->cl = cl;
}

void changeUserInfo::errorConnection()
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->confirm->setDisabled(false);
    ui->cancel->setDisabled(false);
    errorWindow = new errorconnection(this);
    errorWindow->show();
}

void changeUserInfo::errorConnectionLogout(std::string str)
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->confirm->setDisabled(false);
    ui->cancel->setDisabled(false);
    errorLog = new errorlogout(this, QString::fromStdString(str));
    this->close();
    parentWidget()->close();
    errorLog->show();
}

void changeUserInfo::errorEditUser(std::string errorMess)
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->confirm->setDisabled(false);
    ui->cancel->setDisabled(false);
    ui->error->setText(QString::fromStdString(errorMess));
}



void changeUserInfo::successEditUser()
{
    pwd=newpass;
    this->close();
    ui->waiting->hide();
    ui->gif->hide();
    ui->confirm->setDisabled(false);
    ui->cancel->setDisabled(false);
    QString stringa="Your information has been successfully modified";
    QMessageBox::information(parentWidget(),
                             tr("Confirm"), stringa, QMessageBox::Ok);

}

void changeUserInfo::on_confirm2_clicked()
{
    QString newp1=ui->newpwd1->text();
    QString newp2=ui->newpwd2->text();
    if(newp1==newp2)
    {
        newpass=newp1.toStdString();
        ui->waiting->show();
        ui->gif->show();
        ui->confirm2->setDisabled(true);
        ui->cancel2->setDisabled(true);
        ui->error->setText("");
        //QString imagine = QString::fromStdString(img);
        //Symposium::user usNew(us.getUsername(), newp1.toStdString(), us.getNickname(), img, us.getSiteId(), us.getHome());
        //cl->editUser(usNew);

        //------------------------------------------------------------------PARTE DA CANCELLARE
        QString stringa="Your information has been successfully modified";
        pwd=newp1.toStdString();
        QMessageBox::information(parentWidget(),
                                 tr("Confirm"), stringa, QMessageBox::Ok);
        //--------------------------------------------------------------------------
    }
    else
    {
       ui->error->setText("Password in the new password field has to be equal to the password in the repeat password field!");
    }
}

void changeUserInfo::on_cancel2_clicked()
{
    hiddenpwd();
    showinformation();
}

void changeUserInfo::on_changepwd_clicked()
{
    hiddeninformation();
    showpwd();

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
    ui->veroldpwd->show();
    ui->veroldpwd->clear();
    ui->vernewpwd->show();
    ui->vernewpwd->clear();
    ui->error->setText("");
    ui->confirm2->setDisabled(false);
    ui->cancel2->setDisabled(false);
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
