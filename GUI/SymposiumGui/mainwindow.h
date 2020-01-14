#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "about.h"
#include "exit.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_about_clicked();

    void on_exit_clicked();

private:
    Ui::MainWindow *ui;
    about *aboutWindow;
    class exit *exitWindow;
};

#endif // MAINWINDOW_H
