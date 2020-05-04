#include "errorlogout.h"
#include "ui_errorlogout.h"
#include "mainwindow.h"
#include "Dispatcher/clientdispatcher.h"


errorlogout::errorlogout(QWidget *parent, QString str) :
    QDialog(parent),
    str(str), ui(new Ui::errorlogout)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setFixedSize(size());
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
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
