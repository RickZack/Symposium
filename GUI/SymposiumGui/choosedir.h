#ifndef CHOOSEDIR_H
#define CHOOSEDIR_H

#include <QDialog>
#include <QTreeWidgetItem>
#include <algorithm>
#include <stdlib.h>
#include <QMessageBox>
#include <vector>

#include <QDebug>
#include <stack>

namespace Ui {
class choosedir;
}

class choosedir : public QDialog
{
    Q_OBJECT

public:
    std::string pathId;
    std::string nameOfDir;
    std::string pathDir;
    explicit choosedir(QWidget *parent = nullptr);
    ~choosedir();
    void treeGenerate(std::string str, int count);

private:
    Ui::choosedir *ui;

    int spaces;
    std::string id;


    std::string separate_word(std::string& string);
    int number_elements(std::string& string);

private slots:


    void on_choose_clicked();
    void on_tree_itemClicked(QTreeWidgetItem *item, int column);
};

#endif // CHOOSEDIR_H
