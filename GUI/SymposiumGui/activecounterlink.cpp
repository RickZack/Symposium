#include "activecounterlink.h"
#include "ui_activecounterlink.h"

activecounterlink::activecounterlink(QWidget *parent, std::string pathToFile) :
    QDialog(parent),
    ui(new Ui::activecounterlink), pathToFile(pathToFile)
{
    ui->setupUi(this);
    ui->writer->click();
    privilegeToGrant=Symposium::privilege::modify;
}

activecounterlink::~activecounterlink()
{
    delete ui;
}

void activecounterlink::on_ok_clicked()
{
    int i=ui->counter->value();
    this->close();
    if(i==5)
    {
        QMessageBox::information(parentWidget(),
                                    tr("Links"), "All links are active now for a limit of another "+QString::number(i)+" shares.\n This is the link: "+QString::fromStdString(pathToFile), QMessageBox::Ok);
    }

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
