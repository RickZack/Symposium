#ifndef GUI_SYMSERVER_H
#define GUI_SYMSERVER_H

#include <QMainWindow>
#include <QApplication>
#include "serverdispatcher.h"


namespace Ui {
class GUI_SymServer;
}

class GUI_SymServer : public QMainWindow
{
    Q_OBJECT

public:
    explicit GUI_SymServer(QWidget *parent = nullptr);
    ~GUI_SymServer();

private slots:
    void on_pushButton_clicked();

private:
    Ui::GUI_SymServer *ui;
    QStringList indirizzi;
    Symposium::ServerDispatcher server;
};

#endif // GUI_SYMSERVER_H
