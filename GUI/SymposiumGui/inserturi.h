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

public slots:
    void change_text();
    void reset_text();

private:
    Ui::inserturi *ui;
    choosedir *dirWindow;
    int idDir;
    std::string nameDir;
};

#endif // INSERTURI_H
