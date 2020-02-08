#include "directory.h"
#include "ui_directory.h"

directory::directory(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::directory)
{
    ui->setupUi(this);
    QPixmap pix(":/resources/cartelle/new_folder");
    ui->label_3->setPixmap(pix);


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


void directory::on_pushButton_clicked()
{
    folderWindow=new newFolderWindow;
    folderWindow->show();


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

void directory::on_pushButton_3_clicked()
{
    QString name= ui->name->text();
    ui->name->setText(" ");
    QListWidgetItem *item= new QListWidgetItem(QIcon(":/resources/cartelle/folder_icon"),name);
    ui->myListWidget->addItem(item);
}
