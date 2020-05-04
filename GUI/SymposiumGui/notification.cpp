#include "notification.h"
#include "ui_notification.h"

notification::notification(QWidget *parent, QString str) :
    QDialog(parent),
    ui(new Ui::notification), str(str)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setFixedSize(size());
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    ui->text->setText(str);
    connect(ui->ok, SIGNAL(clicked()), this, SLOT(hide()));
    if(parent!=nullptr)
        connect(ui->ok, SIGNAL(clicked()), parentWidget(), SLOT(enableButtonsAfter()));
}

notification::~notification()
{
    delete ui;
}

