#ifndef CHOOSEDIR_H
#define CHOOSEDIR_H

#include <QDialog>
#include <QListWidgetItem>
#include <algorithm>
#include <stdlib.h>
#include <QMessageBox>

namespace Ui {
class choosedir;
}

class choosedir : public QDialog
{
    Q_OBJECT

public:
    int idDir;
    std::string nameOfDir;
    explicit choosedir(QWidget *parent = nullptr);
    ~choosedir();
    void listGenerate(std::string str, int count);

private:
    Ui::choosedir *ui;
    std::string separate_word(std::string& string);
    int number_elements(std::string& string);

private slots:

    void on_list_itemClicked(QListWidgetItem *item);
    void on_choose_clicked();
};

#endif // CHOOSEDIR_H
