#include "deleteaccount.h"
#include "ui_deleteaccount.h"
#include "Dispatcher/clientdispatcher.h"
#include "mainwindow.h"

deleteAccount::deleteAccount(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::deleteAccount)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->cancel, SIGNAL(clicked()), parentWidget(), SLOT(enableButtonsAfter()));
    connect(ui->delete_2, SIGNAL(clicked()), this, SLOT(delete_click()));
    enableButtons();
    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();
}

void deleteAccount::successDeleteAccount()
{
    enableButtons();
    enableStyleButtons();
    this->close();
    parentWidget()->hide();
    mw=new MainWindow();
    mw->show();
    QMessageBox msgBox;
    msgBox.setText("<p align='center'>Your account has been successfully deleted!</p>");
    msgBox.setWindowTitle("Notification");
    QPixmap pix(":/icon/logo1.png");
    QIcon p(pix);
    msgBox.setWindowIcon(p);
    msgBox.setStandardButtons(QMessageBox::Yes);
    msgBox.button(QMessageBox::Yes)->setObjectName("Yes");
    msgBox.button(QMessageBox::Yes)->setText("Ok");
    msgBox.setStyleSheet("QMessageBox { background-color:rgb(249, 247, 241); "
                         "color: rgb(58, 80, 116);"
                         "font: 14pt 'Baskerville Old Face';} "
                         "QLabel{color: rgb(58, 80, 116);} "
                         "QPushButton#Yes { "
                         "background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, "
                         "stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116)); "
                         "color: white; font: 14pt 'Baskerville Old Face'; "
                         "border-radius:15px; width: 100px; height: 30px; "
                         "margin-right:185px;}"
                         "QPushButton#No { "
                         "background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, "
                         "stop: 0 rgb(95, 167, 175), stop: 1 grey); "
                         "color: white; font: 14pt 'Baskerville Old Face'; "
                         "border-radius:15px; width: 80px; height: 30px; "
                         "margin-left:20px;}");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
}

void deleteAccount::errorDeleteUser(std::string errorMess)
{
    enableButtons();
    enableStyleButtons();
    this->close();
    QMessageBox msgBox;
    msgBox.setText("<p align='center'>ERROR: "+QString::fromStdString(errorMess)+"</p>");
    msgBox.setWindowTitle("Notification");
    QPixmap pix(":/icon/logo1.png");
    QIcon p(pix);
    msgBox.setWindowIcon(p);
    msgBox.setStandardButtons(QMessageBox::Yes);
    msgBox.button(QMessageBox::Yes)->setObjectName("Yes");
    msgBox.button(QMessageBox::Yes)->setText("Ok");
    msgBox.setStyleSheet("QMessageBox { background-color:rgb(249, 247, 241); "
                         "color: rgb(58, 80, 116);"
                         "font: 14pt 'Baskerville Old Face';} "
                         "QLabel{color: rgb(58, 80, 116);} "
                         "QPushButton#Yes { "
                         "background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, "
                         "stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116)); "
                         "color: white; font: 14pt 'Baskerville Old Face'; "
                         "border-radius:15px; width: 100px; height: 30px; "
                         "}"
                         "QPushButton#No { "
                         "background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, "
                         "stop: 0 rgb(95, 167, 175), stop: 1 grey); "
                         "color: white; font: 14pt 'Baskerville Old Face'; "
                         "border-radius:15px; width: 80px; height: 30px; "
                         "margin-left:20px;}");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
}


deleteAccount::~deleteAccount()
{
    delete ui;
}

void deleteAccount::delete_click()
{
    disableButtons();
    disableStyleButtons();
    //cl->removeUser();
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

void deleteAccount::setClientDispatcher(Symposium::clientdispatcher *cl){
    this->cl = cl;
}

void deleteAccount::errorConnection()
{
    enableButtons();
    enableStyleButtons();
    errorWindow = new errorconnection(this);
    errorWindow->exec();
}

void deleteAccount::errorConnectionLogout(std::string str)
{
    enableButtons();
    enableStyleButtons();
    errorLog = new errorlogout(nullptr, QString::fromStdString(str));
    this->close();
    parentWidget()->close();
    errorLog->exec();
}


