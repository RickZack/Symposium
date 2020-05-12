#include "activealwayslink.h"
#include "ui_activealwayslink.h"
#include "Dispatcher/clientdispatcher.h"
#include <QMovie>
#include "onoff_networkinteraction.h"

activealwayslink::activealwayslink(QWidget *parent, Symposium::uint_positive_cnt::type documentId, std::string pathFile, Symposium::user us, SymWinInterface& si) :
    QDialog(parent),
    SymModalWinInterface (si, isQDialog::isQDialogType(*this)),
    ui(new Ui::activealwayslink),  documentId(documentId), pathFile(pathFile), us(us)
{
    ui->setupUi(this);
    setFixedSize(size());
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->writer->click();
    privilegeToGrant=Symposium::privilege::modify;
    enableButtons();
    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();

}

void activealwayslink::success()
{
    successLink();
}

void activealwayslink::failure(const QString &toPrint)
{
    if(toPrint=="-1"){
        errorConnectionLogout();
    }else{
        unsuccessLink(toPrint);
    }
}

void activealwayslink::unsuccessLink(const QString& errorMess)
{
    enableButtons();
    enableStyleButtons();
    this->hide();
    ui->errorMess->setText(errorMess);
    ui->errorMess->show();

}

void activealwayslink::successLink()
{
    enableButtons();
    enableStyleButtons();
    backToParent();
    successlinks link(parentWidget(), 1, QString::fromStdString(pathFile), 0, "", us, privilegeToGrant);
    link.exec();
}

void activealwayslink::errorConnectionLogout()
{
    errorlogout errorLog(nullptr);
    int ret=errorLog.exec();
    if(ret==0)
        backToMainWin();
}

activealwayslink::~activealwayslink()
{
    delete ui;
}

void activealwayslink::enableButtonsAfter()
{
    enableStyleButtons();
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
    backToParent();
}

void activealwayslink::on_ok_clicked()
{
    u=Symposium::uri();
    u.activateAlways(privilegeToGrant);
    waiting();
    disableButtons();
    disableStyleButtons();
    #ifdef DISPATCHER_ON
    cl.shareResource(pathFile, std::to_string(documentId), u);
    #else

    //--------------------------------------------PARTE DA CANCELLARE SUCCESSIVAMENTE
    successLink();
    //---------------------------------------------------------------------------------------------------------------
    #endif
}

void activealwayslink::disableButtons()
{
    ui->cancel->setDisabled(true);
    ui->ok->setDisabled(true);
}

void activealwayslink::enableButtons()
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->errorMess->hide();
    ui->cancel->setDisabled(false);
    ui->ok->setDisabled(false);
}

void activealwayslink::enableStyleButtons()
{
    ui->ok->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->cancel->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 grey);color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
}

void activealwayslink::disableStyleButtons()
{
    ui->ok->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->cancel->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
}

void activealwayslink::waiting()
{
    ui->waiting->show();
    ui->gif->show();
    ui->errorMess->hide();
}

void activealwayslink::closeEvent(QCloseEvent *event){
    if(closedByUser())
        backToParent();
    else
        event->accept();
}
