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

notepad::notepad(QWidget *parent, Symposium::uint_positive_cnt::type documentID, Symposium::privilege priv, Symposium::privilege privOpen,std::string pathToFile,Symposium::document doc) :
    QMainWindow(parent),
    documentId(documentId), pathToFile(pathToFile), priv(priv), privOpen(privOpen),doc(doc),ui(new Ui::notepad)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->textEdit);
    backCol=ui->textEdit->textBackgroundColor();


    priv=Symposium::privilege::owner;
    privOpen=Symposium::privilege::owner;
    //this->setToolButtonStyle(Qt::ToolButtonFollowStyle);
    if(privOpen!=Symposium::privilege::readOnly)
        setupTextActions();
    ui->textEdit->setThisUserPrivilege(privOpen);
    pathToFile="/1/2/3/4/5/6/7";
    //us=cl->getUser();
    // Symposium::Color colorSymp=cl->getColor(this->documentId,us->getSiteId());
    // QColor myColor;
    // myColor=static_cast<QColor>(colorSymp);

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


    QMenu *menuSym=menuBar()->addMenu(tr("Prova RichText"));
    menuSym->addAction(tr("Verify Sym"),this,&notepad::verifySymbol2);
    menuSym->addAction(tr("Fill Text"),this,&notepad::provaFill);
    menuSym->addAction(tr("Remote Insert"),this,&notepad::prova_remoteInsert);
    menuSym->addAction(tr("Remote Delete"),this,&notepad::prova_remoteDelete);

    QMenu *userMenu2=menuBar()->addMenu(tr("ProvaCursori"));
    userMenu2->addAction(tr("Add Cursor"), this, &notepad::addCursor);
    userMenu2->addAction(tr("Change Cursor"), this, &notepad::changeCursorPos);
    userMenu2->addAction(tr("Remove Cursor"), this, &notepad::removeCursor);

    ui->actionhighlight->setIcon(QIcon(":/resources/cartelle/color_icon"));
    ui->actionhighlight->setCheckable(true);
    //this->fillTextEdit();
    this->provaFill();
    QTextCharFormat fmt= ui->textEdit->currentCharFormat();
    QColor lightCol=ui->textEdit->textColor();
    lightCol.setAlpha(180);
    fmt.setForeground(lightCol);
    ui->textEdit->mergeCurrentCharFormat(fmt);


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
    std::pair<Symposium::user*, Symposium::sessionData> p1{u1, Symposium::sessionData(Symposium::privilege::modify, 0, 0)};
    std::pair<Symposium::user*, Symposium::sessionData> p2{u2, Symposium::sessionData(Symposium::privilege::modify, 1, 2)};
    std::pair<Symposium::user*, Symposium::sessionData> p3{u3, Symposium::sessionData(Symposium::privilege::readOnly, 2, 0)};
    std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData>> onlineUsers;
    onlineUsers.push_front(p1);
    onlineUsers.push_front(p2);
    onlineUsers.push_front(p3);
    ui->textEdit->constractLabelsCursors(onlineUsers, u1->getSiteId());
    ui->textEdit->insertCurrentUser(onlineUsers, u1->getSiteId());


    //---------------------------------------------------------------------

    ui->textEdit->setCursorWidth(3);

    if(privOpen==Symposium::privilege::readOnly)
         ui->textEdit->setReadOnly(true);

}

notepad::~notepad()
{
    delete ui;
}

template<>
Symposium::Color::operator QColor() const{
    uint8_t r,g,b;
    std::tie(r,g,b)= this->getRgb();
    return QColor(r,g,b);
}


void notepad::moveUserCursor(Symposium::uint_positive_cnt::type siteID, int block, int column)
{
    ui->textEdit->changePosition(siteID, block, column);
}

void notepad::removeUserCursor(Symposium::uint_positive_cnt::type siteID)
{
    ui->textEdit->removeUser(siteID);
}

void notepad::addUserCursor(Symposium::uint_positive_cnt::type siteID, std::string username)
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

    //const QIcon colorsIcon(":/resources/cartelle/color_icon");
    //actionSelect= menu->addAction(colorsIcon,tr("Select"),this,&notepad::colorText);



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
    //mergeFormatOnWordOrSelection(fmt);
    colorChanged(col);

    //set a lighter color
    QColor lightCol=col;
    lightCol.setAlpha(180);
    this->colPos=lightCol;
    ui->textEdit->setTextColor(lightCol);

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
    ui->textEdit->setTextColor(c);

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

void notepad::fillTextEdit(){
    insertOthCh=true;
    QColor qCol;QTextCharFormat chFormat;
    QTextCursor curs=ui->textEdit->textCursor();
    QString ch;
    // save in symbols all the symbols contained in the document
    std::vector<std::vector<Symposium::symbol>> symbols= this->doc.getSymbols();
    for(size_t i=0;i<symbols.size();i++){
        for(size_t j=0;j<symbols[i].size();j++){
            int column=j;
            //extract the symbol
            Symposium::symbol sym=symbols[i][j];
            //estract the character
            ch[0]=sym.getCh();
            Symposium::format format=sym.getCharFormat();
            //estract the information about the font/color
            QFont font;
            font.setFamily(QString::fromStdString(format.familyType));
            font.setBold(format.isBold);
            font.setUnderline(format.isUnderlined);
            font.setItalic(format.isItalic);
            font.setPointSize(format.size);
            Symposium::Color col=format.col;
            //conversion from Color to QColor
            qCol=static_cast<QColor>(col);
            chFormat.setFont(font); chFormat.setForeground(qCol);
            // check if the highlight button is activated;
            // if yes-> highlight the inserted character with the color of the user
            if(this->highActivated){
                Symposium::Color colHigh=cl->getColor(this->documentId,sym.getSiteId());
                QColor colUser;
                colUser=static_cast<QColor>(colHigh);
                chFormat.setBackground(colUser);
            }

            // go to the position of the character
            ui->textEdit->changePosition(i,column);
            curs.insertText(ch,chFormat);
        }
        // to insert another Line
        curs.insertBlock();
    }
    insertOthCh=false;
    this->colorChanged(qCol);
    this->currentCharFormatChanged(chFormat);


    //set a lighter color
    QColor lightCol=qCol;
    lightCol.setAlpha(180);
    this->colPos=lightCol;

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
    ui->actionhighlight->setChecked(false);
    ui->actionhighlight->setCheckable(false);
    cl->closeSource(this->documentId);
    event->ignore();
    this->notepad::hide();



}


void notepad::keyReleaseEvent(QKeyEvent *event)
{

    QColor myC=Qt::yellow;
    myC.setAlpha(160);

    QTextCursor cursor= ui->textEdit->textCursor();

    int pos=cursor.position();
    if (event->key()==Qt::Key_Backspace){
        int column=cursor.positionInBlock();
        int row= cursor.blockNumber();
        const std::pair<int, int> indexes={row,column};
        //cl->localRemouve(this->documentId,&indexes);

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
    }else if(event->text()=="\u0016"){ // Control_V action
        this->contV_action(pos);

    }else{

        // is the first time that the user enters the document and writes it
        if(insertedChars==0){
            QTextCharFormat ch=ui->textEdit->currentCharFormat();
            QColor newCol=ch.foreground().color();
            QString charact=event->text();
            newCol.setAlpha(180);
            ch.setForeground(newCol);
            cursor.deletePreviousChar();

            if(this->highActivated){
               ch.setBackground(myC);
            }

             cursor.insertText(charact,ch);
        }

        // there is a number of chars>0 (don't care how many)
        insertedChars=1;


        QString testo=event->text();

        int column=cursor.positionInBlock();
        column= column-1;
        int row= cursor.blockNumber();

        std::string str=testo.toStdString();
        wchar_t ch=str[0];
        const std::pair<int, int> indexes={row,column};

        QTextCharFormat format = cursor.charFormat();
        QColor col=format.foreground().color();

        QFont font= format.font();
        bool isBold= font.bold();
        bool isUnderlined=font.underline();
        bool isItalic=font.italic();
        unsigned size=font.pointSize();
        std::string fontFamily=font.family().toStdString();
        QColor colC=format.foreground().color();
        int blue=col.blue();
        int red=col.red();
        int green=col.green();
        Symposium::Color myCol(red,blue,green);
        struct Symposium::format charFormat={fontFamily,isBold,isUnderlined,isItalic,size,myCol};
        std::vector<int> pos;
        // SISTEMARE IL SITEID E IL COUNTER IN SYMBOL
        Symposium::symbol sym(ch,0,0,pos,false);
        sym.setCharFormat(charFormat);
        // cl->localInsert(this->documentId, Symposium::symbol &sym, &indexes)

        // useful to understand if the user changes row (block) by using the mouse
        this->posBlock=cursor.blockNumber();
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
        std::string str=charact.toStdString();
        wchar_t ch=str[0];
        const std::pair<int, int> indexes={row,column};
        QTextCharFormat format = curs.charFormat();
        QFont font= format.font();
        unsigned size=font.pointSize();
        bool isBold= font.bold();
        bool isUnderlined=font.underline();
        bool isItalic=font.italic();
        std::string fontFamily=font.family().toStdString();
        QColor col=format.foreground().color();
        int blue=col.blue();
        int red=col.red();
        int green=col.green();
        Symposium::Color myCol(red,blue,green);
        struct Symposium::format charFormat={fontFamily,isBold,isUnderlined,isItalic,size,myCol};
        std::vector<int> pos;
        // SISTEMARE IL SITEID E IL COUNTER IN SYMBOL
        Symposium::symbol sym(ch,0,0,pos,false);
        sym.setCharFormat(charFormat);
        //cl->localInsert(this->documentId, symbol &sym, &indexes)
        count++;posTmp++;

    }

}

void notepad::addCursor()
{
    ui->textEdit->addUser(4, "Tizio");
}

void notepad::removeCursor()
{
    ui->textEdit->removeUser(4);
}

void notepad::changeCursorPos()
{
    ui->textEdit->changePosition(4, 1, 1);
}


void notepad::remoteInsert(Symposium::symbol sym,Symposium::uint_positive_cnt::type siteId, std::pair<int,int> indexes){

    insertOthCh=true;
    // take the position in which the character has to be added.
    int row=indexes.first;
    int column=indexes.second;

    QTextCursor curs=ui->textEdit->textCursor();
    // take the position of the actual cursor
    int actBlock=curs.blockNumber();
    int actColm=curs.positionInBlock();

    //extract information from sym to build the character to insert in the textEdit block
    Symposium::format f= sym.getCharFormat();
    QTextCharFormat ch_format;QFont ch_font;

    ch_font.setFamily(QString::fromStdString(f.familyType));
    ch_font.setBold(f.isBold);
    ch_font.setUnderline(f.isUnderlined);
    ch_font.setItalic(f.isItalic);
    ch_font.setPointSize(f.size);
    Symposium::Color col=f.col;

    // conversion from Symposium::Color to QColor
    QColor qCol;
    qCol=static_cast<QColor>(col);

    // set the font and the color to the character
    ch_format.setFont(ch_font);
    ch_format.setForeground(qCol);

    // check if the highlight button is activated;
    // if yes-> highlight the inserted character with the color of the user
    if(this->highActivated){
        Symposium::Color colHigh=cl->getColor(this->documentId,sym.getSiteId());
        QColor colUser;
        colUser=static_cast<QColor>(colHigh);
        ch_format.setBackground(colUser);
    }

    // go to the position of the character

    curs.movePosition(QTextCursor::Start);
    curs.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor,row);
    curs.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, column);
    ui->textEdit->setTextCursor(curs);
    ui->textEdit->changePosition(siteId,row,column++);

    //insert the character
    wchar_t symch=sym.getCh();
    QString ch;
    ch[0]=symch;
    curs.insertText(ch,ch_format);
    ui->textEdit->changePosition(siteId,row,column++);
    ui->textEdit->changePosition(actBlock,actColm);
    insertOthCh=false;

}
//------------------------------------------------------------------------------------------------------------------------------------------
//prova remoteInsert
void notepad::prova_remoteInsert(){

    insertOthCh=true;
    // take the position in which the character has to be added.
    int row=0;
    int column=1;

    QTextCursor curs=ui->textEdit->textCursor();
    int actBlock=curs.blockNumber();
    int actColm=curs.positionInBlock();

    //QTextCursor curs=ui->textEdit->textCursor();

    //extract information from sym to build the character to insert in the textEdit block

    QTextCharFormat ch_format;QFont ch_font;

    ch_font.setFamily("Times New Roman");
    ch_font.setBold(true);
    ch_font.setUnderline(true);
    ch_font.setItalic(true);
    ch_font.setPointSize(9);
    ch_format.setForeground(QColor(255,0,0));


    // set the font and the color to the character
    ch_format.setFont(ch_font);
    if(this->highActivated){
        QColor colUser=Qt::blue;
        ch_format.setBackground(colUser);
    }


    // go to the position of the character
    //ui->textEdit->changePosition(2,row,column);


    curs.movePosition(QTextCursor::Start);
    curs.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor,row);
    curs.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, column);
    ui->textEdit->setTextCursor(curs);
    ui->textEdit->changePosition(2,row,column++);


    //insert the character

    QString ch="F";

    curs.insertText(ch,ch_format);
    ui->textEdit->changePosition(2,row,column++);
    ui->textEdit->changePosition(actBlock,actColm);

    insertOthCh=false;

}
//------------------------------------------------------------------------------------------------------------------------------------------

void notepad::verifySymbol(Symposium::symbol sym,std::pair<int,int> indexes){

    insertOthCh=true;

    //take the position of the character
    int row=indexes.first;
    int column=indexes.second;

    QTextCursor curs=ui->textEdit->textCursor();
    int actBlock=curs.blockNumber();
    int actColm=curs.positionInBlock();

    //extract information from sym to build the character to insert in the textEdit block
    Symposium::format f= sym.getCharFormat();
    QTextCharFormat ch_format;QFont ch_font;

    ch_font.setFamily(QString::fromStdString(f.familyType));
    ch_font.setBold(f.isBold);
    ch_font.setUnderline(f.isUnderlined);
    ch_font.setItalic(f.isItalic);
    ch_font.setPointSize(f.size);
    Symposium::Color col=f.col;

    // conversion from Symposium::Color to QColor
    QColor qCol;
    qCol=static_cast<QColor>(col);
    QBrush brh(qCol);

    // set the font and the color to the character
    ch_format.setFont(ch_font);
    ch_format.setForeground(brh);

    // check if the highlight button is activated;
    // if yes-> highlight the inserted character with the color of the user
    if(this->highActivated){
        Symposium::Color colHigh=cl->getColor(this->documentId,sym.getSiteId());
        QColor colUser;
        colUser=static_cast<QColor>(colHigh);
        ch_format.setBackground(colUser);
    }

     // go to the position of the character
    curs.movePosition(QTextCursor::Start);
    curs.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, row);
    curs.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, column);

    // delete the character and replace it with the same that has a defined Color
    curs.deleteChar();
    wchar_t symch=sym.getCh();
    QString ch;
    ch[0]=symch;
    curs.insertText(ch,ch_format);

    // go back to the starting position
    ui->textEdit->changePosition(actBlock,actColm);
    insertOthCh=false;
}


//--------------------------------------------------------------------------------------------------------------------------------------------------
// SOLO PER TEST DI VERIFYSYMBOL
void notepad::verifySymbol2(){

    insertOthCh=true;
    int row=0;
    int column=0;


    QTextCursor curs=ui->textEdit->textCursor();
    int actBlock=curs.blockNumber();
    int actColm=curs.positionInBlock();

    //extract information from sym to build the character to insert in the textEdit block
    //Symposium::format f= sym.getCharFormat();
    QTextCharFormat ch_format;QFont ch_font;

    ch_font.setFamily(QString::fromStdString("MShell Dla 2"));
    ch_font.setBold(false);
    ch_font.setUnderline(false);
    ch_font.setItalic(false);
    ch_font.setPointSize(8);
    Symposium::Color col(0, 0, 0);

    // conversion from Symposium::Color to QColor
    QColor qCol;
    qCol=static_cast<QColor>(col);
    QBrush brh(qCol);

    // set the font and the color to the character
    ch_format.setFont(ch_font);
    ch_format.setForeground(brh);

    // check if the highlight button is activated;
    // if yes-> highlight the inserted character with the color of the user
    if(this->highActivated){
        QColor colUser=Qt::red;
        ch_format.setBackground(colUser);
    }

    curs.movePosition(QTextCursor::Start);
    curs.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor,row);
    curs.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, column);
    ui->textEdit->setTextCursor(curs);


    // delete the character and replace it with the same that has a defined Color
    curs.deleteChar();
    //wchar_t symch=sym.getCh();
    QString ch;
    //ch[0]=symch;
    curs.insertText("H",ch_format);

    // go back to the starting position
    ui->textEdit->changePosition(actBlock,actColm);
    insertOthCh=false;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------

void notepad::remoteDelete(std::pair<int,int> indexes,Symposium::uint_positive_cnt::type siteId){

    insertOthCh=true;

    // the position of the character to delete
    int block=indexes.first;
    int column=indexes.second;

    // we use deletePreviousChar -> we need to move the cursor to the next Position
    column++;
    QTextCursor curs=ui->textEdit->textCursor();
    int actBlock=curs.blockNumber();
    int actColm=curs.positionInBlock();

    // go to the position of the character
    curs.movePosition(QTextCursor::Start);
    curs.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor,block);
    curs.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, column);
    ui->textEdit->setTextCursor(curs);
    ui->textEdit->changePosition(2,block,column--);

    curs.deletePreviousChar();

    ui->textEdit->changePosition(siteId,block,column--);
    ui->textEdit->changePosition(actBlock,actColm);

    insertOthCh=false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------
// PROVA REMOTE DELETE DA CANCELLARE
void notepad::prova_remoteDelete(){

    insertOthCh=true;

    // the position of the character to delete
    int block=0;
    int column=1;
    // in Qt the column starts from 1, while in symbols it starts from 0
    column++;

    QTextCursor curs=ui->textEdit->textCursor();
    int actBlock=curs.blockNumber();
    int actColm=curs.positionInBlock();
    // go to the position of the character

    curs.movePosition(QTextCursor::Start);
    curs.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor,block);
    curs.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, column);
    ui->textEdit->setTextCursor(curs);
    ui->textEdit->changePosition(2,block,column--);

    curs.deletePreviousChar();

    ui->textEdit->changePosition(2,block,column--);
    ui->textEdit->changePosition(actBlock,actColm);

    insertOthCh=false;
}


void notepad::setClientDispatcher(Symposium::clientdispatcher *cl)
{
    this->cl = cl;
}

void notepad::on_textEdit_cursorPositionChanged()
{
    QTextCursor cc=ui->textEdit->textCursor();
    ui->textEdit->thisUserChangePosition(1);
    //ui->textEdit->thisUserChangePosition(us.getSiteId())

    if(insertOthCh==false){
       QColor col=ui->textEdit->textColor();
       col.setAlpha(180);
       ui->textEdit->setTextColor(col);

       // The user changes the block by using the mouse
        if(cc.blockNumber()!=this->posBlock) {
            ui->textEdit->setTextColor(this->colPos);
    }

        if(this->highActivated && cc.blockNumber()!=this->posBlock){
            this->insertedChars=0;
            ui->textEdit->setTextColor(this->colPos);
}
     }
}

void notepad::colorText(){

    QTextCursor curs=ui->textEdit->textCursor();
    // save in symbols all the symbols contained in the document
    std::vector<std::vector<Symposium::symbol>> symbols= this->doc.getSymbols();
    for(size_t i=0;i<symbols.size();i++){
        // counter variable to count the number of characters that I have to highlight
        int incr=1;
        int jsupp=0;
        for(size_t j=0;j<symbols[i].size();j++){
            // the actual symbol
            Symposium::symbol symF=symbols[i][j];
            // the starting position of the selection
            std::pair<int,int> indexes={i,jsupp};
            // check if I am at the end of the row (last column) and I can't compare the actual symbol siteId with the next one.
            // If I am at the end of the row, I have to highlight and I have to change the row.
            if(j!=symbols.size()-1){
                Symposium::symbol symS=symbols[i][j+1];
                //check if the two successive symbols have the same siteId;
                if(symF.getSiteId()==symS.getSiteId()){
                incr++;
                }
                 else{
                    // extract the Color assigned to the user
                    //Symposium::Color userColor=cl->getColor(this->documentId,symF.getSiteId());
                    Symposium::Color userColor;
                    //convert the Symposium::Color into a QColor;
                    QColor qCol;
                    qCol=static_cast<QColor>(userColor);

                    int block=indexes.first; int column=indexes.second; column++;
                    QTextCursor supportCurs=ui->textEdit->textCursor();
                    //--------------------------------------------------------------- a me serve solo symF
                    Symposium::format f=symF.getCharFormat();

                    // set the characteristics of the char
                    QFont font;
                    font.setBold(f.isBold); font.setItalic(f.isItalic); font.setUnderline(f.isUnderlined);
                    font.setPointSize(f.size);font.setFamily(QString::fromStdString(f.familyType));
                    QTextCharFormat highlightColor;

                    Symposium::Color charColor;
                    //convert the Symposium::Color into a QColor;
                    QColor cCol;
                    cCol=static_cast<QColor>(userColor);

                    //if the symbol is not verified, its color is more transparent
                    if(!symF.isVerified()){cCol.setAlpha(160);}

                    highlightColor.setForeground(cCol); highlightColor.setBackground(qCol);

                    supportCurs.movePosition(QTextCursor::Start);
                    supportCurs.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor,block);
                    supportCurs.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor,column);
                    int pp=supportCurs.position();

                    //highlight the text
                    curs.setPosition(pp,QTextCursor::MoveAnchor);
                    curs.setPosition(pp+incr,QTextCursor::KeepAnchor);
                    curs.setCharFormat(highlightColor);
                    ui->textEdit->setTextCursor(curs);

                    incr=1;
                    jsupp=j+1;
             }
            }
            // extract the Color assigned to the user
            //Symposium::Color userColor=cl->getColor(this->documentId,symF.getSiteId());
            Symposium::Color userColor;
            //convert the Symposium::Color into a QColor;
            QColor qCol;
            qCol=static_cast<QColor>(userColor);

            int block=indexes.first; int column=indexes.second; column++;
            QTextCursor supportCurs=ui->textEdit->textCursor();
            Symposium::format f=symF.getCharFormat();

            // set the characteristics of the char
            QFont font;
            font.setBold(f.isBold); font.setItalic(f.isItalic); font.setUnderline(f.isUnderlined);
            font.setPointSize(f.size);font.setFamily(QString::fromStdString(f.familyType));
            QTextCharFormat highlightColor;

            Symposium::Color charColor;
            //convert the Symposium::Color into a QColor;
            QColor cCol;
            cCol=static_cast<QColor>(userColor);

            //if the symbol is not verified, its color is more transparent
            if(!symF.isVerified()){cCol.setAlpha(160);}

            highlightColor.setForeground(cCol); highlightColor.setBackground(qCol);

            supportCurs.movePosition(QTextCursor::Start);
            supportCurs.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor,block);
            supportCurs.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor,column);
            int pp=supportCurs.position();

            //highlight the text
            curs.setPosition(pp,QTextCursor::MoveAnchor);
            curs.setPosition(pp+incr,QTextCursor::KeepAnchor);
            curs.setCharFormat(highlightColor);
            ui->textEdit->setTextCursor(curs);
            j=symbols[j].size();

        }
    }
}

void notepad::prova_colorText(){
    std::vector<std::vector<std::pair<int,std::string>>> symbols(4);
    std::vector<std::pair<int,std::string>>interno(12);
    interno[0]={0,"C"};
    interno[1]={0,"I"};
    interno[2]={0,"A"};
    interno[3]={0,"O"};
    interno[4]={0," "};
    interno[5]={1,"A"};
    interno[6]={1," "};
    interno[7]={1,"T"};
    interno[8]={1,"U"};
    interno[9]={1,"T"};
    interno[10]={1,"T"};
    interno[11]={1,"I"};

    symbols[0]=interno;

    std::vector<std::pair<int,std::string>>interno2(4);
    interno2[0]={0,"C"};
    interno2[1]={0,"O"};
    interno2[2]={0,"M"};
    interno2[3]={0,"E"};
    symbols[1]=interno2;

    std::vector<std::pair<int,std::string>>interno3(10);
    interno3[0]={1,"S"};
    interno3[1]={1,"T"};
    interno3[2]={1,"A"};
    interno3[3]={1,"I"};
    interno3[4]={1,"?"};
    interno3[5]={1,"I"};
    interno3[6]={1,"O"};
    interno3[7]={1," "};
    interno3[8]={1,"O"};
    interno3[9]={1,"K"};
    symbols[2]=interno3;
    std::vector<std::pair<int,std::string>>interno4(7);
    interno4[0]={0,"M"};
    interno4[1]={0,"A"};
    interno4[2]={0,"R"};
    interno4[3]={0,"T"};
    interno4[4]={0,"I"};
    interno4[5]={0,"N"};
    interno4[6]={0,"A"};
    symbols[3]=interno4;


    QTextCursor curs=ui->textEdit->textCursor();
    // save in symbols all the symbols contained in the document
    bool verified=true;
    for(size_t i=0;i<symbols.size();i++){
        // counter variable to count the number of characters that I have to highlight
        int incr=1;
        int jsupp=0;
        for(size_t j=0;j<symbols[i].size();j++){
            std::pair<int,std::string> symF=symbols[i][j];
            // the starting position of the selection
            std::pair<int,int> indexes={i,jsupp};
            // check if I am at the end of the row (last column) and I can't compare the actual symbol siteId with the next one.
            // If I am at the end of the row, I have to highlight and I have to change the row.
            if(j!=symbols[i].size()-1){
                 std::pair<int,std::string> symS=symbols[i][j+1];
                //check if the two successive symbols have the same siteId;
                if(symF.first==symS.first){
                incr++;
                }
                 else{
                    QColor qCol;
                    // estraggo dal site id il colore -> chiedere a Cristian
                    if(symF.first==0){
                        qCol=Qt::red;
                        qCol.setAlpha(160);
                    }
                    else if(symF.first==1){
                        qCol=Qt::yellow;
                        qCol.setAlpha(160);
                        verified=false;
                    }

                    QColor ccc(0,0,0);
                    ccc.setAlpha(160);
                    int block=indexes.first; int column=indexes.second;
                    QTextCursor supportCurs=ui->textEdit->textCursor();
                    QTextCharFormat highlightColor;
                    highlightColor.setBackground(qCol);
                    if(verified=false){highlightColor.setForeground(ccc);}
                    QFont font; font.setBold(true); font.setPointSize(9);
                    highlightColor.setFont(font);
                    supportCurs.movePosition(QTextCursor::Start);
                    supportCurs.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor,block);
                    supportCurs.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor,column);
                    int pp=supportCurs.position();

                    //highlight the text
                    curs.setPosition(pp,QTextCursor::MoveAnchor);
                    curs.setPosition(pp+incr,QTextCursor::KeepAnchor);
                    curs.setCharFormat(highlightColor);
                    ui->textEdit->setTextCursor(curs);

                    incr=1;
                    jsupp=j+1;
             }
            }else{
            QColor qCol;
            // estraggo dal site id il colore -> chiedere a Cristian
            if(symF.first==0){
                qCol=Qt::red;
                qCol.setAlpha(160);}
            else if(symF.first==1){
                qCol=Qt::yellow;qCol.setAlpha(160);
            }

            int block=indexes.first; int column=indexes.second;
            QTextCursor supportCurs=ui->textEdit->textCursor();
            QTextCharFormat highlightColor;
            highlightColor.setBackground(qCol);
            QFont font; font.setBold(true); font.setPointSize(9);
            highlightColor.setFont(font);
            supportCurs.movePosition(QTextCursor::Start);
            supportCurs.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor,block);
            supportCurs.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor,column);
            int pp=supportCurs.position();

            //highlight the text
            curs.setPosition(pp,QTextCursor::MoveAnchor);
            curs.setPosition(pp+incr,QTextCursor::KeepAnchor);
            curs.setCharFormat(highlightColor);
            ui->textEdit->setTextCursor(curs);
            j=symbols[i].size();

}

        }
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------
void notepad::provaFill(){
    ui->textEdit->clear();
    QColor qCol;
    insertOthCh=true;
    std::vector<std::vector<std::string>> symbols(100);
    std::vector<std::string>interno(12);
    interno[0]="C";
    interno[1]="I";
    interno[2]="A";
    interno[3]="O";
    interno[4]=" ";
    interno[5]="A";
    interno[6]=" ";
    interno[7]="T";
    interno[8]="U";
    interno[9]="T";
    interno[10]="T";
    interno[11]="I";

    symbols[0]=interno;
    std::vector<std::string>interno2(4);
    interno2[0]="C";
    interno2[1]="O";
    interno2[2]="M";
    interno2[3]="E";
    symbols[1]=interno2;
    std::vector<std::string>interno3(10);
    interno3[0]="S";
    interno3[1]="T";
    interno3[2]="A";
    interno3[3]="I";
    interno3[4]="?";
    interno3[5]="I";
    interno3[6]="O";
    interno3[7]=" ";
    interno3[8]="O";
    interno3[9]="K";
    symbols[2]=interno3;
    std::vector<std::string>interno4(10);
    interno4[0]="M";
    interno4[1]="A";
    interno4[2]="R";
    interno4[3]="T";
    interno4[4]="I";
    interno4[5]="N";
    interno4[6]="A";
    symbols[3]=interno4;
    QString parola;
        QTextCursor curs=ui->textEdit->textCursor();

       // curs.movePosition(QTextCursor::Start);
        for(size_t i=0;i<symbols.size();i++){
                for(size_t j=0;j<symbols[i].size();j++){
                    std::string sym=symbols[i][j];

            int colonna=j;
            //estract the information about the font/color/format
            QFont font; QTextCharFormat chFormat;
            font.setFamily(QString::fromStdString("Comics Sams Ms"));
            font.setBold(true);
            font.setUnderline(false);
            font.setItalic(false);
            font.setPointSize(9);
            Symposium::Color col(0,0,0);
            //conversion from Color to QColor

            qCol=static_cast<QColor>(col);
            chFormat.setFont(font); chFormat.setForeground(qCol);
            QTextBlockFormat f;

            // go to the position of the character
          ui->textEdit->changePosition(i,colonna++);
          //curs.deleteChar();

          curs.insertText(QString::fromStdString(sym),chFormat);
          //ui->textEdit->append(QString::fromStdString(sym));


    }
          curs.insertBlock();
    }

    insertOthCh=false;
    this->colorChanged(qCol);

    //set a lighter color
    QColor lightCol=qCol;
    lightCol.setAlpha(180);
    this->colPos=lightCol;





}
//----------------------------------------------------------------------------------------------------------------------------------------------


void notepad::on_actionhighlight_triggered()
{

    if(highActivated==false){
        this->highActivated=true;
        this->insertedChars=0;
        ui->actionhighlight->setChecked(true);
        this->prova_colorText();
        //this->colorText();

    }
    else{
        this->highActivated=false;
        ui->actionhighlight->setChecked(false);
        this->insertedChars=0;
        this->provaFill();
        //this->fillTextEdit();
        ui->textEdit->setTextBackgroundColor(backCol);
    }
}



