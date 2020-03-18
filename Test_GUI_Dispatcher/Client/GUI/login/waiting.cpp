#include "waiting.h"
#include "ui_waiting.h"

waiting::waiting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::waiting)
{
    ui->setupUi(this);
    QMovie *movie = new QMovie(":/img/loader.gif");
    //QLabel *processLabel = new QLabel(this);
    ui->animationGif->setMovie(movie);
    //processLabel->setMovie(movie);
    movie->start();
}

waiting::~waiting()
{
    delete ui;
}
