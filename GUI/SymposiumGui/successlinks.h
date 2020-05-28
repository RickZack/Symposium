#ifndef SUCCESSLINKS_H
#define SUCCESSLINKS_H

#include <QDialog>
#include <QDesktopServices>

#include "../../user.h"
#include "../../privilege.h"

namespace Ui {
class successlinks;
}

class successlinks : public QDialog
{
    Q_OBJECT

public:
    explicit successlinks(QWidget *parent, int caseLink, QString link, QString numShares, QString time, Symposium::user us, Symposium::privilege priv);
    ~successlinks();

private slots:
    void on_ok_clicked();

    void on_pushButton_clicked();

private:
    Ui::successlinks *ui;
    int caseLink;
    QString link;
    QString numShares;
    QString time;
    Symposium::user us;
    Symposium::privilege priv;

    void hideAll();

    void share();
    void noshare();
    void timeshare();
};

#endif // SUCCESSLINKS_H
