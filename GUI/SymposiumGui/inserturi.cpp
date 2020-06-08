#include "inserturi.h"
#include "ui_inserturi.h"
#include "directory.h"
#include "home.h"
#include "Dispatcher/clientdispatcher.h"
#include "onoff_networkinteraction.h"

inserturi::inserturi(QWidget *parent, std::string pwd, SymWinInterface& si) :
    QDialog(parent),
    SymModalWinInterface (si, isQDialog::isQDialogType(*this)),
    ui(new Ui::inserturi), pwd(pwd)
{
    ui->setupUi(this);
    setFixedSize(size());
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    #ifdef DISPATCHER_ON
    this->showDir=cl.showDir(true);
    this->showDir = manipulationPath(this->showDir);
    #endif

    ui->writer->click();
    QPixmap pix2(":/icon/logo.png");
    int w=ui->logo->width();
    int h=ui->logo->height();
    ui->logo->setPixmap(pix2.scaled(w, h, Qt::KeepAspectRatio));
    hideLabelsError();
    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();
}

inserturi::~inserturi()
{
    delete ui;
}

std::string inserturi::manipulationPath(std::string& s){
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
    while(dim_s != 0  && s!=" "){
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

void inserturi::success(){
    successInsert();
}

void inserturi::failure(const QString &toPrint){
    if(toPrint=="-1"){
        errorConnectionLogout();
    }else{
        unsuccessInsert(toPrint);
    }
}

void inserturi::on_dir_clicked()
{
    disableStyleButtons();
    dirWindow=new choosedir(this, this->showDir);
    int ret=dirWindow->exec();
    pathId=dirWindow->pathId;
    if(ret==0)
        enableStyleButtons();
}

void inserturi::change_text()
{
    path=dirWindow->pathId;
    nameDir=dirWindow->nameOfDir;
    if(!nameDir.empty())
    {
        size_t sizestr=dirWindow->nameOfDir.size();
        std::string text=dirWindow->nameOfDir;
        if(sizestr>40)
        {
            text.erase (text.begin()+40, text.end());
            text.append("...");
        }
        ui->dir->setText(QString::fromStdString(text));
    }
}

void inserturi::reset_text()
{
    path="";
    nameDir="";
    ui->dir->setText("choose the directory...");
}

void inserturi::disableButtons()
{
    ui->lineEdit->setReadOnly(true);
    ui->name->setReadOnly(true);
    ui->add->setDisabled(true);
    ui->cancel->setDisabled(true);
}

void inserturi::enableButtons()
{
    ui->lineEdit->setReadOnly(false);
    ui->name->setReadOnly(false);
    ui->add->setDisabled(false);
    ui->cancel->setDisabled(false);
}

void inserturi::enableStyleButtons()
{
    ui->add->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->cancel->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 grey);color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
}

void inserturi::disableStyleButtons()
{
    ui->add->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->cancel->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
}

void inserturi::hideLabelsError()
{
    ui->dirEmpty->hide();
    ui->linkError->hide();
    ui->nameError->hide();
    ui->formatError->hide();
    ui->waiting->hide();
    ui->gif->hide();
}

void inserturi::waiting()
{
    ui->waiting->show();
    ui->gif->show();
}

void inserturi::on_reader_clicked()
{
    privilege=Symposium::privilege::readOnly;
}

void inserturi::on_writer_clicked()
{
    privilege=Symposium::privilege::modify;
}

void inserturi::on_owner_clicked()
{
    privilege=Symposium::privilege::owner;
}

void inserturi::unsuccessInsert(const QString& errorMess)
{
    hideLabelsError();
    enableStyleButtons();
    enableButtons();
    ui->linkError->setText(errorMess);
    ui->linkError->show();
}

void inserturi::on_add_clicked()
{
    hideLabelsError();
    if(path=="")
    {
        ui->dirEmpty->show();
    }
    else if(ui->name->text()=="")
    {
        ui->nameError->show();
    }
    else
    {
        pathLink=ui->lineEdit->text().toStdString();
        std::size_t found = pathLink.find_last_of('/');
        if(found==std::string::npos)
            ui->formatError->show();
        else{
            resourceId=pathLink.substr(found+1);
            if(resourceId=="")
                ui->formatError->show();
            else
            {
                waiting();
                disableButtons();
                disableStyleButtons();
                title=ui->name->text();
                #ifdef DISPATCHER_ON
                std::string nameToSend=directory::fixNameSource(ui->name->text().toStdString());
                cl.openNewSource(pathLink, privilege, path, nameToSend);
                #endif
            }
        }

    }
}

void inserturi::errorConnectionLogout()
{
    errorlogout errorLog(this);
    int ret=errorLog.exec();
    if(ret==0)
        backToMainWin();
}

void inserturi::successInsert()
{
    hideLabelsError();
    enableButtons();
    ui->lineEdit->setText("");
    ui->name->setText("");

    //check if the privilege granted is the same requested by the user
    if(privilege == cl.getMyPrivilegeOnFileOpen()){
        notification notWindow(this, "Link has been successfully created!");
        int ret=notWindow.exec();
        if(ret==0)
        {
             notepad* notepadWindow= new notepad(nullptr,privilege,privilege,pathLink,cl.getOpenDocument(),cl.getOpenFileID(), *this, true);
             notepadWindow->setWindowTitle(title);
             goToWindow(*notepadWindow);
             notepadWindow->showLabels();
             if(privilege==Symposium::privilege::readOnly)
                 notepadWindow->setreadonly();
        }
    }else{
        privilege = cl.getMyPrivilegeOnFileOpen();
        notification notWindow(this, "You don't have permission to open the file with the selected privilege. The file was added with the highest privilege level assigned by the owner.");
        int ret=notWindow.exec();
        if(ret==0)
        {
             notepad* notepadWindow= new notepad(nullptr,privilege,privilege,pathLink,cl.getOpenDocument(),cl.getOpenFileID(), *this, true);
             notepadWindow->setWindowTitle(title);
             goToWindow(*notepadWindow);
             notepadWindow->showLabels();
             if(privilege==Symposium::privilege::readOnly)
                 notepadWindow->setreadonly();
        }
    }


}

void inserturi::on_cancel_clicked()
{
    backToParent();
}

void inserturi::closeEvent(QCloseEvent *event){
    if(closedByUser())
        backToParent();
    else
        event->accept();
}

void inserturi::enableButtonsAfter()
{
    enableStyleButtons();
}


