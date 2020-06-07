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
#include "symwininterface.h"
#include "waiting.h"

namespace Ui {
class directory;
}

namespace Symposium{
class clientdispatcher;
}

class directory : public QMainWindow, public SymChildWinInterface
{
    Q_OBJECT

    enum action{
        createFolder,
        remove,
        rename,
        createNewSource,
        openSource,         /**< to indicate that a file */
        openSymlink,        /**< to indicate a symlink */
        noAction
    };

public:
    explicit directory(QWidget *parent, std::string pwd, SymWinInterface& si);

    void success() override;

    void failure(const QString& toPrint) override;

    ~directory();
    void listGenerate(std::string list, int count);




    /**
     * @brief successRename renames the selected source
     */
    void successRename();


    /**
     * @brief directory::successRemouve remouves the selected source
     */
    void successRemove();

    /**
     * @brief successCreate create a new folder and updates the string str
     */
    void successCreate();

    void setClientDispatcher(Symposium::clientdispatcher *cl);

    /**
     * @brief successNewSource creates a new file and updates the string str
     */
    notepad* successNewSource();


    /**
     * @brief failureActionDirectory shows an error message in case of the server doesn't answer
     * @param msg
     */
    void failureActionDirectory(const QString& msg);

    /**
     * @brief errorLogout shows an error message and closes the window
     */
     void errorConnectionLogout();

    void openSelectedSource();

    /**
     * @brief successOpen open the document showing all the symbols inside.
     * @param doc
     * @return
     */
    void successOpen();

    /**
     * @brief fixNameSource fix the name chosen by the user for his source, substituing the space with a no printable character
     * @param nameSource the name chosen by the user
     */
    static std::string fixNameSource(const std::string nameSource);

    signals:
    void resNameChanged(Symposium::uint_positive_cnt::type resId, const QString& newName);
    void closeWaiting();

public slots:
    void enableButtonsAfter();
private slots:

    void ProvideContextMenu(const QPoint &);

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

    void on_okButton_3_clicked();

    void on_okButton_4_clicked();

private:
    Ui::directory *ui;

    /*
     * Variables related to success()
     */
    action lastChoice;                                                  /**< indicates the last action requested to the server */
    Symposium::uint_positive_cnt::type curResId;                        /**< resId of the currently selected resource, for an action has been requested */
    QString curResName;                                                 /**< resName of the currently selected resource, for an action has been requested */

    std::map<std::string,std::pair<std::string,std::string>> ids;       /**< contains for all the elements inside a folder, the correspondent id */
    //TODO: spiegare come è usato, a cosa serve
    bool pressed=false;


    /**
     * @brief openFolders this variable is used to say that a window has been opened and to enable/disable the BACK button
     */
    int openFolders=-1;

    /**
     * @brief path is the path that shows all the folders visited like a sequence of folder-id, separate of "/"
     */
    std::string path;
    /**
     * @brief previousId is the id referred to the folder before the one opened. Useful to handle the BACK button
     */
    std::string previousId;
    /**
     * @brief actualId is the id referred to the opened folder. Useful to build the path.
     */
    std::string actualId;

    //TODO: a che serve la password? Abbiamo fatto apposta a non memorizzarla mai in chiaro dentro user, perchè qui la teniamo?
    std::string pwd;

    /**
     * @brief str is a string that contains the list of all the files
     */
    std::string list;

    int countDir;
    /**
     * @brief id related to the file
     */
    std::string id;

    /**
     * @brief count to count the number of elements created
     */
    int count;

    /**
     * @brief selectedId is the ID of selected file.
     */
    std::string selectedId;

    std::string separate_word(std::string& string);
    Symposium::privilege priv=Symposium::privilege::none;
    Symposium::privilege privOpen=Symposium::privilege::none;
    std::string initialPriv;

    /**
     * @brief nameSource is the name of the source of the selected item
     */
    std::string nameSource;

    /**
     * @brief fixedOldName is the name without possible spaces that a source has BEFORE the rename operation
     */
    std::string fixedOldName;

    /**
     * @brief fixedName is the new name modified with non printable characters
     */
    std::string fixedName;

    /**
     * @brief value to indicate if the source is a file/directory/symlink
     */
    QString value;



    /**
     * @brief directory::number_elements counts the number of element inside the @e string
     * @param string that shows all the info about the sources
     * @return #of elements inside the string
     */
    int number_elements(const std::string& string);


    /**
     * @brief directory::generateString deletes from the string the \n in such way to visualize the list of element without spaces
     * @param string that shows all the info about the sources
     * @return the string without \n
     */
    std::string generateString(std::string& list);

    /**
     * @brief directory::openWindow shows the scenario of a folder, opened starting from another one.
     * It saves in a List<QString> the string that cointains all the sources inside the attual folder.
     * This is useful to move from a folder to another one
     * @param str1 cointains all the sources inside the attual folder
     */
    void openWindow(const std::string& str1);


    //void contextMenuEvent(QContextMenuEvent *event) override;

    std::string manipulationPath(std::string& s);

    /**
     * @brief quit from application
     */
    void closeEvent(QCloseEvent *event) override;
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
    void waitingFunction();
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

    void showEvent( QShowEvent* event ) override;

    /**
     * @brief populateMap
     * @param list the list of all the elements inside a directory
     */
    void populateMap(std::string list);


    void handleFailureSymlink();
};



#endif // directory_H
