#ifndef SIGIN_H
#define SIGIN_H

#include <QDialog>
#include "home.h"


namespace Ui {
class sigin;
}

class sigin : public QDialog
{
    Q_OBJECT

public:
    explicit sigin(QWidget *parent = nullptr);
    ~sigin();


private slots:
    void on_signin_clicked();

private:
    Ui::sigin *ui;
    home *homeWindow;
};

#endif // SIGIN_H
