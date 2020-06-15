#ifndef CHOOSEDIR_H
#define CHOOSEDIR_H

#include <QDialog>
#include <QTreeWidgetItem>
#include <algorithm>
#include <stdlib.h>
#include <vector>
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
    explicit choosedir(QWidget *parent = nullptr, std::string pathDir="");
    ~choosedir();

    void treeGenerate(std::string str, int count);

private:
    Ui::choosedir *ui;

    int spaces;
    std::string id;

    /**
     * @brief separate the first word in the string passed
     * @param string the string to proccess
     * @return the first word in the string
     */
    std::string separate_word(std::string& string);
    /**
     * @brief count the number of elements presented in filesystem of the user
     * @param string the string contained all elements
     * @return a number of elemets
     */
    int number_elements(std::string& string);

private slots:
    /**
     * @brief confirm the directory selected
     */
    void on_choose_clicked();
    /**
     * @brief select the directory
     */
    void on_tree_itemClicked(QTreeWidgetItem *item, int column);
};

#endif // CHOOSEDIR_H
