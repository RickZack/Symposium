#include "choosepriv.h"
#include "ui_choosepriv.h"
#include "directory.h"

choosepriv::choosepriv(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::choosepriv)
{
    ui->setupUi(this);
    QPixmap pix_writer(":/resources/cartelle/writer_icon");
    ui->writer_icon->setPixmap(pix_writer);

    QPixmap pix_reader(":/resources/cartelle/reader_icon");
    ui->reader_icon->setPixmap(pix_reader);

    QPixmap pix_owner(":/resources/cartelle/owner_icon");
    ui->owner_icon->setPixmap(pix_owner);

    ui->writer->click();

}

choosepriv::~choosepriv()
{
    delete ui;
}

void choosepriv::on_pushButton_clicked()
{

    notepadWindow= new notepad(this);
    notepadWindow->show();
    this->hide();
}

void choosepriv::on_pushButton_2_clicked()
{
    directory *dirWindow=new directory(this);
    dirWindow->show();
    this->hide();
}
