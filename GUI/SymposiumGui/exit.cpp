#include "exit.h"
#include "ui_exit.h"


exit::exit(QWidget *parent, bool logout) :
    QDialog(parent),
    ui(new Ui::exit), logout(logout)
{
    ui->setupUi(this);
    connect(ui->ok, SIGNAL(clicked()), this, SLOT(doLogout()));
    connect(ui->ok, SIGNAL(clicked()), qApp, SLOT(quit()));
    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(close()));
}

void exit::setClientDispatcher(Symposium::clientdispatcher *cl)
{
    this->cl=cl;
}

exit::~exit()
{
    delete ui;
}

void exit::doLogout()
{
    if(logout)
    {
        //cl->closeConnection();
    }
}


