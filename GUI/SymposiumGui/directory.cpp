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
    textEditWindow= new TextEdit(this);
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
    QListWidgetItem *item= new QListWidgetItem(QIcon(":/resources/cartelle/folder_icon"),"Folder");
    ui->myListWidget->addItem(item);




}

void directory::on_pushButton_clicked()
{

}

void directory::on_pushButton_2_clicked()
{

   QList<QListWidgetItem*> item= ui->myListWidget->selectedItems();
    foreach(QListWidgetItem *items, item){
        ui->myListWidget->removeItemWidget(items);
        delete items;
        ui->myListWidget->currentItem()->setSelected(false);
    }


}
