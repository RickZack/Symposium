#include "inserturi.h"
#include "ui_inserturi.h"

inserturi::inserturi(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::inserturi)
{
    ui->setupUi(this);
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
    dirWindow->show();
}

void inserturi::change_text()
{
    path=dirWindow->path;
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
    privilege="reader";
}

void inserturi::on_writer_clicked()
{
    privilege="writer";
}

void inserturi::on_owner_clicked()
{
    privilege="owner";
}

