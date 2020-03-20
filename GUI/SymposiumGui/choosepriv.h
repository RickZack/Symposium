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
    explicit choosepriv(QWidget *parent = nullptr);
    ~choosepriv();
    void getPath(std::string path,std::string idSource);
    void successOpen();
    void failureOpen();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();



private:
    Ui::choosepriv *ui;
    notepad *notepadWindow;
    std::string privilege;
    std::string path;
    std::string idSource;
};

#endif // CHOOSEPRIV_H
