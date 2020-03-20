#include "activealwayslink.h"
#include "ui_activealwayslink.h"
#include "Dispatcher/clientdispatcher.h"

activealwayslink::activealwayslink(QWidget *parent, int documentId) :
    QDialog(parent),
    ui(new Ui::activealwayslink),  documentId(documentId)
{
    ui->setupUi(this);
    ui->writer->click();
    privilegeToGrant=Symposium::privilege::modify;
}

void activealwayslink::errorLink()
{
    this->close();
    QMessageBox::information(parentWidget(),
                                tr("Links"), "There are some errors.\n", QMessageBox::Ok);

}

void activealwayslink::successLink(std::string path)
{
    this->close();
    QMessageBox::information(parentWidget(),
                                tr("Links"), "All links are active now without any limits.\n This is the link: "+QString::fromStdString(path), QMessageBox::Ok);
}

void activealwayslink::setClientDispatcher(Symposium::clientdispatcher *cl)
{
    this->cl = cl;
}

void activealwayslink::errorConnection()
{
    errorWindow = new errorconnection(this);
    errorWindow->show();
}

void activealwayslink::errorConnectionLogout()
{
    errorLog = new errorlogout(this);
    this->close();
    parentWidget()->close();
    parentWidget()->parentWidget()->close();
    errorLog->show();
}

activealwayslink::~activealwayslink()
{
    delete ui;
}

void activealwayslink::on_owner_clicked()
{
    privilegeToGrant=Symposium::privilege::owner;
}

void activealwayslink::on_writer_clicked()
{
    privilegeToGrant=Symposium::privilege::modify;
}

void activealwayslink::on_reader_clicked()
{
    privilegeToGrant=Symposium::privilege::readOnly;
}

void activealwayslink::on_cancel_clicked()
{
    this->close();
}

void activealwayslink::on_ok_clicked()
{
    u=Symposium::uri();
    u.activateAlways(privilegeToGrant);
    //cl->shareResource(documentId, u);

    //--------------------------------------------PARTE DA CANCELLARE SUCCESSIVAMENTE
    this->close();
    QMessageBox::information(parentWidget(),
                                tr("Links"), "All links are active now without any limits.\n This is the link: "
                             +QString::fromStdString(pathToFile), QMessageBox::Ok);
    //---------------------------------------------------------------------------------------------------------------
}
