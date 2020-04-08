#include "home.h"
#include "ui_home.h"
#include "Dispatcher/clientdispatcher.h"

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
    inserturiWindow = new inserturi(this);
    //inserturiWindow->setClientDispatcher(cl);
    //cl->setInsertUri(inserturiWindow);
    inserturiWindow->exec();
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
    directoryWindow=new directory(this);
    directoryWindow->show();
    //cl->setDirectory(directoryWindow);
    this->hide();
}

void home::logout()
{
    //cl->logout();
    ui->directory->setDisabled(true);
    ui->modify->setDisabled(true);
    ui->InsertUri->setDisabled(true);
    ui->delete_2->setDisabled(true);
    ui->logout->setDisabled(true);
    ui->waiting->show();
    ui->gif->show();
}

void home::closeEvent(QCloseEvent *event)
{
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
    msgBox.setStyleSheet("QMessageBox { background-color:rgb(249, 247, 241); "
                         "color: rgb(58, 80, 116);"
                         "font: 14pt 'Baskerville Old Face';} "
                         "QLabel{color: rgb(58, 80, 116);} "
                         "QPushButton#Yes { "
                         "background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, "
                         "stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116)); "
                         "color: white; font: 14pt 'Baskerville Old Face'; "
                         "border-radius:15px; width: 100px; height: 30px;}"
                         "QPushButton#No { "
                         "background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, "
                         "stop: 0 rgb(95, 167, 175), stop: 1 grey); "
                         "color: white; font: 14pt 'Baskerville Old Face'; "
                         "border-radius:15px; width: 100px; height: 30px;}");
    msgBox.setIcon(QMessageBox::Question);
    int ret=QMessageBox::No;
    ret=msgBox.exec();
    if (ret == QMessageBox::Yes)
        event->accept();
    else
        event->ignore();

}

void home::setClientDispatcher(Symposium::clientdispatcher *cl){
    this->cl = cl;
}

void home::successLogout()
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->directory->setDisabled(false);
    ui->modify->setDisabled(false);
    ui->InsertUri->setDisabled(false);
    ui->delete_2->setDisabled(false);
    ui->logout->setDisabled(false);
    this->hide();
    parentWidget()->show();
}

void home::errorConnection()
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->directory->setDisabled(false);
    ui->modify->setDisabled(false);
    ui->InsertUri->setDisabled(false);
    ui->delete_2->setDisabled(false);
    ui->logout->setDisabled(false);
    errorWindow = new errorconnection(this);
    errorWindow->exec();
}

void home::errorConnectionLogout(std::string str)
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->directory->setDisabled(false);
    ui->modify->setDisabled(false);
    ui->InsertUri->setDisabled(false);
    ui->delete_2->setDisabled(false);
    ui->logout->setDisabled(false);
    errorLog = new errorlogout(this, QString::fromStdString(str));
    this->hide();
    parentWidget()->close();
    errorLog->exec();
}

