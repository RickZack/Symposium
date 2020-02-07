#ifndef ACTIVECOUNTERLINK_H
#define ACTIVECOUNTERLINK_H

#include <QDialog>
#include <QMessageBox>
#include "../../privilege.h"

namespace Ui {
class activecounterlink;
}

class activecounterlink : public QDialog
{
    Q_OBJECT

public:
    explicit activecounterlink(QWidget *parent = nullptr, std::string pathToFile="");
    ~activecounterlink();

private slots:
    void on_ok_clicked();

    void on_owner_clicked();

    void on_writer_clicked();

    void on_reader_clicked();

    void on_cancel_clicked();

private:
    Ui::activecounterlink *ui;
    std::string pathToFile;
    Symposium::privilege privilegeToGrant;
};

#endif // ACTIVECOUNTERLINK_H
