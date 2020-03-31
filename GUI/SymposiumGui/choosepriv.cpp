#include "choosepriv.h"
#include "ui_choosepriv.h"

choosepriv::choosepriv(QWidget *parent,std::string path, std::string idSource, std::string oldPriv) :
    QMainWindow(parent),
    path(path),idSource(idSource),oldPriv(oldPriv),ui(new Ui::choosepriv)
{
    ui->setupUi(this);
    QPixmap pix_writer(":/resources/cartelle/writer_icon");
    ui->writer_icon->setPixmap(pix_writer);

    QPixmap pix_reader(":/resources/cartelle/reader_icon");
    ui->reader_icon->setPixmap(pix_reader);

    QPixmap pix_owner(":/resources/cartelle/owner_icon");
    ui->owner_icon->setPixmap(pix_owner);

    ui->writer->click();

    if (oldPriv=="modify"){
        privOpen= Symposium::privilege::modify;
    }
    else if(oldPriv=="readOnly"){
        privOpen= Symposium::privilege::readOnly;
    }
    else{
        privOpen= Symposium::privilege::owner;
    }

}

choosepriv::~choosepriv()
{
    delete ui;
}

void choosepriv::on_pushButton_clicked()
{


    if(ui->writer->isEnabled())
        //privilege="writer";
        priv= Symposium::privilege::modify;
    else if(ui->reader->isEnabled())
        //privilege="reader";
        priv= Symposium::privilege::readOnly;
    else
        //privilege="owner";
        priv= Symposium::privilege::owner;


    // qui chiamo il metodo in cui gli passo il path e il privilege
     //openSource(this->path,priv,idSource);

    // QUESTO ANDRA' TOLTO DA QUI E LASCIATO SOLTANTO NEL successOpen()
    notepadWindow= new notepad(this,std::stol(this->idSource),priv,privOpen,path);
    this->hide();
    notepadWindow->show();
}

void choosepriv::on_pushButton_2_clicked()
{
    //directory *dirWindow=new directory(this);
    //dirWindow->show();
    this->hide();
}


// il successOpen riceve un documento, e stampa tutti i simboli che sono in esso presenti

notepad* choosepriv::successOpen(){

    notepadWindow= new notepad(this,std::stol(this->idSource),priv,privOpen,path);
    // cl->setNotepad(notepadWindow)
    this->hide();
    notepadWindow->show();
    return notepadWindow;



}

// failure(std::string stringaDaStampare)
void choosepriv::failureOpen(){
     QMessageBox::warning(this, "Error Message","Something has gone wrong");

}

//void choosepriv::errorConnection()-> per ora stampa un messaggio di errore.

