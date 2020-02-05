#include "newfolderwindow.h"
#include "ui_newfolderwindow.h"

newFolderWindow::newFolderWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::newFolderWindow)
{
    ui->setupUi(this);
}

newFolderWindow::~newFolderWindow()
{
    delete ui;
}
