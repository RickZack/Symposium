#include "directory.h"
#include "ui_directory.h"

directory::directory(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::directory)
{
    ui->setupUi(this);

}

directory::~directory()
{
    delete ui;
}

void directory::on_actionNew_Document_triggered()
{
    textEditWindow= new TextEdit;
    textEditWindow->show();
}



void directory::on_actionHome_triggered()
{
    this->close();
}

void directory::on_actionUri_triggered()
{
    uriWindow=new inserturi;
    uriWindow->show();
}

void directory::on_actionNew_Folder_triggered()
{



}
