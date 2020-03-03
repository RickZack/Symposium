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

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::choosepriv *ui;
    notepad *notepadWindow;
};

#endif // CHOOSEPRIV_H
