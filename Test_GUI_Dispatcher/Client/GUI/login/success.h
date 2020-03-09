#ifndef SUCCESS_H
#define SUCCESS_H

#include <QMainWindow>

namespace Ui {
class success;
}

class success : public QMainWindow
{
    Q_OBJECT

public:
    explicit success(QWidget *parent = nullptr);
    ~success();

private:
    Ui::success *ui;
};

#endif // SUCCESS_H
