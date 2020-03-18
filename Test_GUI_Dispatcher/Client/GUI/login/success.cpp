#include "success.h"
#include "ui_success.h"

success::success(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::success)
{
    ui->setupUi(this);
    connect(ui->logout, SIGNAL(clicked()), parent, SLOT(show()));
    connect(ui->logout, SIGNAL(clicked()), this, SLOT(close()));
}

success::~success()
{
    delete ui;
}

