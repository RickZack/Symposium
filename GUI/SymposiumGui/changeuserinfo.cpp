#include "changeuserinfo.h"
#include "ui_changeuserinfo.h"
#include "Dispatcher/clientdispatcher.h"
#include <QMovie>

changeUserInfo::changeUserInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::changeUserInfo)
{
    ui->setupUi(this);
    ui->error->hide();

    //-----------------------------------------------------PARTE DA DECOMENTARE

    /*
     us=cl->getUser();
    ui->username->setText(QString::fromStdString(us.getUsername()));
    ui->password->setText("");
    ui->nickname->setText(QString::fromStdString(us.getNickname()));
    img=us.getIconPath();
     */


    //-----------------------------------------------------PARTE DA CANCELLARE
    ui->username->setText("Mario");
    ui->password->setText("");
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
    iconWindow->show();
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
    ui->error->hide();
    QString username = ui->username->text();
    QString password = ui->password->text();
    QString nickname = ui->nickname->text();
    //QString imagine = QString::fromStdString(img);
    //Symposium::user usNew(username.toStdString(), password.toStdString(), nickname.toStdString(), img, us.getSiteId(), us.getHome());
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

void changeUserInfo::errorConnectionLogout()
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->confirm->setDisabled(false);
    ui->cancel->setDisabled(false);
    errorLog = new errorlogout(this);
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
    ui->error->show();
}



void changeUserInfo::successEditUser()
{
    this->close();
    ui->waiting->hide();
    ui->gif->hide();
    ui->confirm->setDisabled(false);
    ui->cancel->setDisabled(false);
    QString stringa="Your information has been successfully modified";
    QMessageBox::information(parentWidget(),
                             tr("Confirm"), stringa, QMessageBox::Ok);

}
