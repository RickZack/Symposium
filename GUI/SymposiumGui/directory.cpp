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

// this method opens the uri window // !!!!!!
void directory::on_actionUri_triggered()
{
    uriWindow=new inserturi(this);
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
        QList<QListWidgetItem*> selectedItem= ui->myListWidget->selectedItems();
        foreach(QListWidgetItem *items, selectedItem){
             QString value= items->whatsThis();
             if(value=="directory")
             {
                 aperto++;
                 // open the folder Window
                 std::string str1="directory 1 Prova1\n file 9 Document1 owner\n symlink 10 symlink10 modify\n directory 1 Prova2\n directory 3 Prova3\n directory 4 Prova4\n directory 5 Prova5\n directory 6 Prova6\n directory 7 Prova7\n directory 8 Prova8\n";
                 this->openWindow(str1);

             }
             else if(value=="file")

             {
                 //open the notePad Window
                 notepadWindow= new notepad(this);
                 notepadWindow->show();

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
   bool msg=true;
   if(msg)
   {
       if (count>1)
       {
          count--;
          QList<QListWidgetItem*> item= ui->myListWidget->selectedItems();
            foreach(QListWidgetItem *items, item){
                QString name=items->text();
                // nome da passare al server
                std::string name_std=name.toStdString();
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
        item->setWhatsThis("directory");
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
    // server message to say that everything is ok
    bool msg=true;
    if(msg)
    {
        count++;
        QString name= ui->name_2->text();
        ui->name_2->setText(" ");
        //da passare al server: nome del document scelto dall'utente
        std::string name_folder=name.toStdString();
        QListWidgetItem *item= new QListWidgetItem(QIcon(":/resources/cartelle/document_image"),name);
        ui->myListWidget->addItem(item);
        item->setWhatsThis("file");
    }//if
    else
    {
      QMessageBox::warning(this, "Error Message","It is no possible to open the selected folder");
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
