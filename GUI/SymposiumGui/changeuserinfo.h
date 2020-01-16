#ifndef CHANGEUSERINFO_H
#define CHANGEUSERINFO_H

#include <QDialog>
#include "icon.h"
#include <QMessageBox>

namespace Ui {
class changeUserInfo;
}

class changeUserInfo : public QDialog
{
    Q_OBJECT

public:
    explicit changeUserInfo(QWidget *parent = nullptr);
    ~changeUserInfo();

private slots:
    void chooseIcon();
    void on_iconButt_clicked();
    void confirm_click();


    //void on_username_textEdited(const QString &arg1);

private:
    Ui::changeUserInfo *ui;
    icon *iconWindow;
};

#endif // CHANGEUSERINFO_H
