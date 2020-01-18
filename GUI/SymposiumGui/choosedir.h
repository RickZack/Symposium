#ifndef CHOOSEDIR_H
#define CHOOSEDIR_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
class choosedir;
}

class choosedir : public QDialog
{
    Q_OBJECT

public:
    explicit choosedir(QWidget *parent = nullptr);
    ~choosedir();
    void listGenerate(QString str);

private:
    Ui::choosedir *ui;

private slots:

    void on_list_itemClicked(QListWidgetItem *item);
};

#endif // CHOOSEDIR_H
