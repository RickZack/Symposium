#include "symwininterface.h"
#include "winmanager.h"
#include<QWidget>


SymWinInterface::SymWinInterface(SymWinManager &m, clientDispatcher &p): handler(m), cl(p), forceQuit(false)
{
}

bool SymWinInterface::closedByUser() const
{
    return forceQuit==false;
}

SymWinInterface::operator QWidget*()
{
    QWidget* asQWidget=dynamic_cast<QWidget*>(this);
    if(asQWidget==nullptr)
        throw std::logic_error("This is not a QWidget");
    return asQWidget;
}

void SymChildWinInterface::backToParent()
{
    goToWindow(*s_parent);
    this->forceQuit=true;
    this->operator QWidget *()->close();
}

void SymChildWinInterface::backToMainWin()
{
    SymWinInterface* p=this;
    for(; p->s_parent!=nullptr; p=p->s_parent){
        p->forceQuit=true;
        QWidget* asWidget=p->operator QWidget *();
        asWidget->close();
    }
    handler.setActive(*p);
    p->operator QWidget *()->show();
}

void SymWinInterface::goToWindow(SymWinInterface &nextWin)
{
    QWidget* thisAsQWidget=this->operator QWidget *();
    QWidget* nextWinAsWidget=nextWin.operator QWidget *();

    thisAsQWidget->hide();
    handler.setActive(nextWin);
    nextWinAsWidget->showNormal();
}

void SymWinInterface::goToWindow(SymNotepadWinInterface& notepad)
{
    SymWinInterface::goToWindow(static_cast<SymWinInterface&>(notepad));
    handler.addNotepad(notepad.getId(), notepad);
}


SymMainWinInterface::SymMainWinInterface(SymWinManager &m, clientDispatcher& p, isQWidget::QWidgetType): SymWinInterface(m,p)
{
    s_parent=nullptr;
}

SymChildWinInterface::SymChildWinInterface(SymWinInterface &parentScreen, isQWidget::QWidgetType) :SymWinInterface(parentScreen.handler, parentScreen.cl)
{
    s_parent=&parentScreen;
}

SymNotepadWinInterface::SymNotepadWinInterface(SymWinInterface &parentScreen, isQWidget::QWidgetType arg) :SymChildWinInterface(parentScreen, arg)
{
}

void SymNotepadWinInterface::showParent()
{
    handler.setActive(*s_parent);
    QWidget* dir=s_parent->operator QWidget *();
    dir->show();
    dir->activateWindow();
}

void SymNotepadWinInterface::focusToDirectory()
{
    QWidget* dir=s_parent->operator QWidget *();
    if(dir->isHidden()) return;
    showParent();
}

void SymNotepadWinInterface::obtainFocus() {
    handler.setActive(*this);
    QWidget* w=s_parent->operator QWidget *();
    w->hide();
}

void SymNotepadWinInterface::closeNotepad()
{
    handler.removeNotepad(getId());
}

void SymNotepadWinInterface::forceClose()
{
    forceQuit=true;
    this->operator QWidget *()->close();
}

