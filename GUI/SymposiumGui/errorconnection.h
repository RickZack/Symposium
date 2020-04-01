#ifndef ERRORCONNECTION_H
#define ERRORCONNECTION_H

#include <QDialog>

namespace Ui {
class errorconnection;
}

class errorconnection : public QDialog
{
    Q_OBJECT

public:
    explicit errorconnection(QWidget *parent = nullptr);
    ~errorconnection();

private slots:
    /**
     * @brief close current window
     */
    void on_pushButton_clicked();

private:
    Ui::errorconnection *ui;
};

#endif // ERRORCONNECTION_H
