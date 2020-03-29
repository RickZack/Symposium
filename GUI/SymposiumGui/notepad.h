#ifndef NOTEPAD_H
#define NOTEPAD_H

#include <QMainWindow>
#include  <QtCore>
#include  <QtGui>
#include <QTextEdit>
#include <QCloseEvent>


#include "onlineusers.h"
#include "alluser.h"
#include "../../privilege.h"
#include "activetimerlink.h"
#include "activecounterlink.h"
#include "activealwayslink.h"
#include "activenonlink.h"



namespace Symposium{
class clientdispatcher;
}

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
    explicit notepad(QWidget *parent = nullptr, int documentId=0,Symposium::privilege priv=Symposium::privilege::none,Symposium::privilege privOpen=Symposium::privilege::none,std::string pathToFile=" ");
    void setClientDispatcher(Symposium::clientdispatcher *cl);
    ~notepad();

    void moveUserCursor(int siteID, int block, int column);
    void removeUserCursor(int siteID);
    void addUserCursor(int siteID, std::string username);
    void remoteInsert(Symposium::symbol, Symposium::uint_positive_cnt siteId,std::pair<int,int> indexes);
    void remoteDelete(std::pair<int, int> indexes);


private slots:
    void on_actionPaste_triggered();
    void on_Export_PDF_triggered();
    void on_actionCopy_triggered();
    void on_actionCut_triggered();
    void textBold();
    void textUnderline();
    void textItalic();
    void textFamily(const QString &f);
    void textSize(const QString &p);
    void textStyle(int styleIndex);
    void textColor();
    void textAlign(QAction *a);
    void currentCharFormatChanged(const QTextCharFormat &format);
    void on_textEdit_cursorPositionChanged();


private:
    Ui::notepad *ui;
    QString mFilename;
    int documentId;
    Symposium::user us;
    Symposium::clientdispatcher *cl;

    onlineusers *onlineuser;
    alluser *alluserWindow;
    std::string pathToFile;
    Symposium::privilege priv;
    Symposium::privilege privOpen;
    activetimerlink *timerlinkwindow;
    activecounterlink *counterlinkwindow;
    activealwayslink *alwayslinkwindow;
    activenonlink *nonlinkwindow;


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

    void closeEvent(QCloseEvent *event);
    void keyReleaseEvent(QKeyEvent *e);

    void contV_action(int pos);



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

    std::string idDoc;



};

#endif // NOTEPAD_H
