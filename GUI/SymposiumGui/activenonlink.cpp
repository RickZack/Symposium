#include "activenonlink.h"
#include "ui_activenonlink.h"
#include "Dispatcher/clientdispatcher.h"
#include "onoff_networkinteraction.h"

activenonlink::activenonlink(QWidget *parent, Symposium::uint_positive_cnt::type documentId, std::string pathFile, SymWinInterface& si) :
    QDialog(parent),
    SymModalWinInterface (si, isQDialog::isQDialogType(*this)),
    ui(new Ui::activenonlink), pathFile(pathFile), documentId(documentId)
{
    ui->setupUi(this);
    setFixedSize(size());
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    enableButtons();

    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();
}

void activenonlink::success()
{
    successLink();
}

void activenonlink::failure(const QString &toPrint)
{
    if(toPrint=="-1"){
        errorConnectionLogout();
    }else{
        unsuccessLink(toPrint);
    }
}

void activenonlink::unsuccessLink(const QString& errorMess)
{
    enableButtons();
    enableStyleButtons();
    ui->errorMess->setText(errorMess);
    ui->errorMess->show();

}

void activenonlink::successLink()
{
    enableButtons();
    enableStyleButtons();
    backToParent();
    successlinks link(parentWidget(), 3, QString::fromStdString(pathFile), 0, "", Symposium::user(), Symposium::privilege());
    link.exec();
}

void activenonlink::errorConnectionLogout()
{
    backToMainWin();
    enableButtons();
    errorlogout errorLog(nullptr);
    errorLog.exec();
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
    #ifdef DISPATCHER_ON
    cl.shareResource(pathFile, std::to_string(documentId), u);
    #else
    success();
    #endif
}

void activenonlink::on_cancel_clicked()
{
    backToParent();
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

void activenonlink::closeEvent(QCloseEvent *event)
{
    if(closedByUser())
        backToParent();
    else
        event->accept();
}
