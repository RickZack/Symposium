#include "sigin.h"
#include "ui_sigin.h"
#include "mainwindow.h"

sigin::sigin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::sigin)
{
    ui->setupUi(this);
    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(hide()));
    connect(ui->cancel, SIGNAL(clicked()), parent, SLOT(show()));

}

sigin::~sigin()
{
    delete ui;
}

void sigin::on_signin_clicked()
{
    hide();
    homeWindow= new home(parentWidget());
    homeWindow->show();
}

void sigin::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Exit",
                                                                    tr("Are you sure to quit?\n"),
                                                                     QMessageBox::No | QMessageBox::Yes,
                                                                    QMessageBox::Yes);
        if (resBtn != QMessageBox::Yes) {
            event->ignore();
        } else {
            event->accept();
        }

}
