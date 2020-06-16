#include "unsuccessdeleteaccount.h"
#include "ui_unsuccessdeleteaccount.h"

unsuccessdeleteaccount::unsuccessdeleteaccount(QWidget *parent, std::string error) :
    QDialog(parent),
    ui(new Ui::unsuccessdeleteaccount), error(error){
    ui->setupUi(this);
    setFixedSize(size());
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->error->setText(QString::fromStdString(error));
}

unsuccessdeleteaccount::~unsuccessdeleteaccount(){
    delete ui;
}

void unsuccessdeleteaccount::on_pushButton_clicked(){
    close();
}
