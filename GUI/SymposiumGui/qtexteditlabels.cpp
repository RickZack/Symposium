#include "qtexteditlabels.h"
#include "Dispatcher/clientdispatcher.h"
#include "notepad.h"


void qtexteditlabels::scrollContentsBy(int dx, int dy){
    QTextEdit::scrollContentsBy(dx,dy);
    for(auto it:labels)
    {
        QLabel *labelNameReverse=it.second.first;
        QLabel *labelName=it.second.second;
        QPoint pointNaemReverse=labelNameReverse->pos();
        QPoint pointName=labelName->pos();
        labelNameReverse->move(pointNaemReverse.x()+dx, pointNaemReverse.y()+dy);
        labelName->move(pointName.x()+dx, pointName.y()+dy);
    }
}

void qtexteditlabels::scroll(){
    j=0;
    int block=this->textCursor().blockNumber();
    int column=this->textCursor().positionInBlock();
    for(const auto& it:cursors)
        {
        if(it.first==thisUserSiteId)
        {
           block=it.second.first;
           column=it.second.second;
        }
        changePosition(it.second.first, it.second.second);
        Symposium::uint_positive_cnt::type siteId=it.first;
        QLabel *labelNameReverse=labels.find(siteId)->second.first;
        QLabel *labelName=labels.find(siteId)->second.second;
        showLabel(labelNameReverse, labelName);
        }
    changePosition(block, column);
    j=1;
}

void qtexteditlabels::changePosition(int block, int collumn){
    QTextCursor cursor = this->textCursor();
    cursor.movePosition(QTextCursor::Start);
    cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, block);
    cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, collumn);
    this->setTextCursor(cursor);
}

void qtexteditlabels::changePosition(Symposium::uint_positive_cnt::type siteId, int block, int collumn){
    j=0;
    auto pairlabels=labels.find(siteId);
    if(pairlabels!=labels.end())
    {
        int blockThisUser=this->textCursor().blockNumber();
        int columnThisUser=this->textCursor().positionInBlock();
        changePosition(block, collumn);
        QLabel *labelReverseName=pairlabels->second.first;
        QLabel *labelName=pairlabels->second.second;
        showLabel(labelReverseName, labelName);
        cursors.find(siteId)->second.first=block;
        cursors.find(siteId)->second.second=collumn;
        changePosition(blockThisUser, columnThisUser);
    }
    j=1;
}

void qtexteditlabels::constractLabelsCursors(std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData> > users, Symposium::uint_positive_cnt::type siteId){
    j=0;
    for(const auto& it:users)
    {
        if(it.first->getSiteId()!=siteId && it.second.p!=Symposium::privilege::readOnly)
        {
            QTextCursor cursor = this->textCursor();
            cursor.movePosition(QTextCursor::Start);
            cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, static_cast<int>(it.second.row));
            cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, static_cast<int>(it.second.col));
            this->setTextCursor(cursor);

            QString nameLabel="\u25BC"+QString::fromStdString(it.first->getUsername());
            QString nameLabelReverse=QString::fromStdString(it.first->getUsername())+"\u25BC";
            QLabel *labelReverse=new QLabel(nameLabelReverse, this);

            Symposium::Color c=cl->getColor(documentId,it.first->getSiteId());
            QString str=QString::fromStdString(c.rgb_hex_string());
            labelReverse->setStyleSheet("color:  "+str+ "; font-size: 9px; font-weight: bold;");
            QLabel *newLabel=new QLabel(nameLabel, this);
            newLabel->setStyleSheet("color: "+str+ "; font-size: 9px; font-weight: bold;");
            std::pair<QLabel*, QLabel*> pairs=std::make_pair(labelReverse, newLabel);
            labels.insert(std::pair<Symposium::uint_positive_cnt::type, std::pair<QLabel*, QLabel*>>(it.first->getSiteId(), pairs));
            labelReverse->setAttribute(Qt::WA_TranslucentBackground);
            newLabel->setAttribute(Qt::WA_TranslucentBackground);
            showLabel(labelReverse, newLabel);
            cursors.insert(std::pair<Symposium::uint_positive_cnt::type, std::pair<int, int>>
                           (it.first->getSiteId(), std::pair<int, int>(static_cast<int>(it.second.row),
                                                                       static_cast<int>(it.second.col))));
        }
    }
    QTextCursor cursor = this->textCursor();
    cursor.movePosition(QTextCursor::Start);
    this->setTextCursor(cursor);
    j=1;
}

void qtexteditlabels::insertCurrentUser(std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData> > users, Symposium::uint_positive_cnt::type siteId){
    j=0;
    int block=0;
    int column=0;
    thisUserSiteId=siteId;
    for(const auto& it:users)
    {
        if(it.first->getSiteId()==siteId && priv!=Symposium::privilege::readOnly)
        {
            QString nameLabel="\u25BC"+QString::fromStdString(it.first->getUsername());
            QString nameLabelReverse=QString::fromStdString(it.first->getUsername())+"\u25BC";
            QLabel *labelReverse=new QLabel(nameLabelReverse, this);
            Symposium::Color c=cl->getColor(documentId,it.first->getSiteId());
            std::string colorHex(c.rgb_hex_string());
            QString str=QString::fromStdString(colorHex);
            labelReverse->setStyleSheet("color:  "+str+ "; font-size: 9px; font-weight: bold;");
            QLabel *newLabel=new QLabel(nameLabel, this);
            newLabel->setStyleSheet("color: "+str+ "; font-size: 9px; font-weight: bold;");
            std::pair<QLabel*, QLabel*> pairs=std::make_pair(labelReverse, newLabel);
            labelReverse->setAttribute(Qt::WA_TranslucentBackground);
            newLabel->setAttribute(Qt::WA_TranslucentBackground);
            labels.insert(std::pair<Symposium::uint_positive_cnt::type, std::pair<QLabel*, QLabel*>>(it.first->getSiteId(), pairs));
            showLabel(labelReverse, newLabel);
        }

    }
    moveCursor(QTextCursor::Start);
    cursors.insert(std::pair<Symposium::uint_positive_cnt::type, std::pair<int, int>>
                   (thisUserSiteId, std::pair<int, int>(block,column)));
    j=1;
}

void qtexteditlabels::showLabel(QLabel *labelNameReverse, QLabel *labelName){
    const QRect curRect = this->cursorRect(this->textCursor());
    int ty=curRect.y()-2;
    labelName->hide();
    labelNameReverse->hide();

    if(curRect.left()<=this->width()-labelName->rect().width()-25){
        labelName->move(curRect.topLeft().x()-3, ty);
        labelName->show();
    }else{
        labelNameReverse->move(curRect.topLeft().x()-labelName->rect().width()+3, ty);
        labelNameReverse->show();
    }
}

void qtexteditlabels::addUser(Symposium::uint_positive_cnt::type siteId, std::string name){
    j=0;
    int blockThisUser=this->textCursor().blockNumber();
    int columnThisUser=this->textCursor().positionInBlock();
    changePosition(0,0);
    cursors.insert(std::pair<Symposium::uint_positive_cnt::type, std::pair<int, int>>(siteId, std::pair<int, int>(0,0)));
    QString nameLabel="\u25BC"+QString::fromStdString(name);
    QString nameLabelReverse=QString::fromStdString(name)+"\u25BC";
    QLabel *labelNameReverse=new QLabel(nameLabelReverse, this);

    Symposium::Color c=cl->getColor(documentId, siteId);
    QString str=QString::fromStdString(c.rgb_hex_string());
    labelNameReverse->setStyleSheet("color:  "+str+ "; font-size: 9px; font-weight: bold;");
    QLabel *newLabel=new QLabel(nameLabel, this);
    newLabel->setStyleSheet("color: "+str+ "; font-size: 9px; font-weight: bold;");
    std::pair<QLabel*, QLabel*> pairs=std::make_pair(labelNameReverse, newLabel);
    labels.insert(std::pair<Symposium::uint_positive_cnt::type, std::pair<QLabel*, QLabel*>>(siteId, pairs));

    labelNameReverse->setAttribute(Qt::WA_TranslucentBackground);
    newLabel->setAttribute(Qt::WA_TranslucentBackground);
    showLabel(labelNameReverse, newLabel);

    changePosition(blockThisUser, columnThisUser);
    j=1;
}

void qtexteditlabels::removeUser(Symposium::uint_positive_cnt::type siteId){
    if(labels.find(siteId)!=labels.end())
    {
        QLabel *labelHide=labels.find(siteId)->second.first;
        labelHide->hide();
        labelHide=labels.find(siteId)->second.second;
        labelHide->hide();
        labelHide->deleteLater();
        labels.erase(siteId);
        cursors.erase(siteId);
    }
}

void qtexteditlabels::thisUserChangePosition(Symposium::uint_positive_cnt::type siteId){
    if(isReadOnly()) return;
    if(j==1)
    {
        if(priv!=Symposium::privilege::readOnly)
        {
            QLabel *labelName=labels.find(siteId)->second.second;
            QLabel *labelReverseName=labels.find(siteId)->second.first;
            QTextCursor newCursor=this->textCursor();
            int newBlock=newCursor.blockNumber();
            int newColumn=newCursor.positionInBlock();
            cursors.find(siteId)->second.first=newBlock;
            cursors.find(siteId)->second.second=newColumn;
            showLabel(labelReverseName, labelName);
            QTextCursor cursor= this->textCursor();

            int column=cursor.positionInBlock();
            int block= cursor.blockNumber();
            cl->moveMyCursor(documentId,block,column);
        }
    }

}

void qtexteditlabels::setClientDispatcher(Symposium::clientdispatcher *cl){
    this->cl = cl;
}

void qtexteditlabels::setDocumentId(Symposium::uint_positive_cnt::type docId){
    documentId=docId;
}

void qtexteditlabels::setThisUserSiteId(Symposium::uint_positive_cnt::type id){
    thisUserSiteId=id;
}

void qtexteditlabels::setThisUserPrivilege(Symposium::privilege priv){
    this->priv=priv;
}

void qtexteditlabels::translateCursors(const std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData> >& users){
    for(const auto& it:users)
    {
        if(it.second.p!=Symposium::privilege::readOnly)
        {
            int oldBlock=cursors.find(it.first->getSiteId())->second.first;
            int oldColumn=cursors.find(it.first->getSiteId())->second.second;
            int newBlock=static_cast<int>(it.second.row);
            int newColumn=static_cast<int>(it.second.col);
            if(oldBlock!=newBlock || oldColumn!=newColumn){
                cursors.find(it.first->getSiteId())->second.first=newBlock;
                cursors.find(it.first->getSiteId())->second.second=newColumn;
            }
        }
    }
    scroll();
}

void qtexteditlabels::insertFromMimeData(const QMimeData *source) {
    if(isReadOnly()) return;
    QTextCursor cursor = textCursor();
    QTextCharFormat fmt=cursor.charFormat();
    QColor curCol = fmt.foreground().color();
    QColor lighter = curCol;
    lighter.setAlpha(notepad::alphaValue);
    this->setTextColor(lighter);

    int row=cursor.blockNumber();
    int col=cursor.positionInBlock();

    if (source->hasText()) {
        QString sourceText = source->text();
        QString toInsert;

        for(auto& x:sourceText){
            if (x == '\n' || x == "\u2028" || x == "\u2029") {
                x = '\r';
            }
            else if(!x.isPrint()){
                continue;
            }
            toInsert.push_back(x);
        }
        cursor.insertText(toInsert, fmt);
        for (auto& x : toInsert) {
            if (x == '\r') {
                n->sendSymbolToInsert(row, col, QString(x), fmt);
                row++;
                col=0;
            }
            else{
                n->sendSymbolToInsert(row, col, QString(x), fmt);
                col++;
            }
        }
    } else
        return;
}

void qtexteditlabels::moveCursorLabelToEnd() {
    if(this->isReadOnly()) return;
    this->textCursor().movePosition(QTextCursor::End);
    thisUserChangePosition(thisUserSiteId);
    QTextEdit::selectAll();
}
