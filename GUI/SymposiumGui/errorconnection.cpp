#include "errorconnection.h"
#include "ui_errorconnection.h"

errorconnection::errorconnection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::errorconnection)
{
    ui->setupUi(this);
}

errorconnection::~errorconnection()
{
    delete ui;
}

void errorconnection::on_pushButton_clicked()
{
    this->close();
}
