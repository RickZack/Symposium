#include "newdocument.h"
#include "ui_newdocument.h"

newdocument::newdocument(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::newdocument)
{
    ui->setupUi(this);
}

newdocument::~newdocument()
{
    delete ui;
}
