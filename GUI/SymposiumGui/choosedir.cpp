#include "choosedir.h"
#include "ui_choosedir.h"

choosedir::choosedir(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::choosedir)
{
    ui->setupUi(this);
    buttonGenerate("ciao");
    //ui->dir->setText("directory dir1\r\n  file file1 owner\r\n symlink  sym2 modify\r\n");
}

choosedir::~choosedir()
{
    delete ui;
}




void choosedir::buttonGenerate(QString str)
{
    for(int i=0; i<2; i++)
    {
        str=str+QString::number(i);
        prova = new QPushButton(str);
        prova->setFlat(true);
        prova->setStyleSheet("background-color: white; text-align: left; border: none;");
        prova->setObjectName(str);
        ui->dirs->addWidget(prova);
        buttons[i]=prova;
        connect(buttons[i], SIGNAL(clicked()), this, SLOT(choose()));
    }
}

void choosedir::choose()
{
    for(int i=0; i<2; i++)
        buttons[i]->setStyleSheet("background-color: white; text-align: left; border: none;");
    prova=qobject_cast<QPushButton*>(sender());
    prova->setStyleSheet("background-color: blue; color: white; text-align: left; border: none;");
    QString testo=prova->objectName();
    ui->nameDir->setText(testo);
}

