#include "home.h"
#include "ui_home.h"

home::home(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::home)
{
    ui->setupUi(this);
    connect(ui->logout, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->logout, SIGNAL(clicked()), parent, SLOT(show()));
}

home::~home()
{
    delete ui;
}

void home::on_delete_2_clicked()
{
    deleteAccountWindow = new deleteAccount(this);
    deleteAccountWindow->show();
}

void home::on_InsertUri_clicked()
{
    inserturiWindow = new inserturi(this);
    inserturiWindow->show();
}

void home::on_modify_clicked()
{
    changeWindow = new changeUserInfo(this);
    changeWindow->show();
}

void home::on_directory_clicked()
{
    directoryWindow=new directory(this);
    directoryWindow->show();
}
