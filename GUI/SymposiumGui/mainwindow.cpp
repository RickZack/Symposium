#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Dispatcher/clientdispatcher.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QPixmap pix(":/icon/logo.png");
    int w=ui->label->width();
    int h=ui->label->height();
    ui->label->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));


}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_about_clicked()
{
    aboutWindow =new about(this);
    aboutWindow->exec();
}

void MainWindow::on_exit_clicked()
{
    ex = new class exit(this);
    ex->exec();
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
    int ret=msgBox.exec();

        if (ret == QMessageBox::Yes)
                qApp->quit();*/

}

void MainWindow::on_SignIn_clicked()
{
    signinWindow= new sigin();
    //cl->setSignIn(signinWindow);

    signinWindow->setClientDispatcher(cl);

    signinWindow->show();
    hide();

}

void MainWindow::on_SignUp_clicked()
{
    signupWindow= new signup();
    //cl->setSignUp(signupWindow);
    signupWindow->setClientDispatcher(cl);
    signupWindow->show();
    hide();

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    ex = new class exit(this);
    ex->exec();
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
    int ret=msgBox.exec();

        if (ret == QMessageBox::Yes)
                event->accept();
        else event->ignore();*/

}

void MainWindow::setClientDispatcher(Symposium::clientdispatcher *cl){
    this->cl = cl;
}


void MainWindow::showEvent(QShowEvent* event)
{
QMainWindow::showEvent(event);

 QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
      anim->setStartValue(0.0);
      anim->setEndValue(1.0);
      anim->setDuration(1000);
 anim->start(QAbstractAnimation::DeleteWhenStopped);
}
