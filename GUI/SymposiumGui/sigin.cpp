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
