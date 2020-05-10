#include "deleteaccount.h"
#include "ui_deleteaccount.h"
#include "Dispatcher/clientdispatcher.h"
#include "mainwindow.h"
#include "onoff_networkinteraction.h"

deleteAccount::deleteAccount(QWidget *parent, SymWinInterface& si) :
    QDialog(parent),
    SymModalWinInterface (si, isQDialog::isQDialogType(*this)),
    ui(new Ui::deleteAccount)
{
    ui->setupUi(this);
    setFixedSize(size());
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    //connect(ui->cancel, SIGNAL(clicked()), this, SLOT(close()));
    //connect(ui->cancel, SIGNAL(clicked()), parentWidget(), SLOT(enableButtonsAfter()));
    connect(ui->delete_2, SIGNAL(clicked()), this, SLOT(delete_click()));
    enableButtons();
    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();
}

void deleteAccount::success(){
    this->successDeleteAccount();
}

void deleteAccount::failure(const QString& toPrint){
    if(toPrint=="-1"){
        errorConnectionLogout();
    }else{
        errorDeleteUser(toPrint);
    }
}



void deleteAccount::successDeleteAccount()
{
    enableButtons();
    enableStyleButtons();
    backToMainWin();
    QString str="Your account has been successfully deleted!";
    notification notWindow(parentWidget(), str);
    int ret= notWindow.exec();
    //----------------------------DEVE ESSERE INVOCATO SU MAINWINDOW DA SISTEMARE
    if(ret==0)
        ((home*)parentWidget())->enableButtonsAfter();
        //mw->enableButtonsAfter();
}

void deleteAccount::errorDeleteUser(const QString& errorMess)
{
    enableButtons();
    enableStyleButtons();
    ui->haveto->setText(errorMess);
    ui->haveto->show();
}


deleteAccount::~deleteAccount()
{
    delete ui;
}


void deleteAccount::delete_click()
{
    ui->haveto->hide();
    std::string password = (ui->password->text()).toStdString();
    if(password!="")
    {
        disableButtons();
        disableStyleButtons();
        //------------------------------------------------------------------PARTE DA DECOMENTARE
        #ifdef DISPATCHER_ON
        cl.removeUser(password);
        #else
        successDeleteAccount();
        #endif
        //------------------------------------------------------------------
    }
    else
    {
        ui->haveto->setText("You have to digit your password");
        ui->haveto->show();
    }
}

void deleteAccount::disableButtons()
{
    ui->waiting->show();
    ui->gif->show();
    ui->cancel->setDisabled(true);
    ui->delete_2->setDisabled(true);
}

void deleteAccount::enableButtons()
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->cancel->setDisabled(false);
    ui->delete_2->setDisabled(false);
}

void deleteAccount::enableStyleButtons()
{
    ui->cancel->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->delete_2->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 grey);color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
}

void deleteAccount::disableStyleButtons()
{
    ui->delete_2->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->cancel->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
}

void deleteAccount::errorConnectionLogout()
{
    enableButtons();
    enableStyleButtons();
    errorlogout errorLog(nullptr);
    backToMainWin();
    errorLog.exec();
}

void deleteAccount::on_cancel_clicked(){
    backToParent();
    ((home*)parentWidget())->enableButtonsAfter();
}

void deleteAccount::closeEvent(QCloseEvent *event){
    //event->ignore();
    if(closedByUser())
    {
        backToParent();
        ((home*)parentWidget())->enableButtonsAfter();
    }
}


