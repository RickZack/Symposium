#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <QMainWindow>
#include <QCloseEvent>
#include "inserturi.h"
#include "textedit.h"
#include "newfolderwindow.h"
#include "folder1.h"
#include <stdlib.h>

namespace Ui {
class directory;
}

class directory : public QMainWindow
{
    Q_OBJECT

public:
    explicit directory(QWidget *parent = nullptr);
    ~directory();
    void listGenerate(std::string str, int count);
    void remouveFromString(std::string value,std::string str,std::string type);




private slots:
    void on_actionHome_triggered();
    void on_actionUri_triggered();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();




    void on_pushButton_4_clicked();

private:
    Ui::directory *ui;
    inserturi *uriWindow;
    TextEdit *textEditWindow;
    newFolderWindow *folderWindow;
    QListWidgetItem *item1;
    folder1 *folder1Window;

    std::string str;
    int countDir;
    std::string id;
    void closeEvent(QCloseEvent *event);
    // this variable is used to count the number of elements created
    int count=10;
    std::string separate_word(std::string& string);
    int number_elements(std::string& string);

};

#endif // DIRECTORY_H
