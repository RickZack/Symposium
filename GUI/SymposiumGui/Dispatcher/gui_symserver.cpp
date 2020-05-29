#include "gui_symserver.h"
#include "ui_gui_symserver.h"
#include <QMessageBox>

GUI_SymServer::GUI_SymServer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GUI_SymServer), server()
{
    ui->setupUi(this);

    setFixedSize(size());

    QPixmap pix(":/icon/logo_server_GUI.png");
    int w = ui->label->width();
    int h = ui->label->height();
    ui->label->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));
    this->indirizzi.append("127.0.0.1");
    for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
        if ((address.protocol() == QAbstractSocket::IPv4Protocol) && (address != QHostAddress::LocalHost)){
            this->indirizzi.append(address.toString());
        }
    }
    ui->comboBox->addItems(this->indirizzi);
    ui->lineEdit->setValidator(new QRegExpValidator(QRegExp("[0-9]{4}"), ui->lineEdit));
}

GUI_SymServer::~GUI_SymServer()
{
    delete ui;
}

void GUI_SymServer::on_pushButton_clicked()
{
    //start server
    if(ui->lineEdit->text().length() == 0){
        QMessageBox mes(QMessageBox::Warning,"Server port is empty","You must to choose a server port!");
        mes.exec();
    }else if(ui->lineEdit->text().length() != 4){
        QMessageBox mes(QMessageBox::Warning,"Server port is not valid","You must to choose a server port with 4 digits!");
        mes.exec();
    }else{
        if(this->server.startServer(QHostAddress(ui->comboBox->currentText()), ui->lineEdit->text().toUInt())==0){
            ui->comboBox->setEnabled(false);
            ui->lineEdit->setEnabled(false);
            ui->pushButton->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
            ui->label_4->setText("Server is running...");
            ui->label_4->setStyleSheet("color: green;font: 14pt 'Baskerville Old Face';");
            ui->pushButton->setEnabled(false);
        }
    }
}
