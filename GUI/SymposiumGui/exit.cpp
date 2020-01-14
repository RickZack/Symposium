#include "exit.h"
#include "ui_exit.h"

exit::exit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::exit)
{
    ui->setupUi(this);
    //connect(ui->buttonBox, SIGNAL(accept()), parent, SLOT(qApp->quit()));
}

exit::~exit()
{
    delete ui;
}



void exit::on_buttonBox_accepted()
{
    qApp->quit();
    return;
}
