#include "successlinks.h"
#include "ui_successlinks.h"


successlinks::successlinks(QWidget *parent, int caseLink, QString link, QString numShares, QString time, Symposium::user us, Symposium::privilege priv) :
    QDialog(parent),
    ui(new Ui::successlinks), caseLink(caseLink), link(link), numShares(numShares), time(time), us(us), priv(priv)
{
    ui->setupUi(this);
    setFixedSize(size());
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    hideAll();
    if(caseLink==1)
        ui->all->show();
    else if(caseLink==2)
        share();
    else if(caseLink==3)
        noshare();
    else if(caseLink==4)
        timeshare();
    ui->pathPlainEdit->setPlainText(link);
    ui->pathPlainEdit->setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);
}

successlinks::~successlinks(){
    delete ui;
}

void successlinks::on_ok_clicked(){
    close();
}

void successlinks::hideAll(){
    ui->all->hide();
    ui->share1->hide();
    ui->share2->hide();
    ui->share3->hide();
    ui->share4->hide();
    ui->timer1->hide();
    ui->timer2->hide();
}

void successlinks::share(){
    ui->share1->show();
    ui->share2->show();
    ui->share3->show();
    ui->share2->setText(numShares);
}

void successlinks::noshare(){
    ui->label->hide();
    ui->pathPlainEdit->hide();
    ui->share4->show();
    ui->ok->move(282,105);
    ui->pushButton->setDisabled(true);
    ui->pushButton->hide();
    ui->label_2->hide();
    ui->ok->setText("Ok");
    ui->ok->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
}

void successlinks::timeshare(){
    ui->timer1->show();
    ui->timer2->setText(time);
    ui->timer2->show();
}

void successlinks::on_pushButton_clicked(){
    close();
    QString user(QString::fromStdString(us.getNickname()));
    std::ostringstream privString;
    privString<<priv;
    QString privilege(QString::fromStdString(privString.str()));
    QString emailAddr("");
    QString subject("Symposium - invitation to collaborate");
    QString body("You have a new invitation to collaborate.\n\n"+user+" decided to share with you a file with the privilege "+privilege+" on Symposium.\nYou can access it pasting the following resource identifier: "+link+"\n\n\n\nThis email has been automatically generated using Symposium.\nSymposium is a open source, real-time, client-server collaborative text editor that uses Conflict-Free Replicated Data Types (CRDT) to make sure all users stay in-sync.");
    if(caseLink==2)
        body="You have a new invitation to collaborate.\n\n"+user+" decided to share with you and other "+numShares+" shares a file with the privilege "+privilege+" on Symposium.\nYou can access it pasting the following resource identifier: "+link+"\n\n\n\nThis email has been automatically generated using Symposium.\nSymposium is a open source, real-time, client-server collaborative text editor that uses Conflict-Free Replicated Data Types (CRDT) to make sure all users stay in-sync.";
    if(caseLink==4)
        body="You have a new invitation to collaborate.\n\n"+user+" decided to share with you a file with the privilege "+privilege+" on Symposium.\n The link will be available until "+time+".\nYou can access it pasting the following resource identifier: "+link+"\n\n\n\nThis email has been automatically generated using Symposium.\nSymposium is a open source, real-time, client-server collaborative text editor that uses Conflict-Free Replicated Data Types (CRDT) to make sure all users stay in-sync.";
    QDesktopServices::openUrl(QUrl("mailto:"+emailAddr+"?subject="+subject+"&body="+body, QUrl::TolerantMode));
}
