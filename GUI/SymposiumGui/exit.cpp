#include "exit.h"
#include "ui_exit.h"

exit::exit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::exit)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(accepted()), qApp, SLOT(quit()));
}

exit::~exit()
{
    delete ui;
}


