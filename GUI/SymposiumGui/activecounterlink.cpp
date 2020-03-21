#include "activecounterlink.h"
#include "ui_activecounterlink.h"
#include <QMovie>

activecounterlink::activecounterlink(QWidget *parent, int documentId) :
    QDialog(parent),
    ui(new Ui::activecounterlink), documentId(documentId)
{
    ui->setupUi(this);
    ui->writer->click();
    privilegeToGrant=Symposium::privilege::modify;
    numCounter=0;
    ui->waiting->hide();
    ui->gif->hide();
    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();
}

void activecounterlink::unsuccessLink(std::string errorMess)
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->cancel->setDisabled(false);
    ui->ok->setDisabled(false);
    this->close();
    QString error=QString::fromStdString(errorMess);
    QMessageBox::information(parentWidget(),
                             tr("Modify Privilege"), "ERROR: "+error, QMessageBox::Ok);
}

void activecounterlink::successLink(std::string path)
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->cancel->setDisabled(false);
    ui->ok->setDisabled(false);
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
    ui->waiting->hide();
    ui->gif->hide();
    ui->cancel->setDisabled(false);
    ui->ok->setDisabled(false);
    errorWindow = new errorconnection(this);
    errorWindow->show();
}

void activecounterlink::errorConnectionLogout()
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->cancel->setDisabled(false);
    ui->ok->setDisabled(false);
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
        ui->waiting->show();
        ui->gif->show();
        ui->cancel->setDisabled(true);
        ui->ok->setDisabled(true);
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
