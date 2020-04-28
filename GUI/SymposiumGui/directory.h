#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QApplication>
#include <QtWidgets/qwidget.h>
#include <QListWidgetItem>
#include <QMouseEvent>



#include "inserturi.h"
#include "choosepriv.h"
#include "errorconnection.h"
#include "errorlogout.h"
#include "notepad.h"

namespace Ui {
class directory;
}

namespace Symposium{
class clientdispatcher;
}

class directory : public QMainWindow
{
    Q_OBJECT

public:
    explicit directory(QWidget *parent = nullptr, std::string pwd="", Symposium::clientdispatcher *cl=nullptr);
    ~directory();
    void listGenerate(std::string str, int count);




    /**
     * @brief successRename renames the selected source
     */
    void successRename();

    /**
     * @brief errorConnection acts when there is an error on the connection
     * It shows an error msg
     */
    void errorConnection();


    /**
     * @brief directory::successRemouve remouves the selected source
     */
    void successRemove();

    /**
     * @brief successCreate create a new folder and updates the string str adding its info
     * @param id the id of the new created folder
     */
    void successCreate(std::string id);

    void setClientDispatcher(Symposium::clientdispatcher *cl);

    /**
     * @brief successNewSource creates a new file and it adds its info in the generale string
     * @param id  of the file
     * @param priv of the file
     */
    notepad* successNewSource(std::string id, Symposium::document &doc);


    /**
     * @brief failureActionDirectory shows an error message in case of the server doesn't answer
     * @param msg
     */
    void failureActionDirectory(std::string msg);

    /**
     * @brief errorLogout shows an error message and closes the window
     */
     void errorConnectionLogout(std::string str);

    void openSelectedSource();

    /**
     * @brief successOpen open the document showing all the symbols inside.
     * @param doc
     * @return
     */
    notepad *successOpen(Symposium::document &doc);
private slots:

    /**
     * @brief directory::on_actionHome_triggered opens the home window
     */
    void on_actionHome_triggered();


    /**
     * @brief directory::on_actionUri_triggered opens the uri window
     */
    void on_actionUri_triggered();

    /**
     * @brief show add folder labels and buttons
     */
    void on_actionaddFolder_triggered();
    /**
     * @brief show add file labels and buttons
     */
    void on_actionaddfile_triggered();
    /**
     * @brief directory::deleteSource acts when the user chooses to delete a selected source
     */
    void deleteSource();

    /**
     * @brief on_pushButton_3_clicked acts when the user wants to create a new folder
     */
    void on_pushButton_3_clicked();

    /**
     * @brief on_pushButton_4_clicked acts when the user wants to create a new file
     */
    void on_pushButton_4_clicked();

    /**
     * @brief on_back_button_clicked acts when the user clicks on the button "BACK"
     */
    void on_back_button_clicked();

    /**
     * @brief renameSource acts when the user chooses to rename the selected source
     */
    void renameSource();

    /**
     * @brief on_okButton_clicked acts when the user inserts the new name of the folder and
     * clicks on the button "OK"
     */
    void on_okButton_clicked();

    void on_myListWidget_itemDoubleClicked();



    void on_okButton_2_clicked();

    /**
     * @brief on_OkPriv_clicked acts when the button OK is clicked
     */
    void on_OkPriv_clicked();

    void on_cancPriv_clicked();

private:
    Ui::directory *ui;
    inserturi *uriWindow;
    QListWidgetItem *item1;
    choosepriv *chooseprivWindow;
    Symposium::clientdispatcher *cl;
    errorconnection *errorWindow;
    errorlogout *errorLogoutWindow;
    notepad *notepadWindow;

    /**
     * @brief openFolders this variable is used to say that a window has been opened and to enable/disable the BACK button
     */
    int openFolders=-1;

    /**
     * @brief path is the path that shows all the folders visited
     */
    std::string path;
    /**
     * @brief previousId is the id referred to the folder before the one opened. Useful to handle the BACK button
     */
    std::string previousId;
    /**
     * @brief actualId is the id referred to the opened window. Useful to build the path.
     */
    std::string actualId;

    std::string pwd;
    std::string str;
    std::string old_str;
    int countDir;
    std::string id;
    // this variable is used to count the number of elements created
    int count;
    std::string separate_word(std::string& string);
    Symposium::privilege priv=Symposium::privilege::none;
    Symposium::privilege privOpen=Symposium::privilege::none;
    std::string initialPriv;


    /**
     * @brief directory::number_elements counts the number of element inside the @e string
     * @param string that shows all the info about the sources
     * @return #of elements inside the string
     */
    int number_elements(std::string& string);


    /**
     * @brief directory::generateString deletes from the string the \n in such way to visualize the list of element without spaces
     * @param string that shows all the info about the sources
     * @return the string without \n
     */
    std::string generateString(std::string str);

    /**
     * @brief directory::searchForId extracts the id of the selected directory
     * @param word the name of the selected directory
     * @param str the initial string
     * @param count the # of elements inside the string
     * @return id
     */
    std::string searchForId(std::string word,std::string str,int count);


    /**
     * @brief directory::searchForPriv extracts the id and priv of the selected file/symlink
     * @param word the name of the selected source
     * @param str the initial string
     * @param count the # of elements inside the string
     * @return <id,priv>
     */
    std::pair<std::string,std::string> searchForPriv(std::string word,std::string str, int count);


    /**
     * @brief directory::openWindow shows the scenario of a folder, opened starting from another one.
     * It saves in a List<QString> the string that cointains all the sources inside the attual folder.
     * This is useful to move from a folder to another one
     * @param str1 cointains all the sources inside the attual folder
     */
    void openWindow(std::string str1);


    void contextMenuEvent(QContextMenuEvent *event) override;

    std::string manipulationHome(std::string& s);

    /**
     * @brief quit from application
     */
    void closeEvent(QCloseEvent *event);
    /**
     * @brief disable all buttons present so user cannot perform any operation
     */
    void disableButtons();
    /**
     * @brief enable all buttons present
     */
    void enableButtons();
    /**
     * @brief enable the style of buttons
     */
    void enableStyleButtons();
    /**
     * @brief enable the style of buttons
     */
    void disableStyleButtons();
    /**
     * @brief waiting of the conclusion of the operation by clientdispatcher
     */
    void waiting();
    /**
     * @brief hide all label and buttons and diasable the buttons
     */
    void hideAll();
    /**
     * @brief hide all label and buttons and diasable the buttons
     */
    void showNewDirectory();
    /**
     * @brief hide all label and buttons and diasable the buttons
     */
    void showNewFile();
    /**
     * @brief hide all label and buttons and diasable the buttons
     */
    void showRename();
    /**
     * @brief showsPrivilegeButtons handle the buttons referring the privilege
     */
    void showPrivilegeButtons();
    /**
     * @brief hidePrivilegeButtons hide the buttons referring the privilege
     */
    void hidePrivilegeButtons();
};



#endif // directory_H
