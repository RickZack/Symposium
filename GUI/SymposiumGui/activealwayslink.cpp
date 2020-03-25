#include "activealwayslink.h"
#include "ui_activealwayslink.h"
#include "Dispatcher/clientdispatcher.h"
#include <QMovie>

activealwayslink::activealwayslink(QWidget *parent, int documentId, std::string pathFile) :
    QDialog(parent),
    ui(new Ui::activealwayslink),  documentId(documentId), pathFile(pathFile)
{
    ui->setupUi(this);
    ui->writer->click();
    privilegeToGrant=Symposium::privilege::modify;
    ui->waiting->hide();
    ui->gif->hide();
    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();
}

void activealwayslink::unsuccessLink(std::string errorMess)
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

void activealwayslink::successLink(std::string path)
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->cancel->setDisabled(false);
    ui->ok->setDisabled(false);
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
    ui->waiting->hide();
    ui->gif->hide();
    ui->cancel->setDisabled(false);
    ui->ok->setDisabled(false);
    errorWindow = new errorconnection(this);
    errorWindow->show();
}

void activealwayslink::errorConnectionLogout(std::string str)
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->cancel->setDisabled(false);
    ui->ok->setDisabled(false);
    errorLog = new errorlogout(this, QString::fromStdString(str));
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
    ui->waiting->show();
    ui->gif->show();
    ui->cancel->setDisabled(true);
    ui->ok->setDisabled(true);
    //cl->shareResource(pathFile, documentId, u);

    //--------------------------------------------PARTE DA CANCELLARE SUCCESSIVAMENTE
    this->close();
    QMessageBox::information(parentWidget(),
                                tr("Links"), "All links are active now without any limits.\n This is the link: "
                             +QString::fromStdString(pathToFile), QMessageBox::Ok);
    //---------------------------------------------------------------------------------------------------------------
}
