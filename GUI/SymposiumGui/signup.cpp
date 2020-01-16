#include "signup.h"
#include "ui_signup.h"

signup::signup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::signup)
{
    ui->setupUi(this);
    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->cancel, SIGNAL(clicked()), parent, SLOT(show()));
    QPixmap pix(":/resources/avatar/beaver.png");
    int w=ui->img->width();
    int h=ui->img->height();
    ui->img->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));
}

signup::~signup()
{
    delete ui;
}

void signup::on_signin_clicked()
{
    close();
    homeWindow= new home(parentWidget());
    homeWindow->show();
}

void signup::on_iconButt_clicked()
{
    iconWindow = new icon(this);
    iconWindow->show();
}

void signup::chooseIcon()
{
    std::string msg=iconWindow->msg;
    QString msg2=QString::fromStdString(msg);
    QPixmap pix(msg2);
    int w=ui->img->width();
    int h=ui->img->height();
    ui->img->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));
}
