#ifndef EXIT_H
#define EXIT_H

#include <QDialog>
#include <QApplication>


namespace Ui {
class exit;
}

class exit : public QDialog
{
    Q_OBJECT

public:
    explicit exit(QWidget *parent = nullptr);

    ~exit();



private slots:
    void on_buttonBox_accepted();

private:
    Ui::exit *ui;
};

#endif // EXIT_H
