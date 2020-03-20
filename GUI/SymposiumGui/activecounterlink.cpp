#include "activecounterlink.h"
#include "ui_activecounterlink.h"

activecounterlink::activecounterlink(QWidget *parent, int documentId) :
    QDialog(parent),
    ui(new Ui::activecounterlink), documentId(documentId)
{
    ui->setupUi(this);
    ui->writer->click();
    privilegeToGrant=Symposium::privilege::modify;
    numCounter=0;
}

void activecounterlink::errorLink()
{
    this->close();
    QMessageBox::information(parentWidget(),
                                tr("Links"), "There are some errors.\n", QMessageBox::Ok);

}

void activecounterlink::successLink(std::string path)
{
    this->close();
    QMessageBox::information(parentWidget(),
                                    tr("Links"), "All links are active now for a limit of another "
                                 +QString::number(numCounter)+" shares.\n This is the link: "
                                 +QString::fromStdString(path), QMessageBox::Ok);
}

void activecounterlink::setClientDispatcher(Symposium::clientdispatcher *cl)
{
    this->cl = cl;
}

void activecounterlink::errorConnection()
{
    errorWindow = new errorconnection(this);
    errorWindow->show();
}

void activecounterlink::errorConnectionLogout()
{
    errorLog = new errorlogout(this);
    this->close();
    parentWidget()->close();
    parentWidget()->parentWidget()->close();
    errorLog->show();
}


activecounterlink::~activecounterlink()
{
    delete ui;
}

void activecounterlink::on_ok_clicked()
{
    numCounter=ui->counter->value();
    if(numCounter!=0)
    {
        u.activateCount(numCounter, privilegeToGrant);
        //cl->shareResource(documentId, u);
    }

    //--------------------------------------------PARTE DA CANCELLARE SUCCESSIVAMENTE
    this->close();
    QMessageBox::information(parentWidget(),
                                    tr("Links"), "All links are active now for a limit of another "
                                 +QString::number(numCounter)+" shares.\n This is the link: "
                                 +QString::fromStdString(pathToFile), QMessageBox::Ok);

    //----------------------------------------------------------------------------------

}

void activecounterlink::on_owner_clicked()
{
    privilegeToGrant=Symposium::privilege::owner;
}

void activecounterlink::on_writer_clicked()
{
    privilegeToGrant=Symposium::privilege::modify;
}

void activecounterlink::on_reader_clicked()
{
    privilegeToGrant=Symposium::privilege::readOnly;
}

void activecounterlink::on_cancel_clicked()
{
    this->close();
}
