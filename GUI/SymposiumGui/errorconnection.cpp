#include "errorconnection.h"
#include "ui_errorconnection.h"

errorconnection::errorconnection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::errorconnection)
{
    ui->setupUi(this);
    connect(ui->pushButton, SIGNAL(clicked()), parentWidget(), SLOT(enableButtonsAfter()));
}

errorconnection::~errorconnection()
{
    delete ui;
}

void errorconnection::on_pushButton_clicked()
{
    this->close();
}
