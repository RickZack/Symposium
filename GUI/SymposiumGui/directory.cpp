#include "directory.h"
#include "ui_directory.h"
#include <ostream>

directory::directory(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::directory)
{
    ui->setupUi(this);
    QPixmap pix_folder(":/resources/cartelle/new_folder");
    ui->label_3->setPixmap(pix_folder);
    QPixmap pix_document(":/resources/cartelle/Document-Add-icon");
    ui->label_4->setPixmap(pix_document);


    str="directory 1 Folder1\n file 9 Document1 owner\n symlink 10 symlink10 modify\n directory 1 Folder2\n directory 3 Folder3\n directory 4 Folder4\n directory 5 Folder5\n directory 6 Folder6\n directory 7 Folder7\n directory 8 Folder8\n";
    int count=number_elements(str);
    listGenerate(str, count);

}

directory::~directory()
{
    delete ui;
}


void directory::listGenerate(std::string str, int count)
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
            ui->myListWidget->addItem(item);

            //NB riccordarsi di cambiare il codice di Symposium nella print!
        }

        else if(word=="file")
        {
            word=separate_word(str);
            QListWidgetItem *item=new QListWidgetItem(QIcon(":/resources/cartelle/document_image"), QString::fromStdString(word));
            v.setValue(-1);
            item->setData(Qt::UserRole,v);
            item->setWhatsThis("file");
            ui->myListWidget->addItem(item);
            word=separate_word(str);
        }
        else
        {
            word=separate_word(str);
            QListWidgetItem *item=new QListWidgetItem(QIcon("://icon/link.png"), QString::fromStdString(word));
            v.setValue(-1);
            item->setData(Qt::UserRole,v);
            item->setWhatsThis("symlink");
            ui->myListWidget->addItem(item);
            word=separate_word(str);
        }
    }
}

std::string directory::separate_word(std::string& string)
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

int directory::number_elements(std::string& string)
{
    int count=0;
    for(size_t i = 0; i < string.size(); i++)
      if(string[i] == '\n')
          count++;
    string.erase(std::remove(string.begin(), string.end(), '\n'), string.end());
    return count;
}

// this method opens the home window
void directory::on_actionHome_triggered()
{
    this->close();
}

// this method opens the uri window
void directory::on_actionUri_triggered()
{
    uriWindow=new inserturi(this);
    this->hide();
    uriWindow->show();
}


// this method opens a selected folder or a selected document or a selected symlink
void directory::on_pushButton_clicked()
{

    // the bool msg is used to have a connection with the server.
    // Now, it is settsded to true, but then it will be modified by the dispatcher
    bool msg=true;
    if(msg){
        // I have to distinguish if the selected item is a DOCUMENT, a FOLDER or a SYMLINK
        folder1Window=new folder1(this);
        folderWindow=new newFolderWindow(this);
        QList<QListWidgetItem*> selectedItem= ui->myListWidget->selectedItems();
        foreach(QListWidgetItem *items, selectedItem){
             QString value= items->whatsThis();
             if(value=="directory")
             {
                 // open the folder Window
                 folder1Window=new folder1(this);
                 folder1Window->show();

             }
             else if(value=="file")
             {
                 //open the textEdit Window
                 textEditWindow= new TextEdit(this);
                 const QRect availableGeometry = QApplication::desktop()->availableGeometry(textEditWindow);
                 textEditWindow->resize(availableGeometry.width() / 2, (availableGeometry.height() * 2) / 3);
                 textEditWindow->move((availableGeometry.width() - textEditWindow->width()) / 2,
                         (availableGeometry.height() - textEditWindow->height()) / 2);
                 textEditWindow->show();
             }
             else
             {
                 // it is a SymLink
             }

     }// foreach
}//if
    else
    {
        QMessageBox::warning(this, "Error Message","It is no possible to open the selected folder");
    }
}

// SISTEMARE
// this method deletes a selected folder or a selected document or a selected symlink
void directory::on_pushButton_2_clicked()
{
   bool msg=true;
   if(msg)
   {
       if (count>1)
       {
          count--;
          QList<QListWidgetItem*> item= ui->myListWidget->selectedItems();
            foreach(QListWidgetItem *items, item){
                QString name=items->text();
                std::string name_std=name.toStdString();
                std::string type= items->whatsThis().toStdString();
                remouveFromString(name_std,str,type);
                ui->myListWidget->removeItemWidget(items);
                delete items;
                ui->myListWidget->currentItem()->setSelected(false);
        }// foreach
       }// if
       else
       {
           QList<QListWidgetItem*> item= ui->myListWidget->selectedItems();
             foreach(QListWidgetItem *items, item){
                 ui->myListWidget->removeItemWidget(items);
                 delete items;
             }// foreach
           count=0;

       }//else

   }// if
   else
   {
       QMessageBox::warning(this, "Error Message","It is no possible to delete the selected folder");
   }

}

// this method creates a new folder
void directory::on_pushButton_3_clicked()
{
    bool msg=true;
    if(msg)
    {
        count++;
        QString name= ui->name->text();
        ui->name->setText(" ");
        //da passare al server: nome del folder
        std::string name_folder=name.toStdString();
        std::string new_str="directory 1 "+name.toStdString()+'\n';
        QListWidgetItem *item= new QListWidgetItem(QIcon(":/resources/cartelle/folder_icon"),name);
        ui->myListWidget->addItem(item);
    }//if
    else
    {
      QMessageBox::warning(this, "Error Message","It is no possible to open the selected folder");
    }

}

//this method closes the window directory
void directory::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Exit",
                                                                    tr("Are you sure to quit?\n"),
                                                                     QMessageBox::No | QMessageBox::Yes,
                                                                    QMessageBox::Yes);
        if (resBtn != QMessageBox::Yes) {
            event->ignore();
        } else {

            event->accept();
        }

}


//this method creates a new document
void directory::on_pushButton_4_clicked()
{
}
