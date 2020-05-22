#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QDialog>

namespace Ui {
class notification;
}

class notification : public QDialog
{
    Q_OBJECT

public:
    explicit notification(QWidget *parent = nullptr, QString str="");
    void closeEvent(QCloseEvent * e) override;
    ~notification();

private:
    Ui::notification *ui;
    QString str;
};

#endif // NOTIFICATION_H
