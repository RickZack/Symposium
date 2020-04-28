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
