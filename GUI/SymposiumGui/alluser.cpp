#include "alluser.h"
#include "ui_alluser.h"
#include "Dispatcher/clientdispatcher.h"

alluser::alluser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::alluser)
{
    ui->setupUi(this);
    listusers();
    insertusers();
    ui->tree->setColumnCount(2);
    ui->tree->headerItem()->setText(0, "user:");
    ui->tree->headerItem()->setText(1, "privilege:");
    privelege=Symposium::privilege::owner;
    if(privelege!=Symposium::privilege::owner)
    {
        ui->button->setDisabled(true);
        ui->owner->hide();
        ui->owner->setDisabled(true);
        ui->reader->hide();
        ui->reader->setDisabled(true);
        ui->modify->hide();
        ui->modify->setDisabled(true);
        ui->none->hide();
        ui->none->setDisabled(true);
        ui->button->hide();
        ui->label->hide();
    }
    if(privelege==Symposium::privilege::owner)
        ui->modify->click();
}

void alluser::setClientDispatcher(Symposium::clientdispatcher *cl)
{
    this->cl = cl;
}

void alluser::successEditPrivilege()
{
    QMessageBox::information(parentWidget(),
                             tr("Modify Privilege"), tr("The privilege was successfully modify!"), QMessageBox::Ok);

    //onlineUsers=cl->allUser(documentID);
    ui->tree->clear();
    insertusers();


}

void alluser::errorEditPrivilege(std::string errorMess)
{
    QString error=QString::fromStdString(errorMess);
    QMessageBox::information(parentWidget(),
                             tr("Modify Privilege"), error, QMessageBox::Ok);
}

void alluser::errorConnection()
{
    errorWindow = new errorconnection(this);
    errorWindow->show();
}

void alluser::errorConnectionLogout()
{
    errorLog = new errorlogout(this);
    this->close();
    parentWidget()->close();
    errorLog->show();
}

alluser::~alluser()
{
    delete ui;
}

void alluser::listusers()
{
    std::pair p=std::make_pair<std::string, Symposium::privilege>("Carlo", Symposium::privilege::modify);
    std::pair p1=std::make_pair<std::string, Symposium::privilege>("Vincenzo", Symposium::privilege::readOnly);
    std::pair p2=std::make_pair<std::string, Symposium::privilege>("Matteo", Symposium::privilege::owner);
    std::pair p3=std::make_pair<std::string, Symposium::privilege>("Claudio", Symposium::privilege::modify);
    users.insert(p);
    users.insert(p1);
    users.insert(p2);
    users.insert(p3);
}

void alluser::insertusers()
{
    for(auto it:users)
    {
        QTreeWidgetItem *item=new QTreeWidgetItem();
        item->setText(0, QString::fromStdString(it.first));
        std::ostringstream priv;
        priv<<it.second;
        item->setText(1, QString::fromStdString(priv.str()));
        ui->tree->addTopLevelItem(item);
    }
}

void alluser::on_tree_itemClicked(QTreeWidgetItem *item, int column)
{
    username=item->text(0).toStdString();
}

void alluser::on_button_clicked()
{
    if(username!="")
    {
        QString priv;
        switch (newPrivelege)
        {
        case Symposium::privilege::owner:
            priv="owner";
            break;
        case Symposium::privilege::modify:
            priv="modify";
            break;
        case Symposium::privilege::readOnly:
            priv="read only";
            break;
        case Symposium::privilege::none:
            priv="none access";
            break;

        }
        QString text="You are sure you want to change privilege in "+priv+" to "+QString::fromStdString(username)+"?\n";
        QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Change privilege",
                                                                    text,
                                                                    QMessageBox::No | QMessageBox::Yes,
                                                                    QMessageBox::Yes);
        if (resBtn == QMessageBox::Yes)
            ;
        //cl->editPrivilege(username, newPrivelege);
    }
}

void alluser::on_owner_clicked()
{
    newPrivelege=Symposium::privilege::owner;
}

void alluser::on_modify_clicked()
{
    newPrivelege=Symposium::privilege::modify;
}

void alluser::on_reader_clicked()
{
    newPrivelege=Symposium::privilege::readOnly;
}

void alluser::on_none_clicked()
{
    newPrivelege=Symposium::privilege::none;
}
