#ifndef NOTEPAD_H
#define NOTEPAD_H

#include <QMainWindow>
#include  <QtCore>
#include  <QtGui>
#include <QTextEdit>


#include "currentusers.h"
#include "../../privilege.h"
#include "activetimerlink.h"
#include "activecounterlink.h"
#include "activealwayslink.h"



class QComboBox;
class QFontComboBox;
class QPrinter;

namespace Ui {
class notepad;
}

class notepad : public QMainWindow
{
    Q_OBJECT

public:
    explicit notepad(QWidget *parent = nullptr);
    ~notepad();


private slots:
    void on_actionPaste_triggered();
    void on_Export_PDF_triggered();
    void on_actionCopy_triggered();
    void on_actionCut_triggered();
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();

    void textBold();
    void textUnderline();
    void textItalic();
    void textFamily(const QString &f);
    void textSize(const QString &p);
    void textStyle(int styleIndex);
    void textColor();
    void textAlign(QAction *a);
    void currentCharFormatChanged(const QTextCharFormat &format);

private:
    Ui::notepad *ui;
    QString mFilename;

    currentUsers *currentuserswindow;
    Symposium::privilege priv;
    std::string pathToFile;
    activetimerlink *timerlinkwindow;
    activecounterlink *counterlinkwindow;
    activealwayslink *alwayslinkwindow;


    void setupTextActions();
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void colorChanged(const QColor &c);
    void fontChanged(const QFont &f);


    void visualizeUsers();
    void visualizeAllUsers();
    void inactiveLink();
    void activeAlwaysLink();
    void timerLink();
    void counterLink();

    QAction *actionTextBold;
    QAction *actionTextUnderline;
    QAction *actionTextItalic;
    QAction *actionTextColor;
    QAction *actionAlignLeft;
    QAction *actionAlignCenter;
    QAction *actionAlignRight;
    QAction *actionAlignJustify;

    QComboBox *comboStyle;
    QFontComboBox *comboFont;
    QComboBox *comboSize;

    QTextEdit *textEdit;
};

#endif // NOTEPAD_H
