#ifndef SIGNUPMW_H
#define SIGNUPMW_H

#include <QMainWindow>
#include "icon.h"
#include "home.h"
#include "notification.h"

class MainWindow;

namespace Ui {
class signupmw;
}

class signupmw : public QMainWindow
{
    Q_OBJECT

public:
    explicit signupmw(QWidget *parent = nullptr);
    ~signupmw();
    void setClientDispatcher( Symposium::clientdispatcher *cl);

private slots:
    void on_cancel_clicked();

    void on_ok_clicked();

    void chooseIcon();

    void on_iconButt_clicked();



private:
    Ui::signupmw *ui;
    std::string pwd;
    home *homeWindow;
    icon *iconWindow;
    MainWindow *mw;
    class exit *ex;
    notification *notWindow;
    errorconnection *errorWindow;
    std::string iconPath="";
    Symposium::clientdispatcher *cl;

    bool checkPassword(QString passwordToCheck);

    void waiting();
};

#endif // SIGNUPMW_H
