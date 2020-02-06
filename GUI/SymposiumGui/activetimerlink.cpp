#include "activetimerlink.h"
#include "ui_activetimerlink.h"

activetimerlink::activetimerlink(QWidget *parent, std::string pathToFile) :
    QDialog(parent),
    ui(new Ui::activetimerlink), pathToFile(pathToFile)
{
    ui->setupUi(this);
    ui->time->setDateTime(QDateTime::currentDateTime());
    ui->writer->click();
    privilegeToGrant=Symposium::privilege::modify;

}

activetimerlink::~activetimerlink()
{
    delete ui;
}

void activetimerlink::on_ok_clicked()
{
    QDateTime date=ui->time->dateTime();
    std::string time;
    std::tm tm = {};
    std::chrono::system_clock::time_point endTime;
    time=date.toString().toStdString();
    std::stringstream ss(time);
    ss >> std::get_time(&tm, "%b %d %Y %H:%M:%S");
    endTime=std::chrono::system_clock::from_time_t(std::mktime(&tm));
    this->close();
    QMessageBox::information(parentWidget(),
                                    tr("Links"), "All links are active now until "+QString::fromStdString(time)+".\n This is the link: "+QString::fromStdString(pathToFile), QMessageBox::Ok);



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
