#include "activecounterlink.h"
#include "ui_activecounterlink.h"
#include "Dispatcher/clientdispatcher.h"
#include <QMovie>
#include "onoff_networkinteraction.h"

activecounterlink::activecounterlink(QWidget *parent, Symposium::uint_positive_cnt::type documentId, std::string pathFile, SymWinInterface& si) :
    QDialog(parent),
    SymModalWinInterface (si, isQDialog::isQDialogType(*this)),
    ui(new Ui::activecounterlink), pathFile(pathFile), documentId(documentId)
{
    ui->setupUi(this);
    setFixedSize(size());
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->writer->click();
    privilegeToGrant=Symposium::privilege::modify;
    numCounter=0;
    enableButtons();
    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();

    //--------------------------------------------PARTE DA CANCELLARE SUCCESSIVAMENTE
    this->pathFile="/1/2/3/4/5";
    //---------------------------------------------------------------------------------------------------------------
}

void activecounterlink::success()
{
    successLink();
}

void activecounterlink::failure(const QString &toPrint)
{
    if(toPrint=="-1"){
        errorConnectionLogout();
    }else{
        unsuccessLink(toPrint);
    }
}

void activecounterlink::unsuccessLink(const QString& errorMess)
{
    enableButtons();
    enableStyleButtons();
    //this->hide();
    ui->errorMess->setText(errorMess);
    ui->errorMess->show();
}

void activecounterlink::successLink()
{
    enableButtons();
    enableStyleButtons();
    this->hide();
    successlinks link(parentWidget(), 2, QString::fromStdString(pathFile), QString::number(numCounter), "");
    link.exec();
}

/*void activecounterlink::setClientDispatcher(Symposium::clientdispatcher *cl)
{
    this->cl = cl;
}*/

void activecounterlink::errorConnectionLogout()
{
    enableButtons();
    enableStyleButtons();
    this->hide();
    errorlogout errorLog(nullptr);
    parentWidget()->hide();
    parentWidget()->parentWidget()->hide();
    errorLog.show();
}


activecounterlink::~activecounterlink()
{
    delete ui;
}

void activecounterlink::enableButtonsAfter()
{
    enableStyleButtons();
}

void activecounterlink::on_ok_clicked()
{
    numCounter=static_cast<unsigned int>(ui->counter->value());
    if(numCounter!=0)
    {
        u.activateCount(numCounter, privilegeToGrant);
        #ifdef DISPATCHER_ON
        cl.shareResource(pathFile, std::to_string(documentId), u);
        #endif
        waiting();
        disableButtons();
        disableStyleButtons();
    }
    #ifndef DISPATCHER_ON
    //--------------------------------------------PARTE DA CANCELLARE SUCCESSIVAMENTE
    //this->hide();
    enableButtons();
    successlinks link(parentWidget(), 2, QString::fromStdString(pathFile), QString::number(numCounter));
    link.exec();

    //----------------------------------------------------------------------------------
    #endif
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

void activecounterlink::disableButtons()
{
    ui->cancel->setDisabled(true);
    ui->ok->setDisabled(true);
}

void activecounterlink::enableButtons()
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->cancel->setDisabled(false);
    ui->ok->setDisabled(false);
    ui->errorMess->hide();
}

void activecounterlink::enableStyleButtons()
{
    ui->ok->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->cancel->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 grey);color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
}

void activecounterlink::disableStyleButtons()
{
    ui->ok->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->cancel->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
}

void activecounterlink::waiting()
{
    ui->gif->show();
    ui->waiting->show();
    ui->errorMess->hide();
}
