#include "choosepriv.h"
#include "ui_choosepriv.h"

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

    if(ui->writer->isEnabled())
        //privilege="writer";
        Symposium::privilege priv= Symposium::privilege::modify;
    else if(ui->reader->isEnabled())
        //privilege="reader";
        Symposium::privilege priv= Symposium::privilege::readOnly;
    else
        //privilege="owner";
        Symposium::privilege priv= Symposium::privilege::owner;


    // qui chiamo il metodo in cui gli passo il path e il privilege
     //openSource(this->path,priv,idSource);

    // QUESTO ANDRA' TOLTO DA QUI E LASCIATO SOLTANTO NEL successOpen()
    notepadWindow= new notepad(this);
    notepadWindow->setId(this->idSource);
    notepadWindow->setIdDoc(this->idSource);
    this->hide();
    notepadWindow->show();
}

void choosepriv::on_pushButton_2_clicked()
{
    //directory *dirWindow=new directory(this);
    //dirWindow->show();
    this->hide();
}

void choosepriv::getPath(std::string path,std::string idSource)
{
    this->path=path;
    this->idSource=idSource;
    this->show();
}


void choosepriv::successOpen(){

    notepadWindow= new notepad(this);
    notepadWindow->show();
    notepadWindow->setId(this->idSource);
    notepadWindow->setIdDoc(this->idSource);
    this->hide();

}

void choosepriv::failure(){
     QMessageBox::warning(this, "Error Message","Something has gone wrong");

}
