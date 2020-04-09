#include "inserturi.h"
#include "ui_inserturi.h"
#include "directory.h"
#include "home.h"

inserturi::inserturi(QWidget *parent, std::string pwd, bool home) :
    QDialog(parent),
    ui(new Ui::inserturi), pwd(pwd), home(home)
{
    ui->setupUi(this);
    //showDir=cl->showDir(true);
    ui->writer->click();
    QPixmap pix2(":/icon/logo.png");
    int w=ui->logo->width();
    int h=ui->logo->height();
    ui->logo->setPixmap(pix2.scaled(w, h, Qt::KeepAspectRatio));
    hideLabelsError();
    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();
}

inserturi::~inserturi()
{
    delete ui;
}

void inserturi::on_dir_clicked()
{
    dirWindow=new choosedir(this);
    dirWindow->pathDir=showDir;
    dirWindow->exec();
    pathId=dirWindow->pathId;
}

void inserturi::change_text()
{
    path=dirWindow->pathId;
    nameDir=dirWindow->nameOfDir;
    if(!nameDir.empty())
        ui->dir->setText(QString::fromStdString(dirWindow->nameOfDir));
}

void inserturi::reset_text()
{
    path="";
    nameDir="";
    ui->dir->setText("choose the directory...");
}

void inserturi::closeEvent(QCloseEvent *event)
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

void inserturi::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setDuration(1000);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void inserturi::disableButtons()
{
    ui->add->setDisabled(true);
    ui->cancel->setDisabled(true);
}

void inserturi::enableButtons()
{
    ui->add->setDisabled(false);
    ui->cancel->setDisabled(false);
}

void inserturi::enableStyleButtons()
{
    ui->add->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->cancel->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 grey);color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
}

void inserturi::disableStyleButtons()
{
    ui->add->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->cancel->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
}

void inserturi::hideLabelsError()
{
    ui->dirEmpty->hide();
    ui->linkError->hide();
    ui->nameError->hide();
    ui->formatError->hide();
    ui->waiting->hide();
    ui->gif->hide();
}

void inserturi::waiting()
{
    ui->waiting->show();
    ui->gif->show();
}

void inserturi::on_reader_clicked()
{
    privilege=Symposium::privilege::readOnly;
}

void inserturi::on_writer_clicked()
{
    privilege=Symposium::privilege::modify;
}

void inserturi::on_owner_clicked()
{
    privilege=Symposium::privilege::owner;
}

void inserturi::setClientDispatcher(Symposium::clientdispatcher *cl){
    this->cl = cl;
}

void inserturi::unsuccessInsert(std::string errorMess)
{
    hideLabelsError();
    enableStyleButtons();
    enableButtons();
    ui->linkError->setText(QString::fromStdString(errorMess));
    ui->linkError->show();
}

void inserturi::on_add_clicked()
{
    hideLabelsError();
    if(path=="")
    {
        ui->dirEmpty->show();
    }
    else if(ui->name->text()=="")
    {
        ui->nameError->show();
    }
    else
    {
        std::string pathLink=ui->lineEdit->text().toStdString();
        std::size_t found = pathLink.find_last_of('/');
        if(found==std::string::npos)
            ui->formatError->show();
        else{
            std::string resourceId=pathLink.substr(found+1);
            if(resourceId=="")
                ui->formatError->show();
            else
            {
                pressed=true;
                waiting();
                disableButtons();
                disableStyleButtons();
                //cl->openNewSource(pathLink, privilege, path, ui->name->text().toStdString());
            }
        }

    }
}


void inserturi::errorConnection()
{
    hideLabelsError();
    enableStyleButtons();
    enableButtons();
    errorWindow = new errorconnection(this);
    errorWindow->exec();
}

void inserturi::errorConnectionLogout(std::string str)
{
    hideLabelsError();
    enableStyleButtons();
    enableButtons();
    errorLog = new errorlogout(this, QString::fromStdString(str));
    this->hide();
    errorLog->show();
}

void inserturi::successInsert()
{
    hideLabelsError();
    enableStyleButtons();
    enableButtons();
    QMessageBox::information(parentWidget(),
                             tr("Insert Link"), tr("The operation was successfully done!"), QMessageBox::Ok);
}

void inserturi::on_cancel_clicked()
{
    this->hide();
    if(home)
    {
        h=new class home(nullptr, pwd);
        h->show();
    }
    else
    {
        d=new directory();
        d->show();
    }
}


