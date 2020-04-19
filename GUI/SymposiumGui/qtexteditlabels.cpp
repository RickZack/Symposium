#include "qtexteditlabels.h"
#include "Dispatcher/clientdispatcher.h"
#include <QDebug>


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
            Symposium::uint_positive_cnt::type siteId=it.first;
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

void qtexteditlabels::changePosition(Symposium::uint_positive_cnt::type siteId, int block, int collumn)
{
    j=0;
    QTextCursor cursor = this->textCursor();
    changePosition(block, collumn);
    i=0;
    qreal num=this->fontPointSize();
    //Color c=cl->getColor(documentId,it.first->getSiteId());
    //QString str=QString::fromStdString(c.rgb_hex_string());
    QString str="#ff0000";
    QLabel *labelCursor=new QLabel("|", this);
    labelCursor->setStyleSheet("color:  "+str+ "; font-size: "+QString::number(static_cast<int>(num+num/5.0))+"px;");
    QLabel *labelHide=labels.find(siteId)->second.first;
    labelHide->deleteLater();
    labels.find(siteId)->second.first=labelCursor;
    QLabel *labelName=labels.find(siteId)->second.second;
    showLabel(labelCursor, labelName);
    cursors.find(siteId)->second=this->textCursor();
    this->setTextCursor(cursor);
    j=1;
}



void qtexteditlabels::constractLabelsCursors(std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData> > users, Symposium::uint_positive_cnt::type siteId)
{
    j=0;
    for(auto it:users)
    {
        if(it.first->getSiteId()!=siteId && it.second.p!=Symposium::privilege::readOnly)
        {
            QTextCursor cursor = this->textCursor();
            cursor.movePosition(QTextCursor::Start);
            cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, static_cast<int>(it.second.row));
            cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, static_cast<int>(it.second.col));
            this->setTextCursor(cursor);

            QString nameLabel=QString::fromStdString(it.first->getUsername());
            QLabel *labelCursor=new QLabel("|", this);
            qreal num=this->fontPointSize();
            //Color c=cl->getColor(documentId,it.first->getSiteId());
            //QString str=QString::fromStdString(c.rgb_hex_string());
            QString str="#ff0000";
            labelCursor->setStyleSheet("color:  "+str+ "; font-size: "+QString::number(static_cast<int>(num+num/5.0))+"px;");
            QLabel *newLabel=new QLabel(nameLabel, this);
            newLabel->setStyleSheet("color: "+str+ "; font-size: 9px; font-weight: bold;");
            std::pair<QLabel*, QLabel*> pairs=std::make_pair(labelCursor, newLabel);
            labels.insert(std::pair<Symposium::uint_positive_cnt::type, std::pair<QLabel*, QLabel*>>(it.first->getSiteId(), pairs));


            showLabel(labelCursor, newLabel);
            cursors.insert(std::pair<Symposium::uint_positive_cnt::type, QTextCursor>(it.first->getSiteId(), cursor));
        }
    }
    QTextCursor cursor = this->textCursor();
    cursor.movePosition(QTextCursor::End);
    this->setTextCursor(cursor);
    scroll();
    j=1;
}

void qtexteditlabels::insertCurrentUser(std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData> > users, Symposium::uint_positive_cnt::type siteId)
{
    j=0;
    for(auto it:users)
    {
        if(it.first->getSiteId()==siteId && it.second.p!=Symposium::privilege::readOnly)
        {
            QTextCursor cursor = this->textCursor();
            cursor.movePosition(QTextCursor::Start);
            cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, static_cast<int>(it.second.row));
            cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, static_cast<int>(it.second.col));
            QString nameLabel=QString::fromStdString(it.first->getUsername());
            QLabel *labelCursor=new QLabel("|", this);
            qreal num=this->fontPointSize();
            //Color c=cl->getColor(documentId,it.first->getSiteId());
            //QString str=QString::fromStdString(c.rgb_hex_string());
            QString str="#ff0000";
            labelCursor->setStyleSheet("color:  "+str+ "; font-size: "+QString::number(static_cast<int>(num+num/5.0))+"px;");
            QLabel *newLabel=new QLabel(nameLabel, this);
            newLabel->setStyleSheet("color: "+str+ "; font-size: 9px; font-weight: bold;");
            std::pair<QLabel*, QLabel*> pairs=std::make_pair(labelCursor, newLabel);
            labels.insert(std::pair<Symposium::uint_positive_cnt::type, std::pair<QLabel*, QLabel*>>(it.first->getSiteId(), pairs));

            this->setTextCursor(cursor);
            showLabel(labelCursor, newLabel);
            cursors.insert(std::pair<Symposium::uint_positive_cnt::type, QTextCursor>(it.first->getSiteId(), cursor));
        }
    }
    QTextCursor cursor = this->textCursor();
    cursor.movePosition(QTextCursor::End);
    this->setTextCursor(cursor);
    j=1;
}

void qtexteditlabels::showLabel(QLabel *labelCursor, QLabel *labelName)
{
    static double minsize=1000;
    // Obtain size of original cursor
    const QRect qRect = this->cursorRect();
    int x=qRect.height();

    // Obtain size of current font
    QFont font=this->font();
    font.setPointSize(this->fontPointSize());
    QFontMetrics fm(font);
    int y=fm.height();

    //Correction for small characters
    minsize= y<minsize? y:minsize;

    //Top edge where to start drawing the cursor
    int ty=qRect.top()+(x-y)/1.18-minsize/7.0;

    labelName->show();
    labelCursor->show();

    if(qRect.left()<=this->width()-labelName->rect().width()-25)
    {
        labelCursor->move(qRect.topLeft().x()-1, ty);
        labelName->move(qRect.topLeft().x()+5, ty);
    }
    else
    {
        labelCursor->move(qRect.topLeft().x()-1, ty);
        labelName->move(qRect.topLeft().x()-labelName->rect().width(), ty);
    }
}

void qtexteditlabels::addUser(Symposium::uint_positive_cnt::type siteId, std::string name)
{
    j=0;
    QTextCursor cursor = this->textCursor();
    changePosition(0,0);
    cursors.insert(std::pair<Symposium::uint_positive_cnt::type, QTextCursor>(siteId, this->textCursor()));
    QString nameLabel=QString::fromStdString(name);
    QLabel *labelCursor=new QLabel("|", this);
    qreal num=this->fontPointSize();
    //Color c=cl->getColor(documentId,it.first->getSiteId());
    //QString str=QString::fromStdString(c.rgb_hex_string());
    QString str="#ff0000";
    labelCursor->setStyleSheet("color:  "+str+ "; font-size: "+QString::number(static_cast<int>(num+num/5.0))+"px;");
    QLabel *newLabel=new QLabel(nameLabel, this);
    newLabel->setStyleSheet("color: "+str+ "; font-size: 9px; font-weight: bold;");
    std::pair<QLabel*, QLabel*> pairs=std::make_pair(labelCursor, newLabel);
    labels.insert(std::pair<Symposium::uint_positive_cnt::type, std::pair<QLabel*, QLabel*>>(siteId, pairs));

    i=0;
    showLabel(labelCursor, newLabel);

    this->setTextCursor(cursor);
    j=1;
}

void qtexteditlabels::removeUser(Symposium::uint_positive_cnt::type siteId)
{
    if(labels.find(siteId)!=labels.end())
    {
        QLabel *labelHide=labels.find(siteId)->second.first;
        labelHide->hide();
        labelHide=labels.find(siteId)->second.second;
        labelHide->hide();
        labels.erase(siteId);
        cursors.erase(siteId);
    }
}

void qtexteditlabels::thisUserChangePosition(Symposium::uint_positive_cnt::type siteId)
{
    if(j==1)
    {
        if(priv!=Symposium::privilege::readOnly)
        {
            qreal num=this->fontPointSize();
            //Color c=cl->getColor(documentId,it.first->getSiteId());
            //QString str=QString::fromStdString(c.rgb_hex_string());
            QString str="#ff0000";
            QLabel *labelCursor=new QLabel("|", this);
            labelCursor->setStyleSheet("color:  "+str+ "; font-size: "+QString::number(static_cast<int>(num+num/5.0))+"px;");
            QLabel *labelName=labels.find(siteId)->second.second;
            QLabel *labelHide=labels.find(siteId)->second.first;
            labelHide->deleteLater();
            labels.find(siteId)->second.first=labelCursor;
            QTextCursor newCursor=this->textCursor();
            cursors.find(siteId)->second=this->textCursor();
            showLabel(labelCursor, labelName);
            QTextCursor cursor= this->textCursor();
            //int column=cursor.positionInBlock();
            //int block= cursor.blockNumber();
            //cl->moveMyCursor(int documentId, int block, int column);
        }
    }

}

void qtexteditlabels::setClientDispatcher(Symposium::clientdispatcher *cl)
{
    this->cl = cl;
}

void qtexteditlabels::setDocumentId(Symposium::uint_positive_cnt::type docId)
{
    documentId=docId;
}

void qtexteditlabels::setThisUserSiteId(Symposium::uint_positive_cnt::type id)
{
    thisUserSiteId=id;
}

void qtexteditlabels::setThisUserPrivilege(Symposium::privilege priv)
{
    this->priv=priv;
}
