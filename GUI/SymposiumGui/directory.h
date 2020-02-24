#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QApplication>
#include <QtWidgets/qwidget.h>

#include "inserturi.h"
#include "textedit.h"
#include "notepad.h"

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
    void openWindow(std::string str1);
    // this variable is used to say that a window has been opened and to enable the button BACK.
    int aperto=0;
    QList<std::string> values;



private slots:
    void on_actionHome_triggered();
    void on_actionUri_triggered();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_back_button_clicked();

private:
    Ui::directory *ui;
    inserturi *uriWindow;
    TextEdit *textEditWindow;
    QListWidgetItem *item1;
    notepad *notepadWindow;

    std::string str;
    std::string old_str;
    int countDir;
    std::string id;
    void closeEvent(QCloseEvent *event);
    // this variable is used to count the number of elements created
    int count=10;
    std::string separate_word(std::string& string);
    int number_elements(std::string& string);
    std::string generateString(std::string str);

};

#endif // DIRECTORY_H
