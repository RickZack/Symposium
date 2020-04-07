#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>
#include <QPropertyAnimation>

namespace Ui {
class about;
}

class about : public QDialog
{
    Q_OBJECT

public:
    explicit about(QWidget *parent = nullptr);
    void showEvent(QShowEvent* event);
    ~about();

private slots:
    /**
     * @brief button for closing the window
     */
    void on_ok_clicked();

private:
    Ui::about *ui;
};

#endif // ABOUT_H
