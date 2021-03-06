#ifndef ERRORLOGOUT_H
#define ERRORLOGOUT_H

#include <QDialog>

class MainWindow;

namespace Symposium{
class clientdispatcher;
}


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
    /**
     * @brief close current window and show start window of the application
     */
    void on_pushButton_clicked();

private:
    Ui::errorlogout *ui;
};

#endif // ERRORLOGOUT_H
