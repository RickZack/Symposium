#ifndef SUCCESSLINKS_H
#define SUCCESSLINKS_H

#include <QDialog>

namespace Ui {
class successlinks;
}

class successlinks : public QDialog
{
    Q_OBJECT

public:
    explicit successlinks(QWidget *parent = nullptr, int caseLink=0, QString str="", QString numShares="", QString time="");
    ~successlinks();

private slots:
    void on_ok_clicked();

private:
    Ui::successlinks *ui;
    int caseLink;
    QString str;
    QString numShares;
    QString time;

    void hideAll();

    void share();
    void noshare();
    void timeshare();
};

#endif // SUCCESSLINKS_H
