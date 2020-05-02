#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>
#include <QPropertyAnimation>
#include "symwininterface.h"

namespace Ui {
class about;
}

class about : public QDialog, public SymChildWinInterface
{
    Q_OBJECT

    //Not useful on this window
    void success() override{}
    void failure(const QString&) override{}

public:
    explicit about(QWidget *parent, SymWinInterface& si);
    void showEvent(QShowEvent* event) override;
    ~about();

private slots:
    /**
     * @brief button for closing the window
     */
    void on_ok_clicked();

private:
    Ui::about *ui;

    /**
     * @brief quit from application
     */
    void closeEvent(QCloseEvent *event) override;
};

#endif // ABOUT_H
