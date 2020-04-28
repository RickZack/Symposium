//#define DISPATCHER_ON
#include "choosepriv.h"
#include "Dispatcher/clientdispatcher.h"
#include "ui_choosepriv.h"

choosepriv::choosepriv(QWidget *parent,std::string path, std::string idSource, std::string oldPriv, Symposium::clientdispatcher *cl) :
    QMainWindow(parent),
    path(path), idSource(idSource), oldPriv(oldPriv), ui(new Ui::choosepriv), cl(cl)
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

void choosepriv::on_pushButton_clicked(){
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
    #ifdef DISPATCHER_ON
    cl->openSource(this->path,this->idSource,this->priv);
    #else
    notepadWindow= new notepad(this,std::stol(this->idSource),priv,privOpen,path);
    notepadWindow->show();
    notepadWindow->showLabels();
    #endif
    this->hide();
}

void choosepriv::on_pushButton_2_clicked()
{
    this->hide();
}


// il successOpen riceve un documento, e stampa tutti i simboli che sono in esso presenti

notepad* choosepriv::successOpen(Symposium::document &doc){
    notepadWindow= new notepad(this,std::stol(this->idSource),priv,privOpen,path,doc);
    this->hide();
    notepadWindow->show();
    notepadWindow->showLabels();
    return notepadWindow;
}

// DA CANCELLARE!
// failure(std::string stringaDaStampare)
void choosepriv::failureOpen(){
     QMessageBox::warning(this, "Error Message","Something has gone wrong");

}

//void choosepriv::errorConnection()-> per ora stampa un messaggio di errore.

