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
