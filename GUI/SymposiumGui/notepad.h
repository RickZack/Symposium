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
    explicit notepad(QWidget *parent = nullptr, Symposium::uint_positive_cnt::type documentId=0,Symposium::privilege priv=Symposium::privilege::none,Symposium::privilege privOpen=Symposium::privilege::none,std::string pathToFile=" ",Symposium::document doc=Symposium::document());
    void setClientDispatcher(Symposium::clientdispatcher *cl);
    ~notepad();

    void moveUserCursor(Symposium::uint_positive_cnt::type siteID, int block, int column);
    void removeUserCursor(Symposium::uint_positive_cnt::type siteID);
    void addUserCursor(Symposium::uint_positive_cnt::type siteID, std::string username);

    /**
     * @brief remoteInsert inserts a character derived from another user
     * @param siteId referred to the user that wants to insert the char
     * @param indexes the position in which the character has to be inserted
     */
    void remoteInsert(Symposium::symbol, Symposium::uint_positive_cnt::type siteId,std::pair<int,int> indexes);

    /**
     * @brief remoteDelete deletes a character derived from another user
     * @param indexes the position in which the character that has to be removed is is
     * @param siteId referred to the user that want to delete
     */
    void remoteDelete(std::pair<int, int> indexes, Symposium::uint_positive_cnt::type siteId);

    /**
     * @brief verifySymbol inserts the character with the right Color
     * @param sym the symbol that has been verified
     * @param siteId
     * @param indexes
     */
    void verifySymbol(Symposium::symbol sym, std::pair<int, int> indexes);

    /**
     * @brief closeEvent closes the window
     * @param event
     */
    void closeEvent(QCloseEvent *event);

    void showLabels();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void on_actionPaste_triggered();
    void on_Export_PDF_triggered();
    void on_actionCopy_triggered();
    void on_actionCut_triggered();
    void textFamily(const QString &f);
    void textSize(const QString &p);
    void textStyle(int styleIndex);
    void textColor();
    void textAlign(QAction *a);
    void currentCharFormatChanged(const QTextCharFormat &format);
    void on_textEdit_cursorPositionChanged();
    void on_actionhighlight_triggered();

    void on_actionBoldFont_triggered();
    void on_actionItalicFont_triggered();


    void on_actionUnderlineFont_triggered();

    void on_actionAlignTextLeft_triggered();

    void on_actionAlignCenter_triggered();

    void on_actionAlignTextRight_triggered();

    void on_actionAlignTextJustify_triggered();

    void on_actionColorText_triggered();

    //void on_comboBox_activated(int index);

    //void on_fontComboBox_activated(const QString &arg1);

private:
    Ui::notepad *ui;
    QString mFilename;
    Symposium::uint_positive_cnt::type documentId;
    Symposium::user us;
    Symposium::clientdispatcher *cl;
    Symposium::document doc;

    onlineusers *onlineuser;
    alluser *alluserWindow;
    std::string pathToFile;
    Symposium::privilege priv;
    Symposium::privilege privOpen;
    activetimerlink *timerlinkwindow;
    activecounterlink *counterlinkwindow;
    activealwayslink *alwayslinkwindow;
    activenonlink *nonlinkwindow;

    bool insertOthCh=false;
    bool highActivated=false;
    int insertedChars=0;
    int posBlock=0;
    QColor colPos;
    int pos;

    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void colorChanged(const QColor &c);
    void fontChanged(const QFont &f);


    void visualizeUsers();
    void visualizeAllUsers();
    void inactiveLink();
    void activeAlwaysLink();
    void timerLink();
    void counterLink();
    void restoreCursorPos();
    bool isAKeyToIgnore(QKeyEvent* event);
    void handleTextEditKeyPress(QKeyEvent* event);
    void handleDeleteKey();

    /**
     * @brief contV_action to handle with the CONTROL-C CONTROL-V actions
     * @param pos
     */
    void contV_action(int pos);

    void addCursor();
    void removeCursor();
    void changeCursorPos();


    void colorText();
    void addStyleFormat();

    QAction *actionTextBold;
    QAction *actionTextUnderline;
    QAction *actionTextItalic;
    QAction *actionTextColor;
    QAction *actionAlignLeft;
    QAction *actionAlignCenter;
    QAction *actionAlignRight;
    QAction *actionAlignJustify;
    QAction *actionSelect;

    QComboBox *comboStyle;
    QFontComboBox *comboFont;
    QComboBox *comboSize;

    QTextEdit *textEdit;

    std::string idDoc;

    /**
     * @brief fillTextEdit to fill the textEdit block with the characters already present
     */
    void fillTextEdit();

    //------------------------------------------------------------------------------------------------------------------------------------------
    void prova_remoteInsert();
    void verifySymbol2();
    void prova_remoteDelete();

    Symposium::document documentoProva;
    static constexpr wchar_t  emptyChar='~';

    void sendSymbolToInsert(int row, int column, QString text, QTextCharFormat format);
    int charsInLine=0;
    int columnForReturn=0;
    void deleteMultipleLines(int sR, int eR, int c, int sL, bool lines);
    int countCharsInLine(int line) const;
};

#endif // NOTEPAD_H
