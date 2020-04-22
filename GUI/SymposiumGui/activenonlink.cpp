#include "activenonlink.h"
#include "ui_activenonlink.h"

activenonlink::activenonlink(QWidget *parent, Symposium::uint_positive_cnt::type documentId, std::string pathFile) :
    QDialog(parent),
    ui(new Ui::activenonlink), pathFile(pathFile), documentId(documentId)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    enableButtons();

    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();
}

void activenonlink::unsuccessLink(std::string errorMess)
{
    enableButtons();
    enableStyleButtons();
    QString error=QString::fromStdString(errorMess);
    ui->errorMess->setText(error);
    ui->errorMess->show();

}

void activenonlink::successLink(std::string path)
{
    enableButtons();
    enableStyleButtons();
    this->close();
    link = new successlinks(parentWidget(), 3, QString::fromStdString(path));
    link->exec();
}

void activenonlink::setClientDispatcher(Symposium::clientdispatcher *cl)
{
    this->cl = cl;
}

void activenonlink::errorConnection()
{
    enableButtons();
    errorWindow = new errorconnection(this);
    errorWindow->show();
}

void activenonlink::errorConnectionLogout(std::string str)
{
    enableButtons();
    errorLog = new errorlogout(nullptr, QString::fromStdString(str));
    errorLog->setClientDispatcher(cl);
    this->close();
    parentWidget()->close();
    parentWidget()->parentWidget()->hide();
    errorLog->show();
}

activenonlink::~activenonlink()
{
    delete ui;
}

void activenonlink::enableButtonsAfter()
{
    enableStyleButtons();
}

void activenonlink::on_ok_clicked()
{
    waiting();
    disableButtons();
    disableStyleButtons();
    u.deactivate();
    //cl->shareResource(pathFile, documentId, u);
}

void activenonlink::on_cancel_clicked()
{
    this->close();
}

void activenonlink::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setDuration(1000);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void activenonlink::disableButtons()
{
    ui->cancel->setDisabled(true);
    ui->ok->setDisabled(true);
}

void activenonlink::enableButtons()
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->errorMess->hide();
    ui->cancel->setDisabled(false);
    ui->ok->setDisabled(false);
}

void activenonlink::enableStyleButtons()
{
    ui->ok->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->cancel->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 grey);color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
}

void activenonlink::disableStyleButtons()
{
    ui->ok->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->cancel->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
}

void activenonlink::waiting()
{
    ui->waiting->show();
    ui->gif->show();
    ui->errorMess->hide();
}
