#include "option.h"
#include "ui_option.h"
#include "Dispatcher/clientdispatcher.h"

Option::Option(QWidget *parent, Symposium::clientdispatcher* cl) :
    QWidget(parent),
    ui(new Ui::Option), cl(cl)
{
    ui->setupUi(this);
    std::pair<std::string, std::string> inf = cl->getServerAddress();
    ui->SV_Address->setText(QString::fromStdString(inf.first));
    ui->SV_Port->setText(QString::fromStdString(inf.second));
}

Option::~Option()
{
    delete ui;
}

void Option::on_OK_button_clicked()
{
    cl->setServerAddress(ui->SV_Address->text().toStdString(), ui->SV_Port->text().toStdString());
    this->close();
}

void Option::on_Cancel_button_clicked()
{
    this->close();
}
