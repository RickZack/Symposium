#include "waiting.h"
#include "ui_waiting.h"

waiting::waiting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::waiting)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();

    setAttribute( Qt::WA_DeleteOnClose );

    setModal(true);

    setAttribute(Qt::WA_TranslucentBackground);
}

waiting::~waiting()
{
    delete ui;
}
