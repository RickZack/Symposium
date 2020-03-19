#ifndef INSERTURI_H
#define INSERTURI_H

#include <QDialog>
#include "choosedir.h"
#include "../../privilege.h"
#include "errorconnection.h"
#include "errorlogout.h"



namespace Symposium{
class clientdispatcher;
}


namespace Ui {
class inserturi;
}

class inserturi : public QDialog
{
    Q_OBJECT

public:
    explicit inserturi(QWidget *parent = nullptr);
    void setClientDispatcher(Symposium::clientdispatcher *cl);
    void invalidUri();
    void privilegeTooBig();
    void DirectoryFileNotExist();
    void errorConnection();
    void errorConnectionLogout();
    void successInsert();
    ~inserturi();
    bool openWindow;

private slots:
    void on_dir_clicked();

    void on_reader_clicked();

    void on_writer_clicked();

    void on_owner_clicked();

    void on_add_clicked();

public slots:
    void change_text();
    void reset_text();
    void closeEvent(QCloseEvent *event);

private:
    Ui::inserturi *ui;
    choosedir *dirWindow;
    std::string path;
    std::string nameDir;
    Symposium::privilege privilege;
    std::string showDir;
    std::string pathId;
    Symposium::clientdispatcher *cl;
    errorconnection *errorWindow;
    errorlogout *errorLog;
};

#endif // INSERTURI_H
