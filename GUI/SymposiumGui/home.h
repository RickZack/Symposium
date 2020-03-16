#ifndef HOME_H
#define HOME_H

#include <QMainWindow>
#include <QCloseEvent>
#include "deleteaccount.h"
#include "inserturi.h"
#include "changeuserinfo.h"
#include "directory.h"



namespace Symposium{
class clientdispatcher;
}

namespace Ui {
class home;
}

class home : public QMainWindow
{
    Q_OBJECT

public:
    explicit home(QWidget *parent = nullptr);
    void setClientDispatcher(Symposium::clientdispatcher *cl);
    ~home();

private slots:
    void on_delete_2_clicked();

    void on_InsertUri_clicked();

    void on_modify_clicked();

    void on_directory_clicked();

private:
    Ui::home *ui;
    deleteAccount *deleteAccountWindow;
    inserturi *inserturiWindow;
    changeUserInfo *changeWindow;
    directory *directoryWindow;
    Symposium::clientdispatcher *cl;

    void closeEvent(QCloseEvent *event);
};

#endif // HOME_H
