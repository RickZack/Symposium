#ifndef SIGIN_H
#define SIGIN_H

#include <QDialog>


namespace Ui {
class sigin;
}

class sigin : public QDialog
{
    Q_OBJECT

public:
    explicit sigin(QWidget *parent = nullptr);
    ~sigin();


private:
    Ui::sigin *ui;
};

#endif // SIGIN_H
