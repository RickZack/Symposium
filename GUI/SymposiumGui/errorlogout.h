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
    explicit errorlogout(QWidget *parent = nullptr, QString str="");
    ~errorlogout();
    /**
     * @brief setting of clientdispatcher
     * @param cl clientdispatcher for reference
     */
    void setClientDispatcher(Symposium::clientdispatcher *cl);

private slots:
    /**
     * @brief close current window and show start window of the application
     */
    void on_pushButton_clicked();

private:
    QString str;
    Ui::errorlogout *ui;
    Symposium::clientdispatcher *cl;
};

#endif // ERRORLOGOUT_H
