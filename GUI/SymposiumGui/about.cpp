#include "about.h"
#include "ui_about.h"

about::about(QWidget *parent, SymWinInterface& si) :
    QDialog(parent),
    SymChildWinInterface (si, isQWidget::isQwidgetType(*this)),
    ui(new Ui::about)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    //connect(ui->ok, SIGNAL(clicked()), parentWidget(), SLOT(enableButtonsAfter()));
    setAttribute( Qt::WA_DeleteOnClose );
}

about::~about()
{
    delete ui;
}

void about::on_ok_clicked()
{
    this->close();
}

void about::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);

    QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setDuration(1000);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void about::closeEvent(QCloseEvent *event){
    backToParent();
}
