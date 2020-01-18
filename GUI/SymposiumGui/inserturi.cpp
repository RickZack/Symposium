#include "inserturi.h"
#include "ui_inserturi.h"

inserturi::inserturi(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::inserturi)
{
    ui->setupUi(this);
    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(close()));
}

inserturi::~inserturi()
{
    delete ui;
}

void inserturi::on_dir_clicked()
{
    dirWindow=new choosedir(this);
    dirWindow->show();
}
