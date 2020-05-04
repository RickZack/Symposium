#include "alluser.h"
#include "ui_alluser.h"
#include "Dispatcher/clientdispatcher.h"
#include <QMovie>

alluser::alluser(QWidget *parent, Symposium::privilege privelege, Symposium::uint_positive_cnt::type documentID, Symposium::user user, std::string pathFile,
                 std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData>> onlineUsers,
                 std::unordered_map<std::string, Symposium::privilege> users) :
    QDialog(parent),
    privelege(privelege), us(user), pathFile(pathFile),  documentID(documentID),  ui(new Ui::alluser), onlineUsers(onlineUsers), users(users)
{
    ui->setupUi(this);
    setFixedSize(size());
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    insertusers();
    ui->tree->setColumnCount(2);
    ui->tree->headerItem()->setText(0, "user:");
    ui->tree->headerItem()->setText(1, "privilege:");
    ui->tree->setColumnWidth(0, 300);
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
    ui->waiting->hide();
    ui->gif->hide();
    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();
}

void alluser::setClientDispatcher(Symposium::clientdispatcher *cl)
{
    this->cl = cl;
}

void alluser::successEditPrivilege()
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->button->setDisabled(false);
    QMessageBox::information(parentWidget(),
                             tr("Modify Privilege"), tr("The privilege was successfully modify!"), QMessageBox::Ok);

    //onlineUsers=cl->allUser(documentID);
    ui->tree->clear();
    insertusers();


}

void alluser::errorEditPrivilege(std::string errorMess)
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->button->setDisabled(false);
    QString error=QString::fromStdString(errorMess);
    QMessageBox::information(parentWidget(),
                             tr("Modify Privilege"), "ERROR: "+error, QMessageBox::Ok);
}

void alluser::errorConnection()
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->button->setDisabled(false);
    errorWindow = new errorconnection(this);
    errorWindow->exec();
}

void alluser::errorConnectionLogout(std::string str)
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->button->setDisabled(false);
    errorLog = new errorlogout(this, QString::fromStdString(str));
    this->close();
    parentWidget()->close();
    parentWidget()->parentWidget()->close();
    errorLog->exec();
}

alluser::~alluser()
{
    delete ui;
}


void alluser::insertusers()
{
    for(auto it:onlineUsers)
    {
        QTreeWidgetItem *item=new QTreeWidgetItem();
        item->setIcon(0, QIcon(QString::fromStdString(it.first->getIconPath())));
        qDebug()<<QString::fromStdString(us.getUsername());
        if(us.getUsername()==it.first->getUsername())
            item->setText(0, "(you)");
        else
            item->setText(0, QString::fromStdString(it.first->getUsername()));
        std::ostringstream priv;
        priv<<it.second.p;
        item->setText(1, QString::fromStdString(priv.str()));
        ui->tree->addTopLevelItem(item);
    }

    for(auto it:users)
    {
        bool online=false;
        for(auto it2:onlineUsers)
        {
            if(it.first==it2.first->getUsername())
                online=true;
        }
        if(!online)
        {
            if(it.second!=Symposium::privilege::none)
            {
                QTreeWidgetItem *item=new QTreeWidgetItem();
                item->setIcon(0, QIcon(":/icon/offline.png"));
                item->setText(0, QString::fromStdString(it.first)+" (offline)");
                std::ostringstream priv;
                priv<<it.second;
                item->setText(1, QString::fromStdString(priv.str()));
                ui->tree->addTopLevelItem(item);
            }
        }
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
       ui->waiting->show();
       ui->gif->show();
       ui->button->setDisabled(true);
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

void alluser::on_button_2_clicked()
{
    close();
}
