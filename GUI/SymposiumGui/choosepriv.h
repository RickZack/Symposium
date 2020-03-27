#ifndef CHOOSEPRIV_H
#define CHOOSEPRIV_H

#include <QMainWindow>
#include "notepad.h"

namespace Ui {
class choosepriv;
}

class choosepriv : public QMainWindow
{
    Q_OBJECT

public:
    explicit choosepriv(QWidget *parent = nullptr,std::string path=" ", std::string idSource=" ", std::string oldPriv=" ");
    ~choosepriv();
    notepad* successOpen();
    void failureOpen();

    std::string path;
    std::string idSource;
    std::string oldPriv;

    Symposium::privilege priv=Symposium::privilege::none;
    Symposium::privilege privOpen=Symposium::privilege::none;

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();



private:
    Ui::choosepriv *ui;
    notepad *notepadWindow;
    std::string privilege;
};

#endif // CHOOSEPRIV_H
