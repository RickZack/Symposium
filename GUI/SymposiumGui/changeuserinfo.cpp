#include "changeuserinfo.h"
#include "ui_changeuserinfo.h"

changeUserInfo::changeUserInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::changeUserInfo)
{
    ui->setupUi(this);
    ui->username->setText("Mario");
    ui->password->setText("password");
    ui->nickname->setText("Rossi");

    QPixmap pix(":/resources/avatar/beaver.png");
    int w=ui->img->width();
    int h=ui->img->height();
    ui->img->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));


    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->confirm, SIGNAL(clicked()), this, SLOT(close()));
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
    std::string msg=iconWindow->msg;
    QString msg2=QString::fromStdString(msg);
    QPixmap pix(msg2);
    int w=ui->img->width();
    int h=ui->img->height();
    ui->img->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));
}


void changeUserInfo::confirm_click()
{
    QMessageBox::information(parentWidget(),
                             tr("Confirm"), tr("Your information has been successfully modified"), QMessageBox::Ok);
}


//NB: QUESTA FUNZIONE CAMBIA I DATI QUANDO SI CAMBIA IL TESTO NELLA CASELLA
/*void changeUserInfo::on_username_textEdited(const QString &arg1)
{
    QPixmap pix(":/resources/avatar/owl.png");
    int w=ui->img->width();
    int h=ui->img->height();
    ui->img->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));
}*/
