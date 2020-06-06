#include "directory.h"
#include "ui_directory.h"
#include "home.h"
#include "Dispatcher/clientdispatcher.h"
#include <ostream>
#include "onoff_networkinteraction.h"


directory::directory(QWidget *parent, std::string pwd, SymWinInterface& si) :
    QMainWindow(parent),
    SymChildWinInterface (si, isQWidget::isQwidgetType(*this)),
    ui(new Ui::directory), pwd(pwd)
{
    ui->setupUi(this);
    this->actualId = "0";
    this->previousId = "0";
    this->openFolders = 0;

    // METODO DISPATCHER CHE RESTITUISCE LA STRINGA
    #ifdef DISPATCHER_ON
    list=cl.showHome();
    path = "./";
    list = manipulationPath(list);
    this->populateMap(list);
    #else
    list="directory 1 Folder1\n file 9 Folder1 owner\n symlink 10 symlink10 modify\n directory 1 Folder2\n directory 3 Folder3\n directory 4 Folder4\n directory 5 Folder5\n directory 6 Folder6\n directory 7 Folder7\n directory 8 Folder8\n";
    this->populateMap(list);
    #endif
    this->count=number_elements(list);
    listGenerate(list, count);

    ui->back_button->setDisabled(true);
    ui->back_button->hide();
    ui->writerButton->click();

    ui->actionHome->setIcon(QIcon(":/icon/home.png"));

    hideAll();
    setAttribute( Qt::WA_DeleteOnClose );
    ui->pathPlainEdit->setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);

    setFixedSize(size());
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->myListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->myListWidget, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(ProvideContextMenu(const QPoint &)));
    ui->toolBar->setContextMenuPolicy(Qt::PreventContextMenu);
}


void directory::populateMap(std::string list){
    unsigned count=this->number_elements(list);
    std::string type,id,name;
       std::string privilege="none";
       for(size_t i=0;i<count;i++){
           type=separate_word(list);
           id=separate_word(list);
           name = separate_word(list);
           if(type=="directory") {
               auto it=name.find('\n');
               name.erase(it);
           }else {
               privilege = separate_word(list);
               auto it = privilege.find('\n');
               privilege.erase(it);
           }
           ids.insert({name,{id,privilege}});
           privilege="none";
    }

       for(auto i:ids)
               qDebug()<<"Mappa "<<QString::fromStdString(i.first)<<"Nome "<<QString::fromStdString(i.second.first)<<"Id "<<QString::fromStdString(i.second.second)<<"Privilegio ";
}



std::string directory::manipulationPath(std::string& s){

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
    while(dim_s != 0 && s!=" "){
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
    qDebug()<<"Stringa finale"<<QString::fromStdString(result);
    return result;
}

directory::~directory()
{
    delete ui;
}

void directory::success(){
    emit closeWaiting();
    enableStyleButtons();
    hideAll();
    pressed=false;
    switch (lastChoice) {
    case createFolder:{
        successCreate();
        break;
    }case remove:{
        successRemove();
        break;
    }case rename:{
        successRename();
        break;
    }case createNewSource:{
        successNewSource();
        break;
    }case openSource:{
        successOpen();
        break;
    }case openSymlink:{
        successOpen();
        break;
    }default:
        break;
    }
    lastChoice=noAction;
}

void directory::failure(const QString& toPrint){
    enableStyleButtons();
    //if(this->lastChoice != createNewSource)
    emit closeWaiting();
    pressed=false;
    if(lastChoice==openSymlink){
        handleFailureSymlink();
    }else if(lastChoice==openSource){
        QMessageBox::critical(this,tr("Error Message"),tr("This source has been deleted. It will not appear on your FileSystem starting from your next login"));
    }
    if(toPrint=="-1"){
        errorConnectionLogout();
    }else{
        failureActionDirectory(toPrint);
    }
}

void directory::handleFailureSymlink(){
    QMessageBox::critical(this,tr("Error Message"),tr("This source has been deleted by its owner. You are nomore allowed to use it. It will be deleted from your personal FileSystem"));
    //if(QMessageBox::Ok)
    lastChoice = remove;
    disableStyleButtons();
    pressed=true;
    // estract from the map the id of the source that I want to remove
    auto it=this->ids.find(this->fixedName);
    id=it->second.first;
    waitingFunction();
    std::string pathToSend=path;
    if(pathToSend!="./")
    {
        std::size_t found = pathToSend.find_last_of("/");
        pathToSend.erase(found, pathToSend.size());
    }
    cl.removeResource(pathToSend,id);

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
            // substitute the non-printable character with the space one
            for(size_t i=0;i<word.size();i++){
                if(word[i]==0x1F)
                   word.replace(i,1,1,' ');
            }

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
            for(size_t i=0;i<word.size();i++){
                if(word[i]==0x1F)
                   word.replace(i,1,1,' ');

            }
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
            for(size_t i=0;i<word.size();i++){
                if(word[i]==0x1F)
                   word.replace(i,1,1,' ');
            }

            QListWidgetItem *item=new QListWidgetItem(QIcon(":/icon/link.png"), QString::fromStdString(word));
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

std::string directory::generateString(std::string& string)
{
    for(size_t i = 0; i < string.size(); i++)
      if(string[i] == '\n')
          string.erase(std::remove(string.begin(), string.end(), '\n'), string.end());
    return string;
}

int directory::number_elements(const std::string& string)
{
    int count=0;
    for(size_t i = 0; i < string.size(); i++)
      if(string[i] == '\n')
          count++;
    return count;
}

void directory::on_actionHome_triggered()
{
    backToParent();
}

void directory::on_actionUri_triggered()
{
    hideAll();
    inserturi* uriWindow=new inserturi(nullptr, pwd, *this);
    goToWindow(*uriWindow);
}

void directory::on_actionaddFolder_triggered()
{
    hideAll();
    showNewDirectory();
}

void directory::on_actionaddfile_triggered()
{
    hideAll();
    showNewFile();
}


void directory::openWindow(const std::string& str1){
    ui->myListWidget->clear();
    int counter=number_elements(str1);
    listGenerate(str1,counter);
    ui->back_button->setDisabled(false);
    ui->back_button->show();
}

void directory::ProvideContextMenu(const QPoint &pos)
{
    QPoint item = ui->myListWidget->mapToGlobal(pos);

    QMenu submenu;
    QString style="QMenu {border-radius:15px; background-color: white;margin: 2px; border: 1px solid rgb(58, 80, 116); color:  rgb(58, 80, 116);}QMenu::separator {height: 2px;background: rgb(58, 80, 116);margin-left: 10px;margin-right: 5px;}";
    submenu.setStyleSheet(style);
    submenu.addAction(tr("Open"),this,&directory::openSelectedSource);
    submenu.addSeparator();
    submenu.addAction(tr("Delete"),this,&directory::deleteSource);
    submenu.addSeparator();
    submenu.addAction(tr("Rename"),this,&directory::renameSource);

    QListWidgetItem* itemAtPos=ui->myListWidget->itemAt(pos);
    if(itemAtPos!=nullptr)
        submenu.exec(item);
}

//acts when the user clicks on the button "DELETE"
void directory::deleteSource()
{
    // dove ho eliminato
   lastChoice = remove;
   disableStyleButtons();
   pressed=true;
   std::string id;
   QList<QListWidgetItem*> item= ui->myListWidget->selectedItems();
   foreach(QListWidgetItem *items, item){
       nameSource=fixNameSource(items->text().toStdString());
       // estract from the map the id of the source that I want to remove
       auto it=this->ids.find(nameSource);
       id=it->second.first;
       waitingFunction();
       #ifdef DISPATCHER_ON
       std::string pathToSend=path;
       if(pathToSend!="./")
       {
           std::size_t found = pathToSend.find_last_of("/");
           pathToSend.erase(found, pathToSend.size());
       }
       cl.removeResource(pathToSend,id);
       #endif

       //-------------------------------------------------------------------
       //DA RIMUOVERE
       #ifndef DISPATCHER_ON
       enableStyleButtons();
       emit closeWaiting();
       pressed=false;
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
    // update the map
    auto it=this->ids.find(nameSource);
    this->ids.erase(it);
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

void directory::failureActionDirectory(const QString& msg){
    ui->errorMess->setText(msg);
    ui->errorMess->show();
}

// this method creates a new folder
void directory::on_pushButton_3_clicked()
{
    lastChoice = createFolder;
    disableStyleButtons();
    pressed=true;
    QString name= ui->name->text();
    waitingFunction();
    std::string nameFolder=name.toStdString();                       /** < name folder with possible spaces */
    if(nameFolder==" "){
        this->failureActionDirectory("The inserted name is not correct. Please try again");
    }
    this->fixedName=this->fixNameSource(name.toStdString());   /** < name folder without possible spaces */
    #ifdef DISPATCHER_ON
    int numVal=this->ids.count(fixedName);
    std::string pathToSend=path;
    if(pathToSend!="./")
    {
        std::size_t found = pathToSend.find_last_of("/");
        pathToSend.erase(found, pathToSend.size());
    }
    if(numVal==0){
        cl.createNewDir(pathToSend,fixedName);
    }else
        this->failureActionDirectory("You already have an element with this name");
    #else
    // anche questo da eliminare
    ui->name->setText(" ");
    // std::string id=createNewDir(path,nameFolder);
    //--------------------------------------------------------------------
    // DA ELIMINARE
    enableStyleButtons();
    pressed=false;
    emit closeWaiting();
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

void directory::successCreate(){
    std::string temp;
    std::size_t found;

    QString name= ui->name->text();
    ui->name->clear();
    count++;
    if(this->openFolders == 0){
        this->list = cl.showHome();
    }else{
        temp = this->path;
        temp.erase(temp.end()-1);
        found=temp.find_last_of("/");
        temp.erase(found+1,temp.size());
        this->list = cl.getStr(this->actualId, temp);
    }

    this->list = manipulationPath(this->list);
    ui->myListWidget->clear();
    int count=number_elements(list);
    listGenerate(list,count);

    // update the map
    this->ids.clear();
    this->populateMap(this->list);

}


//this method closes the window directory
void directory::closeEvent(QCloseEvent *event)
{
    event->ignore();
    if(closedByUser())
    {
        disableStyleButtons();
        class exit ex(this, true, &cl);
        int ret=ex.exec();
        if(ret==0 && !pressed)
            enableStyleButtons();
    }
    else
        event->accept();

}

void directory::enableStyleButtons()
{
    switch (lastChoice) {
    case createFolder:{
        //directory buttons
        ui->pushButton_3->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
        ui->okButton_3->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 grey);color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
        break;}
    case createNewSource:{
        //file buttons
        ui->pushButton_4->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
        ui->okButton_4->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 grey);color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
        break;}
    case rename:{
        //rename buttons
        ui->okButton->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
        ui->okButton_2->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 grey);color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
        break;}
    case openSource:{
        //open buttons
        ui->OkPriv->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
        ui->cancPriv->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 grey);color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
        break;}
    case remove:{
        break;}
    }
}

void directory::disableStyleButtons()
{
    switch (lastChoice) {
    case createFolder:{
        //directory buttons
        ui->pushButton_3->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
        ui->okButton_3->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
        break;}
    case createNewSource:{
        //file buttons
        ui->pushButton_4->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
        ui->okButton_4->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
        break;}
    case rename:{
        //rename buttons
        ui->okButton->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
        ui->okButton_2->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
        break;}
    case openSource:{
        //open buttons
        ui->OkPriv->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
        ui->cancPriv->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
        break;}
    case remove:{
        break;}
    default:
        break;
    }
}


void directory::waitingFunction()
{
   hideAll();
   class waiting w(this);
   w.move(this->window()->frameGeometry().topLeft()+this->window()->rect().center()-w.rect().center());
   QObject::connect(this, SIGNAL(closeWaiting()), &w, SLOT(close()));
   w.show();

}

void directory::hideAll()
{
    //New folder hide
    ui->label->hide();
    ui->label_3->hide();
    ui->label_2->hide();
    ui->name->hide();
    ui->name->setReadOnly(true);
    ui->name->clear();
    ui->pushButton_3->hide();
    ui->pushButton_3->setDisabled(true);
    ui->okButton_3->hide();
    ui->okButton_3->setDisabled(true);
    ui->label_8->hide();

    //New document hide
    ui->label_4->hide();
    ui->label_5->hide();
    ui->label_6->hide();
    ui->name_2->hide();
    ui->name_2->setReadOnly(true);
    ui->name_2->clear();
    ui->pushButton_4->setDisabled(true);
    ui->pushButton_4->hide();
    ui->okButton_4->setDisabled(true);
    ui->okButton_4->hide();
    ui->label_7->hide();

    //Rename hide
    ui->renameName->hide();
    ui->renameLabel->setReadOnly(true);
    ui->renameLabel->clear();
    ui->renameLabel->hide();
    ui->okButton->hide();
    ui->okButton->setDisabled(true);
    ui->okButton_2->hide();
    ui->okButton_2->setDisabled(true);

    //Resize directory view window
    ui->myListWidget->setFixedWidth(692);

    //The privilege buttons
    hidePrivilegeButtons();

    ui->errorMess->hide();

}

void directory::showNewDirectory()
{
    ui->label->show();
    ui->label_3->show();
    ui->label_2->show();
    ui->name->show();
    ui->name->setReadOnly(false);
    ui->pushButton_3->setDisabled(false);
    ui->pushButton_3->show();
    ui->okButton_3->show();
    ui->okButton_3->setDisabled(false);
    ui->label_8->show();

    //Resize directory view window
    ui->myListWidget->setFixedWidth(270);

    lastChoice=createFolder;
}

void directory::showNewFile()
{
    ui->label_4->show();
    ui->label_5->show();
    ui->label_6->show();
    ui->name_2->show();
    ui->name_2->setReadOnly(false);
    ui->pushButton_4->setDisabled(false);
    ui->okButton_4->show();
    ui->okButton_4->setDisabled(false);
    ui->pushButton_4->show();
    ui->label_7->show();

    //Resize directory view window
    ui->myListWidget->setFixedWidth(270);

    lastChoice=createNewSource;
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

    //Resize directory view window
    ui->myListWidget->setFixedWidth(270);

    lastChoice=rename;
}

// creates a new file
void directory::on_pushButton_4_clicked()
{
    lastChoice = createNewSource;
    disableStyleButtons();
    pressed=true;
    this->curResName =ui->name_2->text();                                                              /** < name with possible spaces */
    if(curResName.toStdString()==" "){
        this->failureActionDirectory("The inserted name is not correct. Please try again");
    }
    std::string fixedName=this->fixNameSource(curResName.toStdString());           /** < name without possible spaces */
    waitingFunction();
    #ifdef DISPATCHER_ON
    int numVal=this->ids.count(fixedName);
    std::string pathToSend=path;
    if(pathToSend!="./")
    {
        std::size_t found = pathToSend.find_last_of("/");
        pathToSend.erase(found, pathToSend.size());
    }
    if(numVal==0)
        cl.createNewSource(pathToSend,fixedName);
    else
        this->failureActionDirectory("You already have an element with this name");
    #else
    // DA RIMUOVERE
    emit closeWaiting();
    enableStyleButtons();
    pressed=false;
    std::string id="1"; //PER ESEMPIO
    if(id!="-1")
         // OK dal dispatcher, posso creare il nuovo documento
    {
        count++;
        std::string new_str=" file "+id+' '+curResName.toStdString()+'\n';
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

notepad* directory::successNewSource(){

    QString name= ui->name_2->text();
    ui->name_2->clear();

    std::string pathToSend=path;
    if(pathToSend!="./")
    {
        std::size_t found = pathToSend.find_last_of("/");
        pathToSend.erase(found, pathToSend.size());
    }

    //open the newly created document
    notepad* nw= new notepad(nullptr,Symposium::privilege::owner,Symposium::privilege::owner,pathToSend,cl.getOpenDocument(), cl.getOpenFileID(), *this);
    nw->setWindowTitle(curResName);
    goToWindow(*nw);
    nw->showLabels();
    //To make notepad's title change when renaming the file
    QObject::connect(this, SIGNAL(resNameChanged(Symposium::uint_positive_cnt::type, const QString&)), nw, SLOT(modifyWinTitle(Symposium::uint_positive_cnt::type, const QString&)));
    return nw;
}


void directory::on_back_button_clicked()
{
    std::string temp;
    std::size_t found;
    hideAll();

    if(this->openFolders!=0){

        if(this->openFolders == 1){
            ui->pathPlainEdit->setPlainText(QString::fromStdString("Path:home/"));
        }else{
            std::string pathLabel=ui->pathPlainEdit->toPlainText().toStdString();
            pathLabel.erase(pathLabel.end()-1);
            found=pathLabel.find_last_of("/");
            pathLabel.erase(found+1,pathLabel.size());
            ui->pathPlainEdit->setPlainText(QString::fromStdString(pathLabel));
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
            list=cl.showHome();
            list = manipulationPath(list);
            this->ids.clear();
            this->populateMap(list);
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
            list=cl.getStr(this->actualId, temp);
            list = manipulationPath(list);
            this->ids.clear();
            this->populateMap(list);
        }
        ui->myListWidget->clear();
        this->count=number_elements(list);
        listGenerate(list,count);
        #endif
    }else{
        std::string pathLabel=ui->pathPlainEdit->toPlainText().toStdString();
        pathLabel.erase(5,pathLabel.size());
        ui->pathPlainEdit->setPlainText(QString::fromStdString(pathLabel));
        ui->back_button->setDisabled(true);
        ui->back_button->hide();
    }
}

void directory::renameSource()
{
    hideAll();
    showRename();
}

void directory::on_okButton_clicked()
{
    lastChoice = rename;
    disableStyleButtons();
    pressed=true;
    this->curResName =ui->renameLabel->text();                              /** < name with possible spaces */
    if(curResName.toStdString()==" "){
        this->failureActionDirectory("The inserted name is not correct. Please try again");
    }
    fixedName=this->fixNameSource(curResName.toStdString());                /** < name without possible spaces */
    QList<QListWidgetItem*> selectedItem= ui->myListWidget->selectedItems();
    foreach(QListWidgetItem *items, selectedItem){
         std:: string oldName=items->text().toStdString();
         fixedOldName= this->fixNameSource(oldName);
         auto it=this->ids.find(fixedOldName);
         std::string id=it->second.first;
         curResId=std::stoul(id);
         waitingFunction();
         #ifdef DISPATCHER_ON
         int numVal=this->ids.count(fixedName);
         std::string pathToSend=path;
         if(pathToSend!="./")
         {
             std::size_t found = pathToSend.find_last_of("/");
             pathToSend.erase(found, pathToSend.size());
         }
         if(numVal==0)
            cl.renameResource(pathToSend, id, fixedName);
         else
             this->failureActionDirectory("You already have an element with this name");
         #else
         enableStyleButtons();
         emit closeWaiting();
         pressed=false;
         items->setText(curResName);
         ui->myListWidget->currentItem()->setSelected(false);
         hideAll();
         #endif
    }

}

void directory::successRename(){
    hideAll();
    // update the map
    auto it=this->ids.find(fixedOldName);
    std::string id=it->second.first;
    std::string priv=it->second.second;
    this->ids.erase(it);
    this->ids.insert({fixedName,{id,priv}});

    //update the list in such a way that the new name of directory appears
    if(this->actualId=="0"){
        list = cl.showHome();
        list = manipulationPath(list);
    }else{
        std::string temp = path;
        temp.erase(temp.end()-1);
        temp.erase(temp.find_last_of("/")+1,temp.size());
        list=cl.getStr(this->actualId, temp);
        list = manipulationPath(list);
    }

    ui->myListWidget->clear();
    int count=number_elements(list);
    listGenerate(list,count);
    //make the right notepad window's title change
    emit resNameChanged(curResId, curResName);
}

void directory::errorConnectionLogout(){
    errorlogout errorLog(nullptr);
    int ret=errorLog.exec();
    if(ret==0)
        backToMainWin();
}


void directory::on_myListWidget_itemDoubleClicked()
{
    openSelectedSource();
}

void directory::openSelectedSource(){
    hideAll();
    // I have to distinguish if the selected item is a DOCUMENT, a FOLDER or a SYMLINK
    QList<QListWidgetItem*> selectedItem= ui->myListWidget->selectedItems();
    foreach(QListWidgetItem *items, selectedItem){
         this->value= items->whatsThis();
         this->nameSource=items->text().toStdString();
         this->fixedName=this->fixNameSource(nameSource);
         // dermine the path of the folders in which I enter.
         if(value=="directory")
         {

             this->previousId=this->actualId;
             //auto it=this->ids.find(nameSource);
             auto it=this->ids.find(fixedName);
             this->actualId=it->second.first;
             this->openFolders++;

             // open the folder Window
             #ifdef DISPATCHER_ON
             list=cl.getStr(this->actualId, this->path);
             list = manipulationPath(list);
             this->ids.clear();
             this->populateMap(list);
             path+=actualId+'/';
             #else
             std::string str1="directory 7 Prova1\n file 9 Document1 owner\n symlink 10 symlink10 modify\n directory 1 Prova2\n directory 3 Prova3\n directory 4 Prova4\n directory 5 Prova5\n directory 6 Prova6\n directory 7 Prova7\n directory 8 Prova8\n";
             this->ids.clear();
             this->populateMap(str1);
             #endif
             ui->pathPlainEdit->setPlainText(ui->pathPlainEdit->toPlainText()+QString::fromStdString(nameSource)+'/');
             this->openWindow(list);

         }else{
              ui->myListWidget->setFixedWidth(270);
              auto it=this->ids.find(fixedName);
              this->selectedId=it->second.first;
              this->initialPriv=it->second.second;
              curResName=QString::fromStdString(nameSource);

              // The user has to choose the privilege:
              this->showPrivilegeButtons();
              // set the old Privilege
              if (this->initialPriv=="writer")
                  priv= Symposium::privilege::modify;
              else if(this->initialPriv=="reader")
                  priv= Symposium::privilege::readOnly;
              else
                  priv= Symposium::privilege::owner;
         }

 }// foreach

}

void directory::showPrivilegeButtons(){
    ui->OkPriv->show();
    ui->cancPriv->show();
    ui->privilegeLine->show();
    ui->writerButton->show();
    ui->readerButton->show();
    //lastChoice=openSource;
    if(this->value=="file")
        lastChoice=openSource;
    else
        lastChoice=openSymlink;

}

void directory::on_okButton_2_clicked()
{
     hideAll();

}

void directory::on_OkPriv_clicked()
{
    //lastChoice = openSource;
    if(this->value=="file")
        lastChoice=openSource;
    else {
        lastChoice=openSymlink;
    }
    disableStyleButtons();
    pressed=true;
    if(ui->writerButton->isChecked())
        privOpen= Symposium::privilege::modify;
    else if(ui->readerButton->isChecked())
        privOpen= Symposium::privilege::readOnly;
    waitingFunction();
    #ifdef DISPATCHER_ON
    std::string pathToSend=path;
    if(pathToSend!="./")
    {
        std::size_t found = pathToSend.find_last_of("/");
        pathToSend.erase(found, pathToSend.size());
    }
    cl.openSource(pathToSend,this->selectedId,privOpen);
    #else
    priv=Symposium::privilege::owner;
    hideAll();
    enableStyleButtons();
    pressed=false;
    emit closeWaiting();
    Symposium::document d;
    notepad* notepadWindow= new notepad(nullptr,priv,privOpen,path,d,0, *this);
    notepadWindow->setWindowTitle(curResName);
    goToWindow(*notepadWindow);
    notepadWindow->showLabels();
    if(privOpen==Symposium::privilege::readOnly)
        notepadWindow->setreadonly();
    #endif
}

void directory::successOpen(){
    std::string pathToSend=path;
    if(pathToSend!="./")
    {
        std::size_t found = pathToSend.find_last_of("/");
        pathToSend.erase(found, pathToSend.size());
    }
    notepad* notepadWindow= new notepad(nullptr,priv,privOpen,pathToSend,cl.getOpenDocument(), cl.getOpenFileID(), *this);
    //TODO: sarebbe meglio passare al costruttore le informazioni al costruttore e nella classe notepad chiamare le varie
    // setTitle, showLabels, ecc..
    notepadWindow->setWindowTitle(curResName);
    goToWindow(*notepadWindow);
    notepadWindow->showLabels();
    //TODO: ad esempio la setreadonly potrebbe essere chiamata da notepad, ed essere quinid un afunzione privata
    if(privOpen==Symposium::privilege::readOnly)
        notepadWindow->setreadonly();
    //To make notepad's title change when renaming the file
    QObject::connect(this, SIGNAL(resNameChanged(Symposium::uint_positive_cnt::type, const QString&)), notepadWindow, SLOT(modifyWinTitle(Symposium::uint_positive_cnt::type, const QString&)));
}

void directory::enableButtonsAfter()
{
    enableStyleButtons();
}

void directory::on_cancPriv_clicked()
{
    hideAll();
}

void directory::hidePrivilegeButtons(){
    ui->privilegeLine->hide();
    ui->writerButton->hide();
    ui->readerButton->hide();
    ui->OkPriv->hide();
    ui->cancPriv->hide();
}

void directory::showEvent(QShowEvent *event)
{
    QWidget::showEvent( event );
    std::string temp;
    std::size_t found;
    if(this->openFolders == 0){
        this->list = cl.showHome();
    }else{
        temp = this->path;
        temp.erase(temp.end()-1);
        found=temp.find_last_of("/");
        temp.erase(found+1,temp.size());
        this->list = cl.getStr(this->actualId, temp);
    }
    this->list = manipulationPath(this->list);
    ui->myListWidget->clear();
    this->count=number_elements(list);
    listGenerate(list,count);
    this->populateMap(list);
}

void directory::on_okButton_3_clicked()
{
    hideAll();
}

void directory::on_okButton_4_clicked()
{
    hideAll();
}

std::string directory::fixNameSource(const std::string nameSource){
    std::string newName=nameSource;
    for(size_t i=0;i<nameSource.size();i++){
        if(nameSource[i]==' '){
            newName.replace(i,1,1,0x1F);
        }
    }
    return newName;
}



