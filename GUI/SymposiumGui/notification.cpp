#include "notification.h"
#include "ui_notification.h"

notification::notification(QWidget *parent, QString str) :
    QDialog(parent),
    ui(new Ui::notification), str(str)
{
    ui->setupUi(this);
    ui->text->setText(str);
    connect(ui->ok, SIGNAL(clicked()), this, SLOT(hide()));
}

notification::~notification()
{
    delete ui;
}

