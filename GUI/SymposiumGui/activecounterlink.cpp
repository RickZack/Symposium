#include "activecounterlink.h"
#include "ui_activecounterlink.h"
#include <QMovie>

activecounterlink::activecounterlink(QWidget *parent, Symposium::uint_positive_cnt::type documentId, std::string pathFile) :
    QDialog(parent),
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

void activecounterlink::unsuccessLink(std::string errorMess)
{
    enableButtons();
    enableStyleButtons();
    this->hide();
    QString error=QString::fromStdString(errorMess);
    ui->errorMess->setText(error);
    ui->errorMess->show();
}

void activecounterlink::successLink(std::string path)
{
    enableButtons();
    enableStyleButtons();
    this->hide();
    link = new successlinks(parentWidget(), 2, QString::fromStdString(path), QString::number(numCounter), "");
    link->exec();
}

void activecounterlink::setClientDispatcher(Symposium::clientdispatcher *cl)
{
    this->cl = cl;
}

void activecounterlink::errorConnection()
{
    enableButtons();
    enableStyleButtons();
    this->hide();
    errorWindow = new errorconnection();
    errorWindow->exec();
}

void activecounterlink::errorConnectionLogout(std::string str)
{
    enableButtons();
    enableStyleButtons();
    this->hide();
    errorLog = new errorlogout(nullptr, QString::fromStdString(str));
    parentWidget()->hide();
    parentWidget()->parentWidget()->hide();
    errorLog->setClientDispatcher(cl);
    errorLog->show();
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
        //cl->shareResource(pathFile, documentId, u);
        waiting();
        disableButtons();
        disableStyleButtons();
    }

    //--------------------------------------------PARTE DA CANCELLARE SUCCESSIVAMENTE
    this->hide();
    enableButtons();
    link = new successlinks(parentWidget(), 2, QString::fromStdString(pathFile), QString::number(numCounter));
    link->exec();

    //----------------------------------------------------------------------------------

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

void activecounterlink::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setDuration(1000);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
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
