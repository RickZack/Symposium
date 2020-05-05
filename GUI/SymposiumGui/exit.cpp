#include "exit.h"
#include "ui_exit.h"
#include "Dispatcher/clientdispatcher.h"


exit::exit(QWidget *parent, bool logout) :
    QDialog(parent),
    ui(new Ui::exit), logout(logout)
{
    ui->setupUi(this);
    setFixedSize(size());
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    connect(ui->ok, SIGNAL(clicked()), this, SLOT(doLogout()));
    connect(ui->ok, SIGNAL(clicked()), qApp, SLOT(quit()));

    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->cancel, SIGNAL(clicked()), parentWidget(), SLOT(enableButtonsAfter()));
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
        cl->closeConnection();
    }
}



