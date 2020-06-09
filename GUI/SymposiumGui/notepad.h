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
    void success() override;

    void failure(const QString&) override;

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
     * @brief update the alignment and/or the indexStyle of the document's block @e row
     * @param newLineStyle new alignment and indexStyle to apply
     * @param row the row to apply @e newLineStyle to
     */
    void editLineStyle(const std::pair<Symposium::alignType, unsigned int> &newLineStyle, unsigned int row) override;

    /**
     * @brief closeEvent closes the window
     * @param event
     */
    void closeEvent(QCloseEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

    void showLabels();

    void setreadonly();

    /**
     * @brief called by clientdispatcher when there is some error with connection and perform logout of the page
     */
    void errorConnectionLogout();

    /**
     * @brief fixAlignment to fix for each row the alignment stile
     */
    void fixAlignment();




protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void alignmentChanged(Qt::Alignment a);

private slots:
    void on_actionPaste_triggered();
    void on_Export_PDF_triggered();
    void on_actionCopy_triggered();
    void on_actionCut_triggered();
    void textFamily(const QString &f);
    void textSize(const QString &p);
    QTextListFormat::Style textStyle(int styleIndex);
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
    void modifyWinTitle(Symposium::uint_positive_cnt::type resId, const QString& newName);
    void showTextEditContextMenu(const QPoint &pos);

public:
    static constexpr unsigned alphaValue=50; /**< value used as soften color for insertion and highlight*/

private:
    Ui::notepad *ui;
    QString mFilename;
    Symposium::uint_positive_cnt::type documentId;
    Symposium::uint_positive_cnt::type fileId;
    Symposium::user us;
    const Symposium::document& doc;
    bool showUsers=true;
    QMenu *m_textEditContextMenu;

    std::string pathToFile;
    Symposium::privilege priv;
    Symposium::privilege privOpen;
    bool NotRefreshLabels=false;                                        /**< indicates whether a refresh of users' labels may be needed */
    bool highActivated=false;
    QColor colPos;                                                      /**< indicates the color the text to insert */
    bool okPaste=false;
    unsigned dim;                                                       /**< to understand the dimension of the pasted string (Useful for ctrl+C,ctrl+V options) */
    Symposium::alignType alignment=Symposium::alignType::left;          /**< to set the alignment type for the style */
    unsigned indexStyle;                                                /**< to set the index for the style */                                                              
    unsigned numChars;                                                  /**< to save the numChars contained in the document */
    std::string labelChars;                                             /**< to convert the @numChars into a string*/
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
    void handleDeleteKey(QKeyEvent *event);
    std::string constructAbsolutePath();

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


    Symposium::document documentoProva;

    /**
     * @brief sendSymbolToInsert to set the correct parameters in such a way to call the localInsert method
     * @param row
     * @param column
     * @param text: the character inserted into the document
     * @param format: the format of the character inserted into the document
     */
public:
    void sendSymbolToInsert(unsigned row, unsigned column, QString text, QTextCharFormat format);

    void insertusers();

private:
    /**
     * @brief countCharsInLine: counts the number of the chars in a line
     * @param line: index of the line
     * @return number of chars
     */
    unsigned int countCharsInLine(const unsigned line) const;

    /**
     * @brief uncolorText to restore the default background color
     */
    void uncolorText();

    void handleChangeFormat(unsigned int i, unsigned int f);
};

#endif // NOTEPAD_H
