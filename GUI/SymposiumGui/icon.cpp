#include "icon.h"
#include "ui_icon.h"
#include <QPixmap>

icon::icon(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::icon)
{
    ui->setupUi(this);
    msg=":/resources/avatar/beaver.png";
    QPixmap pix(":/resources/avatar/beaver.png");
    int w=ui->img->width();
    int h=ui->img->height();
    ui->img->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));

    QPixmap pix2(":/resources/avatar/boar.png");
    int w2=ui->img2->width();
    int h2=ui->img2->height();
    ui->img2->setPixmap(pix2.scaled(w2, h2, Qt::KeepAspectRatio));

    QPixmap pix3(":/resources/avatar/bull.png");
    int w3=ui->img3->width();
    int h3=ui->img3->height();
    ui->img3->setPixmap(pix3.scaled(w3, h3, Qt::KeepAspectRatio));

    QPixmap pix4(":/resources/avatar/deer.png");
    int w4=ui->img4->width();
    int h4=ui->img4->height();
    ui->img4->setPixmap(pix4.scaled(w4, h4, Qt::KeepAspectRatio));

    QPixmap pix5(":/resources/avatar/duck.png");
    int w5=ui->img5->width();
    int h5=ui->img5->height();
    ui->img5->setPixmap(pix5.scaled(w5, h5, Qt::KeepAspectRatio));

    QPixmap pix6(":/resources/avatar/hedgehog.png");
    int w6=ui->img6->width();
    int h6=ui->img6->height();
    ui->img6->setPixmap(pix6.scaled(w6, h6, Qt::KeepAspectRatio));

    QPixmap pix7(":/resources/avatar/hippo.png");
    int w7=ui->img7->width();
    int h7=ui->img7->height();
    ui->img7->setPixmap(pix7.scaled(w7, h7, Qt::KeepAspectRatio));

    QPixmap pix8(":/resources/avatar/ostrich.png");
    int w8=ui->img8->width();
    int h8=ui->img8->height();
    ui->img8->setPixmap(pix8.scaled(w8, h8, Qt::KeepAspectRatio));

    QPixmap pix9(":/resources/avatar/owl.png");
    int w9=ui->img9->width();
    int h9=ui->img9->height();
    ui->img9->setPixmap(pix9.scaled(w9, h9, Qt::KeepAspectRatio));

    QPixmap pix10(":/resources/avatar/zoo.png");
    int w10=ui->img10->width();
    int h10=ui->img10->height();
    ui->img10->setPixmap(pix10.scaled(w10, h10, Qt::KeepAspectRatio));

    ui->beaver->click();



    connect(ui->pushButton, SIGNAL(clicked()), parent, SLOT(chooseIcon()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(close()));
}

icon::~icon()
{
    delete ui;
}

void icon::on_pushButton_clicked()
{
    hide();
}

void icon::on_beaver_clicked()
{
    msg=":/resources/avatar/beaver.png";
}

void icon::on_boar_clicked()
{
    msg=":/resources/avatar/boar.png";
}

void icon::on_bull_clicked()
{
    msg=":/resources/avatar/bull.png";
}

void icon::on_deer_clicked()
{
    msg=":/resources/avatar/deer.png";
}

void icon::on_duck_clicked()
{
    msg=":/resources/avatar/duck.png";
}

void icon::on_hed_clicked()
{
    msg=":/resources/avatar/hedgehog.png";
}

void icon::on_hippo_clicked()
{
    msg=":/resources/avatar/hippo.png";
}

void icon::on_ostrich_clicked()
{
    msg=":/resources/avatar/ostrich.png";
}

void icon::on_owl_clicked()
{
    msg=":/resources/avatar/owl.png";
}

void icon::on_zoo_clicked()
{
    msg=":/resources/avatar/zoo.png";
}
