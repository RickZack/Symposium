#ifndef FOLDER1_H
#define FOLDER1_H

#include <QMainWindow>
#include <QWidget>
#include <QListWidgetItem>
#include <QApplication>
#include <QDesktopWidget>

#include "textedit.h"
#include "newfolderwindow.h"



namespace Ui {
class folder1;
}

class folder1 : public QMainWindow
{
    Q_OBJECT

public:
    explicit folder1(QWidget *parent = nullptr);
    ~folder1();
    void listGenerate(std::string str, int count);
    void openWindow(std::string str);

protected:
    void closeEvent(QCloseEvent *e);


private slots:

    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_9_clicked();


private:
    Ui::folder1 *ui;
    QListWidgetItem *item1;
    TextEdit *textEditWindow;
    newFolderWindow *newFolder;
    // count is a variable used to count the number of elements that are present in the folder1
    // At the beginning they are 4
    int count=4;
    int countDir;
    std::string id;
    std::string separate_word(std::string& string);
    int number_elements(std::string& string);
};

#endif // FOLDER1_H
