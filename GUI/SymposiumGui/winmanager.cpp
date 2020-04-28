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
 * File:   winmanager.cpp
 * Project: Symposium
  * Authors:
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Cristian Gianetto <s265387 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 28 Aprile 2020, 16:05
 */
#include "winmanager.h"
#include<QDebug>

SymWinManager::SymWinManager()
{
    current=nullptr;
}

SymWinInterface &SymWinManager::activeWindow()
{
    return *current;
}

void SymWinManager::setActive(SymWinInterface &newScreen){
    current=&newScreen;
    qDebug()<<"Now active"<<dynamic_cast<QWidget*>(current)->metaObject()->className();
}

void SymWinManager::addNotepad(int id, SymNotepadWinInterface &tx) {
    editors.push_front({id, &tx});
}

SymNotepadWinInterface &SymWinManager::getNotepad(int id) {
    for(auto& ed: editors)
        if (ed.first==id)
            return *ed.second;
    throw std::exception();
}

void SymWinManager::removeNotepad(int id)
{
    for(auto& ed: editors)
        if (ed.first==id)
            return editors.remove(ed);
}

void SymWinManager::closeAllNotepads()
{
    while(!editors.empty()){
        editors.front().second->forceClose();
        editors.pop_front();
    }
}
