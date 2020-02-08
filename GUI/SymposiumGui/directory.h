#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <QMainWindow>
#include "inserturi.h"
#include "textedit.h"
#include "newfolderwindow.h"

namespace Ui {
class directory;
}

class directory : public QMainWindow
{
    Q_OBJECT

public:
    explicit directory(QWidget *parent = nullptr);
    ~directory();

private slots:
    void on_actionNew_Document_triggered();


    void on_actionHome_triggered();

    void on_actionUri_triggered();

    void on_actionNew_Folder_triggered();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::directory *ui;
    inserturi *uriWindow;
    TextEdit *textEditWindow;
    newFolderWindow *folderWindow;



};

#endif // DIRECTORY_H
