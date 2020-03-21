#include "activenonlink.h"
#include "ui_activenonlink.h"

activenonlink::activenonlink(QWidget *parent, int documentId, std::string pathFile) :
    QDialog(parent),
    ui(new Ui::activenonlink), pathFile(pathFile), documentId(documentId)
{
    ui->setupUi(this);
    ui->waiting->hide();
    ui->gif->hide();
    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();
}

void activenonlink::unsuccessLink(std::string errorMess)
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

void activenonlink::successLink(std::string path)
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->cancel->setDisabled(false);
    ui->ok->setDisabled(false);
    this->close();
    QMessageBox::information(parentWidget(),
                                    tr("Links"), "All links have been deactivate now.\n", QMessageBox::Ok);
}

void activenonlink::setClientDispatcher(Symposium::clientdispatcher *cl)
{
    this->cl = cl;
}

void activenonlink::errorConnection()
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->cancel->setDisabled(false);
    ui->ok->setDisabled(false);
    errorWindow = new errorconnection(this);
    errorWindow->show();
}

void activenonlink::errorConnectionLogout()
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

activenonlink::~activenonlink()
{
    delete ui;
}

void activenonlink::on_ok_clicked()
{
    ui->waiting->show();
    ui->gif->show();
    ui->cancel->setDisabled(true);
    ui->ok->setDisabled(true);
    u.deactivate();
    //cl->shareResource(pathFile, documentId, u);
}

void activenonlink::on_cancel_clicked()
{
    this->close();
}
