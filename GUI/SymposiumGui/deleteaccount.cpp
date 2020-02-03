#include "deleteaccount.h"
#include "ui_deleteaccount.h"

deleteAccount::deleteAccount(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::deleteAccount)
{
    ui->setupUi(this);
    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->delete_2, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->delete_2, SIGNAL(clicked()), parent, SLOT(hide()));
    connect(ui->delete_2, SIGNAL(clicked()), parent->parentWidget(), SLOT(show()));
    connect(ui->delete_2, SIGNAL(clicked()), this, SLOT(delete_click()));
}

deleteAccount::~deleteAccount()
{
    delete ui;
}

void deleteAccount::delete_click()
{
    QMessageBox::information(parentWidget(),
                             tr("Delete Account"), tr("Your account has been successfully deleted"), QMessageBox::Ok);
}
