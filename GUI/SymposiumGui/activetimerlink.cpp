#include "activetimerlink.h"
#include "ui_activetimerlink.h"

activetimerlink::activetimerlink(QWidget *parent, Symposium::uint_positive_cnt::type documentId, std::string pathFile) :
    QDialog(parent),
    ui(new Ui::activetimerlink), pathFile(pathFile), documentId(documentId)
{
    ui->setupUi(this);
    setFixedSize(size());
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->time->setDateTime(QDateTime::currentDateTime());
    ui->writer->click();
    privilegeToGrant=Symposium::privilege::modify;
    enableButtons();
    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();

    //--------------------------------------------PARTE DA CANCELLARE SUCCESSIVAMENTE
    this->pathFile="1/2/3/4/5";
    //----------------------------------------------------------------------------------------------------
}

void activetimerlink::unsuccessLink(std::string errorMess)
{
    enableButtons();
    enableStyleButtons();
    QString error=QString::fromStdString(errorMess);
    ui->errorMess->setText(error);
    ui->errorMess->show();

}

void activetimerlink::successLink(std::string path)
{
    enableButtons();
    enableStyleButtons();
    this->hide();
    successlinks link(parentWidget(), 4, QString::fromStdString(path), "", QString::fromStdString(time));
    link.exec();
}

void activetimerlink::setClientDispatcher(Symposium::clientdispatcher *cl)
{
    this->cl = cl;
}

void activetimerlink::errorConnectionLogout()
{
    enableButtons();
    enableStyleButtons();
    this->hide();
    errorlogout errorLog(nullptr);
    parentWidget()->hide();
    parentWidget()->parentWidget()->hide();
    errorLog.show();
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
    waiting();
    disableButtons();
    disableStyleButtons();
    //cl->shareResource(pathFile, documentId, u);

    //--------------------------------------------PARTE DA CANCELLARE SUCCESSIVAMENTE
    successLink(pathFile);
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

void activetimerlink::enableButtonsAfter()
{
    enableStyleButtons();
}

void activetimerlink::disableButtons()
{
    ui->cancel->setDisabled(true);
    ui->ok->setDisabled(true);
}

void activetimerlink::enableButtons()
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->errorMess->hide();
    ui->cancel->setDisabled(false);
    ui->ok->setDisabled(false);
}

void activetimerlink::enableStyleButtons()
{
    ui->ok->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->cancel->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 grey);color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
}

void activetimerlink::disableStyleButtons()
{
    ui->ok->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->cancel->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
}

void activetimerlink::waiting()
{
    ui->waiting->show();
    ui->gif->show();
    ui->errorMess->hide();
}
