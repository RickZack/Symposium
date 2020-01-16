#ifndef INSERTURI_H
#define INSERTURI_H

#include <QDialog>

namespace Ui {
class inserturi;
}

class inserturi : public QDialog
{
    Q_OBJECT

public:
    explicit inserturi(QWidget *parent = nullptr);
    ~inserturi();

private:
    Ui::inserturi *ui;
};

#endif // INSERTURI_H
