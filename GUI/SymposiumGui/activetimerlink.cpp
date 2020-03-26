#include "activetimerlink.h"
#include "ui_activetimerlink.h"

activetimerlink::activetimerlink(QWidget *parent, Symposium::uint_positive_cnt::type documentId, std::string pathFile) :
    QDialog(parent),
    ui(new Ui::activetimerlink), pathFile(pathFile), documentId(documentId)
{
    ui->setupUi(this);
    ui->time->setDateTime(QDateTime::currentDateTime());
    ui->writer->click();
    privilegeToGrant=Symposium::privilege::modify;
    ui->waiting->hide();
    ui->gif->hide();
    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();

}

void activetimerlink::unsuccessLink(std::string errorMess)
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

void activetimerlink::successLink(std::string path)
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->cancel->setDisabled(false);
    ui->ok->setDisabled(false);
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
    ui->waiting->hide();
    ui->gif->hide();
    ui->cancel->setDisabled(false);
    ui->ok->setDisabled(false);
    errorWindow = new errorconnection(this);
    errorWindow->show();
}

void activetimerlink::errorConnectionLogout(std::string str)
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
    ui->waiting->show();
    ui->gif->show();
    ui->cancel->setDisabled(true);
    ui->ok->setDisabled(true);
    //cl->shareResource(pathFile, documentId, u);

    //--------------------------------------------PARTE DA CANCELLARE SUCCESSIVAMENTE
    this->close();
    QMessageBox::information(parentWidget(),
                                    tr("Links"), "All links are active now until "+QString::fromStdString(time)
                             +".\n This is the link: "+QString::fromStdString(pathFile), QMessageBox::Ok);
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
