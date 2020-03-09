#ifndef WAITING_H
#define WAITING_H

#include <QDialog>
#include <QMovie>

namespace Ui {
class waiting;
}

class waiting : public QDialog
{
    Q_OBJECT

public:
    explicit waiting(QWidget *parent = nullptr);
    ~waiting();

private:
    Ui::waiting *ui;
};

#endif // WAITING_H
