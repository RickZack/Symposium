#include "activealwayslink.h"
#include "ui_activealwayslink.h"

activealwayslink::activealwayslink(QWidget *parent,  std::string pathToFile) :
    QDialog(parent),
    ui(new Ui::activealwayslink),  pathToFile(pathToFile)
{
    ui->setupUi(this);
    ui->writer->click();
    privilegeToGrant=Symposium::privilege::modify;
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
    this->close();
    QMessageBox::information(parentWidget(),
                                tr("Links"), "All links are active now without any limits.\n This is the link: "+QString::fromStdString(pathToFile), QMessageBox::Ok);
}
