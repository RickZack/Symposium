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
    // str=showHome(); === getStr()=Ksenia
    str="directory 1 Folder1\n file 9 Folder1 owner\n symlink 10 symlink10 modify\n directory 1 Folder2\n directory 3 Folder3\n directory 4 Folder4\n directory 5 Folder5\n directory 6 Folder6\n directory 7 Folder7\n directory 8 Folder8\n";
    int count=number_elements(str);
    listGenerate(str, count);
    values.push_back(str);

    ui->back_button->setDisabled(true);
    ui->back_button->hide();

    ui->renameLabel->hide();
    ui->okButton->hide();
    ui->renameName->hide();
    ui->okButton_2->hide();


    ui->actionHome->setIcon(QIcon(":/resources/cartelle/home_icon"));
    ui->actionUri->setIcon(QIcon(":/resources/cartelle/link_icon"));

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
            }
}

   return id_to_return;
}

std::pair<std::string,std::string> directory::searchForPriv(std::string word,std::string str,int count)
{
    std::string priv_to_return;
    std::string id_to_return;
    std::pair<std::string, std::string> idPriv;
    for(int i=0;i<count;i++){

            std::string type=separate_word(str);
            if(type=="directory"){
                std::string id=separate_word(str);
                std::string name=separate_word(str);
            }else{
                std::string id=separate_word(str);
                std::string name=separate_word(str);
                std::string priv=separate_word(str);
                if(name==word){
                    priv.erase(std::remove(priv.begin(), priv.end(), '\n'), priv.end());
                    priv_to_return=priv;
                    id_to_return=id;
                    i=count;
                }
           }
       }

    idPriv={id_to_return,priv_to_return};
    return idPriv;
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

std::string directory::generateString(std::string string)
{
    for(size_t i = 0; i < string.size(); i++)
      if(string[i] == '\n')
          string.erase(std::remove(string.begin(), string.end(), '\n'), string.end());
    return string;
}

int directory::number_elements(std::string& string)
{
    int count=0;
    for(size_t i = 0; i < string.size(); i++)
      if(string[i] == '\n')
          count++;
    return count;
}

void directory::on_actionHome_triggered()
{
    home *homeWindow=new home(this);
    homeWindow->show();
    this->hide();
}

void directory::on_actionUri_triggered()
{
    uriWindow=new inserturi(this);
    uriWindow->show();
    this->hide();
}


void directory::openWindow(std::string str1){
    ui->myListWidget->clear();
    int counter=number_elements(str1);
    listGenerate(str1,counter);
    values.push_back(str1);
    ui->back_button->setDisabled(false);
    ui->back_button->show();
    this->show();
}

void directory::contextMenuEvent(QContextMenuEvent *event)
{
        QMenu submenu;
        submenu.addAction(tr("Open"),this,&directory::openSelectedSource);
        submenu.addSeparator();
        submenu.addAction(tr("Delete"),this,&directory::deleteSource);
        submenu.addSeparator();
        submenu.addAction(tr("Rename"),this,&directory::renameSource);
        QPoint globalPos=ui->myListWidget->cursor().pos();
        submenu.exec(globalPos);
    }


//acts when the user clicks on the button "DELETE"
void directory::deleteSource()
{
   std::string id;
   QList<QListWidgetItem*> item= ui->myListWidget->selectedItems();
   foreach(QListWidgetItem *items, item){
       std::string nameSource=items->text().toStdString();
       std::string typeSource=items->whatsThis().toStdString();
       if(typeSource=="directory"){
            id=searchForId(nameSource,str,count);
       }else{
           std::pair<std::string,std::string> idPriv=searchForPriv(nameSource,str,count);
           id=idPriv.first;
       }
       //cl->remouveResource(path,id);

       //-------------------------------------------------------------------
       //DA RIMUOVERE

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
   //----------------------------------------------------------------------------------------------------
}

void directory::successRemove(){
    QList<QListWidgetItem*> item= ui->myListWidget->selectedItems();
    foreach(QListWidgetItem *items, item){
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

    }
}

void directory::failureActionDirectory(std::string msg){
    QMessageBox::warning(this,"Warning Message",QString::fromStdString(msg));
}

// this method creates a new folder
void directory::on_pushButton_3_clicked()
{

    QString name= ui->name->text();
    std::string nameFolder=name.toStdString();
    // anche questo da eliminare
    ui->name->setText(" ");
    // std::string id=createNewDir(path,nameFolder);
    //--------------------------------------------------------------------
    // DA ELIMINARE
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
    //-----------------------------------------------------------------------------------------

}

void directory::successCreate(std::string id){
    QString name= ui->name->text();
    ui->name->setText(" ");
    count++;
    std::string new_str=" directory "+id+' '+name.toStdString()+'\n';
    str=str+new_str;
    ui->myListWidget->clear();
    int count=number_elements(str);
    listGenerate(str,count);

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

// creates a new file
void directory::on_pushButton_4_clicked()
{

    QString name= ui->name_2->text();
    std::string nameDocument=name.toStdString();

    ui->name_2->setText(" "); // da rimuovere
    //cl->createNewSource(path,nameDocument);
    //----------------------------------------------------------------------
    // DA RIMUOVERE
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
    //-----------------------------------------------------------------------------------------------
}

void directory::successNewSource(std::string id){

    QString name= ui->name_2->text();
    std::string nameDocument=name.toStdString();
    ui->name_2->setText(" ");
    count++;
    std::string new_str=" file "+id+' '+name.toStdString()+' '+"owner"+'\n';
    str=str+new_str;
    ui->myListWidget->clear();
    int count=number_elements(str);
    listGenerate(str,count);
}


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

        //two times because I have to remouve id/n
        path.pop_back();
        path.pop_back();

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

void directory::renameSource()
{
    ui->renameName->show();
    ui->renameLabel->show();
    ui->okButton->show();
    ui->okButton_2->show();
}

void directory::on_okButton_clicked()
{
    QString newName=ui->renameLabel->text();
    QList<QListWidgetItem*> selectedItem= ui->myListWidget->selectedItems();
    foreach(QListWidgetItem *items, selectedItem){
         std::string oldName=items->text().toStdString();
         std::string id=searchForId(oldName,str,count);
         //cl->renameResource(this->path,id, newName);
         //------------------------------------------------------
         // QUESTE RIGHE VANNO LASCIATE SOLO NEL SUCCESS RENAME
         items->setText(" ");
         items->setText(newName);
         ui->myListWidget->currentItem()->setSelected(false);
         ui->renameLabel->setText(" ");
         ui->okButton->hide();
         ui->renameLabel->hide();
         ui->renameName->hide();
         ui->okButton_2->hide();
         //-------------------------------------------------------
    }

}

void directory::successRename(){
    QString newName=ui->renameName->text();
    ui->renameName->setText(" ");
    QList<QListWidgetItem*> selectedItem= ui->myListWidget->selectedItems();
    foreach(QListWidgetItem *items, selectedItem){
        items->setText(" ");
        items->setText(newName);
        ui->myListWidget->currentItem()->setSelected(false);
        ui->renameLabel->setText(" ");
        ui->okButton->hide();
        ui->renameLabel->hide();
        ui->renameName->hide();
        ui->okButton_2->hide();
    }
}


void directory::errorConnection(){
    errorWindow = new errorconnection(this);
    errorWindow->show();
}

void directory::setClientDispatcher(Symposium::clientdispatcher *cl)
{
    this->cl = cl;
}

void directory::errorConnectionLogout(){
    errorLogoutWindow= new errorlogout(this);
    this->close();
    errorLogoutWindow->show();

}


void directory::on_myListWidget_itemDoubleClicked(QListWidgetItem *item)
{

    QString value= item->whatsThis();
    std::string nameSource=item->text().toStdString();
    // dermine the path of the folders in which I enter.
    if(value=="directory")
    {
        std::string id=searchForId(nameSource,str,count);
        path+=id+'/'; // ok lo crea in modo corretto
        std::string pth=path; //solo per visualizzare che lo crea in modo corretto. Dopo si può cancellare
        aperto++;
        // open the folder Window
        // str1=cl->getStr(this->id, this->path);
        std::string str1="directory 7 Prova1\n file 9 Document1 owner\n symlink 10 symlink10 modify\n directory 1 Prova2\n directory 3 Prova3\n directory 4 Prova4\n directory 5 Prova5\n directory 6 Prova6\n directory 7 Prova7\n directory 8 Prova8\n";
        str=str1;

        this->openWindow(str);

    }

    else if(value=="file")

        // id and path e privilegio con cui apre
    {
        std::pair<std::string,std::string> idPriv= searchForPriv(nameSource,str,count);
        std::string id=idPriv.first;
        std::string initialPriv=idPriv.second;
        // I have to open the choosepriv first
        chooseprivWindow= new choosepriv(this,this->path,this->id,initialPriv);
        chooseprivWindow->show();
    }
    else
    {
        // it is a SymLink
        // TECNICAMENTE IO DOVREI TROVARE IL PATH E IL NOME ed inviarlo al DISPATCHER
        // path e nome che ce li ho.




    }
}

void directory::openSelectedSource(){
    // I have to distinguish if the selected item is a DOCUMENT, a FOLDER or a SYMLINK
    QList<QListWidgetItem*> selectedItem= ui->myListWidget->selectedItems();
    foreach(QListWidgetItem *items, selectedItem){
         QString value= items->whatsThis();
         std::string nameSource=items->text().toStdString();
         // dermine the path of the folders in which I enter.
         if(value=="directory")
         {
             std::string id=searchForId(nameSource,str,count);
             path+=id+'/'; // ok lo crea in modo corretto
             std::string pth=path; //solo per visualizzare che lo crea in modo corretto. Dopo si può cancellare
             aperto++;
             // open the folder Window
             // str1=cl->getStr(this->id, this->path);
             std::string str1="directory 7 Prova1\n file 9 Document1 owner\n symlink 10 symlink10 modify\n directory 1 Prova2\n directory 3 Prova3\n directory 4 Prova4\n directory 5 Prova5\n directory 6 Prova6\n directory 7 Prova7\n directory 8 Prova8\n";
             str=str1;

             this->openWindow(str);

         }

         else if(value=="file")

             // id and path e privilegio con cui apre
         {
             std::pair<std::string,std::string> idPriv= searchForPriv(nameSource,str,count);
             std::string id=idPriv.first;
             std::string initialPriv=idPriv.second;
             // I have to open the choosepriv first
             chooseprivWindow= new choosepriv(this,this->path,this->id,initialPriv);
             //cl->setchoosepriv(chooseprivWindow)
             chooseprivWindow->show();
         }
         else
         {
             // it is a SymLink
             // TECNICAMENTE IO DOVREI TROVARE IL PATH E IL NOME ed inviarlo al DISPATCHER
             // path e nome che ce li ho.




         }

 }// foreach

}


void directory::on_okButton_2_clicked()
{
     ui->renameLabel->hide();
     ui->renameName->hide();
     ui->okButton->hide();
     ui->okButton_2->hide();

}
