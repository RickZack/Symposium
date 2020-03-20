#include "activenonlink.h"
#include "ui_activenonlink.h"

activenonlink::activenonlink(QWidget *parent, int documentId) :
    QDialog(parent),
    ui(new Ui::activenonlink), documentId(documentId)
{
    ui->setupUi(this);
}

void activenonlink::errorLink()
{
    this->close();
    QMessageBox::information(parentWidget(),
                                tr("Links"), "There are some errors.\n", QMessageBox::Ok);

}

void activenonlink::successLink(std::string path)
{
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
    errorWindow = new errorconnection(this);
    errorWindow->show();
}

void activenonlink::errorConnectionLogout()
{
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

void activenonlink::on_pushButton_2_clicked()
{
    this->close();
}

void activenonlink::on_pushButton_clicked()
{
    u.deactivate();
    //cl->shareResource(documentId, u);
}
