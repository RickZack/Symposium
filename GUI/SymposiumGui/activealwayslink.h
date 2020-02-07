#ifndef ACTIVEALWAYSLINK_H
#define ACTIVEALWAYSLINK_H

#include <QDialog>
#include "../../privilege.h"
#include <QMessageBox>

namespace Ui {
class activealwayslink;
}

class activealwayslink : public QDialog
{
    Q_OBJECT

public:
    explicit activealwayslink(QWidget *parent = nullptr,  std::string pathToFile="");
    ~activealwayslink();

private slots:
    void on_owner_clicked();

    void on_writer_clicked();

    void on_reader_clicked();

    void on_cancel_clicked();

    void on_ok_clicked();

private:
    Ui::activealwayslink *ui;
    std::string pathToFile;
    Symposium::privilege privilegeToGrant;
};

#endif // ACTIVEALWAYSLINK_H
