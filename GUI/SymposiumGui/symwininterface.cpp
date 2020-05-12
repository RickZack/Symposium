/*
 * Copyright (c) 2019 Riccardo Zaccone, Ksenia Del Conte Akimova, Cristian Gianetto, Martina Bellissimo
 *
 * This file is part of Symposium.
 * Symposium is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Symposium is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Symposium.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * File:   symwininterface.cpp
 * Project: Symposium
  * Authors:
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Cristian Gianetto <s265387 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 28 Aprile 2020, 16:05
 */
#include "symwininterface.h"
#include "winmanager.h"
#include<QWidget>
#include<QDialog>


SymWinInterface::SymWinInterface(SymWinManager &m, Symposium::clientdispatcher &p): cl(p), handler(m), forceQuit(false)
{
}

bool SymWinInterface::closedByUser() const
{
    return !forceQuit;
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
    handler.closeAllNotepads();
    goToWindow(*s_parent);
    this->forceQuit=true;
    this->operator QWidget *()->close();
}

void SymChildWinInterface::backToMainWin()
{
    handler.closeAllNotepads();
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
    handler.addNotepad(static_cast<Symposium::uint_positive_cnt::type>(notepad.getId()), notepad);
}

void SymWinInterface::goToWindow(SymModalWinInterface &nextWin)
{
    handler.setActive(nextWin);
    QDialog* nextAsQDialog=nextWin.operator QDialog*();
    nextAsQDialog->exec();
}

SymMainWinInterface::SymMainWinInterface(SymWinManager &m, Symposium::clientdispatcher& p, isQWidget::QWidgetType): SymWinInterface(m,p)
{
    s_parent=nullptr;
}

SymChildWinInterface::SymChildWinInterface(SymWinInterface &parentScreen) :SymWinInterface(parentScreen.handler, parentScreen.cl)
{
    s_parent=&parentScreen;
}

SymChildWinInterface::SymChildWinInterface(SymWinInterface &parentScreen, isQWidget::QWidgetType) :SymWinInterface(parentScreen.handler, parentScreen.cl)
{
    s_parent=&parentScreen;
}

SymNotepadWinInterface::SymNotepadWinInterface(SymWinInterface &parentScreen, isQWidget::QWidgetType arg, bool parentIsTransient) :SymChildWinInterface(parentScreen, arg)
{
    if(parentIsTransient)
        s_parent=parentScreen.s_parent;
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
    handler.removeNotepad(static_cast<Symposium::uint_positive_cnt::type>(getId()));
}

void SymNotepadWinInterface::backToMainWin()
{
    handler.removeNotepad(static_cast<Symposium::uint_positive_cnt::type>(getId()));
    SymChildWinInterface::backToMainWin();
}

bool SymNotepadWinInterface::isLastNotepadOpened() const
{
    return handler.getNumOfActiveEditors()==1;
}

void SymNotepadWinInterface::forceClose()
{
    forceQuit=true;
    this->operator QWidget *()->close();
}

SymModalWinInterface::SymModalWinInterface(SymWinInterface &parentScreen, isQDialog::QDialogType) :SymChildWinInterface(parentScreen)
{
    s_parent=&parentScreen;
}

void SymModalWinInterface::backToParent()
{
    handler.setActive(*s_parent);
    this->forceQuit=true;
    this->operator QWidget *()->close();
}

SymModalWinInterface::operator QDialog *()
{
    QDialog* asQDialog=dynamic_cast<QDialog*>(this);
    if(asQDialog==nullptr)
        throw std::logic_error("This is not a QDialog");
    return asQDialog;
}
