#include "onlineusers.h"
#include "ui_onlineusers.h"
#include "Dispatcher/clientdispatcher.h"
#include <QMovie>




onlineusers::onlineusers(QWidget *parent, Symposium::privilege privelege, int documentID, Symposium::user user, std::string pathFile) :
    QDialog(parent),
    pathFile(pathFile), user(user),  ui(new Ui::onlineusers),  documentID(documentID), privelege(privelege)
{
    ui->setupUi(this);
    ui->tree->setColumnCount(2);
    ui->tree->headerItem()->setText(0, "user:");
    ui->tree->headerItem()->setText(1, "privilege:");
    ui->tree->setColumnWidth(0, 200);
    this->user=Symposium::user("Vincenzo", "AP@ssw0rd!", "Vinci", ":/resources/avatar/deer.png", 4, nullptr);
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
    listusers();
    insertusers();
    ui->waiting->hide();
    ui->gif->hide();
    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();

}

onlineusers::~onlineusers()
{
    delete ui;
}


//--------------------------------------------PARTE DA CANCELLARE SUCCESSIVAMENTE PERCHE' DEVE ESSERE FATTA DAL NOTEPAD.CPP
void onlineusers::listusers()
{
    Symposium::user *u1=new Symposium::user("Mario", "AP@ssw0rd!", "Mariuz", ":/resources/avatar/beaver.png", 1, nullptr);
    Symposium::user *u2=new Symposium::user("Carlo", "AP@ssw0rd!", "Carluz", ":/resources/avatar/boar.png", 2, nullptr);
    Symposium::user *u3=new Symposium::user("Federico", "AP@ssw0rd!", "Fede", ":/resources/avatar/bull.png", 3, nullptr);
    Symposium::user *u4=new Symposium::user("Vincenzo", "AP@ssw0rd!", "Vinci", ":/resources/avatar/deer.png", 4, nullptr);
    std::pair<Symposium::user*, Symposium::sessionData> p1{u1, Symposium::sessionData(Symposium::privilege::modify)};
    std::pair<Symposium::user*, Symposium::sessionData> p2{u2, Symposium::sessionData(Symposium::privilege::modify)};
    std::pair<Symposium::user*, Symposium::sessionData> p3{u3, Symposium::sessionData(Symposium::privilege::readOnly)};
    std::pair<Symposium::user*, Symposium::sessionData> p4{u4, Symposium::sessionData(Symposium::privilege::owner)};
    onlineUsers.push_front(p1);
    onlineUsers.push_front(p2);
    onlineUsers.push_front(p3);
    onlineUsers.push_front(p4);

}
//----------------------------------------------------------------------------------------

void onlineusers::insertusers()
{
    for(auto it:onlineUsers)
    {
        QTreeWidgetItem *item=new QTreeWidgetItem();
        item->setIcon(0, QIcon(QString::fromStdString(it.first->getIconPath())));
        if(user.getUsername()==it.first->getUsername())
            item->setText(0, "(you)");
        else
            item->setText(0, QString::fromStdString(it.first->getUsername()));
        std::ostringstream priv;
        priv<<it.second.p;
        item->setText(1, QString::fromStdString(priv.str()));
        ui->tree->addTopLevelItem(item);
    }

}

void onlineusers::changeList()
{
    ui->tree->clear();
    insertusers();

}

void onlineusers::setClientDispatcher(Symposium::clientdispatcher *cl){
    this->cl = cl;
}

void onlineusers::successEditPrivilege()
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->button->setDisabled(false);
    QMessageBox::information(parentWidget(),
                             tr("Modify Privilege"), tr("The privilege was successfully modify!"), QMessageBox::Ok);

    //onlineUsers=cl->allUser(documentID);
    changeList();

}

void onlineusers::errorEditPrivilege(std::string errorMess)
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->button->setDisabled(false);
    QString error=QString::fromStdString(errorMess);
    QMessageBox::information(parentWidget(),
                             tr("Modify Privilege"), "ERROR: "+error, QMessageBox::Ok);
}

void onlineusers::errorConnection()
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->button->setDisabled(false);
    errorWindow = new errorconnection(this);
    errorWindow->exec();
}

void onlineusers::errorConnectionLogout()
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->button->setDisabled(false);
    errorLog = new errorlogout(this);
    this->close();
    parentWidget()->close();
    parentWidget()->parentWidget()->close();
    errorLog->exec();
}

void onlineusers::on_button_clicked()
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
            //cl->editPrivilege(username, pathFile, newPrivelege, documentID);
            ui->waiting->show();
            ui->gif->show();
            ui->button->setDisabled(true);

            //--------------------------------------------PARTE DA CANCELLARE SUCCESSIVAMENTE
            changeList();

            //---------------------------------------------------------------------------------
        }
}

void onlineusers::on_tree_itemClicked(QTreeWidgetItem *item, int column)
{
    username=item->text(0).toStdString();
}

void onlineusers::on_owner_clicked()
{
    newPrivelege=Symposium::privilege::owner;
}

void onlineusers::on_modify_clicked()
{
    newPrivelege=Symposium::privilege::modify;
}

void onlineusers::on_reader_clicked()
{
    newPrivelege=Symposium::privilege::readOnly;
}

void onlineusers::on_none_clicked()
{
    newPrivelege=Symposium::privilege::none;
}
