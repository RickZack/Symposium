#ifndef CHOOSEDIR_H
#define CHOOSEDIR_H

#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <vector>

namespace Ui {
class choosedir;
}

class choosedir : public QDialog
{
    Q_OBJECT

public:
    explicit choosedir(QWidget *parent = nullptr);
    ~choosedir();
    void buttonGenerate(QString str);

private:
    Ui::choosedir *ui;
    QPushButton *prova;
    QPushButton* buttons[100];
    std::vector<QPushButton *>::iterator it;
private slots:
    void choose();

};

#endif // CHOOSEDIR_H
