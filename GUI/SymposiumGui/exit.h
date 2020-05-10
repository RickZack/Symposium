#ifndef EXIT_H
#define EXIT_H

#include <QDialog>
#include <QApplication>


namespace Symposium{
class clientdispatcher;
}

namespace Ui {
class exit;
}

class exit : public QDialog
{
    Q_OBJECT

public:
    explicit exit(QWidget *parent, bool logout, Symposium::clientdispatcher *cl);

    ~exit();

private slots:
    /**
     * @brief perform close connection if it is necessary
     */
    void doLogout();

signals:
    void closeSig();

private:
    Ui::exit *ui;
    bool logout;
    Symposium::clientdispatcher *cl;
};

#endif // EXIT_H
