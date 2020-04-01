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
    explicit unsuccessdeleteaccount(QWidget *parent = nullptr, std::string error="");
    ~unsuccessdeleteaccount();

private slots:
    /**
     * @brief the user confirm he read the error information
     */
    void on_pushButton_clicked();

private:
    Ui::unsuccessdeleteaccount *ui;
    std::string error;
};

#endif // UNSUCCESSDELETEACCOUNT_H
