#ifndef OPTION_H
#define OPTION_H

#include <QWidget>

namespace Ui {
class Option;
}

namespace Symposium{
class clientdispatcher;
}

class Option : public QWidget
{
    Q_OBJECT

public:
    explicit Option(QWidget *parent, Symposium::clientdispatcher* cl);
    ~Option();

private slots:
    void on_OK_button_clicked();

    void on_Cancel_button_clicked();

private:
    Ui::Option *ui;
    Symposium::clientdispatcher* cl;
};

#endif // OPTION_H
