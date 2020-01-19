#include "directory.h"
#include "ui_directory.h"

directory::directory(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::directory)
{
    ui->setupUi(this);
    QPixmap pix(":/resources/cartelle/cartella_wind.png");

    ui->dir_1->setPixmap(pix.scaled(90,90));
    ui->dir_2->setPixmap(pix.scaled(90,90));
    ui->dir_3->setPixmap(pix.scaled(90,90));
    ui->dir_4->setPixmap(pix.scaled(90,90));
    ui->dir_6->setPixmap(pix.scaled(90,90));

}

directory::~directory()
{
    delete ui;
}

void directory::on_actionNew_Document_triggered()
{
    editor= new QTextEdit;
    editor->showMaximized();
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

    QPixmap pix(":/resources/cartelle/cartella_wind.png");
    ui->dir_5->setPixmap(pix.scaled(90,90));

}
