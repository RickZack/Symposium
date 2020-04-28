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
 * File:   winmanager.h
 * Project: Symposium
  * Authors:
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Cristian Gianetto <s265387 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 28 Aprile 2020, 16:05
 */

#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include "symwininterface.h"
#include<forward_list>

class SymWinManager{
    SymWinInterface* current;
    std::forward_list<std::pair<int, SymNotepadWinInterface*>> editors;
public:
    SymWinManager();
    /**
     * @brief activeWindow used to retrieve the currenctly active window
     * @return a reference to the currently active SymWinInterface window
     */
    SymWinInterface& activeWindow();
    /**
     * @brief setActive sets a different window as active
     * @param newScreen the window to be considered active from now on
     */
    void setActive(SymWinInterface& newScreen);

    /**
     * @brief addNotepad registers a SymNotepadWinInterface window among the opened ones
     * @param id the identifier of the SymNotepadWinInterface window to register
     * @param tx the reference of the SymNotepadWinInterface window to register
     */
    void addNotepad(int id, SymNotepadWinInterface &tx);
    /**
     * @brief getNotepad retrieves a SymNotepadWinInterface window given its id
     * @param id the identifier of the SymNotepadWinInterface window to be retrieved
     * @return the reference to the SymNotepadWinInterface window to be retrieved
     */
    SymNotepadWinInterface& getNotepad(int id);
    /**
     * @brief removeNotepad unregisters a SymNotepadWinInterface window from the opened ones
     * @param id the identifier of the SymNotepadWinInterface window to unregister
     */
    void removeNotepad(int id);
    /**
     * @brief closeAllNotepads unregisters all of the registered SymNotepadWinInterface
     * windows and close them.
     */
    void closeAllNotepads();

    SymWinManager(SymWinManager&)= delete;
    SymWinManager(SymWinManager&&)= delete;
    SymWinManager operator=(SymWinManager&)= delete;
    SymWinManager operator=(SymWinManager&&)= delete;
};

#endif // SCREENMANAGER_H
