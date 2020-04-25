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

        /*this->append("Hello, World!\nAnotherLine. Hello, World! Hello, World! Hello, "
                     "World! Helloooo, World! Hello, World! Hello, World! Hello, World! "
                     "Hello, World! Hello, World! Hello, World! Hello, World!");
        this->append("Hello, World!\nAnotherLine.");
        this->append("Hello, World!\nAnotherLine.");
        this->append("Hello, World!\nAnotherLine.");
        this->append("Hello, World!\nAnotherLine.");
        this->append("Hello, World!\nAnotherLine.");
        this->append("Hello, World!\nAnotherLine.");
        this->append("Hello, World!\nAnotherLine.");
        this->append("Hello, World!\nAnotherLine.");*/

    }

    /**
     * @brief rewrite scrollContentsBy of the parent TextEdit class
     */
    void scrollContentsBy(int dx, int dy) override;
    /**
     * @brief scroll labels
     */
    void scroll();
    /**
     * @brief change position of cursor
     * @param block the line of the text file
     * @param collumn the number of character
     */
    void changePosition(int block, int collumn);
    /**
     * @brief change position of cursor of particular user
     * @param siteId siteId of the user
     * @param block the line of the text file
     * @param collumn the number of character
     */
    void changePosition(Symposium::uint_positive_cnt::type siteId, int block, int collumn);
    /**
     * @brief show the cursors of all users
     * @param siteId siteId of the user who opened this document
     * @param users forward_list of users who used now this document
     */
    void constractLabelsCursors(std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData>> users, Symposium::uint_positive_cnt::type siteId);
    /**
     * @brief insert the cursor of user who opened this document
     * @param siteId siteId of the user who opened this document
     * @param users forward_list of users who used now this document
     */
    void insertCurrentUser(std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData> > users, Symposium::uint_positive_cnt::type siteId);
    /**
     * @brief show the cursor of user
     * @param labelname the label of name
     * @param labelCursor the label of cursor
     */
    void showLabel(QLabel *labelNameReverse, QLabel *labelName);
    /**
     * @brief called by notepad when there is a new user who open this document
     * @param siteId siteId of the user
     * @param name username of the user
     */
    void addUser(Symposium::uint_positive_cnt::type siteId, std::string name);
    /**
     * @brief called by notepad when one of the users closed this document
     * @param siteId siteId of the user
     */
    void removeUser(Symposium::uint_positive_cnt::type siteId);
    /**
     * @brief called by notepad when one of the users' cursors changed it's position
     * @param siteId siteId of the user
     */
    void thisUserChangePosition(Symposium::uint_positive_cnt::type siteId);
    /**
     * @brief setting of clientdispatcher
     * @param cl clientdispatcher for reference
     */
    void setClientDispatcher(Symposium::clientdispatcher *cl);
    /**
     * @brief setting the Id of the opened document
     * @param docId the Id of the document
     */
    void setDocumentId(Symposium::uint_positive_cnt::type docId);
    /**
     * @brief setting the Id of the user who opened this document
     * @param id the siteId of the user
     */
    void setThisUserSiteId(Symposium::uint_positive_cnt::type id);
    /**
     * @brief setting the privilege of the user who opened this document
     * @param priv the privilege of the user
     */
    void setThisUserPrivilege(Symposium::privilege priv);

private:

    int j=0;
    int i=0;
    Symposium::uint_positive_cnt::type thisUserSiteId=1;
    Symposium::uint_positive_cnt::type documentId;
    std::map<Symposium::uint_positive_cnt::type, std::pair<QLabel*, QLabel*>> labels;
    std::map<Symposium::uint_positive_cnt::type, QTextCursor> cursors;
    std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData>> users;
    Symposium::clientdispatcher *cl;
    Symposium::privilege priv;
};

#endif // QTEXTEDITLABELS_H
