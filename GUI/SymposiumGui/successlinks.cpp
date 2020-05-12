#include "successlinks.h"
#include "ui_successlinks.h"


successlinks::successlinks(QWidget *parent, int caseLink, QString str, QString numShares, QString time) :
    QDialog(parent),
    ui(new Ui::successlinks), caseLink(caseLink), str(str), numShares(numShares), time(time)
{
    ui->setupUi(this);
    setFixedSize(size());
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    hideAll();
    if(caseLink==1)
        ui->all->show();
    else if(caseLink==2)
        share();
    else if(caseLink==3)
        noshare();
    else if(caseLink==4)
        timeshare();
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
    ui->share4->hide();
    ui->timer1->hide();
    ui->timer2->hide();
}

void successlinks::share()
{
    ui->share1->show();
    ui->share2->show();
    ui->share3->show();
    ui->share2->setText(numShares);
}

void successlinks::noshare()
{
    ui->label->hide();
    ui->link->hide();
    ui->share4->show();
}

void successlinks::timeshare()
{
    ui->timer1->show();
    ui->timer2->setText(time);
    ui->timer2->show();
}
