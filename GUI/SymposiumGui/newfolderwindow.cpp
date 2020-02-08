#include "newfolderwindow.h"
#include "ui_newfolderwindow.h"


newFolderWindow::newFolderWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::newFolderWindow)
{
    ui->setupUi(this);
    QPixmap pix(":/resources/cartelle/document_image");
    ui->label_3->setPixmap(pix);

}

newFolderWindow::~newFolderWindow()
{
    delete ui;
}



void newFolderWindow::on_createButton_clicked()
{
     textEditWindow =new TextEdit;
     //textEditWindow->show();
     if(textEditWindow->fileSave()==true || textEditWindow->maybeSave()==true){
         nameDocument=textEditWindow->getName();
         QListWidgetItem *item= new QListWidgetItem(QIcon(":/resources/cartelle/Document-Add-icon"),nameDocument);
         ui->listWidget->addItem(item);
     }

     //nameDocument= ui->name->text();
     //ui->name->setText(" ");
     //QListWidgetItem *item= new QListWidgetItem(QIcon(":/resources/cartelle/Document-Add-icon"),nameDocument);
     //ui->listWidget->addItem(item);


}

void newFolderWindow::on_pushButton_2_clicked()
{

    QList<QListWidgetItem*> item= ui->listWidget->selectedItems();
     foreach(QListWidgetItem *items, item){
         ui->listWidget->removeItemWidget(items);
         delete items;
         ui->listWidget->currentItem()->setSelected(false);
     }



}


void newFolderWindow::on_pushButton_clicked()
{
    textEditWindow =new TextEdit;
    //textEditWindow->setCurrentFileName(nameDocument);
    //textEditWindow->fileOpen();
    //textEditWindow->show();



}
