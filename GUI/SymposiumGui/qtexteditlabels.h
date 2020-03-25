#ifndef QTEXTEDITLABELS_H
#define QTEXTEDITLABELS_H
#include <QTextEdit>
#include <QLabel>
#include <forward_list>
#include "../../user.h"
#include "../../Color.h"
#include "../../document.h"

namespace Symposium{
class clientdispatcher;
}

class qtexteditlabels : public QTextEdit
{
public:
    qtexteditlabels(QWidget* a):QTextEdit(a)
    {
        this->append("Hello, World!\nAnotherLine. Hello, World! Hello, World! Hello, "
                     "World! Helloooo, World! Hello, World! Hello, World! Hello, World! "
                     "Hello, World! Hello, World! Hello, World! Hello, World!");
        this->append("Hello, World!\nAnotherLine.");
        this->append("Hello, World!\nAnotherLine.");
        this->append("Hello, World!\nAnotherLine.");
        this->append("Hello, World!\nAnotherLine.");
        this->append("Hello, World!\nAnotherLine.");
        this->append("Hello, World!\nAnotherLine.");
        this->append("Hello, World!\nAnotherLine.");
        this->append("Hello, World!\nAnotherLine.");
        this->append("Hello, World!\nAnotherLine.");
        this->append("Hello, World!\nAnotherLine.");
        this->append("Hello, World!\nAnotherLine.");
        this->append("Hello, World!\nAnotherLine.");
        this->append("Hello, World!\nAnotherLine. Hello, World! Hello, World! Hello, "
                     "World! Helloooo, World! Hello, World! Hello, World! Hello, World! "
                     "Hello, World! Hello, World! Hello, World! Hello, World!");
        this->append("Hello, World!\nAnotherLine.");
        this->append("Hello, World!\nAnotherLine.");
        this->append("Hello, World!\nAnotherLine.");
        this->append("Hello, World!\nAnotherLine.");
        this->append("Hello, World!\nAnotherLine.");
        this->append("Hello, World!\nAnotherLine.");
        this->append("Hello, World!\nAnotherLine.");
        this->append("Hello, World!\nAnotherLine.");
        this->append("Hello, World!\nAnotherLine.");
        this->append("Hello, World!\nAnotherLine.");
        this->append("Hello, World!\nAnotherLine.");
        this->append("Hello, World!\nAnotherLine.");
    }
    void scrollContentsBy(int dx, int dy) override;
    void scroll();
    void changePosition(int block, int collumn);
    void changePosition(int siteId, int block, int collumn);
    void constractLabelsCursors(std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData>> users, int siteId);
    void insertCurrentUser(std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData> > users, int siteId);
    void showLabel(QLabel *labelCursor, QLabel *labelName);
    void addUser(int siteId, std::string name);
    void removeUser(int siteId);
    void thisUserChangePosition(int siteId);
    void setClientDispatcher(Symposium::clientdispatcher *cl);
    void setDocumentId(int docId);
    void setThisUserSiteId(int id);
    void setThisUserPrivilege(Symposium::privilege priv);


private:
    std::map<int, std::pair<QLabel*, QLabel*>> labels;
    std::map<int, QTextCursor> cursors;
    int j=0;
    int i=0;
    std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData>> users;
    Symposium::privilege priv;
    Symposium::clientdispatcher *cl;
    int thisUserSiteId=1;
    int documentId;
};

#endif // QTEXTEDITLABELS_H
