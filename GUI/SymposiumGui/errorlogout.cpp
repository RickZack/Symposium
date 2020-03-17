#include "errorlogout.h"
#include "ui_errorlogout.h"
#include "mainwindow.h"


errorlogout::errorlogout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::errorlogout)
{
    ui->setupUi(this);
}

errorlogout::~errorlogout()
{
    delete ui;
}

void errorlogout::on_pushButton_clicked()
{
    this->close();
    w->show();
}
