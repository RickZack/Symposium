#include "home.h"
#include "ui_home.h"
#include "Dispatcher/clientdispatcher.h"

home::home(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::home)
{
    ui->setupUi(this);
    connect(ui->logout, SIGNAL(clicked()), this, SLOT(hide()));
    connect(ui->logout, SIGNAL(clicked()), parent, SLOT(show()));
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
    deleteAccountWindow->show();
}

void home::on_InsertUri_clicked()
{
    inserturiWindow = new inserturi(this);
    //inserturiWindow->setClientDispatcher(cl);
    //cl->setInsertUri(inserturiWindow);
    inserturiWindow->show();
}

void home::on_modify_clicked()
{
    changeWindow = new changeUserInfo(this);
    changeWindow->setClientDispatcher(cl);
    //cl->setChangeUserInfo(changeWindow);
    changeWindow->show();
}

void home::on_directory_clicked()
{
    directoryWindow=new directory(this);
    directoryWindow->show();
    this->hide();
}

void home::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Exit",
                                                                    tr("Are you sure to quit?\n"),
                                                                     QMessageBox::No | QMessageBox::Yes,
                                                                    QMessageBox::Yes);
        if (resBtn != QMessageBox::Yes) {
            event->ignore();
        } else {
            cl->logout();
            event->accept();
        }

}

void home::setClientDispatcher(Symposium::clientdispatcher *cl){
    this->cl = cl;
}

