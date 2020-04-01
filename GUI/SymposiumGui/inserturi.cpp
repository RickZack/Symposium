#include "inserturi.h"
#include "ui_inserturi.h"
#include "dirwind.h"

inserturi::inserturi(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::inserturi)
{
    ui->setupUi(this);
    //showDir=cl->showDir(true);
    ui->writer->click();
    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(close()));
    ui->dirEmpty->hide();
    ui->linkError->hide();
    ui->nameError->hide();
    ui->formatError->hide();
    ui->waiting->hide();
    ui->gif->hide();
    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();
}

inserturi::~inserturi()
{
    delete ui;
}

void inserturi::on_dir_clicked()
{
    dirWindow=new choosedir(this);
    dirWindow->pathDir=showDir;
    dirWindow->exec();
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

void inserturi::closeEvent(QCloseEvent *event)
{
        parentWidget()->show();

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

void inserturi::unsuccessInsert(std::string errorMess)
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->linkError->setText(QString::fromStdString(errorMess));
}

void inserturi::on_add_clicked()
{
    ui->dirEmpty->hide();
    ui->linkError->hide();
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
            {
                ui->waiting->show();
                ui->gif->show();
                ui->add->setDisabled(true);
                ui->cancel->setDisabled(true);
                //cl->openNewSource(pathLink, privilege, path, ui->name->text().toStdString());
            }
        }

    }
}


void inserturi::errorConnection()
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->add->setDisabled(false);
    ui->cancel->setDisabled(false);
    errorWindow = new errorconnection(this);
    errorWindow->exec();
}

void inserturi::errorConnectionLogout(std::string str)
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->add->setDisabled(false);
    ui->cancel->setDisabled(false);
    errorLog = new errorlogout(this, QString::fromStdString(str));
    this->close();
    parentWidget()->close();
    errorLog->exec();
}

void inserturi::successInsert()
{
    ui->add->setDisabled(false);
    ui->cancel->setDisabled(false);
    ui->waiting->hide();
    ui->gif->hide();
    QMessageBox::information(parentWidget(),
                             tr("Insert Link"), tr("The operation was successfully done!"), QMessageBox::Ok);
}
