#include "activealwayslink.h"
#include "ui_activealwayslink.h"
#include "Dispatcher/clientdispatcher.h"
#include <QMovie>

activealwayslink::activealwayslink(QWidget *parent, Symposium::uint_positive_cnt::type documentId, std::string pathFile) :
    QDialog(parent),
    ui(new Ui::activealwayslink),  documentId(documentId), pathFile(pathFile)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setFixedSize(size());
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    ui->writer->click();
    privilegeToGrant=Symposium::privilege::modify;
    enableButtons();
    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();

    //--------------------------------------------PARTE DA CANCELLARE SUCCESSIVAMENTE
    this->pathFile="/1/2/3/4/5";
    //---------------------------------------------------------------------------------------------------------------

}

void activealwayslink::unsuccessLink(std::string errorMess)
{
    enableButtons();
    enableStyleButtons();
    this->hide();
    QString error=QString::fromStdString(errorMess);
    ui->errorMess->setText(error);
    ui->errorMess->show();

}

void activealwayslink::successLink(std::string path)
{
    enableButtons();
    enableStyleButtons();
    this->hide();
    link = new successlinks(parentWidget(), 1, QString::fromStdString(path));
    link->exec();
}

void activealwayslink::setClientDispatcher(Symposium::clientdispatcher *cl)
{
    this->cl = cl;
}

void activealwayslink::errorConnection()
{
    enableButtons();
    enableStyleButtons();
    this->hide();
    errorWindow = new errorconnection();
    errorWindow->exec();
}

void activealwayslink::errorConnectionLogout(std::string str)
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
    this->close();
}

void activealwayslink::on_ok_clicked()
{
    u=Symposium::uri();
    u.activateAlways(privilegeToGrant);
    waiting();
    disableButtons();
    disableStyleButtons();
    //cl->shareResource(pathFile, documentId, u);

    //--------------------------------------------PARTE DA CANCELLARE SUCCESSIVAMENTE
    this->hide();
    enableButtons();
    link = new successlinks(parentWidget(), 1, QString::fromStdString(pathFile));
    link->exec();
    //---------------------------------------------------------------------------------------------------------------
}

void activealwayslink::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setDuration(1000);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
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
