#include "activetimerlink.h"
#include "ui_activetimerlink.h"
#include "Dispatcher/clientdispatcher.h"

activetimerlink::activetimerlink(QWidget *parent, Symposium::uint_positive_cnt::type fileId, std::string pathFile, std::string pathAssoluto, Symposium::user us, SymWinInterface& si) :
    QDialog(parent),
    SymModalWinInterface (si, isQDialog::isQDialogType(*this)),
    ui(new Ui::activetimerlink), pathFile(pathFile), pathAssoluto(pathAssoluto), fileId(fileId), us(us)
{
    ui->setupUi(this);
    setFixedSize(size());
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QDateTime dateNow=QDateTime::currentDateTime();

    ui->time->setDateTime(dateNow);
    ui->time->setMinimumDateTime(dateNow);
    ui->writer->click();
    privilegeToGrant=Symposium::privilege::modify;
    enableButtons();
    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();
}

void activetimerlink::success()
{
    successLink();
}

void activetimerlink::failure(const QString &toPrint)
{
    if(toPrint=="-1"){
        errorConnectionLogout();
    }else{
        unsuccessLink(toPrint);
    }
}


void activetimerlink::unsuccessLink(const QString& errorMess)
{
    enableButtons();
    enableStyleButtons();
    ui->errorMess->setText(errorMess);
    ui->errorMess->show();

}

void activetimerlink::successLink()
{
    enableButtons();
    enableStyleButtons();
    backToParent();
    successlinks link(parentWidget(), 4, QString::fromStdString(pathAssoluto), "", time, us, privilegeToGrant);
    link.exec();
}

void activetimerlink::errorConnectionLogout()
{
    enableButtons();
    errorlogout errorLog(nullptr);
    int ret=errorLog.exec();
    if(ret==0)
        backToMainWin();
}


activetimerlink::~activetimerlink()
{
    delete ui;
}

void activetimerlink::on_ok_clicked()
{
    QDateTime date=ui->time->dateTime();
    time=date.toString();

    std::chrono::milliseconds fromEpoch{date.toMSecsSinceEpoch()};
    std::chrono::system_clock::time_point endTime{fromEpoch};

    u.activateTimer(endTime, privilegeToGrant);
    waiting();
    disableButtons();
    disableStyleButtons();
    cl.shareResource(pathFile, std::to_string(fileId), u);
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
    backToParent();
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

void activetimerlink::closeEvent(QCloseEvent *event)
{
    if(closedByUser())
        backToParent();
    else
        event->accept();
}
