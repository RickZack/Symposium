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
    /**
     * @brief confirm the selection
     */
    void on_pushButton_clicked();
    /**
     * @brief set the path to the selected image
     */
    void on_beaver_clicked();
    /**
     * @brief set the path to the selected image
     */
    void on_boar_clicked();
    /**
     * @brief set the path to the selected image
     */
    void on_bull_clicked();
    /**
     * @brief set the path to the selected image
     */
    void on_deer_clicked();
    /**
     * @brief set the path to the selected image
     */
    void on_duck_clicked();
    /**
     * @brief set the path to the selected image
     */
    void on_hed_clicked();
    /**
     * @brief set the path to the selected image
     */
    void on_hippo_clicked();
    /**
     * @brief set the path to the selected image
     */
    void on_ostrich_clicked();
    /**
     * @brief set the path to the selected image
     */
    void on_owl_clicked();
    /**
     * @brief set the path to the selected image
     */
    void on_zoo_clicked();

private:
    Ui::icon *ui;
};

#endif // ICON_H
