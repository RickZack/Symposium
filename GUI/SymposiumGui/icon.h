#ifndef ICON_H
#define ICON_H

#include <QDialog>

namespace Ui {
class icon;
}

class icon : public QDialog
{
    Q_OBJECT

public:
    explicit icon(QWidget *parent = nullptr);
    std::string msg;
    ~icon();

private slots:
    void on_pushButton_clicked();

    void on_beaver_clicked();

    void on_boar_clicked();

    void on_bull_clicked();

    void on_deer_clicked();

    void on_duck_clicked();

    void on_hed_clicked();

    void on_hippo_clicked();

    void on_ostrich_clicked();

    void on_owl_clicked();

    void on_zoo_clicked();

private:
    Ui::icon *ui;
};

#endif // ICON_H
