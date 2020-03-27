#ifndef QTEXTEDITLABELS_H
#define QTEXTEDITLABELS_H
#include <QTextEdit>
#include <QLabel>
#include <forward_list>
#include "../../user.h"
#include "../../Color.h"
#include "../../document.h"
#include "../../Symposium.h"

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
    void changePosition(Symposium::uint_positive_cnt::type siteId, int block, int collumn);
    void constractLabelsCursors(std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData>> users, Symposium::uint_positive_cnt::type siteId);
    void insertCurrentUser(std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData> > users, Symposium::uint_positive_cnt::type siteId);
    void showLabel(QLabel *labelCursor, QLabel *labelName);
    void addUser(Symposium::uint_positive_cnt::type siteId, std::string name);
    void removeUser(Symposium::uint_positive_cnt::type siteId);
    void thisUserChangePosition(Symposium::uint_positive_cnt::type siteId);
    void setClientDispatcher(Symposium::clientdispatcher *cl);
    void setDocumentId(Symposium::uint_positive_cnt::type docId);
    void setThisUserSiteId(Symposium::uint_positive_cnt::type id);
    void setThisUserPrivilege(Symposium::privilege priv);


private:
    std::map<Symposium::uint_positive_cnt::type, std::pair<QLabel*, QLabel*>> labels;
    std::map<Symposium::uint_positive_cnt::type, QTextCursor> cursors;
    int j=0;
    int i=0;
    std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData>> users;
    Symposium::clientdispatcher *cl;
    Symposium::uint_positive_cnt::type thisUserSiteId=1;
    Symposium::uint_positive_cnt::type documentId;
    Symposium::privilege priv;
};

#endif // QTEXTEDITLABELS_H
