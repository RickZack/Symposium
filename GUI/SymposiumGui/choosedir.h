#ifndef CHOOSEDIR_H
#define CHOOSEDIR_H

#include <QDialog>
#include <QListWidgetItem>
#include <algorithm>
#include <stdlib.h>
#include <QMessageBox>
#include <vector>

#include <QDebug>

namespace Ui {
class choosedir;
}

class choosedir : public QDialog
{
    Q_OBJECT

public:
    std::string path;
    std::string nameOfDir;
    explicit choosedir(QWidget *parent = nullptr);
    ~choosedir();
    void listGenerate(std::string str, int count);

private:
    Ui::choosedir *ui;
    int countDir;
    int spaces;
    std::string id;

    std::string pathForUser;
    std::vector<std::string>paths;
    std::vector<std::string>pathsForUser;
    std::string separate_word(std::string& string);
    int number_elements(std::string& string);

private slots:

    void on_list_itemClicked(QListWidgetItem *item);
    void on_choose_clicked();
};

#endif // CHOOSEDIR_H
