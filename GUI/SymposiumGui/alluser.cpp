#include "alluser.h"
#include "ui_alluser.h"
#include "Dispatcher/clientdispatcher.h"
#include <QMovie>

alluser::alluser(QWidget *parent, Symposium::privilege privelege, Symposium::uint_positive_cnt::type documentID, Symposium::user user, std::string pathFile,
                 std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData>> onlineUsers,
                 std::unordered_map<std::string, Symposium::privilege> users, SymWinInterface& si) :
    QDialog(parent),
    SymModalWinInterface (si, isQDialog::isQDialogType(*this)),
    privelege(privelege), us(user), pathFile(pathFile),  documentID(documentID),  ui(new Ui::alluser), onlineUsers(onlineUsers), users(users)
{
    ui->setupUi(this);
    setFixedSize(size());
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    insertusers();
    ui->tree->setColumnCount(3);
    ui->tree->headerItem()->setText(0, "user:");
    ui->tree->headerItem()->setText(1, "privilege:");
    ui->tree->headerItem()->setText(2, "username");
    ui->tree->setColumnHidden(2, true);
    ui->tree->setColumnWidth(0, 300);
    ui->notification->hide();
    ui->errorMess->hide();
    ui->waiting->hide();
    ui->gif->hide();
    QMovie *movie = new QMovie(":/icon/ajax-loader.gif");
    ui->gif->setMovie(movie);
    movie->start();

    if(privelege==Symposium::privilege::readOnly)
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
        ui->button_2->move(185, 670);
        ui->label_2->setText("Here you can see all users\nwho have access to this file");
        ui->tree->resize(470, 530);
        ui->button_2->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    }
}

void alluser::success(){
    successEditPrivilege();
}

void alluser::failure(const QString& toPrint){
    if(toPrint=="-1"){
        errorConnectionLogout();
    }else{
        errorEditPrivilege(toPrint);
    }
}

void alluser::successEditPrivilege()
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->button->setDisabled(false);
    ui->notification->show();
    enableButtons();
    #ifdef DISPATCHER_ON
    onlineUsers=cl.onlineUser(documentID);
    users=cl.allUser(documentID);
    #endif
    ui->tree->clear();
    insertusers();


}

void alluser::errorEditPrivilege(const QString& errorMess)
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->button->setDisabled(false);
    enableButtons();
    ui->errorMess->show();
    ui->errorMess->setText(errorMess);
}

void alluser::errorConnectionLogout()
{
    ui->waiting->hide();
    ui->gif->hide();
    ui->button->setDisabled(false);
    errorlogout errorLog(this);
    disableButtons();
    int ret=errorLog.exec();
    if(ret==0)
       backToMainWin();
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
        if(us.getUsername()==it.first->getUsername())
            item->setText(0, "(you)");
        else
            item->setText(0, QString::fromStdString(it.first->getUsername()));

        for(auto it2:users)
        {
            if(it.first->getUsername()==it2.first)
            {
                std::ostringstream priv;
                priv<<it2.second;
                item->setText(1, QString::fromStdString(priv.str()));
            }
        }
        item->setText(2, QString::fromStdString(it.first->getUsername()));
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
                item->setText(2, QString::fromStdString(it.first));
                ui->tree->addTopLevelItem(item);
            }
        }
    }
}

void alluser::disableButtons()
{
    ui->button->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->button_2->setStyleSheet("background-color: grey;color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->button->setDisabled(true);
    ui->button_2->setDisabled(true);
}

void alluser::enableButtons()
{
    ui->button->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 rgb(58, 80, 116));color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->button_2->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(95, 167, 175), stop: 1 grey);color: rgb(249, 247, 241);font: 14pt 'Baskerville Old Face';border-radius:15px;");
    ui->button->setDisabled(false);
    ui->button_2->setDisabled(false);
}

void alluser::on_tree_itemClicked(QTreeWidgetItem *item, int)
{
    username=item->text(2).toStdString();
    ui->modify->click();
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
       ui->notification->hide();
       ui->errorMess->hide();
       disableButtons();
       #ifdef DISPATCHER_ON
       cl.editPrivilege(username, pathFile, newPrivelege, documentID);
       #endif
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
    backToParent();
}
