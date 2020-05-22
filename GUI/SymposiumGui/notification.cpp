#include "notification.h"
#include "ui_notification.h"

notification::notification(QWidget *parent, QString str) :
    QDialog(parent),
    ui(new Ui::notification), str(str)
{
    ui->setupUi(this);
    setFixedSize(size());
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->text->setText(str);
    connect(ui->ok, SIGNAL(clicked()), this, SLOT(hide()));
    if(parent!=nullptr)
        connect(ui->ok, SIGNAL(clicked()), parentWidget(), SLOT(enableButtonsAfter()));
}

notification::~notification()
{
    delete ui;
}

void notification::closeEvent(QCloseEvent *e) {
    emit ui->ok->click();
    QDialog::closeEvent(e);
}

