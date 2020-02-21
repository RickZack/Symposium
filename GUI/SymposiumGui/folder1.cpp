#include "folder1.h"
#include "ui_folder1.h"
#include "directory.h"

folder1::folder1(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::folder1)
{
    ui->setupUi(this);
    QPixmap pix(":/resources/cartelle/Document-Add-icon");
    ui->label_7->setPixmap(pix);
    QPixmap pix_folder(":/resources/cartelle/new_folder");
    ui->label_10->setPixmap(pix_folder);


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


// When the button "CREATE" is pushed, a new folder is created
void folder1::on_pushButton_3_clicked()
{
    /*
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
    */

}



//DA RIFARE
// SVUOTO LA WIDGET LIST E POI LA RIEMPIO CON UNA NUOVA STRINGA..
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


// DA RIFARE
// the method deletes a selected document or a selected folder
void folder1::on_pushButton_2_clicked()
{
    /*
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
    */
}

void folder1::on_pushButton_9_clicked()
{
    this->hide();
}

void folder1::listGenerate(std::string str, int count)
{
    std::string word;
    countDir=-1;
    for(int i=0; i<count; i++)
    {
        word=separate_word(str);
        QVariant v;
        id=separate_word(str);

        if(word=="directory")
        {
            word=separate_word(str);
            QListWidgetItem *item=new QListWidgetItem(QIcon(":/resources/cartelle/folder_icon"), QString::fromStdString(word));
            countDir++;
            v.setValue(countDir);
            item->setData(Qt::UserRole,v);
            item->setWhatsThis("directory");
            ui->myListWidget_2->addItem(item);
            //NB riccordarsi di cambiare il codice di Symposium nella print!
        }

        else if(word=="file")
        {
            word=separate_word(str);
            QListWidgetItem *item=new QListWidgetItem(QIcon(":/resources/cartelle/document_image"), QString::fromStdString(word));
            v.setValue(-1);
            item->setData(Qt::UserRole,v);
            item->setWhatsThis("file");
            ui->myListWidget_2->addItem(item);
            word=separate_word(str);
        }
        else
        {
            word=separate_word(str);
            QListWidgetItem *item=new QListWidgetItem(QIcon("://icon/link.png"), QString::fromStdString(word));
            v.setValue(-1);
            item->setData(Qt::UserRole,v);
            item->setWhatsThis("symlink");
            ui->myListWidget_2->addItem(item);
            word=separate_word(str);
        }
    }
}

std::string folder1::separate_word(std::string& string)
{
    std::string separato;
    std::size_t found = string.find_first_of(' ');
    if(found==std::string::npos)
        return string;
    separato.append(string, 0, found);
    string.erase(0, found+1);
    if(separato=="")
    {
        return " "+separate_word(string);
    }
    return separato;
}

int folder1::number_elements(std::string& string)
{
    int count=0;
    for(size_t i = 0; i < string.size(); i++)
      if(string[i] == '\n')
          count++;
    string.erase(std::remove(string.begin(), string.end(), '\n'), string.end());
    return count;
}

void folder1::openWindow(std::string str)
{
    int counter=number_elements(str);
    listGenerate(str,counter);
    this->show();
}
