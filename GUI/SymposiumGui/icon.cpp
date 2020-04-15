#include "icon.h"
#include "ui_icon.h"
#include <QPixmap>

icon::icon(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::icon)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    msg=":/resources/avatar/beaver.png";

    QPixmap pix(":/resources/avatar/beaver.png");
    int w=85;
    int h=85;
    ui->img->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));

    QPixmap pix2(":/resources/avatar/boar.png");
    ui->img2->setPixmap(pix2.scaled(w, h, Qt::KeepAspectRatio));

    QPixmap pix3(":/resources/avatar/bull.png");
    ui->img3->setPixmap(pix3.scaled(w, h, Qt::KeepAspectRatio));

    QPixmap pix4(":/resources/avatar/deer.png");
    ui->img4->setPixmap(pix4.scaled(w, h, Qt::KeepAspectRatio));

    QPixmap pix5(":/resources/avatar/duck.png");
    ui->img5->setPixmap(pix5.scaled(w, h, Qt::KeepAspectRatio));

    QPixmap pix6(":/resources/avatar/hedgehog.png");
    ui->img6->setPixmap(pix6.scaled(w, h, Qt::KeepAspectRatio));

    QPixmap pix7(":/resources/avatar/hippo.png");
    ui->img7->setPixmap(pix7.scaled(w, h, Qt::KeepAspectRatio));

    QPixmap pix8(":/resources/avatar/ostrich.png");
    ui->img8->setPixmap(pix8.scaled(w, h, Qt::KeepAspectRatio));

    QPixmap pix9(":/resources/avatar/owl.png");
    ui->img9->setPixmap(pix9.scaled(w, h, Qt::KeepAspectRatio));

    QPixmap pix10(":/resources/avatar/zoo.png");
    ui->img10->setPixmap(pix10.scaled(w, h, Qt::KeepAspectRatio));

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
