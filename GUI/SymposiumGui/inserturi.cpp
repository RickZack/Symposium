#include "inserturi.h"
#include "ui_inserturi.h"
#include "directory.h"

inserturi::inserturi(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::inserturi)
{
    ui->setupUi(this);
    //showDir=cl->showDir(true);
    ui->writer->click();
    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(close()));
    ui->dirError->hide();
    ui->dirEmpty->hide();
    ui->linkError->hide();
    ui->privError->hide();
    ui->nameError->hide();
    ui->formatError->hide();
}

inserturi::~inserturi()
{
    delete ui;
}

void inserturi::on_dir_clicked()
{
    dirWindow=new choosedir(this);
    dirWindow->pathDir=showDir;
    dirWindow->show();
    pathId=dirWindow->pathId;
}

void inserturi::change_text()
{
    path=dirWindow->pathId;
    nameDir=dirWindow->nameOfDir;
    if(!nameDir.empty())
        ui->dir->setText(QString::fromStdString(dirWindow->nameOfDir));
}

void inserturi::reset_text()
{
    path="";
    nameDir="";
    ui->dir->setText("choose the directory...");
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

void inserturi::setClientDispatcher(Symposium::clientdispatcher *cl){
    this->cl = cl;
}

void inserturi::invalidUri()
{
    ui->linkError->show();
}

void inserturi::privilegeTooBig()
{
    ui->privError->show();
}

void inserturi::DirectoryFileNotExist()
{
    //showDir=cl->showDir(true);
    ui->dirError->show();
}

void inserturi::on_add_clicked()
{
    ui->dirError->hide();
    ui->dirEmpty->hide();
    ui->linkError->hide();
    ui->privError->hide();
    ui->nameError->hide();
    ui->formatError->hide();
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
        std::string pathLink=ui->lineEdit->text().toStdString();
        std::size_t found = pathLink.find_last_of('/');
        if(found==std::string::npos)
            ui->formatError->show();
        else{
            std::string resourceId=pathLink.substr(found+1);
            if(resourceId=="")
                ui->formatError->show();
            else
                ;
            //cl->openNewSource(const std::string &resourceId, privilege, path, ui->name->text().toStdString());
        }

    }
}


void inserturi::errorConnection()
{
    errorWindow = new errorconnection(this);
    errorWindow->show();
}

void inserturi::errorConnectionLogout()
{
    errorLog = new errorlogout(this);
    this->close();
    parentWidget()->close();
    errorLog->show();
}

void inserturi::successInsert()
{
    QMessageBox::information(parentWidget(),
                             tr("Insert Link"), tr("The operation was successfully done!"), QMessageBox::Ok);
}
