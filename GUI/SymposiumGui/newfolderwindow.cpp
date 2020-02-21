#include "newfolderwindow.h"
#include "ui_newfolderwindow.h"


newFolderWindow::newFolderWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::newFolderWindow)
{
    ui->setupUi(this);
    QPixmap pix(":/resources/cartelle/Document-Add-icon");
    ui->label_3->setPixmap(pix);
    QPixmap pix_folder(":/resources/cartelle/new_folder");
    ui->label_5->setPixmap(pix_folder);

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

// to ensure the correct closing of the window
void newFolderWindow::closeEvent(QCloseEvent *e)
{
    QWidget::closeEvent(e);
}

// When the button "DELETE" is pushed, the name of the folder will be deleted and the folder will not be created.
void newFolderWindow::on_pushButton_4_clicked()
{
    QString name= ui->name->text();
    ui->name->setText(" ");
}

// When the button "CREATE" is pushed, a new folder is created
void newFolderWindow::on_pushButton_3_clicked()
{
    QString name= ui->name->text();
    ui->name->setText(" ");
    QListWidgetItem *item= new QListWidgetItem(QIcon(":/resources/cartelle/folder_icon"),name);
    ui->listWidget->addItem(item);

}
