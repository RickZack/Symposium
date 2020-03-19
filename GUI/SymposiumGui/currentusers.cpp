#include "currentusers.h"
#include "ui_currentusers.h"
#include "Dispatcher/clientdispatcher.h"


currentUsers::currentUsers(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::currentUsers){

    userPrivilege();
    if(privilegeUser!=Symposium::privilege::owner)
    {
        ui->setupUi(this);
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
    ui->tree->setColumnCount(3);
    ui->tree->headerItem()->setText(0, "");
    ui->tree->headerItem()->setText(1, "username:");
    ui->tree->headerItem()->setText(2, "privilege:");
    ui->tree->header()->setVisible(true);
    ui->tree->setColumnWidth(0, 10);
    listusers();
    insertusers();

}

currentUsers::~currentUsers()
{
    delete ui;
}

void currentUsers::setClientDispatcher(Symposium::clientdispatcher *cl){
    this->cl = cl;
}

void currentUsers::successEditPrivilege()
{
    QMessageBox::information(parentWidget(),
                             tr("Modify Privilege"), tr("The privilege was successfully modify!"), QMessageBox::Ok);

    //onlineUsers=cl->allUser(documentID);
    changeList();

}

void currentUsers::errorEditPrivilege(std::string errorMess)
{
    QString error=QString::fromStdString(errorMess);
    QMessageBox::information(parentWidget(),
                             tr("Modify Privilege"), error, QMessageBox::Ok);
}

void currentUsers::errorConnection()
{
    errorWindow = new errorconnection(this);
    errorWindow->show();
}

void currentUsers::errorConnectionLogout()
{
    errorLog = new errorlogout(this);
    this->close();
    parentWidget()->close();
    errorLog->show();
}

void currentUsers::insertusers()
{
    //---------------------------------------------PARTE DA DECOMENTARE

   /*

    for(auto it: onlineUsers)
    {
        QTreeWidgetItem *item=new QTreeWidgetItem();
        std::string icon=it.first->getIconPath();
        item->setIcon(0, QIcon(QString::fromStdString(icon)));
        if(it.first->getUsername()==user->getUsername())
            item->setText(1, QString::fromStdString(it.first->getUsername())+" (you)");
        else
            item->setText(1, QString::fromStdString(it.first->getUsername()));
        std::ostringstream priv;
        priv<<it.second.p;
        item->setText(2, QString::fromStdString(priv.str()));
        ui->tree->addTopLevelItem(item);

    }
    */
    //---------------------------------------------------------

    //--------------------------------------------PARTE DA CANCELLARE SUCCESSIVAMENTE

    for(auto it: activeUsers)
    {
        QTreeWidgetItem *item=new QTreeWidgetItem();
        std::string icon=it.first->getIconPath();
        item->setIcon(0, QIcon(QString::fromStdString(icon)));
        if(it.first->getUsername()==user->getUsername())
            item->setText(1, QString::fromStdString(it.first->getUsername())+" (you)");
        else
            item->setText(1, QString::fromStdString(it.first->getUsername()));
        std::ostringstream priv;
        priv<<it.second;
        item->setText(2, QString::fromStdString(priv.str()));
        ui->tree->addTopLevelItem(item);

    }

    //----------------------------------------------------------------------------------------
}


//--------------------------------------------PARTE DA CANCELLARE SUCCESSIVAMENTE PERCHE' DEVE ESSERE FATTA DAL NOTEPAD.CPP
void currentUsers::listusers()
{
    Symposium::user *u1=new Symposium::user("Mario", "AP@ssw0rd!", "Mariuz", ":/resources/avatar/beaver.png", 1, nullptr);
    Symposium::user *u2=new Symposium::user("Carlo", "AP@ssw0rd!", "Carluz", ":/resources/avatar/boar.png", 2, nullptr);
    Symposium::user *u3=new Symposium::user("Federico", "AP@ssw0rd!", "Fede", ":/resources/avatar/bull.png", 3, nullptr);
    std::pair<Symposium::user*, Symposium::privilege> p1{u1, Symposium::privilege::owner};
    std::pair<Symposium::user*, Symposium::privilege> p2{u2, Symposium::privilege::modify};
    std::pair<Symposium::user*, Symposium::privilege> p3{u3, Symposium::privilege::readOnly};
    std::pair<Symposium::user*, Symposium::privilege> p4{user, Symposium::privilege::owner};
    activeUsers.push_front(p1);
    activeUsers.push_front(p2);
    activeUsers.push_front(p3);
    activeUsers.push_front(p4);    
}
//----------------------------------------------------------------------------------------


void currentUsers::userPrivilege(){
    privilegeUser=Symposium::privilege::owner;
}



void currentUsers::on_owner_clicked()
{
    newPrivelege=Symposium::privilege::owner;
}

void currentUsers::on_modify_clicked()
{
    newPrivelege=Symposium::privilege::modify;
}

void currentUsers::on_reader_clicked()
{
    newPrivelege=Symposium::privilege::readOnly;
}

void currentUsers::on_none_clicked()
{
    newPrivelege=Symposium::privilege::none;
}

void currentUsers::on_button_clicked()
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
        {
            //cl->editPrivilege(username, newPrivelege);

            //--------------------------------------------PARTE DA CANCELLARE SUCCESSIVAMENTE
            changeList();
            //---------------------------------------------------------------------------------
        }

}



void currentUsers::changeList()
{
    ui->tree->clear();
    for(auto it: onlineUsers)
    {
        QTreeWidgetItem *item=new QTreeWidgetItem();
        std::string icon=it.first->getIconPath();
        item->setIcon(0, QIcon(QString::fromStdString(icon)));
        if(it.first->getUsername()==user->getUsername())
            item->setText(1, QString::fromStdString(it.first->getUsername())+" (you)");
        else
            item->setText(1, QString::fromStdString(it.first->getUsername()));
        std::ostringstream priv;
        priv<<it.second.p;
        item->setText(2, QString::fromStdString(priv.str()));
        ui->tree->addTopLevelItem(item);

    }
}


void currentUsers::on_tree_itemClicked(QTreeWidgetItem *item, int column)
{
    username=item->text(1).toStdString();
}
