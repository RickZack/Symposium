#include "qtexteditlabels.h"
#include "Dispatcher/clientdispatcher.h"


void qtexteditlabels::scrollContentsBy(int dx, int dy)
{
    QTextEdit::scrollContentsBy(dx,dy);
    if(i==0)
    {
        i=1;
        scroll();
        for(auto it:cursors)
        {
            if(it.first==thisUserSiteId)
            {
                QTextCursor cursorUser=it.second;
                this->setTextCursor(cursorUser);
            }
        }
     }

    else{
        for(auto it:labels)
        {
            QLabel *labelCursor=it.second.first;
            QLabel *labelName=it.second.second;
            QPoint pointCurs=labelCursor->pos();
            QPoint pointName=labelName->pos();
            labelCursor->move(pointCurs.x()+dx, pointCurs.y()+dy);
            labelName->move(pointName.x()+dx, pointName.y()+dy);
        }
    }
}

void qtexteditlabels::scroll()
{
    j=0;
    for(auto it:cursors)
        {
            QTextCursor cursorUser=it.second;
            this->setTextCursor(cursorUser);
            int siteId=it.first;
            QLabel *labelCursor=labels.find(siteId)->second.first;
            QLabel *labelName=labels.find(siteId)->second.second;
            showLabel(labelCursor, labelName);
        }
    j=1;
}

void qtexteditlabels::changePosition(int block, int collumn)
{
    QTextCursor cursor = this->textCursor();
    cursor.movePosition(QTextCursor::Start);
    cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, block);
    cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, collumn);
    this->setTextCursor(cursor);
}

void qtexteditlabels::changePosition(int siteId, int block, int collumn)
{
    j=0;
    QTextCursor cursor = this->textCursor();
    changePosition(block, collumn);
    i=0;
    QLabel *labelCursor=labels.find(siteId)->second.first;
    QLabel *labelName=labels.find(siteId)->second.second;
    showLabel(labelCursor, labelName);
    cursors.find(siteId)->second=this->textCursor();
    this->setTextCursor(cursor);
    j=1;
}

void qtexteditlabels::constractLabelsCursors(std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData> > users, int siteId)
{
    j=0;
    for(auto it:users)
    {
        if(it.first->getSiteId()!=siteId && it.second.p!=Symposium::privilege::readOnly)
        {
            QString nameLabel=QString::fromStdString(it.first->getUsername());
            QLabel *labelCursor=new QLabel("|", this);
            labelCursor->setStyleSheet("color: red; font-weight: bold;");
            QLabel *newLabel=new QLabel(nameLabel, this);
            newLabel->setStyleSheet("color: red; font-size: 9px; font-weight: bold;");
            std::pair<QLabel*, QLabel*> pairs=std::make_pair(labelCursor, newLabel);
            labels.insert(std::pair<int, std::pair<QLabel*, QLabel*>>(it.first->getSiteId(), pairs));

            QTextCursor cursor = this->textCursor();
            cursor.movePosition(QTextCursor::Start);
            cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, it.second.row);
            cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, it.second.col);
            this->setTextCursor(cursor);
            showLabel(labelCursor, newLabel);
            cursors.insert(std::pair<int, QTextCursor>(it.first->getSiteId(), cursor));
        }
    }
    QTextCursor cursor = this->textCursor();
    cursor.movePosition(QTextCursor::End);
    this->setTextCursor(cursor);
    j=1;
}

void qtexteditlabels::insertCurrentUser(std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData> > users, int siteId)
{
    j=0;
    for(auto it:users)
    {
        if(it.first->getSiteId()==siteId && it.second.p!=Symposium::privilege::readOnly)
        {
            QString nameLabel=QString::fromStdString(it.first->getUsername());
            QLabel *labelCursor=new QLabel("|", this);
            labelCursor->setStyleSheet("color: red; font-weight: bold;");
            QLabel *newLabel=new QLabel(nameLabel, this);
            newLabel->setStyleSheet("color: red; font-size: 9px; font-weight: bold;");
            std::pair<QLabel*, QLabel*> pairs=std::make_pair(labelCursor, newLabel);
            labels.insert(std::pair<int, std::pair<QLabel*, QLabel*>>(it.first->getSiteId(), pairs));

            QTextCursor cursor = this->textCursor();
            cursor.movePosition(QTextCursor::Start);
            cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, it.second.row);
            cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, it.second.col);
            this->setTextCursor(cursor);
            showLabel(labelCursor, newLabel);
            cursors.insert(std::pair<int, QTextCursor>(it.first->getSiteId(), cursor));
        }
    }
    QTextCursor cursor = this->textCursor();
    cursor.movePosition(QTextCursor::End);
    this->setTextCursor(cursor);
    j=1;
}

void qtexteditlabels::showLabel(QLabel *labelCursor, QLabel *labelName)
{
    const QRect qRect = this->cursorRect();
    QFont f=this->font();
    labelCursor->setFont(f);
    labelName->show();
    labelCursor->show();

    if(qRect.left()<=this->width()-labelName->rect().width()-25)
    {
        labelCursor->move(qRect.left()-1, qRect.top());
        labelName->move(qRect.left()+4, qRect.top()-4);
    }
    else
    {
        labelCursor->move(qRect.left()-1, qRect.top());
        labelName->move(qRect.left()-labelName->rect().width(), qRect.top()-4);
    }
}

void qtexteditlabels::addUser(int siteId, std::string name)
{
    j=0;
    QString nameLabel=QString::fromStdString(name);
    QLabel *labelCursor=new QLabel("|", this);
    labelCursor->setStyleSheet("color: red; font-weight: bold;");
    QLabel *newLabel=new QLabel(nameLabel, this);
    newLabel->setStyleSheet("color: red; font-size: 9px; font-weight: bold;");
    std::pair<QLabel*, QLabel*> pairs=std::make_pair(labelCursor, newLabel);
    labels.insert(std::pair<int, std::pair<QLabel*, QLabel*>>(siteId, pairs));

    QTextCursor cursor = this->textCursor();
    changePosition(0,0);
    cursors.insert(std::pair<int, QTextCursor>(siteId, this->textCursor()));
    i=0;
    showLabel(labelCursor, newLabel);

    this->setTextCursor(cursor);
    j=1;
}

void qtexteditlabels::removeUser(int siteId)
{
    QLabel *labelHide=labels.find(siteId)->second.first;
    labelHide->hide();
    labelHide=labels.find(siteId)->second.second;
    labelHide->hide();
    labels.erase(siteId);
    cursors.erase(siteId);
}

void qtexteditlabels::thisUserChangePosition(int siteId)
{
    if(j==1)
    {
        if(priv!=Symposium::privilege::readOnly)
        {
            QLabel *labelCursor=labels.find(siteId)->second.first;
            QLabel *labelName=labels.find(siteId)->second.second;
            QTextCursor newCursor=this->textCursor();
            cursors.find(siteId)->second=this->textCursor();
            showLabel(labelCursor, labelName);
            QTextCursor cursor= this->textCursor();
            int column=cursor.positionInBlock();
            int block= cursor.blockNumber();
            //cl->moveMyCursor(int documentId, int block, int column);
        }
    }

}

void qtexteditlabels::setClientDispatcher(Symposium::clientdispatcher *cl)
{
    this->cl = cl;
}

void qtexteditlabels::setDocumentId(int docId)
{
    documentId=docId;
}

void qtexteditlabels::setThisUserSiteId(int id)
{
    thisUserSiteId=id;
}

void qtexteditlabels::setThisUserPrivilege(Symposium::privilege priv)
{
    this->priv=priv;
}
