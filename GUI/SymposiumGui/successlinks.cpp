#include "successlinks.h"
#include "ui_successlinks.h"


successlinks::successlinks(QWidget *parent, int caseLink, QString str, QString numShares) :
    QDialog(parent),
    ui(new Ui::successlinks), caseLink(caseLink), str(str), numShares(numShares)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    hideAll();
    if(caseLink==1)
        ui->all->show();
    else if(caseLink==2)
        share(numShares);
    ui->link->setText(str);
}

successlinks::~successlinks()
{
    delete ui;
}

void successlinks::on_ok_clicked()
{
    this->close();
}

void successlinks::hideAll()
{
    ui->all->hide();
    ui->share1->hide();
    ui->share2->hide();
    ui->share3->hide();
}

void successlinks::share(QString num)
{
    ui->share1->show();
    ui->share2->show();
    ui->share3->show();
    ui->share2->setText(num);
}
