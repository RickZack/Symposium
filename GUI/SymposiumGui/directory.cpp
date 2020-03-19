#include "directory.h"
#include "ui_directory.h"
#include "home.h"
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

    // METODO DISPATCHER CHE RESTITUISCE LA STRINGA
    // str=getStr();
    str="directory 1 Folder1\n file 9 Document1 owner\n symlink 10 symlink10 modify\n directory 1 Folder2\n directory 3 Folder3\n directory 4 Folder4\n directory 5 Folder5\n directory 6 Folder6\n directory 7 Folder7\n directory 8 Folder8\n";
    int count=number_elements(str);
    listGenerate(str, count);
    values.push_back(str);

    ui->back_button->setDisabled(true);
    ui->back_button->hide();

    ui->actionHome->setIcon(QIcon(":/resources/cartelle/home_icon"));
    ui->actionUri->setIcon(QIcon(":/resources/cartelle/link_icon"));

    ui->pushButton->setIcon(QIcon(":/resources/cartelle/open_icon"));
    ui->pushButton_2->setIcon(QIcon(":/resources/cartelle/delete_icon"));
    ui->pushButton_3->setIcon(QIcon(":/resources/cartelle/create_icon"));
    ui->pushButton_4->setIcon(QIcon(":/resources/cartelle/create_icon"));
    ui->back_button->setIcon(QIcon(":/resources/cartelle/back_icon"));




}

directory::~directory()
{
    delete ui;
}


void directory::listGenerate(std::string str_first, int count)
{
    std::string str=generateString(str_first);
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
            item->setText(QString::fromStdString(word));
            ui->myListWidget->addItem(item);


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

std::string directory::searchForId(std::string word,std::string str,int count)
{
    std::string id_to_return;
    for(int i=0;i<count;i++){

        std::string type=separate_word(str);
        if(type=="directory"){
        std::string id=separate_word(str);
        std::string name=separate_word(str);
        name.erase(std::remove(name.begin(), name.end(), '\n'), name.end());
        if(name==word){
            id_to_return=id;
            i=count;
        }
        }
        else{
            std::string id=separate_word(str);
            std::string name=separate_word(str);
            std::string priv=separate_word(str);
            if(name==word){
                id_to_return=id;
                i=count;
            }
            }
}

   return id_to_return;
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

//this method is useful to delete from the string the \n in such way to visualize the list of element without spaces.
std::string directory::generateString(std::string string)
{
    for(size_t i = 0; i < string.size(); i++)
      if(string[i] == '\n')
          string.erase(std::remove(string.begin(), string.end(), '\n'), string.end());
    return string;
}

// this method counts the number of elements in the string
int directory::number_elements(std::string& string)
{
    int count=0;
    for(size_t i = 0; i < string.size(); i++)
      if(string[i] == '\n')
          count++;
    return count;
}

// this method opens the home window
void directory::on_actionHome_triggered()
{
    home *homeWindow=new home(this);
    homeWindow->show();
    this->hide();
}

// this method opens the uri window
void directory::on_actionUri_triggered()
{
    uriWindow=new inserturi(this);
    uriWindow->show();
    this->hide();
}


// this method opens a selected folder or a selected document or a selected symlink
void directory::on_pushButton_clicked()
{

        // I have to distinguish if the selected item is a DOCUMENT, a FOLDER or a SYMLINK
        QList<QListWidgetItem*> selectedItem= ui->myListWidget->selectedItems();
        foreach(QListWidgetItem *items, selectedItem){
             QString value= items->whatsThis();
             std::string nameSource=items->text().toStdString();
             std::string id=searchForId(nameSource,str,count);
             // dermine the path of the folders in which I enter.

             if(value=="directory")
             {
                 path+=id+'/'; // ok lo crea in modo corretto
                 std::string pth=path; //solo per visualizzare che lo crea in modo corretto. Dopo si può cancellare
                 aperto++;
                 // open the folder Window
                 // str1=getStr(INT ID);
                 std::string str1="directory 7 Prova1\n file 9 Document1 owner\n symlink 10 symlink10 modify\n directory 1 Prova2\n directory 3 Prova3\n directory 4 Prova4\n directory 5 Prova5\n directory 6 Prova6\n directory 7 Prova7\n directory 8 Prova8\n";
                 str=str1;
                 this->openWindow(str);

             }

             else if(value=="file")

                 // id and path
             {
                 std::string id=searchForId(nameSource,str,count);
                 // I have to open the choosepriv first
                 chooseprivWindow= new choosepriv(this);
                 chooseprivWindow->getPath(path,id);
                 this->hide();
             }
             else
             {
                 // it is a SymLink
                 // TECNICAMENTE IO DOVREI TROVARE IL PATH E IL NOME ed inviarlo al DISPATCHER
                 // path e nome che ce li ho.


             }

     }// foreach

}

//this method opens the new window referred to another folder and it saves in the List<QString> the string referred to the attual folder
// this is useful to move from a folder to another one.
void directory::openWindow(std::string str1){
    ui->myListWidget->clear();
    int counter=number_elements(str1);
    listGenerate(str1,counter);
    values.push_back(str1);
    ui->back_button->setDisabled(false);
    ui->back_button->show();
    this->show();
}

// this method deletes a selected folder or a selected document or a selected symlink
void directory::on_pushButton_2_clicked()
{

   QList<QListWidgetItem*> item= ui->myListWidget->selectedItems();
   foreach(QListWidgetItem *items, item){
       std::string nameSource=items->text().toStdString();
       std::string id=searchForId(nameSource,str,count);
       std::string pt=path;
       // bool msg=remouveResource(path, id)
       bool msg=true;
       if(msg)
       {
           if (count>1)
           {
              count--;
                    ui->myListWidget->removeItemWidget(items);
                    delete items;
                    ui->myListWidget->currentItem()->setSelected(false);
           }// if
           else
           {
                     ui->myListWidget->removeItemWidget(items);
                     delete items;
               count=0;

           }//else

       }// if
       else
       {
           QMessageBox::warning(this, "Error Message","It is no possible to delete the selected folder");
       }

   }//foreach
}

// this method creates a new folder
void directory::on_pushButton_3_clicked()
{

    QString name= ui->name->text();
    std::string nameFolder=name.toStdString();
    ui->name->setText(" ");
    // std::string id=createNewDir(path,id);
    std::string id="1"; //PER ESEMPIO

    if(id!="-1")
     // OK dal dispatcher, posso creare la nuova cartella. Tutti i controlli sono andati bene
    {
        count++;
        std::string new_str=" directory "+id+' '+name.toStdString()+'\n';
        str=str+new_str;
        ui->myListWidget->clear();
        int count=number_elements(str);
        listGenerate(str,count);

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

    QString name= ui->name_2->text();
    std::string nameDocument=name.toStdString();
    ui->name_2->setText(" ");
    //std::string id=createNewDir(path,nameDocument);
    std::string id="1"; //PER ESEMPIO

    if(id!="-1")
         // OK dal dispatcher, posso creare il nuovo documento
    {
        count++;
        std::string new_str=" file "+id+' '+name.toStdString()+'\n';
        str=str+new_str;
        ui->myListWidget->clear();
        int count=number_elements(str);
        listGenerate(str,count);
    }//if
    else
    {
      QMessageBox::warning(this, "Error Message","It is no possible to open the selected Document");
    }
}

//this method acts when the back button is clicked.
void directory::on_back_button_clicked()
{
    if(aperto!=0)
    {
        ui->myListWidget->clear();
        int dim=values.size();
        if(dim!=1)
            values.removeLast();
        // prelevo la stringa relativa alla finestra precedente a quella aperta
        std::string new_str= values.last();
        dim=values.size();
        int count=number_elements(new_str);
        listGenerate(new_str,count);
        aperto--;
    }
    if(aperto==0)
    {
        ui->back_button->setDisabled(true);
        ui->back_button->hide();
    }

    this->show();
}
