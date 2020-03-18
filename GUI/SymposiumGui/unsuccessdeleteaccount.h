#ifndef UNSUCCESSDELETEACCOUNT_H
#define UNSUCCESSDELETEACCOUNT_H

#include <QDialog>

namespace Ui {
class unsuccessdeleteaccount;
}

class unsuccessdeleteaccount : public QDialog
{
    Q_OBJECT

public:
    explicit unsuccessdeleteaccount(QWidget *parent = nullptr);
    ~unsuccessdeleteaccount();

private slots:
    void on_pushButton_clicked();

private:
    Ui::unsuccessdeleteaccount *ui;
};

#endif // UNSUCCESSDELETEACCOUNT_H
