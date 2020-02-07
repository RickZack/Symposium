#ifndef ACTIVETIMERLINK_H
#define ACTIVETIMERLINK_H

#include <QDialog>
#include <iostream>
#include <sstream>
#include <locale>
#include <iomanip>
#include <QMessageBox>
#include "../../privilege.h"

namespace Ui {
class activetimerlink;
}

class activetimerlink : public QDialog
{
    Q_OBJECT

public:
    explicit activetimerlink(QWidget *parent = nullptr, std::string pathToFile="");
    ~activetimerlink();

private slots:
    void on_ok_clicked();

    void on_owner_clicked();

    void on_writer_clicked();

    void on_reader_clicked();

    void on_cancel_clicked();

private:
    Ui::activetimerlink *ui;
    std::string pathToFile;
    Symposium::privilege privilegeToGrant;

};

#endif // ACTIVETIMERLINK_H
