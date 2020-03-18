#include "currentusers.h"
#include "ui_currentusers.h"



currentUsers::currentUsers(QWidget *parent, bool modify) :
    QDialog(parent),
    ui(new Ui::currentUsers), modify(modify)
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
    userPrivilege();
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
        QVariant v;
        v.setValue(it.first->getSiteId());
        std::string nick=it.first->getNickname();
        if(it.first->getUsername()==user->getUsername())
            nick="You";
        nick=nick+" with a privilege ";
        std::ostringstream priv;
        priv<<it.second.p;
        nick=nick+priv.str();
        std::string icon=it.first->getIconPath();
        QListWidgetItem *item=new QListWidgetItem(QIcon(QString::fromStdString(icon)), QString::fromStdString(nick));
        item->setData(Qt::UserRole,v);
        ui->userslist->addItem(item);
    }
    */
    //---------------------------------------------------------

    //--------------------------------------------PARTE DA CANCELLARE SUCCESSIVAMENTE
    for(auto it: activeUsers)
    {
        QVariant v;
        v.setValue(it.first->getSiteId());
        std::string nick=it.first->getNickname();
        if(it.first->getUsername()==user->getUsername())
            nick="You";
        nick=nick+" with a privilege ";
        std::ostringstream priv;
        priv<<it.second;
        nick=nick+priv.str();
        std::string icon=it.first->getIconPath();
        QListWidgetItem *item=new QListWidgetItem(QIcon(QString::fromStdString(icon)), QString::fromStdString(nick));
        item->setData(Qt::UserRole,v);
        ui->userslist->addItem(item);

    }

    //----------------------------------------------------------------------------------------
}

void currentUsers::on_userslist_itemClicked(QListWidgetItem *item)
{
    QVariant v = item->data(Qt::UserRole);

    if(privilegeUser==Symposium::privilege::owner && modify)
    {
        ui->label->show();
        ui->button->setDisabled(false);
        ui->owner->show();
        ui->owner->setDisabled(false);
        ui->reader->show();
        ui->reader->setDisabled(false);
        ui->modify->show();
        ui->modify->setDisabled(false);
        ui->none->show();
        ui->none->setDisabled(false);
        ui->button->show();
        ui->owner->click();
    }
    userToChangeSiteId = v.value<int>();

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
    QString nick;
    std::string username;
    QString priv;
    for(auto it: activeUsers)
    {
        if(userToChangeSiteId==it.first->getSiteId())
        {
            nick=QString::fromStdString(it.first->getNickname());
            username=it.first->getUsername();
        }

    }

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

    QString text="You are sure you want to change privilege in "+priv+" to "+nick+"?\n";
    //std::string str=text.toStdString();
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Change privilege",
                                                                    text,
                                                                     QMessageBox::No | QMessageBox::Yes,
                                                                    QMessageBox::Yes);
        if (resBtn == QMessageBox::Yes)
        {
            //cl->editPrivilege(username, newPrivelege);

            //--------------------------------------------PARTE DA CANCELLARE SUCCESSIVAMENTE
            changeUserPrivilege();
            changeList();
            //---------------------------------------------------------------------------------
        }

}






//--------------------------------------------PARTE DA CANCELLARE SUCCESSIVAMENTE
void currentUsers::changeUserPrivilege()
{
    std::pair<Symposium::user *, Symposium::privilege> p;
    bool changeOk=false;
    for(auto it: activeUsers)
    {
        if(it.first->getSiteId()==userToChangeSiteId)
        {
            if(it.second!=newPrivelege)
            { p=it;
              p.second=newPrivelege;
              changeOk=true;}
        }
    }
    if(changeOk)
    {
        activeUsers.remove_if([this](std::pair<Symposium::user *, Symposium::privilege> it)
        {return it.first->getSiteId()==userToChangeSiteId && newPrivelege!=it.second;});
        activeUsers.push_front(p);
    }

}
//---------------------------------------------------------------------------------

void currentUsers::changeList()
{
     //---------------------------------------------PARTE DA DECOMENTARE

    /*
     ui->userslist->clear();
    for(auto it: onlineUsers)
    {
        QVariant v;
        v.setValue(it.first->getSiteId());
        std::string nick=it.first->getNickname();
        if(it.first->getUsername()==user->getUsername())
            nick="You";
        nick=nick+" with a privilege ";
        std::ostringstream priv;
        priv<<it.second.p;
        nick=nick+priv.str();
        std::string icon=it.first->getIconPath();
        QListWidgetItem *item=new QListWidgetItem(QIcon(QString::fromStdString(icon)), QString::fromStdString(nick));
        item->setData(Qt::UserRole,v);
        ui->userslist->addItem(item);
    }
     */


    //-------------------------------------------------------------------



    //--------------------------------------------PARTE DA CANCELLARE SUCCESSIVAMENTE
    ui->userslist->clear();
    for(auto it: activeUsers)
    {
        QVariant v;
        v.setValue(it.first->getSiteId());
        std::string nick=it.first->getNickname();
        if(it.first->getUsername()==user->getUsername())
            nick="You";
        nick=nick+" with a privilege ";
        std::ostringstream priv;
        priv<<it.second;
        nick=nick+priv.str();
        std::string icon=it.first->getIconPath();
        QListWidgetItem *item=new QListWidgetItem(QIcon(QString::fromStdString(icon)), QString::fromStdString(nick));
        item->setData(Qt::UserRole,v);
        ui->userslist->addItem(item);

    }
    //---------------------------------------------------------------------------------
}

