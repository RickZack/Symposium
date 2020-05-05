#include "errorlogout.h"
#include "ui_errorlogout.h"
#include "mainwindow.h"
#include "Dispatcher/clientdispatcher.h"


errorlogout::errorlogout(QWidget *parent, QString str) :
    QDialog(parent),
    str(str), ui(new Ui::errorlogout)
{
    ui->setupUi(this);
    setFixedSize(size());
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->label->setText(str);
}

errorlogout::~errorlogout()
{
    delete ui;
}

void errorlogout::setClientDispatcher(Symposium::clientdispatcher *cl)
{
    this->cl=cl;
}

void errorlogout::on_pushButton_clicked()
{
    this->close();
    /*w=new MainWindow();
    w->setClientDispatcher(cl);
    w->show();*/
}
