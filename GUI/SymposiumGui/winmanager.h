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
