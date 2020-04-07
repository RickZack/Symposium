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
    explicit exit(QWidget *parent = nullptr, bool logout=false);

    /**
     * @brief setting of clientdispatcher
     * @param cl clientdispatcher for reference
     */
    void setClientDispatcher( Symposium::clientdispatcher *cl);

    ~exit();

private slots:
    /**
     * @brief perform close connection if it is necessary
     */
    void doLogout();

private:
    Ui::exit *ui;
    bool logout;
    Symposium::clientdispatcher *cl;
};

#endif // EXIT_H
