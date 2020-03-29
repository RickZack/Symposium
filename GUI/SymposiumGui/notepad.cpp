#include "notepad.h"
#include "ui_notepad.h"
//#include "directory.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QComboBox>
#include <QFontComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDatabase>
#include <QMenu>
#include <QMenuBar>
#include <QTextCodec>
#include <QTextEdit>
#include <QStatusBar>
#include <QToolBar>
#include <QTextCursor>
#include <QTextDocumentWriter>
#include <QTextList>
#include <QtDebug>
#include <QCloseEvent>
#include <QMessageBox>
#include <QMimeData>
#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printer)
#if QT_CONFIG(printdialog)
#include <QPrintDialog>
#endif
#include <QPrinter>
#if QT_CONFIG(printpreviewdialog)
#include <QPrintPreviewDialog>
#endif
#endif
#endif

#ifdef Q_OS_MAC
const QString rsrcPath = ":/resources/images/mac";
#else
const QString rsrcPath = ":/resources/images/win";
#endif

#include "Dispatcher/clientdispatcher.h"

notepad::notepad(QWidget *parent, int documentId, Symposium::privilege priv, Symposium::privilege privOpen,std::string pathToFile) :
    QMainWindow(parent),
    documentId(documentId), pathToFile(pathToFile), priv(priv), privOpen(privOpen),ui(new Ui::notepad)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->textEdit);
    priv=Symposium::privilege::owner;
    privOpen=Symposium::privilege::owner;
    //this->setToolButtonStyle(Qt::ToolButtonFollowStyle);
    if(privOpen!=Symposium::privilege::readOnly)
        setupTextActions();
    ui->textEdit->setThisUserPrivilege(privOpen);
    pathToFile="/1/2/3/4/5/6/7";
    //us=cl->getUser();
    QMenu *userMenu=menuBar()->addMenu(tr("Users"));
    userMenu->addAction(tr("Online Users"), this, &notepad::visualizeUsers);
    if(priv==Symposium::privilege::owner)
        userMenu->addAction(tr("All users"), this, &notepad::visualizeAllUsers);
    if(priv==Symposium::privilege::owner)
    {
        QMenu *shareMenu=menuBar()->addMenu(tr("Share File"));
        shareMenu->addAction(tr("Make all links inactive"), this, &notepad::inactiveLink);
        shareMenu->addAction(tr("Make all links active"), this, &notepad::activeAlwaysLink);
        shareMenu->addAction(tr("Make all links active for a certain period"), this, &notepad::timerLink);
        shareMenu->addAction(tr("Make all links active for a limit number of shares"), this, &notepad::counterLink);
    }

    //------------------------------------------------------PARTE DA DECOMMENTARE

    /*ui->textEdit->setDocumentId(documentId);
    ui->textEdit->setClientDispatcher(cl);
    ui->textEdit->setThisUserSiteId(us.getSiteId());
    const std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData>> users=cl->onlineUser(int documentID)
    ui->textEdit->constractLabelsCursors(users, us.getSiteId());
    */
    //----------------------------------------------------------------------------

    //-------------------------------------------------PARTE DA CANCELLARE
    Symposium::user *u1=new Symposium::user("Mario", "AP@ssw0rd!", "Mariuz", ":/resources/avatar/beaver.png", 1, nullptr);
    Symposium::user *u2=new Symposium::user("Carlo", "AP@ssw0rd!", "Carluz", ":/resources/avatar/boar.png", 2, nullptr);
    Symposium::user *u3=new Symposium::user("Federico", "AP@ssw0rd!", "Fede", ":/resources/avatar/bull.png", 3, nullptr);
    std::pair<Symposium::user*, Symposium::sessionData> p1{u1, Symposium::sessionData(Symposium::privilege::modify, 10, 3)};
    std::pair<Symposium::user*, Symposium::sessionData> p2{u2, Symposium::sessionData(Symposium::privilege::modify, 7, 6)};
    std::pair<Symposium::user*, Symposium::sessionData> p3{u3, Symposium::sessionData(Symposium::privilege::readOnly, 20, 0)};
    std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData>> onlineUsers;
    onlineUsers.push_front(p1);
    onlineUsers.push_front(p2);
    onlineUsers.push_front(p3);
    ui->textEdit->constractLabelsCursors(onlineUsers, u1->getSiteId());
    ui->textEdit->insertCurrentUser(onlineUsers, u1->getSiteId());
    ui->textEdit->addUser(4, "Tizio");
    ui->textEdit->changePosition(4, 1, 1);


    //---------------------------------------------------------------------

    ui->textEdit->setCursorWidth(3);
    if(privOpen==Symposium::privilege::readOnly)
         ui->textEdit->setReadOnly(true);

}

notepad::~notepad()
{
    delete ui;
}

void notepad::moveUserCursor(int siteID, int block, int column)
{
    ui->textEdit->changePosition(siteID, block, column);
}

void notepad::removeUserCursor(int siteID)
{
    ui->textEdit->removeUser(siteID);
}

void notepad::addUserCursor(int siteID, std::string username)
{
    ui->textEdit->addUser(siteID, username);
}


void notepad::on_Export_PDF_triggered()
{

#ifndef QT_NO_PRINTER
//! [0]
    QFileDialog fileDialog(this, tr("Export PDF"));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setMimeTypeFilters(QStringList("application/pdf"));
    fileDialog.setDefaultSuffix("pdf");
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    QString fileName = fileDialog.selectedFiles().first();
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    ui->textEdit->document()->print(&printer);
    statusBar()->showMessage(tr("Exported \"%1\"")
                             .arg(QDir::toNativeSeparators(fileName)));
//! [0]
#endif

}

void notepad::on_actionCopy_triggered()
{
    ui->textEdit->copy();
}

void notepad::on_actionCut_triggered()
{
    ui->textEdit->cut();
}

void notepad::on_actionPaste_triggered()
{
    ui->textEdit->paste();
}


void notepad::setupTextActions()
{
    QToolBar *tb = addToolBar(tr("Format Actions"));
    QMenu *menu = menuBar()->addMenu(tr("F&ormat"));

    const QIcon boldIcon = QIcon::fromTheme("format-text-bold", QIcon(rsrcPath + "/textbold.png"));
    actionTextBold = menu->addAction(boldIcon, tr("&Bold"), this, &notepad::textBold);
    actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
    actionTextBold->setPriority(QAction::LowPriority);
    QFont bold;
    bold.setBold(true);
    actionTextBold->setFont(bold);
    tb->addAction(actionTextBold);
    actionTextBold->setCheckable(true);

    const QIcon italicIcon = QIcon::fromTheme("format-text-italic", QIcon(rsrcPath + "/textitalic.png"));
    actionTextItalic = menu->addAction(italicIcon, tr("&Italic"), this, &notepad::textItalic);
    actionTextItalic->setPriority(QAction::LowPriority);
    actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
    QFont italic;
    italic.setItalic(true);
    actionTextItalic->setFont(italic);
    tb->addAction(actionTextItalic);
    actionTextItalic->setCheckable(true);

    const QIcon underlineIcon = QIcon::fromTheme("format-text-underline", QIcon(rsrcPath + "/textunder.png"));
    actionTextUnderline = menu->addAction(underlineIcon, tr("&Underline"), this, &notepad::textUnderline);
    actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
    actionTextUnderline->setPriority(QAction::LowPriority);
    QFont underline;
    underline.setUnderline(true);
    actionTextUnderline->setFont(underline);
    tb->addAction(actionTextUnderline);
    actionTextUnderline->setCheckable(true);

    menu->addSeparator();

    const QIcon leftIcon = QIcon::fromTheme("format-justify-left", QIcon(rsrcPath + "/textleft.png"));
    actionAlignLeft = new QAction(leftIcon, tr("&Left"), this);
    actionAlignLeft->setShortcut(Qt::CTRL + Qt::Key_L);
    actionAlignLeft->setCheckable(true);
    actionAlignLeft->setPriority(QAction::LowPriority);
    const QIcon centerIcon = QIcon::fromTheme("format-justify-center", QIcon(rsrcPath + "/textcenter.png"));
    actionAlignCenter = new QAction(centerIcon, tr("C&enter"), this);
    actionAlignCenter->setShortcut(Qt::CTRL + Qt::Key_E);
    actionAlignCenter->setCheckable(true);
    actionAlignCenter->setPriority(QAction::LowPriority);
    const QIcon rightIcon = QIcon::fromTheme("format-justify-right", QIcon(rsrcPath + "/textright.png"));
    actionAlignRight = new QAction(rightIcon, tr("&Right"), this);
    actionAlignRight->setShortcut(Qt::CTRL + Qt::Key_R);
    actionAlignRight->setCheckable(true);
    actionAlignRight->setPriority(QAction::LowPriority);
    const QIcon fillIcon = QIcon::fromTheme("format-justify-fill", QIcon(rsrcPath + "/textjustify.png"));
    actionAlignJustify = new QAction(fillIcon, tr("&Justify"), this);
    actionAlignJustify->setShortcut(Qt::CTRL + Qt::Key_J);
    actionAlignJustify->setCheckable(true);
    actionAlignJustify->setPriority(QAction::LowPriority);

    // Make sure the alignLeft  is always left of the alignRight
    QActionGroup *alignGroup = new QActionGroup(this);
    connect(alignGroup, &QActionGroup::triggered, this, &notepad::textAlign);

    if (QApplication::isLeftToRight()) {
        alignGroup->addAction(actionAlignLeft);
        alignGroup->addAction(actionAlignCenter);
        alignGroup->addAction(actionAlignRight);
    } else {
        alignGroup->addAction(actionAlignRight);
        alignGroup->addAction(actionAlignCenter);
        alignGroup->addAction(actionAlignLeft);
    }
    alignGroup->addAction(actionAlignJustify);

    tb->addActions(alignGroup->actions());
    menu->addActions(alignGroup->actions());

    menu->addSeparator();

    QPixmap pix(16, 16);
    pix.fill(Qt::black);
    actionTextColor = menu->addAction(pix, tr("&Color..."), this, &notepad::textColor);
    tb->addAction(actionTextColor);

    tb = addToolBar(tr("Format Actions"));
    tb->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    addToolBarBreak(Qt::TopToolBarArea);
    addToolBar(tb);

    comboStyle = new QComboBox(tb);
    tb->addWidget(comboStyle);
    comboStyle->addItem("Standard");
    comboStyle->addItem("Bullet List (Disc)");
    comboStyle->addItem("Bullet List (Circle)");
    comboStyle->addItem("Bullet List (Square)");
    comboStyle->addItem("Ordered List (Decimal)");
    comboStyle->addItem("Ordered List (Alpha lower)");
    comboStyle->addItem("Ordered List (Alpha upper)");
    comboStyle->addItem("Ordered List (Roman lower)");
    comboStyle->addItem("Ordered List (Roman upper)");
    comboStyle->addItem("Heading 1");
    comboStyle->addItem("Heading 2");
    comboStyle->addItem("Heading 3");
    comboStyle->addItem("Heading 4");
    comboStyle->addItem("Heading 5");
    comboStyle->addItem("Heading 6");

    connect(comboStyle, QOverload<int>::of(&QComboBox::activated), this, &notepad::textStyle);

    comboFont = new QFontComboBox(tb);
    tb->addWidget(comboFont);
    connect(comboFont, QOverload<const QString &>::of(&QComboBox::activated), this, &notepad::textFamily);

    comboSize = new QComboBox(tb);
    comboSize->setObjectName("comboSize");
    tb->addWidget(comboSize);
    comboSize->setEditable(true);

    const QList<int> standardSizes = QFontDatabase::standardSizes();
    foreach (int size, standardSizes)
        comboSize->addItem(QString::number(size));
    comboSize->setCurrentIndex(standardSizes.indexOf(QApplication::font().pointSize()));

    connect(comboSize, QOverload<const QString &>::of(&QComboBox::activated), this, &notepad::textSize);
}

void notepad::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}

void notepad::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(actionTextUnderline->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void notepad::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(actionTextItalic->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void notepad::textStyle(int styleIndex)
{
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextListFormat::Style style = QTextListFormat::ListStyleUndefined;

    switch (styleIndex) {
    case 1:
        style = QTextListFormat::ListDisc;
        break;
    case 2:
        style = QTextListFormat::ListCircle;
        break;
    case 3:
        style = QTextListFormat::ListSquare;
        break;
    case 4:
        style = QTextListFormat::ListDecimal;
        break;
    case 5:
        style = QTextListFormat::ListLowerAlpha;
        break;
    case 6:
        style = QTextListFormat::ListUpperAlpha;
        break;
    case 7:
        style = QTextListFormat::ListLowerRoman;
        break;
    case 8:
        style = QTextListFormat::ListUpperRoman;
        break;
    default:
        break;
    }
    cursor.beginEditBlock();

    QTextBlockFormat blockFmt = cursor.blockFormat();

    if (style == QTextListFormat::ListStyleUndefined) {
        blockFmt.setObjectIndex(-1);
        int headingLevel = styleIndex >= 9 ? styleIndex - 9 + 1 : 0; // H1 to H6, or Standard
        blockFmt.setHeadingLevel(headingLevel);
        cursor.setBlockFormat(blockFmt);

        int sizeAdjustment = headingLevel ? 4 - headingLevel : 0; // H1 to H6: +3 to -2
        QTextCharFormat fmt;
        fmt.setFontWeight(headingLevel ? QFont::Bold : QFont::Normal);
        fmt.setProperty(QTextFormat::FontSizeAdjustment, sizeAdjustment);
        cursor.select(QTextCursor::LineUnderCursor);
        cursor.mergeCharFormat(fmt);
        ui->textEdit->mergeCurrentCharFormat(fmt);
    } else {
        QTextListFormat listFmt;
        if (cursor.currentList()) {
            listFmt = cursor.currentList()->format();
        } else {
            listFmt.setIndent(blockFmt.indent() + 1);
            blockFmt.setIndent(0);
            cursor.setBlockFormat(blockFmt);
        }
        listFmt.setStyle(style);
        cursor.createList(listFmt);
    }

    cursor.endEditBlock();
}



void notepad::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = ui->textEdit->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    ui->textEdit->mergeCurrentCharFormat(format);
}


void notepad::textColor()
{
    QColor col = QColorDialog::getColor(ui->textEdit->textColor(), this);
    if (!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
    colorChanged(col);
}

void notepad::textFamily(const QString &f)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeFormatOnWordOrSelection(fmt);
}

void notepad::textSize(const QString &p)
{
    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
}

void notepad::textAlign(QAction *a)
{
    if (a == actionAlignLeft)
        ui->textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    else if (a == actionAlignCenter)
        ui->textEdit->setAlignment(Qt::AlignHCenter);
    else if (a == actionAlignRight)
        ui->textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    else if (a == actionAlignJustify)
        ui->textEdit->setAlignment(Qt::AlignJustify);
}

void notepad::colorChanged(const QColor &c)
{
    QPixmap pix(16, 16);
    pix.fill(c);
    actionTextColor->setIcon(pix);
}

void notepad::fontChanged(const QFont &f)
{
    comboFont->setCurrentIndex(comboFont->findText(QFontInfo(f).family()));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(f.pointSize())));
    actionTextBold->setChecked(f.bold());
    actionTextItalic->setChecked(f.italic());
    actionTextUnderline->setChecked(f.underline());
}

void notepad::currentCharFormatChanged(const QTextCharFormat &format)
{
    fontChanged(format.font());
    colorChanged(format.foreground().color());

}


void notepad::visualizeUsers()
{
    onlineuser = new onlineusers(this, priv, documentId, us, pathToFile);
    onlineuser->setClientDispatcher(cl);
    //onlineuser->onlineUsers=cl->onlineUser(documentID);
    //cl->setOnlineUser(onlineuser);
    onlineuser->exec();
}

void notepad::visualizeAllUsers()
{
    alluserWindow = new alluser(this,  priv, documentId, us, pathToFile);
    alluserWindow->setClientDispatcher(cl);
    //alluserWindow->users=cl->allUser(documentID);
    //cl->setAllUser(alluserWindow);
    alluserWindow->exec();
}

void notepad::inactiveLink()
{
    nonlinkwindow = new activenonlink(this, documentId, pathToFile);
    nonlinkwindow->setClientDispatcher(cl);
    //cl->setActiveNonLink(nonlinkwindow);
    nonlinkwindow->exec();

}

void notepad::activeAlwaysLink()
{
    alwayslinkwindow = new activealwayslink(this, documentId, pathToFile);
    alwayslinkwindow->setClientDispatcher(cl);
    //cl->setActiveAlwaysLink(alwayslinkwindow);
    alwayslinkwindow->exec();

}

void notepad::timerLink()
{
    timerlinkwindow = new activetimerlink(this, documentId, pathToFile);
    timerlinkwindow->setClientDispatcher(cl);
    //cl->setActiveTimerLink(timerlinkwindow);
    timerlinkwindow->exec();
}

void notepad::counterLink()
{
    counterlinkwindow = new activecounterlink(this, documentId, pathToFile);
    counterlinkwindow->setClientDispatcher(cl);
    //cl->setActiveCounterLink(counterlinkwindow);
    counterlinkwindow->exec();
}



void notepad::closeEvent(QCloseEvent *event){
    // potrebbe essere passata dal server la stringa relativa all'ultima cartella aperta
    // oppure aprire direttamente la directory iniziale

    int doc=documentId;
    //cl->closeSource(this->doc)
    event->ignore();
    this->notepad::hide();



}


void notepad::keyReleaseEvent(QKeyEvent *event)
{

    QTextCursor cursor= ui->textEdit->textCursor();
    int pos=cursor.position();
    if (event->key()==Qt::Key_Backspace){
        int column=cursor.positionInBlock();
        int row= cursor.blockNumber();
        const std::pair<int, int> indexes={row,column};
        // id del documento che ho aperto e la coppia di indici
        //localRemouve(this->documentId,&indexes);

    }else if(event->key()==Qt::Key_CapsLock || event->key()==Qt::Key_Shift || event->key()==Qt::Key_Control
             ||event->key()==Qt::Key_Alt || event->key()==Qt::Key_Escape || event->key()==Qt::Key_F1 ||event->key()==Qt::Key_F2 ||
             event->key()==Qt::Key_F3 ||event->key()==Qt::Key_F4 ||event->key()==Qt::Key_F5 ||event->key()==Qt::Key_F6 ||
             event->key()==Qt::Key_F7 ||event->key()==Qt::Key_F8 ||event->key()==Qt::Key_F9 ||event->key()==Qt::Key_F10 ||
             event->key()==Qt::Key_F11 || event->key()==Qt::Key_F12 || event->key()==Qt::Key_Menu ||
             event->key()==Qt::Key_Pause || event->key()==Qt::Key_Insert ||event->key()==Qt::Key_AltGr ||
             event->key()==Qt::Key_Tab || event->key()==Qt::Key_Up || event->key()==Qt::Key_Down ||
             event->key()==Qt::Key_Delete || event->key()==Qt::Key_NumLock || event->key()==Qt::Key_Left ||
             event->key()==Qt::Key_Right || event->key()==Qt::Key_Meta ||event->key()==Qt::Key_unknown || event->modifiers() & Qt::ControlModifier & event->text()!="\u0016"){
        return;
    }else if(event->text()=="\u0016"){
        this->contV_action(pos);


    }else{
        QString testo=event->text();
        int column=cursor.positionInBlock();
        column= column-1;
        int row= cursor.blockNumber();

        const wchar_t* ch=testo.toStdWString().c_str();
        const std::pair<int, int> indexes={row,column};

        QTextCharFormat format = cursor.charFormat();
        QFont font= format.font();
        bool isBold= font.bold();
        bool isUnderlined=font.underline();
        bool isItalic=font.italic();
        std::string fontFamily=font.family().toStdString();
        QColor col=format.foreground().color();
        int blue=col.blue();
        int red=col.red();
        int green=col.green();

        // struct format charFormat={familyFont,isBold,isUnderlined,isItalic,blue,red,yellow}
        //std::vector<int> &pos;
        //bool verified=false;

        //cl->localInsert(this->documentId, symbol &sym, &indexes)

    }
}

void notepad::contV_action(int pos){
    QTextCursor curs= ui->textEdit->textCursor();
    int posAct= curs.position();
    int posTmp=pos;
    int dim=posAct-pos;
    int count=0;

    while(count!=dim){
        curs.setPosition(posTmp,QTextCursor::MoveAnchor);
        curs.setPosition(posTmp+1,QTextCursor::KeepAnchor);
        QString charact=curs.selectedText();
        curs.clearSelection();
        int column=curs.positionInBlock();
        column= column-1;
        int row= curs.blockNumber();
        const wchar_t* ch=charact.toStdWString().c_str();
        const std::pair<int, int> indexes={row,column};
        QTextCharFormat format = curs.charFormat();
        QFont font= format.font();
        bool isBold= font.bold();
        bool isUnderlined=font.underline();
        bool isItalic=font.italic();
        std::string fontFamily=font.family().toStdString();
        QColor col=format.foreground().color();
        int blue=col.blue();
        int red=col.red();
        int green=col.green();
        //cl->localInsert(this->documentId, &sym, &indexes)
        count++;posTmp++;

    }

}

void notepad::remoteInsert(Symposium::symbol sym,Symposium::uint_positive_cnt siteId, std::pair<int,int> indexes){

    /*
    int row=indexes.first;
    int column=indexes.second;

    QTextCursor curs=ui->textCursor();
    int actBlock=curs.blockNumber();
    int actColm=curs.positionInBlock();

    Symposium::format f= sym.getCharFormat();
    QTextCharFormat ch_format;
    QFont ch_font;
    ch_font.setFamily(QString::fromStdString(f.familyType));
    ch_font.setBold(f.isBold);
    ch_font.setUnderline(f.isUnderlined);
    ch_font.setItalic(f.isItalic);
    QColor col(f.red,f.green,f.blue);
    QBrush brh(col);

    ch_format.setFont(ch_font);
    ch_format.setBackground(brh);

    ui->textEdit->changePosition(row,column);
    curs.insertText(sym.getChar,ch_format);
    ui->textEdit->changePosition(actRow,actColm);
   */

}

void notepad::remoteDelete(std::pair<int,int> indexes){
    int block=indexes.first;
    int column=indexes.second;
    column++;
    QTextCursor curs=ui->textEdit->textCursor();
    int actBlock=curs.blockNumber();
    int actColm=curs.positionInBlock();
    ui->textEdit->changePosition(block,column);
    curs.deleteChar();
    ui->textEdit->changePosition(actBlock,actColm);

}


void notepad::setClientDispatcher(Symposium::clientdispatcher *cl)
{
    this->cl = cl;
}

void notepad::on_textEdit_cursorPositionChanged()
{
    ui->textEdit->thisUserChangePosition(1);
    //ui->textEdit->thisUserChangePosition(us.getSiteId());

}

