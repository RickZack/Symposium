#include "errorlogout.h"
#include "ui_errorlogout.h"
#include "mainwindow.h"


errorlogout::errorlogout(QWidget *parent, QString str) :
    QDialog(parent),
    str(str), ui(new Ui::errorlogout)
{
    ui->setupUi(this);
    ui->label->setText(str);
}

errorlogout::~errorlogout()
{
    delete ui;
}

void errorlogout::on_pushButton_clicked()
{
    this->close();
    w=new MainWindow();
    w->show();
}
