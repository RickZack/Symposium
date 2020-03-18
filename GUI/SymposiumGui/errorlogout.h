#ifndef ERRORLOGOUT_H
#define ERRORLOGOUT_H

#include <QDialog>

class MainWindow;

namespace Ui {
class errorlogout;
}

class errorlogout : public QDialog
{
    Q_OBJECT

public:
    explicit errorlogout(QWidget *parent = nullptr);
    ~errorlogout();

private slots:
    void on_pushButton_clicked();

private:
    Ui::errorlogout *ui;
    MainWindow *w;
};

#endif // ERRORLOGOUT_H
