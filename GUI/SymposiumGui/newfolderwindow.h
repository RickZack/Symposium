#ifndef NEWFOLDERWINDOW_H
#define NEWFOLDERWINDOW_H

#include <QMainWindow>

namespace Ui {
class newFolderWindow;
}

class newFolderWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit newFolderWindow(QWidget *parent = nullptr);
    ~newFolderWindow();

private:
    Ui::newFolderWindow *ui;
};

#endif // NEWFOLDERWINDOW_H
