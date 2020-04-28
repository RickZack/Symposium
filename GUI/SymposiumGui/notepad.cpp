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

notepad::notepad(QWidget *parent, Symposium::uint_positive_cnt::type documentId, Symposium::privilege priv, Symposium::privilege privOpen,std::string pathToFile,Symposium::document doc) :
    QMainWindow(parent),
    documentId(documentId), pathToFile(pathToFile), priv(priv), privOpen(privOpen),doc(doc),ui(new Ui::notepad)
{
    ui->setupUi(this);

    qApp->installEventFilter(this);

    QActionGroup *alignGroup= new QActionGroup(this);
    alignGroup->addAction(ui->actionAlignCenter);
    alignGroup->addAction(ui->actionAlignTextLeft);
    alignGroup->addAction(ui->actionAlignTextRight);
    alignGroup->addAction(ui->actionAlignTextJustify);

    ui->actionAlignTextLeft->setChecked(true);
    addStyleFormat();

    connect(ui->fontComboBox, QOverload<const QString &>::of(&QComboBox::activated), this, &notepad::textFamily);
    ui->fontComboBox->activated("Times New Roman");

    //----------------------------------------------------------------------------------------------------------------------------
    std::pair<int, int> i1={0,0}, i2={0,1}, i3={0,2},i4={0,3},iacapo={0,4},i5={1,0},i6={1,1},i7={1,2},i8={1,3},i9={1,4},i10={1,5},i11={1,6},i12={1,7},i13={1,8},iacapo2={1,9};
        Symposium::symbol s1('C', 1, 0, std::vector<int>(), false),
                          s2('i', 1, 1, std::vector<int>(), false),
                          s3('a', 1, 2, std::vector<int>(), false),
                          s4('o', 1, 3, std::vector<int>(), false),
                          s5('c', 1, 4, std::vector<int>(), false),
                          s6('o', 1, 5, std::vector<int>(), false),
                          s7('m', 1, 6, std::vector<int>(), false),
                          s8('e', 1, 7, std::vector<int>(), false),
                          s9(' ', 1, 8, std::vector<int>(), false),
                          s10('s', 1, 9, std::vector<int>(), false),
                          s11('t', 1, 7, std::vector<int>(), false),
                          s12('a', 1, 7, std::vector<int>(), false),
                          s13('i', 1, 7, std::vector<int>(), false),
                          acapo('\r',1,7, std::vector<int>(), false),
                          a1('E',2,3,std::vector<int>(), false),
                          a2('h',2,3,std::vector<int>(), false),
                          a3('i',2,3,std::vector<int>(), false),
                          a4(' ',2,8, std::vector<int>(), false);



        Symposium::Color nero(0,0,0);
        Symposium::Color rosso(255,0,0);
        const std::string ft="Times New Roman";
        unsigned size=9;
        Symposium::format f(ft,false,false,false,size,nero);
         Symposium::format f1("Times New Roman",true,true,false,9,rosso);
         //Ciao
        s1.setCharFormat(f1);
        s2.setCharFormat(f1);
        s3.setCharFormat(f1);
        s4.setCharFormat(f1);


        // Come stai
        s5.setCharFormat(f1);
        s6.setCharFormat(f1);
        s7.setCharFormat(f1);
        s8.setCharFormat(f1);
        s9.setCharFormat(f);
        s10.setCharFormat(f);
        s11.setCharFormat(f);
        s12.setCharFormat(f);
        s13.setCharFormat(f);

        a1.setCharFormat(f);
        a2.setCharFormat(f);
        a3.setCharFormat(f);
        a4.setCharFormat(f);

        // Ciao
        documentoProva.localInsert(i1,s1);
        documentoProva.localInsert(i2,s2);
        documentoProva.localInsert(i3,s3);
        documentoProva.localInsert(i4,s4);
        documentoProva.localInsert(iacapo,acapo);

        // Ehi
        documentoProva.localInsert(i1,a1);
        documentoProva.localInsert(i2,a2);
        documentoProva.localInsert(i3,a3);
        documentoProva.localInsert(i4,a4);

        // Come stai
        documentoProva.localInsert(i5,s5);
        documentoProva.localInsert(i6,s6);
        documentoProva.localInsert(i7,s7);
        documentoProva.localInsert(i8,s8);
        documentoProva.localInsert(i9,s9);
        documentoProva.localInsert(i10,s10);
        documentoProva.localInsert(i11,s11);
        documentoProva.localInsert(i12,s12);
        documentoProva.localInsert(i13,s13);
        documentoProva.localInsert(iacapo2,acapo);


        std::pair<int, int> in1={2,0}, in2={2,1}, in3={2,2},in4={2,3},in5={2,4},in6={2,5},in7={2,6},in8={2,7},in9={2,8};
            Symposium::symbol sn1('T', 2, 0, std::vector<int>(), false),
                              sn2('u', 2, 1, std::vector<int>(), false),
                              sn3('t', 2, 2, std::vector<int>(), false),
                              sn4('t', 2, 3, std::vector<int>(), false),
                              sn5('o', 2, 4, std::vector<int>(), false),
                              sn6(' ', 2, 5, std::vector<int>(), false),
                              sn7('o', 2, 6, std::vector<int>(), false),
                              sn8('k', 2, 7, std::vector<int>(), false),
                              sn9('!', 2, 8, std::vector<int>(), false);

            Symposium::Color colore(200,30,0);
            const std::string ft1="Lucida Handwriting";
            unsigned size2=9;
            Symposium::format fn1(ft1,false,false,true,size2,colore);
            sn1.setCharFormat(fn1);
            sn2.setCharFormat(fn1);
            sn3.setCharFormat(fn1);
            sn4.setCharFormat(fn1);
            sn5.setCharFormat(fn1);
            sn6.setCharFormat(fn1);
            sn7.setCharFormat(fn1);
            sn8.setCharFormat(fn1);
            sn9.setCharFormat(fn1);

            documentoProva.localInsert(in1,sn1);
            documentoProva.localInsert(in2,sn2);
            documentoProva.localInsert(in3,sn3);
            documentoProva.localInsert(in4,sn4);
            documentoProva.localInsert(in5,sn5);
            documentoProva.localInsert(in6,sn6);
            documentoProva.localInsert(in7,sn7);
            documentoProva.localInsert(in8,sn8);
            documentoProva.localInsert(in9,sn9);


  //---------------------------------------------------------------------------------------------------------

    priv=Symposium::privilege::owner;
    privOpen=Symposium::privilege::owner;
    //this->setToolButtonStyle(Qt::ToolButtonFollowStyle);
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
    menuSym->addAction(tr("Remote Insert"),this,&notepad::prova_remoteInsert);
    menuSym->addAction(tr("Remote Delete"),this,&notepad::prova_remoteDelete);

    QMenu *userMenu2=menuBar()->addMenu(tr("ProvaCursori"));
    userMenu2->addAction(tr("Add Cursor"), this, &notepad::addCursor);
    userMenu2->addAction(tr("Change Cursor"), this, &notepad::changeCursorPos);
    userMenu2->addAction(tr("Remove Cursor"), this, &notepad::removeCursor);

    ui->actionhighlight->setIcon(QIcon(":/resources/cartelle/color_icon"));
    ui->actionhighlight->setCheckable(true);

    this->fillTextEdit();

    ui->textEdit->setCursorWidth(3);

    if(privOpen==Symposium::privilege::readOnly)
    {
        ui->textEdit->setReadOnly(true);
        ui->formatText->hide();
        ui->fontComboBox->hide();
        ui->sizeBox->hide();
        ui->styleBox->hide();
    }
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

void notepad::addStyleFormat()
{
    ui->styleBox->addItem("Standard");
    ui->styleBox->addItem("Bullet List (Disc)");
    ui->styleBox->addItem("Bullet List (Circle)");
    ui->styleBox->addItem("Bullet List (Square)");
    ui->styleBox->addItem("Ordered List (Decimal)");
    ui->styleBox->addItem("Ordered List (Alpha lower)");
    ui->styleBox->addItem("Ordered List (Alpha upper)");
    ui->styleBox->addItem("Ordered List (Roman lower)");
    ui->styleBox->addItem("Ordered List (Roman upper)");
    ui->styleBox->addItem("Heading 1");
    ui->styleBox->addItem("Heading 2");
    ui->styleBox->addItem("Heading 3");
    ui->styleBox->addItem("Heading 4");
    ui->styleBox->addItem("Heading 5");
    ui->styleBox->addItem("Heading 6");

    connect(ui->styleBox, QOverload<int>::of(&QComboBox::activated), this, &notepad::textStyle);

    const QList<int> standardSizes = QFontDatabase::standardSizes();
    foreach (int size, standardSizes)
        ui->sizeBox->addItem(QString::number(size));
    ui->sizeBox->setCurrentIndex(standardSizes.indexOf(QApplication::font().pointSize()));

    connect(ui->sizeBox, QOverload<const QString &>::of(&QComboBox::activated), this, &notepad::textSize);
}


void notepad::on_actionColorText_triggered()
{
  textColor();
}


void notepad::on_actionAlignTextLeft_triggered()
{
    textAlign(ui->actionAlignTextLeft);
}

void notepad::on_actionAlignCenter_triggered()
{
    textAlign(ui->actionAlignCenter);
}


void notepad::on_actionAlignTextRight_triggered()
{
   textAlign(ui->actionAlignTextRight);
}

void notepad::on_actionAlignTextJustify_triggered()
{
    textAlign(ui->actionAlignTextJustify);
}

void notepad::on_actionBoldFont_triggered()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(ui->actionBoldFont->isChecked() ? QFont::Bold : QFont::Normal);
    ui->textEdit->mergeCurrentCharFormat(fmt);
    //mergeFormatOnWordOrSelection(fmt);
}

void notepad::on_actionItalicFont_triggered()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(ui->actionItalicFont->isChecked());
    ui->textEdit->mergeCurrentCharFormat(fmt);
    //mergeFormatOnWordOrSelection(fmt);
}

void notepad::on_actionUnderlineFont_triggered()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(ui->actionUnderlineFont->isChecked());
    ui->textEdit->mergeCurrentCharFormat(fmt);
   // mergeFormatOnWordOrSelection(fmt);
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
    //mergeFormatOnWordOrSelection(fmt);
    ui->textEdit->mergeCurrentCharFormat(fmt);
}

void notepad::textSize(const QString &p)
{
    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        //mergeFormatOnWordOrSelection(fmt);
         ui->textEdit->mergeCurrentCharFormat(fmt);
    }
}

void notepad::textAlign(QAction *a)
{
    if (a == ui->actionAlignTextLeft)
        ui->textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    else if (a == ui->actionAlignCenter)
        ui->textEdit->setAlignment(Qt::AlignHCenter);
    else if (a == ui->actionAlignTextRight)
        ui->textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    else if (a == ui->actionAlignTextJustify)
        ui->textEdit->setAlignment(Qt::AlignJustify);
}

void notepad::colorChanged(const QColor &c)
{
    QPixmap pix(64, 64);
    pix.fill(c);
    ui->actionColorText->setIcon(pix);
    ui->textEdit->setTextColor(c);

}

void notepad::fontChanged(const QFont &f)
{
    ui->fontComboBox->setCurrentIndex(ui->fontComboBox->findText(QFontInfo(f).family()));
    ui->sizeBox->setCurrentIndex(ui->sizeBox->findText(QString::number(f.pointSize())));
    ui->actionBoldFont->setChecked(f.bold());
    ui->actionItalicFont->setChecked(f.italic());
    ui->actionUnderlineFont->setChecked(f.underline());
}

void notepad::currentCharFormatChanged(const QTextCharFormat &format)
{
    fontChanged(format.font());
    colorChanged(format.foreground().color());

}

void notepad::fillTextEdit(){
    insertOthCh=true;
    QTextCharFormat chFormat;
    QColor qCol;
    QTextCursor curs=ui->textEdit->textCursor();
    QString ch;
    // save in symbols all the symbols contained in the document
    //std::vector<std::vector<Symposium::symbol>> symbols= this->doc.getSymbols();
    std::vector<std::vector<Symposium::symbol>> symbols= this->documentoProva.getSymbols();
    for(size_t i=0;i<symbols.size();i++){
        for(size_t j=0;j<symbols[i].size();j++){
            int column=j;
            //extract the symbol
            Symposium::symbol sym=symbols[i][j];
            //estract the character
            ch[0]=sym.getCh();
           if(ch[0]!=emptyChar){
                QTextCharFormat chFormat;
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
                if(!sym.isVerified())
                  {
                    qCol.setAlpha(160);
                  }
                chFormat.setFont(font); chFormat.setForeground(qCol);
                // go to the position of the character
                ui->textEdit->changePosition(i,column);
                curs.insertText(ch,chFormat);
    }

}
    }
    insertOthCh=false;
    //this->fontChanged(chFormat.font());


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

    event->ignore();
    this->notepad::hide();
    //cl->closeSource(this->documentId);

}

void notepad::showLabels()
{
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
    std::pair<Symposium::user*, Symposium::sessionData> p2{u2, Symposium::sessionData(Symposium::privilege::modify, 2, 2)};
    std::pair<Symposium::user*, Symposium::sessionData> p3{u3, Symposium::sessionData(Symposium::privilege::readOnly, 0, 0)};
    std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData>> onlineUsers;
    onlineUsers.push_front(p1);
    onlineUsers.push_front(p2);
    onlineUsers.push_front(p3);
    ui->textEdit->constractLabelsCursors(onlineUsers, u1->getSiteId());
    ui->textEdit->insertCurrentUser(onlineUsers, u1->getSiteId());
    //---------------------------------------------------------------------
}

bool notepad::isAKeyToIgnore(QKeyEvent* event){
    return event->key()==Qt::Key_CapsLock || event->key()==Qt::Key_Shift || event->key()==Qt::Key_Control
            ||event->key()==Qt::Key_Alt || event->key()==Qt::Key_Escape || event->key()==Qt::Key_F1 ||event->key()==Qt::Key_F2 ||
            event->key()==Qt::Key_F3 ||event->key()==Qt::Key_F4 ||event->key()==Qt::Key_F5 ||event->key()==Qt::Key_F6 ||
            event->key()==Qt::Key_F7 ||event->key()==Qt::Key_F8 ||event->key()==Qt::Key_F9 ||event->key()==Qt::Key_F10 ||
            event->key()==Qt::Key_F11 || event->key()==Qt::Key_F12 || event->key()==Qt::Key_Menu ||
            event->key()==Qt::Key_Pause || event->key()==Qt::Key_Insert ||event->key()==Qt::Key_AltGr ||
            event->key()==Qt::Key_Tab || event->key()==Qt::Key_Up || event->key()==Qt::Key_Down ||
            event->key()==Qt::Key_Delete || event->key()==Qt::Key_NumLock || event->key()==Qt::Key_Left ||
            event->key()==Qt::Key_Right || event->key()==Qt::Key_Meta ||event->key()==Qt::Key_unknown || event->modifiers() & Qt::ControlModifier& event->text()!="\u0016";
}
void notepad::handleTextEditKeyPress(QKeyEvent* event){
    QTextCursor cursor= ui->textEdit->textCursor();
    QTextCharFormat format = cursor.charFormat();
    QString testo=event->text();
    int row, column;
    int pos=cursor.position();

    if(isAKeyToIgnore(event))
        return;
    else if(event->key()==Qt::Key_Backspace)
        return handleDeleteKey();
    else if(event->text()=="\u0016") // Control_V action
        return this->contV_action(pos);
    else{ //carattere alfabetico
        row=cursor.blockNumber();
        column=cursor.positionInBlock();
    }

     this->sendSymbolToInsert(row,column,testo,format);
}

bool notepad::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
           qDebug() << "key " << keyEvent->key() << "from" << obj;
           if(obj->objectName()=="textEdit"){
               ui->textEdit->setTextColor(ui->textEdit->textColor());
               handleTextEditKeyPress(keyEvent);
           }
    }
    return QObject::eventFilter(obj, event);
}

void notepad::handleDeleteKey(){
    int row, col;
    QTextCursor cursor= ui->textEdit->textCursor();
    row=cursor.blockNumber();
    col=cursor.positionInBlock()-1;
    qDebug()<<"handleDeleteKey: row="<<row<<" col="<<col;
    if(col<0 && row>0){ // handle remove of '\r' in preceding line
        row=cursor.blockNumber()-1;
        cursor.movePosition(QTextCursor::PreviousBlock);
        cursor.movePosition(QTextCursor::EndOfBlock);
        col=cursor.positionInBlock();
        //Now restore the cursor position
        cursor.movePosition(QTextCursor::NextBlock);
        cursor.movePosition(QTextCursor::StartOfBlock);
    }
    else if(col<0) //deleting from an empty document, discard the action
        return;
    documentoProva.localRemove({row, col}, 1 /*dummy site id*/);
}

void notepad::sendSymbolToInsert(int row, int column,QString text, QTextCharFormat format){

    std::wstring str=text.toStdWString();
    wchar_t ch=str[0];
    const std::pair<int, int> indexes={row,column};
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
    Symposium::Color myCol(red,green,blue);
    Symposium::format charFormat={fontFamily,isBold,isUnderlined,isItalic,size,myCol};
    std::vector<int> pos;

    // per test adesso, il mio siteid è 1 e anche il mio counter
    Symposium::symbol sym(ch,1,1,pos,false);
    sym.setCharFormat(charFormat);
    this->documentoProva.localInsert(indexes,sym);
    //cl->localInsert(this->documentId,sym,indexes);

    this->posBlock=ui->textEdit->textCursor().positionInBlock();
    this->colPos=colC;
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
        Symposium::Color myCol(red,green,blue);
        struct Symposium::format charFormat={fontFamily,isBold,isUnderlined,isItalic,size,myCol};
        std::vector<int> pos;
        // SISTEMARE IL SITEID E IL COUNTER IN SYMBOL
        Symposium::symbol sym(ch,1,0,pos,false);
        sym.setCharFormat(charFormat);
        this->documentoProva.localInsert(indexes,sym);
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
     if(insertOthCh==false){
        ui->textEdit->thisUserChangePosition(1);
        //ui->textEdit->thisUserChangePosition(us.getSiteId())
     }

     if(!cc.hasSelection()){
        QTextCharFormat ch=ui->textEdit->currentCharFormat();
        QColor newCol=ch.foreground().color();
        qDebug()<<"Colore prima del set"<<newCol;
        newCol.setAlpha(255);
        qDebug()<<"Colore dopo del set"<<newCol;
        fontChanged(ch.font());
        colorChanged(newCol);
        this->currentCharFormatChanged(ch);
}

    QColor myC=Qt::yellow;
    myC.setAlpha(160);

    if(this->highActivated){ui->textEdit->setTextBackgroundColor(myC);
}

}

void notepad::colorText(){
    insertOthCh=true;
    bool acapo=false;
    QTextCharFormat chFormat;
    QColor qCol;
    QTextCursor curs=ui->textEdit->textCursor();
    QString ch;
    // save in symbols all the symbols contained in the document
    //std::vector<std::vector<Symposium::symbol>> symbols= this->doc.getSymbols();
    std::vector<std::vector<Symposium::symbol>> symbols= this->documentoProva.getSymbols();
    for(size_t i=0;i<symbols.size();i++){
        for(size_t j=0;j<symbols[i].size();j++){
            int column=j;
            //extract the symbol
            Symposium::symbol sym=symbols[i][j];
            Symposium::Color uC(243,121,137);
            QColor userCol;
            if(sym.getSiteId()==1){userCol=Qt::yellow;}
            else {userCol= static_cast<QColor>(uC);}
            //estract the character
            ch[0]=sym.getCh();
            if(ch[0]!=emptyChar){
                QTextCharFormat chFormat;
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
                colorChanged(qCol);

                if(!sym.isVerified())
                  {
                    qCol.setAlpha(160);
                    userCol.setAlpha(160);
                  }
                chFormat.setFont(font); chFormat.setForeground(qCol);chFormat.setBackground(userCol);
                // go to the position of the character
                ui->textEdit->changePosition(i,column);
                curs.insertText(ch,chFormat);

            }  
    }

    }

    insertOthCh=false;
}





void notepad::on_actionhighlight_triggered()
{

    if(highActivated==false){
        this->highActivated=true;
        this->insertedChars=0;
        ui->actionhighlight->setChecked(true);
        ui->textEdit->clear();
        this->colorText();
    }
    else{
        this->highActivated=false;
        ui->actionhighlight->setChecked(false);
        this->insertedChars=0;
        ui->textEdit->clear();
        this->fillTextEdit();

    }
}

