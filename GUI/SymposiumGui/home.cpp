#include "home.h"
#include "ui_home.h"
#include "Dispatcher/clientdispatcher.h"
#include "mainwindow.h"

home::home(QWidget *parent, std::string pwd) :
    QMainWindow(parent),
    ui(new Ui::home), pwd(pwd)
{
    ui->setupUi(this);
    connect(ui->logout, SIGNAL(clicked()), this, SLOT(logout()));
    ui->waiting->hide();
    ui->gif->hide();
    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();
    QPixmap pix2(":/icon/logo.png");
    int w=ui->logo->width();
    int h=ui->logo->height();
    ui->logo->setPixmap(pix2.scaled(w, h, Qt::KeepAspectRatio));
    ButtonHoverWatcher * watcher = new ButtonHoverWatcher(this);
    ui->logout->installEventFilter(watcher);
    ui->logout->setToolTip("Logout");

}

home::~home()
{
    delete ui;
}

void home::on_delete_2_clicked()
{
    deleteAccountWindow = new deleteAccount(this);
    deleteAccountWindow->setClientDispatcher(cl);
    //cl->setDeleteAccount(deleteAccountWindow);
    deleteAccountWindow->exec();
}

void home::on_InsertUri_clicked()
{
    inserturiWindow = new inserturi(nullptr, pwd);
    //inserturiWindow->setClientDispatcher(cl);
    //cl->setInsertUri(inserturiWindow);
    inserturiWindow->show();
    this->hide();
}

void home::on_modify_clicked()
{
    changeWindow = new changeUserInfo(this, pwd);
    changeWindow->setClientDispatcher(cl);
    //cl->setChangeUserInfo(changeWindow);
    changeWindow->exec();
    pwd=changeWindow->pwd;
}

void home::on_directory_clicked()
{
    directoryWindow=new directory(nullptr, pwd);
    directoryWindow->show();
    //cl->setDirectory(directoryWindow);
    this->hide();
}

void home::logout()
{
    //cl->logout();
    pressed=true;
    waiting();
    disableButtons();
    disableStyleButtons();
}

void home::closeEvent(QCloseEvent *event)
{
    disableButtons();
    disableStyleButtons();
    QMessageBox msgBox;
    msgBox.setText("<p align='center'>Are you sure to quit?</p>");
    msgBox.setWindowTitle("Exit");
    QPixmap pix(":/icon/logo1.png");
    QIcon p(pix);
    msgBox.setWindowIcon(p);
    msgBox.setStandardButtons(QMessageBox::Yes| QMessageBox::No);
    msgBox.button(QMessageBox::Yes)->setObjectName("Yes");
    msgBox.button(QMessageBox::Yes)->setText("Quit");
    msgBox.button(QMessageBox::No)->setObjectName("No");
    msgBox.button(QMessageBox::No)->setText("Remain");
    msgBox.setBaseSize(QSize(390, 120));
    msgBox.setStyleSheet("QMessageBox { background-color:rgb(249, 247, 241); "
                         "color: rgb(58, 80, 116);"
                         "font: 14pt 'Baskerville Old Face';} "
                         "QLabel{color: rgb(58, 80, 116);} "
                         "QPushButton#Yes { "
                         "background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, "
                         "stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116)); "
                         "color: white; font: 14pt 'Baskerville Old Face'; "
                         "border-radius:15px; width: 100px; height: 30px; "
                         "margin-right:50px;}"
                         "QPushButton#No { "
                         "background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, "
                         "stop: 0 rgb(95, 167, 175), stop: 1 grey); "
                         "color: white; font: 14pt 'Baskerville Old Face'; "
                         "border-radius:15px; width: 80px; height: 30px; "
                         "}");
    msgBox.setIcon(QMessageBox::Question);
    int ret=msgBox.exec();
    if (ret == QMessageBox::Yes)
        event->accept();
    else
    {
        if(!pressed)
        {
            enableButtons();
            enableStyleButtons();
        }
        event->ignore();
    }
}

void home::disableButtons()
{
    ui->directory->setDisabled(true);
    ui->modify->setDisabled(true);
    ui->InsertUri->setDisabled(true);
    ui->delete_2->setDisabled(true);
    ui->logout->setDisabled(true);
}

void home::enableButtons()
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->directory->setDisabled(false);
    ui->modify->setDisabled(false);
    ui->InsertUri->setDisabled(false);
    ui->delete_2->setDisabled(false);
    ui->logout->setDisabled(false);
}

void home::enableStyleButtons()
{
    ui->directory->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->modify->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->InsertUri->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->delete_2->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
}

void home::disableStyleButtons()
{
    ui->directory->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->modify->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->InsertUri->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->delete_2->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
}

void home::waiting()
{
    ui->waiting->show();
    ui->gif->show();
}

void home::setClientDispatcher(Symposium::clientdispatcher *cl){
    this->cl = cl;
}

void home::successLogout()
{
    enableButtons();
    enableStyleButtons();
    this->hide();
    mw=new MainWindow();
    mw->show();
}

void home::errorConnection()
{
    enableButtons();
    enableStyleButtons();
    errorWindow = new errorconnection();
    errorWindow->exec();
}

void home::errorConnectionLogout(std::string str)
{
    enableButtons();
    enableStyleButtons();
    errorLog = new errorlogout(nullptr, QString::fromStdString(str));
    this->hide();
    errorLog->show();
}

