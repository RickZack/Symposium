#include "inserturi.h"
#include "ui_inserturi.h"
#include "directory.h"
#include "home.h"
#include "Dispatcher/clientdispatcher.h"

inserturi::inserturi(QWidget *parent, std::string pwd, Symposium::clientdispatcher *cl) :
    QDialog(parent),
    ui(new Ui::inserturi), cl(cl), pwd(pwd)
{
    ui->setupUi(this);
    setFixedSize(size());
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    //showDir=cl->showDir(true);
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

void inserturi::on_dir_clicked()
{
    disableStyleButtons();
    dirWindow=new choosedir(this, showDir);
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
        ui->dir->setText(QString::fromStdString(dirWindow->nameOfDir));
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

void inserturi::setClientDispatcher(Symposium::clientdispatcher *cl){
    this->cl = cl;
}

void inserturi::unsuccessInsert(std::string errorMess)
{
    hideLabelsError();
    enableStyleButtons();
    enableButtons();
    ui->linkError->setText(QString::fromStdString(errorMess));
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
                waiting();
                disableButtons();
                disableStyleButtons();
                //cl->openNewSource(pathLink, privilege, path, ui->name->text().toStdString());
            }
        }

    }
}


void inserturi::errorConnection()
{
    hideLabelsError();
    enableButtons();
    errorconnection errorWindow(this);
    int ret=errorWindow.exec();
    if(ret==0)
        enableStyleButtons();
}

void inserturi::errorConnectionLogout(std::string str)
{
    hideLabelsError();
    enableStyleButtons();
    enableButtons();
    errorlogout errorLog(this, QString::fromStdString(str));
    errorLog.setClientDispatcher(cl);
    this->hide();
    errorLog.show();
}

void inserturi::successInsert()
{
    hideLabelsError();
    enableButtons();
    ui->lineEdit->setText("");
    ui->name->setText("");
    notification notWindow(this, "Link has been successfully created!");
    int ret=notWindow.exec();
    if(ret==0)
        enableStyleButtons();
}

void inserturi::on_cancel_clicked()
{
    this->close();
}

void inserturi::enableButtonsAfter()
{
    enableStyleButtons();
}


