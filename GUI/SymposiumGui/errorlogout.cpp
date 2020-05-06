#include "errorlogout.h"
#include "ui_errorlogout.h"
#include "mainwindow.h"
#include "Dispatcher/clientdispatcher.h"


errorlogout::errorlogout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::errorlogout)
{
    ui->setupUi(this);
    setFixedSize(size());
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

errorlogout::~errorlogout()
{
    delete ui;
}

void errorlogout::on_pushButton_clicked()
{
    this->close();
    /*w=new MainWindow();
    w->setClientDispatcher(cl);
    w->show();*/
}
