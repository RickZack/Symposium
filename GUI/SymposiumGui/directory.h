#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <QMainWindow>
#include "inserturi.h"
#include "textedit.h"
#include <QLabel>
#include <QPixmap>

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

private:
    Ui::directory *ui;
    inserturi *uriWindow;
    TextEdit *textEditWindow;


};

#endif // DIRECTORY_H
