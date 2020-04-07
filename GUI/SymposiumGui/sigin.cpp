#include "sigin.h"
#include "ui_sigin.h"
#include "mainwindow.h"
#include "Dispatcher/clientdispatcher.h"

sigin::sigin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::sigin)
{
    ui->setupUi(this);
    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(hide()));
    connect(ui->cancel, SIGNAL(clicked()), parent, SLOT(show()));
    ui->haveto->hide();
    ui->waiting->hide();
    ui->tryAgain->hide();
    ui->password->setEchoMode(QLineEdit::Password);
    ui->gif->hide();
    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();

}

sigin::~sigin()
{
    delete ui;
}

void sigin::errorConnection()
{
    ui->signin->setDisabled(false);
    ui->cancel->setDisabled(false);
    errorWindow = new errorconnection(this);
    errorWindow->exec();
    ui->haveto->hide();
    ui->tryAgain->hide();
    ui->waiting->hide();
    ui->gif->hide();
}

void sigin::errorSignIn()
{
    ui->signin->setDisabled(false);
    ui->cancel->setDisabled(false);
    ui->haveto->hide();
    ui->tryAgain->show();
    ui->waiting->hide();
    ui->gif->hide();

}

void sigin::successSignIn()
{
    hide();
    homeWindow = new home(parentWidget(), pwd);
    homeWindow->setClientDispatcher(cl);
    //cl->setHome(home *homeWindow);
    homeWindow->show();
}




void sigin::setClientDispatcher(Symposium::clientdispatcher *cl){
    this->cl = cl;
}

void sigin::on_signin_clicked()
{
    ui->haveto->hide();
    ui->waiting->hide();
    ui->tryAgain->hide();
    ui->gif->hide();

    QString username= ui->username->text();
    QString password = ui->password->text();
    pwd=password.toStdString();

    //---------------------------------------------PARTE DA DECOMENTARE

    /*if(username!="" && password!=""){
        waiting();
        this->cl->logIn(username.toStdString(), password.toStdString());
    }
    else {
        ui->haveto->show();
    }*/


    //--------------------------------------------PARTE DA CANCELLARE SUCCESSIVAMENTE
    if(username=="test" && password=="test")
    {
        hide();
        homeWindow= new home(parentWidget(), pwd);
        homeWindow->show();
    }
    else {
        ui->haveto->hide();
        ui->waiting->hide();
        ui->tryAgain->hide();
        ui->gif->hide();
        ui->signin->setDisabled(false);
        ui->cancel->setDisabled(false);
        ui->msg->setText("This credentials are not valid");
    }
    //--------------------------------------------------


}

void sigin::closeEvent(QCloseEvent *event)
{
    /*QMessageBox msgBox;
    msgBox.setText("    Are you sure to quit??");
    msgBox.setWindowTitle("Exit");
    QPixmap pix(":/icon/logo1.png");
    QIcon p(pix);
    msgBox.setWindowIcon(p);
    msgBox.setStandardButtons(QMessageBox::Yes| QMessageBox::No);
    msgBox.button(QMessageBox::Yes)->setObjectName("Yes");
    msgBox.button(QMessageBox::No)->setObjectName("No");
    msgBox.setBaseSize(QSize(390, 120));
    msgBox.setStyleSheet("QMessageBox { background-color:rgb(249, 247, 241); "
                         "color: rgb(58, 80, 116);"
                         "font: 14pt 'Baskerville Old Face';} "
                         "QLabel{color: rgb(58, 80, 116);} "
                         "QPushButton#Yes { "
                         "background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, "
                         "stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116)); "
                         "color: white; font: 14pt 'Baskerville Old Face'; "
                         "border-radius:15px; width: 80px; height: 30px; "
                         "margin-left:50px; margin-right:50px;}"
                         "QPushButton#No { "
                         "background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, "
                         "stop: 0 rgb(95, 167, 175), stop: 1 grey); "
                         "color: white; font: 14pt 'Baskerville Old Face'; "
                         "border-radius:15px; width: 80px; height: 30px; "
                         "margin-left:50px; margin-right:50px;}");
    msgBox.setIcon(QMessageBox::Question);
    int ret=QMessageBox::No;
    ret=msgBox.exec();
        if (ret == QMessageBox::Yes)
                event->accept();
        else event->ignore();*/







    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Exit",
                                                                    tr("Are you sure to quit?\n"),
                                                                     QMessageBox::No | QMessageBox::Yes,
                                                                    QMessageBox::Yes);
        if (resBtn != QMessageBox::Yes) {
            event->ignore();
        } else {
            event->accept();
            //cl->closeConnection();
        }

}

void sigin::waiting()
{
    ui->waiting->show();
    ui->gif->show();
    ui->signin->setDisabled(true);
    ui->cancel->setDisabled(true);

}
