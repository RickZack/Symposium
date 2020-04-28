//#define DISPATCHER_ON

#include "directory.h"
#include "ui_directory.h"
#include "home.h"
#include "Dispatcher/clientdispatcher.h"
#include <ostream>


directory::directory(QWidget *parent, std::string pwd, Symposium::clientdispatcher *cl) :
    QMainWindow(parent),
    ui(new Ui::directory), cl(cl), pwd(pwd)
{
    ui->setupUi(this);
    this->actualId = "0";
    this->previousId = "0";
    this->openFolders = 0;
    // METODO DISPATCHER CHE RESTITUISCE LA STRINGA
    #ifdef DISPATCHER_ON
    str=cl->showHome();
    path = "./";
    qDebug() << "prima della manipolazione str: " << QString::fromStdString(str);
    str = manipulationHome(str);
    #else
    str="directory 1 Folder1\n file 9 Folder1 owner\n symlink 10 symlink10 modify\n directory 1 Folder2\n directory 3 Folder3\n directory 4 Folder4\n directory 5 Folder5\n directory 6 Folder6\n directory 7 Folder7\n directory 8 Folder8\n";
    #endif
    this->count=number_elements(str);
    qDebug() << "Numero elementi: " << QString::fromStdString(str);
    listGenerate(str, count);

    ui->back_button->setDisabled(true);
    ui->back_button->hide();

    ui->renameLabel->hide();
    ui->okButton->hide();
    ui->renameName->hide();
    ui->okButton_2->hide();

    // set the buttons for the privilege disabled
    // they will appear when I choose to open a document
    this->hidePrivilegeButtons();


    ui->actionHome->setIcon(QIcon(":/icon/home.png"));
    ui->back_button->setIcon(QIcon(":/resources/cartelle/back_icon"));
    //hideAll();
}

std::string directory::manipulationHome(std::string& s){
    std::string result;
    std::string estratta;
    std::size_t found;
    std::size_t foundn = 0;
    std::size_t dim_s = s.size();
    //delete first username and space
    found = s.find_first_of(" ");
    s.erase(0, found+1);
    //check the first character is space yet
    while(s.substr(0,1) == " "){
        s.erase(0,1);
    }
    dim_s = s.size();
    //check if there is no directory
    if(dim_s == 0)
        return s;
    //add space between directory
    while(dim_s != 0){
        foundn = s.find_first_of("\n");
        estratta = s.substr(0,foundn+1);
        //delete start space
        while(estratta.substr(0,1) == " "){
            estratta.erase(0,1);
        }
        //delete final space
        while(estratta.substr(estratta.size()-1,1) == " "){
            estratta.erase(estratta.size()-1,1);
        }
        result = result +  estratta + " ";
        s.erase(0,foundn+1);
        dim_s = s.size();
    }
    //delete final space
    found = result.find_last_of(" ");
    dim_s = result.size();
    result.erase(found, dim_s);
    return result;
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
            QListWidgetItem *item=new QListWidgetItem(QIcon(":/icon/folder.png"), QString::fromStdString(word));
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
            QListWidgetItem *item=new QListWidgetItem(QIcon(":/icon/file.png"), QString::fromStdString(word));
            v.setValue(-1);
            item->setData(Qt::UserRole,v);
            item->setWhatsThis("file");
            ui->myListWidget->addItem(item);
            word=separate_word(str);
        }
        else
        {
            word=separate_word(str);
            QListWidgetItem *item=new QListWidgetItem(QIcon(":/icon/link.png"), QString::fromStdString(word));
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

    idPriv=std::make_pair(id_to_return, priv_to_return);
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
    home *homeWindow=new home(nullptr, pwd);
    homeWindow->setClientDispatcher(cl);
    #ifdef DISPATCHER_ON
    cl->setHome(homeWindow);
    #endif
    homeWindow->show();
    this->hide();
}

void directory::on_actionUri_triggered()
{
    uriWindow=new inserturi(nullptr, pwd, false);
    uriWindow->show();
    this->hide();
}

void directory::on_actionaddFolder_triggered()
{
    showNewDirectory();
}

void directory::on_actionaddfile_triggered()
{
    showNewFile();
}


void directory::openWindow(std::string str1){
    ui->myListWidget->clear();
    int counter=number_elements(str1);
    listGenerate(str1,counter);
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
       #ifdef DISPATCHER_ON
       cl->removeResource(path,id);
       #endif

       //-------------------------------------------------------------------
       //DA RIMUOVERE
       #ifndef DISPATCHER_ON
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
       #endif
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
    #ifdef DISPATCHER_ON
    cl->createNewDir(path,nameFolder);
    #else
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
    #endif
}

void directory::successCreate(std::string id){
    QString name= ui->name->text();
    ui->name->clear();
    count++;
    std::string new_str;
    if(str.empty()){new_str="directory "+id+' '+name.toStdString()+'\n';}
            else{new_str=" directory "+id+' '+name.toStdString()+'\n';}
    //qDebug() << "new_str: " << QString::fromStdString(new_str) << " name: " << name << " " << QString::fromStdString(str);
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

void directory::disableButtons()
{

}

void directory::enableButtons()
{

}

void directory::enableStyleButtons()
{

}

void directory::disableStyleButtons()
{

}

void directory::waiting()
{

}

void directory::hideAll()
{
    //New folder hide
    ui->label->hide();
    ui->label_3->hide();
    ui->label_2->hide();
    ui->name->hide();
    ui->name->setReadOnly(true);
    ui->pushButton_3->hide();
    ui->pushButton_3->setDisabled(true);
    ui->label_8->hide();

    //New document hide
    ui->label_4->hide();
    ui->label_5->hide();
    ui->label_6->hide();
    ui->name_2->hide();
    ui->name_2->setReadOnly(true);
    ui->pushButton_4->setDisabled(true);
    ui->pushButton_4->hide();
    ui->label_7->hide();

    //Rename hide
    ui->renameName->hide();
    ui->renameLabel->setReadOnly(true);
    ui->renameLabel->hide();
    ui->okButton->hide();
    ui->okButton->setDisabled(true);
    ui->okButton_2->hide();
    ui->okButton_2->setDisabled(true);

    //Resize directory view window
    ui->myListWidget->setFixedWidth(500);

}

void directory::showNewDirectory()
{
    ui->label->show();
    ui->label_3->show();
    ui->label_2->show();
    ui->name->show();
    ui->name->setReadOnly(false);
    ui->pushButton_3->setDisabled(false);
    ui->label_8->show();
}

void directory::showNewFile()
{
    ui->label_4->show();
    ui->label_5->show();
    ui->label_6->show();
    ui->name_2->show();
    ui->name_2->setReadOnly(false);
    ui->pushButton_4->setDisabled(false);
    ui->pushButton_4->show();
    ui->label_7->show();
}

void directory::showRename()
{
    ui->renameName->show();
    ui->renameLabel->setReadOnly(false);
    ui->renameLabel->show();
    ui->okButton->show();
    ui->okButton->setDisabled(false);
    ui->okButton_2->show();
    ui->okButton_2->setDisabled(false);
}

// creates a new file
void directory::on_pushButton_4_clicked()
{

    QString name= ui->name_2->text();
    std::string nameDocument=name.toStdString();

    //ui->name_2->setText(" "); // da rimuovere
    #ifdef DISPATCHER_ON
    cl->createNewSource(this->path,nameDocument);
    #else
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
    #endif
}

notepad* directory::successNewSource(std::string id, Symposium::document &doc){
    QString name= ui->name_2->text();
    ui->name_2->clear();
    count++;
    if(str.empty()){
        str = "file "+id+' '+name.toStdString()+' '+"owner"+'\n';
    }else{
        str = str + " file "+id+' '+name.toStdString()+' '+"owner"+'\n';
    }
    ui->myListWidget->clear();
    int count=number_elements(str);
    listGenerate(str,count);
    //open the newly created document
    notepad* nw= new notepad(this,std::stol(id),Symposium::privilege::owner,Symposium::privilege::owner,path,doc);
    nw->show();
    nw->showLabels();
    return nw;
}


void directory::on_back_button_clicked()
{
    std::string temp;
    std::size_t found;

    if(this->openFolders!=0){

        if(this->openFolders == 1){
            ui->pathLabel->setText(QString::fromStdString("Path:"));
        }else{
            std::string pathLabel=ui->pathLabel->text().toStdString();
            pathLabel.erase(pathLabel.end()-1);
            found=pathLabel.find_last_of("/");
            pathLabel.erase(found+1,pathLabel.size());
            ui->pathLabel->setText(QString::fromStdString(pathLabel));
        }
        #ifdef DISPATCHER_ON
        //aggiorniamo il path
        //togliamo l'ultimo "/"
        path.erase(path.end()-1);
        //troviamo la posizione dell'ultimo "/"
        found=path.find_last_of("/");
        //aggiorniamo il path
        path.erase(found+1,path.size());
        this->openFolders--;
        if(this->openFolders==0){
            actualId = "0";
            previousId = "0";
            str=cl->showHome();
            str = manipulationHome(str);
            ui->back_button->hide();
        }else{
            actualId = previousId;
            temp = path;
            temp.erase(temp.end()-1);
            found=temp.find_last_of("/");
            temp.erase(found,temp.size());
            if(temp.size()==1){
                previousId = "0";
            }else{
                found=temp.find_last_of("/");
                previousId = temp.substr(found+1,temp.size());
            }
            temp = temp + "/";
            str=cl->getStr(this->actualId, temp);
            str = manipulationHome(str);
        }
        ui->myListWidget->clear();
        int count=number_elements(str);
        listGenerate(str,count);
        #endif
    }else{
        std::string pathLabel=ui->pathLabel->text().toStdString();
        pathLabel.erase(5,pathLabel.size());
        ui->pathLabel->setText(QString::fromStdString(pathLabel));
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
         #ifdef DISPATCHER_ON
         cl->renameResource(this->path,id, newName.toStdString());
         #else
         items->setText(newName);
         ui->myListWidget->currentItem()->setSelected(false);
         ui->renameLabel->clear();
         ui->okButton->hide();
         ui->renameLabel->hide();
         ui->renameName->hide();
         ui->okButton_2->hide();
         #endif
    }

}

void directory::successRename(){
    QString newName=ui->renameLabel->text();
    ui->renameLabel->clear();
    QList<QListWidgetItem*> selectedItem= ui->myListWidget->selectedItems();
    foreach(QListWidgetItem *items, selectedItem){
        items->setText(newName);
        ui->myListWidget->currentItem()->setSelected(false);
        ui->renameLabel->clear();
        ui->okButton->hide();
        ui->renameLabel->hide();
        ui->renameName->hide();
        ui->okButton_2->hide();
    }
    //aggiorniamo la stringa del contenuto in modo che ci sia il nuovo nome della directory
    if(this->actualId=="0"){
        str = cl->showHome();
        str = manipulationHome(str);
    }else{
        std::string temp = path;
        temp.erase(temp.end()-1);
        temp.erase(temp.find_last_of("/")+1,temp.size());
        str=cl->getStr(this->actualId, temp);
        str = manipulationHome(str);
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

void directory::errorConnectionLogout(std::string str){
    errorLogoutWindow= new errorlogout(this, QString::fromStdString(str));
    this->close();
    errorLogoutWindow->show();

}


void directory::on_myListWidget_itemDoubleClicked()
{
    this->openSelectedSource();
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
             this->previousId=this->actualId;
             this->actualId=searchForId(nameSource,str,count);
             this->openFolders++;
             // open the folder Window
             #ifdef DISPATCHER_ON
             str=cl->getStr(this->actualId, this->path);
             str = manipulationHome(str);
             path+=actualId+'/';
             #else
             std::string str1="directory 7 Prova1\n file 9 Document1 owner\n symlink 10 symlink10 modify\n directory 1 Prova2\n directory 3 Prova3\n directory 4 Prova4\n directory 5 Prova5\n directory 6 Prova6\n directory 7 Prova7\n directory 8 Prova8\n";
             #endif
             ui->pathLabel->setText(ui->pathLabel->text()+QString::fromStdString(nameSource)+'/');
             this->openWindow(str);

         }else if(value=="file")

             // id and path e privilegio con cui apre
         {
             std::pair<std::string,std::string> idPriv= searchForPriv(nameSource,str,count);
             std::string id=idPriv.first;
             this->initialPriv=idPriv.second;
             // The user has to choose the privilege:
             this->showPrivilegeButtons();
             // set the old Privilege
             if (this->initialPriv=="modify"){
                 privOpen= Symposium::privilege::modify;
             }
             else if(this->initialPriv=="readOnly"){
                 privOpen= Symposium::privilege::readOnly;
             }
             else{
                 privOpen= Symposium::privilege::owner;
             }


             // I have to open the choosepriv first
             //chooseprivWindow = new choosepriv(this, this->path, id, initialPriv, this->cl);
             #ifdef DISPATCHER_ON
             //cl->setchoosepriv(chooseprivWindow);
             #endif
             //chooseprivWindow->show();
         }else{
             // it is a SymLink
             // TECNICAMENTE IO DOVREI TROVARE IL PATH E IL NOME ed inviarlo al DISPATCHER
             // path e nome che ce li ho.

             std::pair<std::string,std::string> idPriv= searchForPriv(nameSource,str,count);
             std::string id=idPriv.first;
             std::string initialPriv=idPriv.second;
             // I have to open the choosepriv first
             chooseprivWindow= new choosepriv(this,this->path,this->id,initialPriv);
             //cl->setchoosepriv(chooseprivWindow)
             chooseprivWindow->show();

         }
    qDebug() << "str: " << QString::fromStdString(str) << " path: " << QString::fromStdString(path);
 }// foreach

}

void directory::showPrivilegeButtons(){
    ui->OkPriv->show();
    ui->cancPriv->show();
    ui->privilegeLine->show();
    ui->writerButton->show();
    ui->readerButton->show();
    ui->ownerButton->show();

}
void directory::on_okButton_2_clicked()
{
     ui->renameLabel->hide();
     ui->renameName->hide();
     ui->okButton->hide();
     ui->okButton_2->hide();

}

void directory::on_OkPriv_clicked()
{
    if(ui->writerButton->isEnabled())
        priv= Symposium::privilege::modify;
    else if(ui->readerButton->isEnabled())
        priv= Symposium::privilege::readOnly;
    else
        priv= Symposium::privilege::owner;

    #ifdef DISPATCHER_ON
    cl->openSource(this->path,this->id,this->priv);
    #else
    notepadWindow= new notepad(this,std::stol(this->id),priv,privOpen,path);
    notepadWindow->show();
    notepadWindow->showLabels();
    #endif

    this->hidePrivilegeButtons();
}

notepad* directory::successOpen(Symposium::document &doc){
    notepadWindow= new notepad(this,std::stol(this->id),priv,privOpen,path,doc);
    this->hide();
    notepadWindow->show();
    notepadWindow->showLabels();
    return notepadWindow;
}

void directory::on_cancPriv_clicked()
{
    this->hidePrivilegeButtons();
}

void directory::hidePrivilegeButtons(){
    ui->privilegeLine->hide();
    ui->writerButton->hide();
    ui->readerButton->hide();
    ui->ownerButton->hide();
    ui->OkPriv->hide();
    ui->cancPriv->hide();
}
