#include "about.h"
#include "ui_about.h"

about::about(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::about)
{
    ui->setupUi(this);
    setFixedSize(size());
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    connect(ui->ok, SIGNAL(clicked()), parentWidget(), SLOT(enableButtonsAfter()));
}

about::~about()
{
    delete ui;
}

void about::on_ok_clicked()
{
    this->close();

}

void about::closeEvent(QCloseEvent *e) {
    emit ui->ok->click();
    QDialog::closeEvent(e);
}

