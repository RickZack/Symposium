#include "activetimerlink.h"
#include "ui_activetimerlink.h"

activetimerlink::activetimerlink(QWidget *parent, int documentId) :
    QDialog(parent),
    ui(new Ui::activetimerlink), documentId(documentId)
{
    ui->setupUi(this);
    ui->time->setDateTime(QDateTime::currentDateTime());
    ui->writer->click();
    privilegeToGrant=Symposium::privilege::modify;

}

void activetimerlink::errorLink()
{
    this->close();
    QMessageBox::information(parentWidget(),
                                tr("Links"), "There are some errors.\n", QMessageBox::Ok);

}

void activetimerlink::successLink(std::string path)
{
    this->close();
    QMessageBox::information(parentWidget(),
                                    tr("Links"), "All links are active now until "
                             +QString::fromStdString(time)+".\n This is the link: "
                             +QString::fromStdString(path), QMessageBox::Ok);
}

void activetimerlink::setClientDispatcher(Symposium::clientdispatcher *cl)
{
    this->cl = cl;
}

void activetimerlink::errorConnection()
{
    errorWindow = new errorconnection(this);
    errorWindow->show();
}

void activetimerlink::errorConnectionLogout()
{
    errorLog = new errorlogout(this);
    this->close();
    parentWidget()->close();
    parentWidget()->parentWidget()->close();
    errorLog->show();
}


activetimerlink::~activetimerlink()
{
    delete ui;
}

void activetimerlink::on_ok_clicked()
{
    QDateTime date=ui->time->dateTime();
    std::tm tm = {};
    std::chrono::system_clock::time_point endTime;
    time=date.toString().toStdString();
    std::stringstream ss(time);
    ss >> std::get_time(&tm, "%b %d %Y %H:%M:%S");
    endTime=std::chrono::system_clock::from_time_t(std::mktime(&tm));

    u.activateTimer(endTime, privilegeToGrant);
    //cl->shareResource(documentId, u);

    //--------------------------------------------PARTE DA CANCELLARE SUCCESSIVAMENTE
    this->close();
    QMessageBox::information(parentWidget(),
                                    tr("Links"), "All links are active now until "+QString::fromStdString(time)
                             +".\n This is the link: "+QString::fromStdString(pathToFile), QMessageBox::Ok);
    //----------------------------------------------------------------------------------------------------



}

void activetimerlink::on_owner_clicked()
{
    privilegeToGrant=Symposium::privilege::owner;
}

void activetimerlink::on_writer_clicked()
{
    privilegeToGrant=Symposium::privilege::modify;
}

void activetimerlink::on_reader_clicked()
{
    privilegeToGrant=Symposium::privilege::readOnly;
}

void activetimerlink::on_cancel_clicked()
{
    this->close();
}
