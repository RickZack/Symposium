#include "folder1.h"
#include "ui_folder1.h"

folder1::folder1(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::folder1)
{
    ui->setupUi(this);
    QPixmap pix(":/resources/cartelle/Document-Add-icon");
    ui->label_3->setPixmap(pix);
    QPixmap pix_folder(":/resources/cartelle/new_folder");
    ui->label_5->setPixmap(pix_folder);

}

folder1::~folder1()
{
    delete ui;
}


// To ensure the correct closing of the window
void folder1::closeEvent(QCloseEvent *e)
{
    QWidget::closeEvent(e);


}

// When the button "DELETE" is pushed, the name of the folder will be deleted
void folder1::on_pushButton_4_clicked()
{
    QString name= ui->name->text();
    ui->name->setText(" ");
}


// When the button "CREATE" is pushed, a new folder is created
void folder1::on_pushButton_3_clicked()
{
    // msg is used to have a connection with the server
    bool msg=true;
    if(msg)
    {
        QString name= ui->name->text();
        ui->name->setText(" ");
        QListWidgetItem *item= new QListWidgetItem(QIcon(":/resources/cartelle/folder_icon"),name);
        ui->listWidget->addItem(item);
        item->setWhatsThis("folder");

    }// if
    else
    {
        QMessageBox::warning(this,"Error Message","It is not possible to create a new folder");
    }

}



// DA RIFARE
// this method is used to open a document (or a folder)
void folder1::on_pushButton_clicked()
{
    /*
    // serverMessage
    bool msg=true;
    if(msg)
    {
        textEditWindow= new TextEdit(this);
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(textEditWindow);
        textEditWindow->resize(availableGeometry.width() / 2, (availableGeometry.height() * 2) / 3);
        textEditWindow->move((availableGeometry.width() - textEditWindow->width()) / 2,
                (availableGeometry.height() - textEditWindow->height()) / 2);

        QList<QListWidgetItem*> selectedItem= ui->listWidget->selectedItems();
        foreach(QListWidgetItem *items, selectedItem){
            QString name=items->text();
         if(name=="Information")
         {
           textEditWindow->load(":/Information.txt");
           textEditWindow->show();
         }
         else if(name=="Document1")
         {
              textEditWindow->load(":/Document1.txt");
              textEditWindow->show();
         }

         else if(name=="Document2")
         {
              textEditWindow->load(":/Document2.txt");
              textEditWindow->show();
         }

         else if(name=="Document3")
         {
             textEditWindow->load(":/Document3.txt");
             textEditWindow->show();
         }

         else
         {
             // devo riconoscere se è una cartella o un documento e aprire in modo diverso
             QString value= items->whatsThis();
             if(value=="folder"){
                 newFolder=new newFolderWindow(this);
                 newFolder->show();
             }//if
             else
             {
                textEditWindow->show();
             }//else

         }//else
     }//foreach
    }
    else
    {
        QMessageBox::information(this,"Error Message","It is not possible to satisfy your request");
    }//else
*/
}//method


// the method deletes a selected document or a selected folder
void folder1::on_pushButton_2_clicked()
{
    bool msg=true;
    if(msg)
    {
        if (count>1)
        {
           count--;
           QList<QListWidgetItem*> item= ui->listWidget->selectedItems();
             foreach(QListWidgetItem *items, item){
                 ui->listWidget->removeItemWidget(items);
                 delete items;
                 ui->listWidget->currentItem()->setSelected(false);
         }// foreach
        }// if
        else
        {
            QList<QListWidgetItem*> item= ui->listWidget->selectedItems();
              foreach(QListWidgetItem *items, item){
                  ui->listWidget->removeItemWidget(items);
                  delete items;
              }// foreach
            count=0;

        }//else

    }// if
    else
    {
        QMessageBox::warning(this, "Error Message","It is no possible to delete the selected item");
    }
}
