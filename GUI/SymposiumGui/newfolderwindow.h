#ifndef NEWFOLDERWINDOW_H
#define NEWFOLDERWINDOW_H

#include <QMainWindow>
#include "textedit.h"

namespace Ui {
class newFolderWindow;
}

class newFolderWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit newFolderWindow(QWidget *parent = nullptr);
    ~newFolderWindow();
    QString nameDocument;

private slots:


    void on_createButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::newFolderWindow *ui;
    TextEdit *textEditWindow;
};

#endif // NEWFOLDERWINDOW_H
