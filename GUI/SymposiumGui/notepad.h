#ifndef NOTEPAD_H
#define NOTEPAD_H

#include <QMainWindow>
#include  <QtCore>
#include  <QtGui>
#include <QTextEdit>
#include <QCloseEvent>
#include <QResizeEvent>
#include <symwininterface.h>


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

class notepad : public QMainWindow, public SymNotepadWinInterface
{
    Q_OBJECT

public:
    explicit notepad(QWidget *parent,Symposium::privilege priv,Symposium::privilege privOpen,std::string pathToFile,const Symposium::document& doc, Symposium::uint_positive_cnt::type fileID, SymWinInterface& si, bool parentIsTransient=false);

    //Not used at the moment
    void success() override{}
    void failure(const QString&) override{}
    Symposium::uint_positive_cnt::type getId() override;

    void setClientDispatcher(Symposium::clientdispatcher *cl);
    ~notepad() override;

    void moveUserCursor(Symposium::uint_positive_cnt::type siteID, unsigned block, unsigned column) override;
    void removeUserCursor(Symposium::uint_positive_cnt::type siteID) override;
    void addUserCursor(Symposium::uint_positive_cnt::type siteID, const std::string& username) override;

    /**
     * @brief remoteInsert inserts a character derived from another user
     * @param siteId referred to the user that wants to insert the char
     * @param indexes the position in which the character has to be inserted
     */
    void remoteInsert(const Symposium::symbol& sym, Symposium::uint_positive_cnt::type siteId, const std::pair<unsigned,unsigned>& indexes) override;

    /**
     * @brief remoteDelete deletes a character derived from another user
     * @param indexes the position in which the character that has to be removed is is
     * @param siteId referred to the user that want to delete
     */
    void remoteDelete(const std::pair<unsigned, unsigned>& indexes, Symposium::uint_positive_cnt::type siteId) override;

    /**
     * @brief verifySymbol inserts the character with the right Color
     * @param sym the symbol that has been verified
     * @param siteId
     * @param indexes
     */
    void verifySymbol(const Symposium::symbol& sym, const std::pair<unsigned, unsigned>& indexes) override;

    /**
     * @brief closeEvent closes the window
     * @param event
     */
    void closeEvent(QCloseEvent *event);

    void resizeEvent(QResizeEvent *event);

    void showLabels();

    void setreadonly();

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

    void on_textEdit_textChanged();

    void on_hideUsers_clicked();

    void on_showUsers_clicked();

    bool event(QEvent *e) override;

    void on_pushButton_clicked();

private:
    Ui::notepad *ui;
    QString mFilename;
    Symposium::uint_positive_cnt::type documentId;
    Symposium::uint_positive_cnt::type fileId;
    //Symposium::user& us;
    const Symposium::document& doc;
    static constexpr wchar_t  emptyChar='~';
    bool showUsers=true;

    std::string pathToFile;
    Symposium::privilege priv;
    Symposium::privilege privOpen;
    bool insertOthCh=false;
    bool highActivated=false;
    QColor colPos;
    bool okPaste=false;
    int dim;

    unsigned indexStyle;                                                /**< to set the index for the style */
    unsigned left;                                                      /**< to set for each row if there is a left alignment */
    unsigned right;                                                     /**< to set for each row if there is a right alignment */
    unsigned center;                                                    /**< to set for each row if there is a center alignment */
    unsigned justify;                                                   /**< to set for each row if there is a justified alignment */

    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void colorChanged(const QColor &c);
    void fontChanged(const QFont &f);

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
     */
    void contV_action();

    void addCursor();
    void removeCursor();
    void changeCursorPos();


    void colorText();
    void addStyleFormat();


    std::string idDoc;

    /**
     * @brief fillTextEdit to fill the textEdit block with the characters already present
     */
    void fillTextEdit();

    /**
     * @brief fixAlignment to fix for each row the alignment stile
     */
    void fixAlignment();

    Symposium::document documentoProva;

    /**
     * @brief sendSymbolToInsert to set the correct parameters in such a way to call the localInsert method
     * @param row
     * @param column
     * @param text: the character inserted into the document
     * @param format: the format of the character inserted into the document
     */
    void sendSymbolToInsert(int row, int column, QString text, QTextCharFormat format);

    /**
     * @brief deleteMultipleLines to handle the elimination of multiple lines by selection
     * @param sR: the starting row index of the selection
     * @param eR: the ending row index of the selection
     * @param c:  the starting column index of the selection
     * @param sL: the dimension of the selection
     * @param lines: a boolen to declare if the last line has been deleted or not
     */
    void deleteMultipleLines(int sR, int eR, int c, int sL, bool lines);

    /**
     * @brief countCharsInLine: counts the number of the chars in a line
     * @param line: index of the line
     * @return number of chars
     */
    int countCharsInLine(int line) const;
    void insertusers();

};

#endif // NOTEPAD_H
