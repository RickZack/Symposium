#ifndef INSERTURI_H
#define INSERTURI_H

#include <QDialog>
#include "choosedir.h"
namespace Ui {
class inserturi;
}

class inserturi : public QDialog
{
    Q_OBJECT

public:
    explicit inserturi(QWidget *parent = nullptr);
    ~inserturi();

private slots:
    void on_dir_clicked();

private:
    Ui::inserturi *ui;
    choosedir *dirWindow;
};

#endif // INSERTURI_H
