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

notepad::notepad(QWidget *parent, Symposium::uint_positive_cnt::type documentId, Symposium::privilege priv, Symposium::privilege privOpen,std::string pathToFile,Symposium::document doc, SymWinInterface& si, bool parentIsTransient) :
    QMainWindow(parent),
    SymNotepadWinInterface (si, isQWidget::isQwidgetType(*this), parentIsTransient),
    documentId(documentId), pathToFile(pathToFile), priv(priv), privOpen(privOpen),doc(doc),ui(new Ui::notepad)
{
    ui->setupUi(this);
    setMinimumSize(800, 600);

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
    Symposium::symbol s1('C', 1, 0, {1}, false),
                          s2('i', 1, 1, {2}, false),
                          s3('a', 1, 2,{3}, false),
                          s4('o', 1, 3,{4}, false),
                          s5('c', 2, 4, std::vector<int>(), false),
                          s6('o', 2, 5, std::vector<int>(), false),
                          s7('m', 2, 6, std::vector<int>(), false),
                          s8('e', 2, 7, std::vector<int>(), false),
                          s9(' ', 2, 8, std::vector<int>(), false),
                          s10('s', 1, 9, std::vector<int>(), false),
                          s11('t', 1, 7, std::vector<int>(), false),
                          s12('a', 1, 7, std::vector<int>(), false),
                          s13('i', 1, 7, std::vector<int>(), false),
                          acapo('\r',1,7, {5}, false),
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

        // Ciao -> inserito con la remoteInsert

        documentoProva.remoteInsert(1,s1);
        documentoProva.remoteInsert(1,s2);
        documentoProva.remoteInsert(1,s3);
        documentoProva.remoteInsert(1,s4);
        documentoProva.remoteInsert(1,acapo);


/*
        // Ciao
        documentoProva.localInsert(i1,s1);
        documentoProva.localInsert(i2,s2);
        documentoProva.localInsert(i3,s3);
        documentoProva.localInsert(i4,s4);
        documentoProva.localInsert(iacapo,acapo);

        */

        // Ehi
        documentoProva.localInsert(i1,a1);
        documentoProva.localInsert(i2,a2);
        documentoProva.localInsert(i3,a3);
        documentoProva.localInsert(i4,a4);


        // Come stai -> inserito con la localInsert
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


        std::pair<int, int> in1={2,0}, in2={2,1}, in3={2,2},in4={2,3},in5={2,4},in6={2,5},in7={2,6},in8={2,7},in9={2,8},in10={2,9};
            Symposium::symbol sn1('T', 2, 0, std::vector<int>(), false),
                              sn2('u', 2, 1, std::vector<int>(), false),
                              sn3('t', 2, 2, std::vector<int>(), false),
                              sn4('t', 2, 3, std::vector<int>(), false),
                              sn5('o', 2, 4, std::vector<int>(), false),
                              sn6(' ', 2, 5, std::vector<int>(), false),
                              sn7('o', 2, 6, std::vector<int>(), false),
                              sn8('k', 2, 7, std::vector<int>(), false),
                              sn9('!', 2, 8, std::vector<int>(), false),
                              sn10('\r', 2, 8, std::vector<int>(), false);

            Symposium::Color colore(200,30,0);
            const std::string ft1="Lucida Console";
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
            documentoProva.localInsert(in10,sn10);

  //---------------------------------------------------------------------------------------------------------

    #ifndef DISPATCHER_ON
    ui->textEdit->setThisUserPrivilege(privOpen);
    this->pathToFile="/1/2/3/4/5/6/7";
    us=Symposium::user("Mario", "AP@ssw0rd!", "Mariuz", ":/resources/avatar/beaver.png", 1, nullptr);
    #else
    us=cl->getUser();
    #endif

    //this->setToolButtonStyle(Qt::ToolButtonFollowStyle);


    // Symposium::Color colorSymp=cl->getColor(this->documentId,us->getSiteId());
    // QColor myColor;
    // myColor=static_cast<QColor>(colorSymp);


    if(privOpen==Symposium::privilege::readOnly)
    {
        ui->pushButton->move(10, 20);
        ui->menubar->clear();
        ui->actionCopy->setDisabled(true);
        ui->actionCopy->setVisible(false);
        ui->actionCut->setDisabled(true);
        ui->actionCut->setVisible(false);
        ui->actionPaste->setDisabled(true);
        ui->actionPaste->setVisible(false);
        ui->actionCopy->setDisabled(true);
        ui->actionCopy->setVisible(false);
    }
    if(priv==Symposium::privilege::owner)
    {
        QMenu *userMenu=menuBar()->addMenu(tr("Users' privilege"));
        userMenu->addAction(tr("Show all users"), this, &notepad::visualizeAllUsers);
        QMenu *shareMenu=menuBar()->addMenu(tr("Share File"));
        shareMenu->addAction(tr("Disable links"), this, &notepad::inactiveLink);
        shareMenu->addAction(tr("Enable links"), this, &notepad::activeAlwaysLink);
        shareMenu->addAction(tr("Timer links"), this, &notepad::timerLink);
        shareMenu->addAction(tr("Number links"), this, &notepad::counterLink);
    }


    QMenu *menuSym=menuBar()->addMenu(tr("Prova RichText"));
    menuSym->addAction(tr("Verify Sym"),this,&notepad::verifySymbol2);

    QMenu *userMenu2=menuBar()->addMenu(tr("ProvaCursori"));
    userMenu2->addAction(tr("Add Cursor"), this, &notepad::addCursor);
    userMenu2->addAction(tr("Change Cursor"), this, &notepad::changeCursorPos);
    userMenu2->addAction(tr("Remove Cursor"), this, &notepad::removeCursor);

    ui->actionhighlight->setIcon(QIcon(":/resources/cartelle/color_icon"));
    ui->actionhighlight->setCheckable(true);

    this->fillTextEdit();

    ui->textEdit->setCursorWidth(3);

    ui->tree->setColumnCount(1);
    ui->tree->header()->setVisible(false);
    insertusers();
    ui->tree->header()->setStretchLastSection(false);
    ui->tree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->showUsers->setDisabled(true);
    ui->showUsers->hide();


    setFocusPolicy(Qt::StrongFocus);
    setAttribute( Qt::WA_DeleteOnClose );
}

Symposium::uint_positive_cnt::type notepad::getId()
{
    return 1;
}

void notepad::successfullInsert(const Symposium::symbol &sym)
{
    //sym.dummyMethod(); //unuseful, just to suppress warning
    QString mex("id of the current notepad is: ");
    mex.append(this->getId());

    //testCall(mex);
}

void notepad::failedInsert(const Symposium::symbol &sym)
{
    //sym.dummyMethod(); //unuseful, just to suppress warning
    //testCall("");
}

void notepad::failedRemove(const Symposium::symbol &sym)
{
    //sym.dummyMethod(); //unuseful, just to suppress warning
    //testCall("");
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
    insertusers();
}

void notepad::addUserCursor(Symposium::uint_positive_cnt::type siteID, std::string username)
{
    ui->textEdit->addUser(siteID, username);
    insertusers();
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
    this->okPaste=true;
    const QMimeData *md = QApplication::clipboard()->mimeData();
    this->dim=md->text().length();
    this->contV_action();
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
    //ui->textEdit->setTextColor(c);

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
    if(symbols[0][0].getCh()==emptyChar){
        QColor black=Qt::black;
        black.setAlpha(160);
        ui->textEdit->setTextColor(black);
        ui->textEdit->setText("");
        return;
    }
    else{
    for(size_t i=0;i<symbols.size();i++){
        for(size_t j=0;j<symbols[i].size();j++){
            int column=j;
            //extract the symbol
            Symposium::symbol sym=symbols[i][j];
            ch=sym.getCh();
            if(ch!=emptyChar){
            //estract the character
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
             this->currentCharFormatChanged(chFormat);
            }
         }
       }
     }
    insertOthCh=false;
}

void notepad::visualizeAllUsers()
{
    #ifndef DISPATCHER_ON
    std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData>> onlineUsers;
    Symposium::user *u1=new Symposium::user("Mario", "AP@ssw0rd!", "Mariuz", ":/resources/avatar/beaver.png", 1, nullptr);
    Symposium::user *u2=new Symposium::user("Carlo", "AP@ssw0rd!", "Carluz", ":/resources/avatar/boar.png", 2, nullptr);
    Symposium::user *u3=new Symposium::user("Federico", "AP@ssw0rd!", "Fede", ":/resources/avatar/bull.png", 3, nullptr);
    std::pair<Symposium::user*, Symposium::sessionData> p1{u1, Symposium::sessionData(Symposium::privilege::modify, 0, 0)};
    std::pair<Symposium::user*, Symposium::sessionData> p2{u2, Symposium::sessionData(Symposium::privilege::modify, 2, 2)};
    std::pair<Symposium::user*, Symposium::sessionData> p3{u3, Symposium::sessionData(Symposium::privilege::readOnly, 0, 0)};

    onlineUsers.push_front(p1);
    onlineUsers.push_front(p2);
    onlineUsers.push_front(p3);

    std::unordered_map<std::string, Symposium::privilege> users;
    std::pair p=std::make_pair<std::string, Symposium::privilege>("Carlo", Symposium::privilege::modify);
    std::pair p4=std::make_pair<std::string, Symposium::privilege>("Vincenzo", Symposium::privilege::readOnly);
    std::pair p5=std::make_pair<std::string, Symposium::privilege>("Matteo", Symposium::privilege::owner);
    std::pair p6=std::make_pair<std::string, Symposium::privilege>("Claudio", Symposium::privilege::modify);
    std::pair p7=std::make_pair<std::string, Symposium::privilege>("Mario", Symposium::privilege::owner);
    std::pair p8=std::make_pair<std::string, Symposium::privilege>("Federico", Symposium::privilege::modify);
    users.insert(p);
    users.insert(p4);
    users.insert(p5);
    users.insert(p6);
    users.insert(p7);
    users.insert(p8);
    Symposium::user thisUs=Symposium::user("Mario", "AP@ssw0rd!", "Mariuz", ":/resources/avatar/beaver.png", 1, nullptr);
    #else
    std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData>> onlineUsers=cl->onlineUser(documentID);
    std::unordered_map<std::string, Symposium::privilege> users=cl->allUser(documentID);
    #endif
    alluser* alluserWindow = new alluser(this,  priv, documentId, thisUs, pathToFile, onlineUsers, users, *this);
    goToWindow(*alluserWindow);
}

void notepad::inactiveLink()
{
    activenonlink* nonlinkwindow = new activenonlink(this, documentId, pathToFile, *this);
    goToWindow(*nonlinkwindow);
}

void notepad::activeAlwaysLink()
{
    activealwayslink* alwayslinkwindow = new activealwayslink(this, documentId, pathToFile, us, *this);
    goToWindow(*alwayslinkwindow);
}

void notepad::timerLink()
{
    activetimerlink* timerlinkwindow = new activetimerlink(this, documentId, pathToFile, us, *this);
    goToWindow(*timerlinkwindow);
}

void notepad::counterLink()
{
    activecounterlink* counterlinkwindow = new activecounterlink(this, documentId, pathToFile, us, *this);
    goToWindow(*counterlinkwindow);
}



void notepad::closeEvent(QCloseEvent *event){

    event->ignore();
    this->notepad::hide();
    //cl->closeSource(this->documentId);

}

void notepad::resizeEvent(QResizeEvent *event)
{
    int w=event->size().width();
    int h=event->size().height();
    int w1=w-240;
    int w2=w-260;
    int w3=w-30;
    int w4=w-118;
    if(showUsers)
        w=w-30-240;
    else
        w=w-39;
    h=h-159;
    ui->hideUsers->move(w2, 60);
    ui->showUsers->move(w3, 60);
    ui->tree->move(w1, 90);
    ui->labelUser->move(w1, 60);
    ui->labelCountUser->move(w4, 60);
    ui->tree->resize(ui->tree->width(), h-30);
    ui->hideUsers->resize(ui->hideUsers->width(), h);
    ui->showUsers->resize(ui->showUsers->width(), h);
    ui->textEdit->resize(w, h);
    ui->textEdit->scroll();

}

void notepad::showLabels()
{
    #ifdef DISPATCHER_ON
    ui->textEdit->setDocumentId(documentId);
    ui->textEdit->setClientDispatcher(cl);
    ui->textEdit->setThisUserSiteId(us.getSiteId());
    const std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData>> onlineUsers=cl->onlineUser(int documentID);
    #else
    std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData>> onlineUsers;
    Symposium::user *u1=new Symposium::user("Mario", "AP@ssw0rd!", "Mariuz", ":/resources/avatar/beaver.png", 1, nullptr);
    Symposium::user *u2=new Symposium::user("Carlo", "AP@ssw0rd!", "Carluz", ":/resources/avatar/boar.png", 2, nullptr);
    Symposium::user *u3=new Symposium::user("Federico", "AP@ssw0rd!", "Fede", ":/resources/avatar/bull.png", 3, nullptr);
    std::pair<Symposium::user*, Symposium::sessionData> p1{u1, Symposium::sessionData(Symposium::privilege::modify, 0, 0)};
    std::pair<Symposium::user*, Symposium::sessionData> p2{u2, Symposium::sessionData(Symposium::privilege::modify, 2, 2)};
    std::pair<Symposium::user*, Symposium::sessionData> p3{u3, Symposium::sessionData(Symposium::privilege::readOnly, 0, 0)};

    onlineUsers.push_front(p1);
    onlineUsers.push_front(p2);
    onlineUsers.push_front(p3);
    #endif
    ui->textEdit->constractLabelsCursors(onlineUsers, u1->getSiteId());
    if(privOpen!=Symposium::privilege::readOnly)
        ui->textEdit->insertCurrentUser(onlineUsers, u1->getSiteId());
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
    this->pos=cursor.position();

    if(event->key()==Qt::Key_Backspace)
        return handleDeleteKey();
    else if(QKeySequence(event->key()+int(event->modifiers())) == QKeySequence("Ctrl+V")) // Control_V action
        return; //this->on_actionPaste_triggered();
                //this->contV_action(pos);
    else if(QKeySequence(event->key()+int(event->modifiers())) == QKeySequence("Ctrl+C")) // Control_C action
        return; //this->on_actionCopy_triggered();
    else if(isAKeyToIgnore(event))
        return;
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
               if(keyEvent->matches(QKeySequence::Paste)){
                   this->okPaste=true;
                   const QMimeData *md = QApplication::clipboard()->mimeData();
                   this->dim=md->text().length();
               }
               handleTextEditKeyPress(keyEvent);
           }
    }
    return QObject::eventFilter(obj, event);
}

void notepad::handleDeleteKey(){
    int row, col;
    QTextCursor cursor= ui->textEdit->textCursor();
    /* handle the delete of a selected text*/
    if(cursor.hasSelection()){
       int start,end,row_start,row_end,dim,numLines;
        start=cursor.selectionStart();
        end=cursor.selectionEnd();
        cursor.setPosition(start,QTextCursor::MoveAnchor);
        row_start=cursor.blockNumber();
        col=cursor.positionInBlock();
        cursor.setPosition(end,QTextCursor::MoveAnchor);
        row_end=cursor.blockNumber();
        dim=end-start;
        qDebug()<<"Start"<<row_start;
        qDebug()<<"end"<<row_end;
        qDebug()<<"Dimensione"<<end-start;

        /* I'm on the same line*/
        if(row_start==row_end){
            while(dim>0){
                this->documentoProva.localRemove({row_start,col},1);
                dim--;
            }

        }else{
            /*I'm deleting multiple lines*/
            numLines=documentoProva.countsNumLines();
            qDebug()<<"NumLines"<<numLines;
            dim=row_end-row_start;
            /*I'm deleting also the last line: I have to delete also the enter character*/
            if(row_end==numLines-1){
                deleteMultipleLines(row_start,row_end,col,dim+1,true);
            }else{/*I'm not deleting the last line: in the previous line we have to maintain the enter character*/
                deleteMultipleLines(row_start,row_end,col,dim+1,false);
            }
        }
        qDebug()<<"Caratteri"<<this->documentoProva.toText();
        return;
 }else{
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
    if(row==0 && col==0){
        QColor black=Qt::black;
        black.setAlpha(160);
        ui->textEdit->setTextColor(black);
        ui->textEdit->setText("");
     }
    }
}

void notepad::deleteMultipleLines(int sR,int eR,int c,int sL,bool lines){
    QTextCursor cursor;
    int length;
    qDebug()<<"start Row"<<sR;
    qDebug()<<"endRow"<<eR;
    qDebug()<<"SelectedLines"<<sL;
    while(sR<=eR){
        cursor.setPosition(sR,QTextCursor::MoveAnchor);
      if(lines==true){
        length=countCharsInLine(sR)-c;
      }else{
          if(sL==1){
             length=countCharsInLine(sR)-c;
          }else{
          length=countCharsInLine(sR)-1-c;}
      }
        while(length>0){
            this->documentoProva.localRemove({sR,c},1);
            length--;
        }
        sR++;c=0;sL--;
    }
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

void notepad::contV_action(){
    QTextCursor curs=ui->textEdit->textCursor();
    int posAct= curs.position();
    qDebug()<<"PosAct"<<posAct;
    int posTmp=posAct-this->dim;
    qDebug()<<"PosTmp"<<posTmp;
    qDebug()<<"Dimensione"<<this->dim;
    int count=0;

    while(count!=this->dim){
        curs.setPosition(posTmp);
        curs.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor);
        //ui->textEdit->setTextCursor(curs);
        QString charact=curs.selectedText();
        qDebug()<<"charact"<<charact;
        int column=curs.positionInBlock();
        column= column-1;
        int row= curs.blockNumber();
        qDebug()<<"row"<<row;
        qDebug()<<"Column"<<column;
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
        Symposium::symbol sym(ch,1,1,pos,false);
        sym.setCharFormat(charFormat);
        this->documentoProva.localInsert(indexes,sym);
        //cl->localInsert(this->documentId, symbol &sym, &indexes)
        count++;posTmp++;
    }
    this->okPaste=false;

}

void notepad::addCursor()
{
    ui->textEdit->addUser(4, "Tizio");
}



void notepad::removeCursor()
{
    ui->textEdit->removeUser(4);
    insertusers();
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
        Symposium::Color colHigh=cl.getColor(this->documentId,sym.getSiteId());
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
        Symposium::Color colHigh=cl.getColor(this->documentId,sym.getSiteId());
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


void notepad::on_textEdit_cursorPositionChanged()
{

    QTextCursor cc=ui->textEdit->textCursor();
    QTextCharFormat ch=ui->textEdit->currentCharFormat();
     if(insertOthCh==false){
        ui->textEdit->thisUserChangePosition(us.getSiteId());
     }

     if(!cc.hasSelection()){

        QColor newCol=ch.foreground().color();
        newCol.setAlpha(255);
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
    /*
     std::vector<std::vector<Symposium::symbol>> symbols= this->documentoProva.getSymbols();
     QTextCursor curs=ui->textEdit->textCursor();
     curs.movePosition(QTextCursor::Start);
     int pi=curs.position();
     curs.movePosition(QTextCursor::End);
     int pf=curs.position();
     curs.movePosition(QTextCursor::Start);
     while(pi!=pf-1){
       curs.setPosition(pi,QTextCursor::MoveAnchor);
       curs.setPosition(pi+1,QTextCursor::KeepAnchor);
       QTextCharFormat format=curs.charFormat();
       format.setBackground(Qt::yellow);
       curs.setCharFormat(format);
       curs.setPosition(pi+1);
       pi++;

     }
     */

    std::vector<std::vector<Symposium::symbol>> symbols= this->documentoProva.getSymbols();
    QTextCursor curs=ui->textEdit->textCursor();
    int pi=0,pf=0,siteId;
    curs.movePosition(QTextCursor::End);
    int pend=curs.position();
    curs.movePosition(QTextCursor::Start);
    QColor userCol;
    for(size_t i=0; i<symbols.size();i++){
        for(size_t j=0;j<symbols[i].size();j++){
            if(symbols[i][j].getSiteId()==symbols[i][j+1].getSiteId()){
                siteId=symbols[i][j].getSiteId();
                pf++;
            }else{
                //Symposium::Color colHigh=cl->getColor(this->documentId,siteId);
                if(siteId==1){
                    userCol=Qt::yellow;
                    userCol.setAlpha(160);
                }else{
                    userCol=Qt::blue;
                    userCol.setAlpha(160);
                }
                while(pi!=pf){
                  curs.setPosition(pi,QTextCursor::MoveAnchor);
                  curs.setPosition(pi+1,QTextCursor::KeepAnchor);
                  QTextCharFormat format=curs.charFormat();
                  format.setBackground(userCol);
                  curs.setCharFormat(format);
                  curs.setPosition(pi+1);
                  pi++;
                  if (pi==pend)
                      return;

                }
                pi=pf;
                pf+=1;
            }
        }
    }

    /*
    QTextCharFormat chFormat;
    QColor qCol;
    QTextCursor curs=ui->textEdit->textCursor();
    QString ch;
    // save in symbols all the symbols contained in the document
    //std::vector<std::vector<Symposium::symbol>> symbols= this->doc.getSymbols();
    std::vector<std::vector<Symposium::symbol>> symbols= this->documentoProva.getSymbols();
    if(symbols[0][0].getCh()==emptyChar){
        QColor black=Qt::black;
        black.setAlpha(160);
        ui->textEdit->setTextColor(black);
        ui->textEdit->setText("");
        return;
    }
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
                this->currentCharFormatChanged(chFormat);
                curs.insertText(ch,chFormat);

            }
    }

    }
*/
    insertOthCh=false;
 }






void notepad::on_actionhighlight_triggered()
{

    if(highActivated==false){
        this->highActivated=true;
        this->insertedChars=0;
        ui->actionhighlight->setChecked(true);
        //ui->textEdit->clear();
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

int notepad::countCharsInLine(int line)const {
    std::vector<std::vector<Symposium::symbol>> symbols= this->documentoProva.getSymbols();
    int ch=0;
    for(size_t i=0;i<symbols[line].size();i++){
        if(symbols[line][i].getCh()!=emptyChar)
            ch++;
    }
    return ch;
}

void notepad::insertusers()
{
    int count=0;
    ui->tree->clear();
    #ifdef DISPATCHER_ON
    const std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData>> onlineUsers=cl->onlineUser(documentID);
    #else
    std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData>> onlineUsers;
    Symposium::user *u1=new Symposium::user("Mario", "AP@ssw0rd!", "Mariuz", ":/resources/avatar/beaver.png", 1, nullptr);
    Symposium::user *u2=new Symposium::user("Carlo", "AP@ssw0rd!", "Carluz", ":/resources/avatar/boar.png", 2, nullptr);
    Symposium::user *u3=new Symposium::user("Federico", "AP@ssw0rd!", "Fede", ":/resources/avatar/bull.png", 3, nullptr);
    std::pair<Symposium::user*, Symposium::sessionData> p1{u1, Symposium::sessionData(Symposium::privilege::modify, 0, 0)};
    std::pair<Symposium::user*, Symposium::sessionData> p2{u2, Symposium::sessionData(Symposium::privilege::modify, 2, 2)};
    std::pair<Symposium::user*, Symposium::sessionData> p3{u3, Symposium::sessionData(Symposium::privilege::readOnly, 0, 0)};

    onlineUsers.push_front(p1);
    onlineUsers.push_front(p2);
    onlineUsers.push_front(p3);
    #endif
    for(auto it:onlineUsers)
    {
        QTreeWidgetItem *item=new QTreeWidgetItem();
        item->setIcon(0, QIcon(QString::fromStdString(it.first->getIconPath())));
        if(us.getUsername()==it.first->getUsername())
            item->setText(0, "(you)");
        else
            item->setText(0, QString::fromStdString(it.first->getUsername()));
        ui->tree->addTopLevelItem(item);
        count++;
    }
    ui->labelCountUser->setText(QString::number(count));
}

void notepad::on_textEdit_textChanged()
{
    if(okPaste){
        qDebug()<<"testo"<<ui->textEdit->toPlainText();
        QTextCursor c=ui->textEdit->textCursor();
        qDebug()<<"Posizione ultima"<<c.position();
        this->contV_action();

    }
}

void notepad::on_hideUsers_clicked()
{
    showUsers=false;

    int w=this->size().width();
    int h=this->size().height();
    w=w-39;
    h=h-159;
    ui->textEdit->resize(w, h);
    ui->textEdit->scroll();

    ui->hideUsers->setDisabled(true);
    ui->hideUsers->hide();
    ui->tree->hide();
    ui->labelUser->hide();
    ui->labelCountUser->hide();

    ui->showUsers->setDisabled(false);
    ui->showUsers->show();
}

void notepad::on_showUsers_clicked()
{
    showUsers=true;
    int w=this->size().width();
    int h=this->size().height();
    w=w-30-240;
    h=h-159;
    ui->textEdit->resize(w, h);
    ui->textEdit->scroll();

    ui->hideUsers->setDisabled(false);
    ui->hideUsers->show();
    ui->tree->show();
    ui->labelUser->show();
    ui->labelCountUser->show();

    ui->showUsers->setDisabled(true);
    ui->showUsers->hide();
}

bool notepad::event(QEvent *e)
{
    if (e->type() == QEvent::WindowActivate) {
        obtainFocus();
    }
    else if(e->type() == QEvent::WindowDeactivate){
        focusToDirectory();
    }
    return QWidget::event(e);
}

void notepad::on_pushButton_clicked()
{
    showParent();
}

void notepad::setreadonly()
{
    ui->textEdit->setReadOnly(true);
    ui->formatText->hide();
    ui->fontComboBox->hide();
    ui->sizeBox->hide();
    ui->styleBox->hide();
}
