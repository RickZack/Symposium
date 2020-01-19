#include "choosedir.h"
#include "ui_choosedir.h"

choosedir::choosedir(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::choosedir)
{
    ui->setupUi(this);
    listGenerate("ciao");

    //ui->dir->setText("directory dir1\r\n  file file1 owner\r\n symlink  sym2 modify\r\n");
}

choosedir::~choosedir()
{
    delete ui;
}




void choosedir::listGenerate(QString str)
{
    for(int i=0; i<10; i++)
    {
        str=str+QString::number(i);
        QListWidgetItem *item=new QListWidgetItem(str);
        ui->list->addItem(item);
    }
}


void choosedir::on_list_itemClicked(QListWidgetItem *item)
{
    ui->nameDir->setText(item->text());
}

