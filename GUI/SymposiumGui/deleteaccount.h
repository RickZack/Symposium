#ifndef DELETEACCOUNT_H
#define DELETEACCOUNT_H

#include <QDialog>
#include <QMessageBox>

namespace Ui {
class deleteAccount;
}

class deleteAccount : public QDialog
{
    Q_OBJECT

public:
    explicit deleteAccount(QWidget *parent = nullptr);
    ~deleteAccount();

private slots:
    void delete_click();

private:
    Ui::deleteAccount *ui;
};

#endif // DELETEACCOUNT_H
