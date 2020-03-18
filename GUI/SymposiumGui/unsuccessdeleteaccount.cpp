#include "unsuccessdeleteaccount.h"
#include "ui_unsuccessdeleteaccount.h"

unsuccessdeleteaccount::unsuccessdeleteaccount(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::unsuccessdeleteaccount)
{
    ui->setupUi(this);
}

unsuccessdeleteaccount::~unsuccessdeleteaccount()
{
    delete ui;
}

void unsuccessdeleteaccount::on_pushButton_clicked()
{
    this->close();
}
