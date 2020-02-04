#ifndef INSERTURI_H
#define INSERTURI_H

#include <QDialog>
#include "choosedir.h"
namespace Ui {
class inserturi;
}

class inserturi : public QDialog
{
    Q_OBJECT

public:
    explicit inserturi(QWidget *parent = nullptr);
    ~inserturi();

private slots:
    void on_dir_clicked();

    void on_reader_clicked();

    void on_writer_clicked();

    void on_owner_clicked();

public slots:
    void change_text();
    void reset_text();

private:
    Ui::inserturi *ui;
    choosedir *dirWindow;
    std::string path;
    std::string nameDir;
    std::string privilege;
};

#endif // INSERTURI_H
