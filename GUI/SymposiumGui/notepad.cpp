#include "notepad.h"
#include "ui_notepad.h"

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

Symposium::alignType fromQTextAlignment(Qt::Alignment curAl){
    switch(curAl){
        case Qt::AlignLeft:
            return Symposium::alignType::left;
        case Qt::AlignRight:
            return Symposium::alignType::right;
        case Qt::AlignHCenter:
            return Symposium::alignType::center;
        case Qt::AlignJustify:
            return Symposium::alignType::justify;
        default:
            return Symposium::alignType::left;
    }
}

notepad::notepad(QWidget *parent, Symposium::privilege priv, Symposium::privilege privOpen,std::string pathToFile,const Symposium::document& doc, Symposium::uint_positive_cnt::type fileID, SymWinInterface& si, bool parentIsTransient) :
    QMainWindow(parent),
    SymNotepadWinInterface (si, isQWidget::isQwidgetType(*this), parentIsTransient), pathToFile(pathToFile), priv(priv), privOpen(privOpen),doc(doc), fileId(fileID), ui(new Ui::notepad)
{
    ui->setupUi(this);
    setMinimumSize(800, 600);
    ui->textEdit->setNotepad(this);
    ui->statusbar->addWidget(ui->labelChars);
    ui->textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->textEdit,&QWidget::customContextMenuRequested,this,&notepad::showTextEditContextMenu);
    m_textEditContextMenu=new QMenu(this);
    const auto stylesheet= QStringLiteral("QMenu {border-radius:15px; background-color: white;margin: 2px; border: 1px solid rgb(58, 80, 116); color:  rgb(58, 80, 116);}QMenu::separator {height: 2px;background: rgb(58, 80, 116);margin-left: 10px;margin-right: 5px;}");
    m_textEditContextMenu->setStyleSheet(stylesheet);
    m_textEditContextMenu->addAction(tr("Copy"),this,&notepad::on_actionCopy_triggered);
    m_textEditContextMenu->addSeparator();
    m_textEditContextMenu->addAction(tr("Cut"),this,&notepad::on_actionCut_triggered);
    m_textEditContextMenu->addSeparator();
    m_textEditContextMenu->addAction(tr("Paste"),this,&notepad::on_actionPaste_triggered);
    m_textEditContextMenu->addSeparator();

    QActionGroup *alignGroup= new QActionGroup(this);
    alignGroup->addAction(ui->actionAlignCenter);
    alignGroup->addAction(ui->actionAlignTextLeft);
    alignGroup->addAction(ui->actionAlignTextRight);
    alignGroup->addAction(ui->actionAlignTextJustify);

    ui->actionAlignTextLeft->setChecked(true);
    this->alignment=Symposium::alignType::left;


    addStyleFormat();
    this->textStyle(0);

    connect(ui->fontComboBox, QOverload<const QString &>::of(&QComboBox::activated), this, &notepad::textFamily);
    ui->fontComboBox->activated("Times New Roman");

    this->documentId = doc.getId();

    if(privOpen==Symposium::privilege::readOnly){
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
        setreadonly();
    }
    else
        ui->textEdit->installEventFilter(this);

    if(priv==Symposium::privilege::owner){
        QMenu *shareMenu=menuBar()->addMenu(tr("Sharing Policy"));
        shareMenu->addAction(tr("Disable links"), this, &notepad::inactiveLink);
        shareMenu->addAction(tr("Enable links"), this, &notepad::activeAlwaysLink);
        shareMenu->addAction(tr("Timer links"), this, &notepad::timerLink);
        shareMenu->addAction(tr("Number links"), this, &notepad::counterLink);
        shareMenu->addAction(tr("Change Privilege"), this, &notepad::visualizeAllUsers);
    }
    else{
        QMenu *usersMenu=menuBar()->addMenu(tr("Users"));
        usersMenu->addAction(tr("All users"), this, &notepad::visualizeAllUsers);
    }

    ui->actionhighlight->setIcon(QIcon(":/resources/cartelle/color_icon"));
    ui->actionhighlight->setCheckable(true);

    this->fillTextEdit();

    this->numChars=this->doc.getNumchar();
    this->labelChars=std::to_string(this->numChars);
    ui->labelChars->setText("Total Chars: " + QString::fromStdString(this->labelChars));

    ui->textEdit->setCursorWidth(3);

    ui->tree->setColumnCount(2);
    ui->tree->setColumnWidth(0, 1);
    ui->tree->header()->setVisible(false);
    insertusers();
    ui->tree->header()->setStretchLastSection(false);
    ui->tree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->showUsers->setDisabled(true);
    ui->showUsers->hide();

    setFocusPolicy(Qt::StrongFocus);
    setAttribute( Qt::WA_DeleteOnClose );

    this->colPos = Qt::GlobalColor::black;
    colorChanged(this->colPos);
    this->styleSend=true;
}

template<>
Symposium::Color::operator QColor() const{
    uint8_t red,green,blue;
    std::tie(red,green,blue)= this->getRgb();
    return QColor(red,green,blue);
}


Symposium::uint_positive_cnt::type notepad::getId(){
    return documentId;
}


notepad::~notepad(){
    delete ui;
    cl.closeSource(this->documentId);
}

void notepad::failure(const QString &toPrint){
    if(toPrint == "-2"){
        notification notWindow(nullptr, "There is a problem with the current document,\nso it will be closed.");
        int ret=notWindow.exec();
        if(ret==0){
            close();
            showParent();
        }
    }else{
        errorConnectionLogout();
    }
}

void notepad::errorConnectionLogout(){
    errorlogout errorLog(this);
    int ret=errorLog.exec();
    if(ret==0)
        backToMainWin();
}

void notepad::moveUserCursor(Symposium::uint_positive_cnt::type siteID, unsigned block, unsigned column){
    ui->textEdit->changePosition(siteID, block, column);
}

void notepad::removeUserCursor(Symposium::uint_positive_cnt::type siteID){
    ui->textEdit->removeUser(siteID);
    insertusers();
    if(highActivated==true){
        cl.mapSiteIdToUser(doc);
    }
}

void notepad::addUserCursor(Symposium::uint_positive_cnt::type siteID, const std::string& username){
    int block=ui->textEdit->textCursor().blockNumber();
    int column=ui->textEdit->textCursor().positionInBlock();
    NotRefreshLabels=true;
    ui->textEdit->changePosition(0,0);
    Qt::Alignment old=ui->textEdit->alignment();
    ui->textEdit->setAlignment(Qt::AlignLeft);
    ui->textEdit->scroll();
    ui->textEdit->addUser(siteID, username);
    insertusers();
    if(highActivated==true){
        cl.mapSiteIdToUser(doc);
    }
    ui->textEdit->setAlignment(old);
    ui->textEdit->scroll();
    ui->textEdit->changePosition(block, column);
    NotRefreshLabels=false;
}

void notepad::on_Export_PDF_triggered(){
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

void notepad::on_actionCopy_triggered(){
    ui->textEdit->copy();
    NotRefreshLabels=false;
}

void notepad::on_actionCut_triggered(){
    auto *fake_cut = new QKeyEvent(QKeyEvent::KeyPress, Qt::Key_X, Qt::ControlModifier, "");
    QApplication::sendEvent(ui->textEdit, fake_cut);
}

void notepad::on_actionPaste_triggered(){
    QColor lightColor=ui->textEdit->textCursor().charFormat().foreground().color();
    lightColor.setAlpha(alphaValue);
    ui->textEdit->setTextColor(lightColor);
    ui->textEdit->paste();
    this->okPaste=true;
    const QMimeData *md = QApplication::clipboard()->mimeData();
    this->dim=md->text().length();
    this->contV_action();
}

void notepad::addStyleFormat(){
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

void notepad::on_actionColorText_triggered(){
  textColor();
  ui->textEdit->setFocus();
}

void notepad::on_actionAlignTextLeft_triggered(){
    std::pair<Symposium::alignType, unsigned int> oldLineStyle(this->alignment, this->indexStyle);
    QTextCursor cursor=ui->textEdit->textCursor();
    unsigned int row=static_cast<unsigned int>(cursor.blockNumber());

    textAlign(ui->actionAlignTextLeft);
    this->alignment=Symposium::alignType::left;

    std::pair<Symposium::alignType, unsigned int> newLineStyle(this->alignment, this->indexStyle);

    ui->textEdit->scroll();

    if(styleSend)
        cl.localEditLineStyle(documentId, oldLineStyle, newLineStyle, row);

    ui->textEdit->setFocus();
}

void notepad::on_actionAlignCenter_triggered(){
    std::pair<Symposium::alignType, unsigned int> oldLineStyle(this->alignment, this->indexStyle);

    QTextCursor cursor=ui->textEdit->textCursor();
    unsigned int row=static_cast<unsigned int>(cursor.blockNumber());

    textAlign(ui->actionAlignCenter);
    this->alignment=Symposium::alignType::center;

    std::pair<Symposium::alignType, unsigned int> newLineStyle(this->alignment, this->indexStyle);

    ui->textEdit->scroll();

    if(styleSend)
        cl.localEditLineStyle(documentId, oldLineStyle, newLineStyle, row);

    ui->textEdit->setFocus();
}

void notepad::on_actionAlignTextRight_triggered(){
    std::pair<Symposium::alignType, unsigned int> oldLineStyle(this->alignment, this->indexStyle);

    QTextCursor cursor=ui->textEdit->textCursor();
    unsigned int row=static_cast<unsigned int>(cursor.blockNumber());

   textAlign(ui->actionAlignTextRight);
   this->alignment=Symposium::alignType::right;

   std::pair<Symposium::alignType, unsigned int> newLineStyle(this->alignment, this->indexStyle);

   ui->textEdit->scroll();

   if(styleSend)
       cl.localEditLineStyle(documentId, oldLineStyle, newLineStyle, row);

   ui->textEdit->setFocus();

}

void notepad::on_actionAlignTextJustify_triggered(){
    std::pair<Symposium::alignType, unsigned int> oldLineStyle(this->alignment, this->indexStyle);

    QTextCursor cursor=ui->textEdit->textCursor();
    unsigned int row=static_cast<unsigned int>(cursor.blockNumber());

    textAlign(ui->actionAlignTextJustify);
    this->alignment=Symposium::alignType::left;

    std::pair<Symposium::alignType, unsigned int> newLineStyle(this->alignment, this->indexStyle);

    ui->textEdit->scroll();

    if(styleSend)
        cl.localEditLineStyle(documentId, oldLineStyle, newLineStyle, row);

    ui->textEdit->setFocus();
}

void notepad::on_actionBoldFont_triggered(){
    QTextCharFormat fmt;
    QTextCursor cursor=ui->textEdit->textCursor();
    fmt.setFontWeight(ui->actionBoldFont->isChecked() ? QFont::Bold : QFont::Normal);
    ui->textEdit->mergeCurrentCharFormat(fmt);
    if(cursor.hasSelection())
        this->handleChangeFormat(cursor.selectionStart(),cursor.selectionEnd());

    ui->textEdit->setFocus();

}

void notepad::on_actionItalicFont_triggered(){
    QTextCharFormat fmt;
    QTextCursor cursor= ui->textEdit->textCursor();
    fmt.setFontItalic(ui->actionItalicFont->isChecked());
    ui->textEdit->mergeCurrentCharFormat(fmt);
    if(cursor.hasSelection())
        this->handleChangeFormat(cursor.selectionStart(),cursor.selectionEnd());
    ui->textEdit->setFocus();
}

void notepad::on_actionUnderlineFont_triggered(){
    QTextCharFormat fmt;
    QTextCursor cursor= ui->textEdit->textCursor();
    fmt.setFontUnderline(ui->actionUnderlineFont->isChecked());
    ui->textEdit->mergeCurrentCharFormat(fmt);
    if(cursor.hasSelection())
        this->handleChangeFormat(cursor.selectionStart(),cursor.selectionEnd());
    ui->textEdit->setFocus();
}

QTextListFormat::Style notepad::textStyle(int styleIndex){
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
        int headingLevel = styleIndex >= 9 ? styleIndex - 9 + 1 : 0;
        blockFmt.setHeadingLevel(headingLevel);
        cursor.setBlockFormat(blockFmt);

        int sizeAdjustment = headingLevel ? 4 - headingLevel : 0;
        QTextCharFormat fmt;
        fmt.setProperty(QTextFormat::FontSizeAdjustment, sizeAdjustment);
        cursor.select(QTextCursor::LineUnderCursor);
    }else{
        QTextListFormat listFmt;
        if (cursor.currentList()){
            listFmt = cursor.currentList()->format();
        }else{
            listFmt.setIndent(blockFmt.indent() + 1);
            blockFmt.setIndent(0);

            cursor.setBlockFormat(blockFmt);
        }
        listFmt.setStyle(style);
        cursor.createList(listFmt);
    }

    cursor.endEditBlock();
    ui->textEdit->setFocus();

    std::pair<Symposium::alignType, unsigned int> oldLineStyle(this->alignment, this->indexStyle);
    this->indexStyle=styleIndex;
    std::pair<Symposium::alignType, unsigned int> newLineStyle(this->alignment, this->indexStyle);
    unsigned int row=static_cast<unsigned int>(cursor.blockNumber());
    if(styleSend)
        cl.localEditLineStyle(documentId, oldLineStyle, newLineStyle, row);
    ui->textEdit->scroll();
    return style;
}

void notepad::mergeFormatOnWordOrSelection(const QTextCharFormat &format){
    QTextCursor cursor = ui->textEdit->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    ui->textEdit->mergeCurrentCharFormat(format);

}

void notepad::handleChangeFormat(unsigned int i, unsigned int f){
      NotRefreshLabels=true;
      QTextCursor curs=ui->textEdit->textCursor();
      QTextCharFormat format;
      QString character;
      unsigned int count=0;
      unsigned dimSelection=f-i;
      int prCol,col,row;
      while(count!=dimSelection){
        curs.setPosition(i);
        curs.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor);
        prCol=col; col=curs.positionInBlock()-1;
        if(col!=-1){
           prCol=col;
           row=curs.blockNumber();
           ui->textEdit->changePosition(row,col);
           format=curs.charFormat();
           QColor c = format.foreground().color();
           character=curs.selectedText();
           cl.localRemove(this->documentId,{row,col});
           this->sendSymbolToInsert(row,col,character,format);
        }
            count++; i++;
      }
      ui->textEdit->changePosition(row,++col);
      ui->textEdit->thisUserChangePosition(cl.getUser().getSiteId());
      NotRefreshLabels=false;
}

void notepad::textColor(){
    QColor col = QColorDialog::getColor(ui->textEdit->textColor(), this);
    if (!col.isValid())
        return;
    this->colPos = col;
    colorChanged(col);
    QColor lightCol=col;
    lightCol.setAlpha(alphaValue);
    ui->textEdit->setTextColor(lightCol);
    QTextCursor c=ui->textEdit->textCursor();
    if(c.hasSelection())
        this->handleChangeFormat(c.selectionStart(),c.selectionEnd());
}

void notepad::textFamily(const QString &f){
    QTextCharFormat fmt;
    QTextCursor cursor=ui->textEdit->textCursor();
    fmt.setFontFamily(f);
    ui->textEdit->mergeCurrentCharFormat(fmt);
    if(cursor.hasSelection())
        this->handleChangeFormat(cursor.selectionStart(),cursor.selectionEnd());
    ui->textEdit->setFocus();
}

void notepad::textSize(const QString &p){
    qreal pointSize = p.toFloat();
    QTextCursor cursor=ui->textEdit->textCursor();
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        ui->textEdit->mergeCurrentCharFormat(fmt);
        if(cursor.hasSelection())
             this->handleChangeFormat(cursor.selectionStart(),cursor.selectionEnd());
    }
    ui->textEdit->setFocus();
}

void notepad::textAlign(QAction *a){
    if (a == ui->actionAlignTextLeft)
        ui->textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    else if (a == ui->actionAlignCenter)
        ui->textEdit->setAlignment(Qt::AlignHCenter);
    else if (a == ui->actionAlignTextRight)
        ui->textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    else if (a == ui->actionAlignTextJustify)
        ui->textEdit->setAlignment(Qt::AlignJustify);
    QTextCursor cursor=ui->textEdit->textCursor();
    if(cursor.hasSelection())
        this->handleChangeFormat(cursor.selectionStart(),cursor.selectionEnd());
}

void notepad::colorChanged(const QColor &c){
    QPixmap pix(64, 64);
    pix.fill(c);
    ui->actionColorText->setIcon(pix);
}

void notepad::fontChanged(const QFont &f){
    ui->fontComboBox->setCurrentIndex(ui->fontComboBox->findText(QFontInfo(f).family()));
    ui->sizeBox->setCurrentIndex(ui->sizeBox->findText(QString::number(f.pointSize())));
    ui->actionBoldFont->setChecked(f.bold());
    ui->actionItalicFont->setChecked(f.italic());
    ui->actionUnderlineFont->setChecked(f.underline());
}

void notepad::currentCharFormatChanged(const QTextCharFormat &format){
    fontChanged(format.font());
    colorChanged(format.foreground().color());
}

void notepad::fillTextEdit(){
    NotRefreshLabels=true;
    NotRefreshLabels=true;
    styleSend=false;
    QTextCharFormat chFormat;
    QColor qCol;
    QTextCursor curs=ui->textEdit->textCursor();
    QString ch;
    auto& symbols= this->doc.getSymbols();
    auto alignmentStyle=this->doc.getAlignmentStyle();
    if(symbols[0][0].getCh()==Symposium::document::emptyChar){
        QColor black=Qt::black;
        black.setAlpha(alphaValue);
        ui->textEdit->setTextColor(black);
        ui->textEdit->setText("");
        return;
    }
    else{
        unsigned valStyle=20;
        auto valAlign=Symposium::alignType::left;
        for(size_t i=0; i<symbols.size();i++){
            auto style=alignmentStyle[i];
            if(valStyle!=style.second){
                this->textStyle(style.second);
                valStyle=style.second;
            }
            for(size_t j=0;j<symbols[i].size();j++){
                Symposium::symbol sym=symbols[i][j];
                ch=sym.getCh();
                if(ch!=Symposium::document::emptyChar){
                    QTextCharFormat chFormat;
                    Symposium::format format=sym.getCharFormat();
                    QFont font;
                    font.setFamily(QString::fromStdString(format.familyType));
                    font.setBold(format.isBold);
                    font.setUnderline(format.isUnderlined);
                    font.setItalic(format.isItalic);
                    font.setPointSize(format.size);
                    Symposium::Color col=format.col;
                    qCol=static_cast<QColor>(col);
                    if(!sym.isVerified())
                        qCol.setAlpha(alphaValue);
                    chFormat.setFont(font); chFormat.setForeground(qCol);
                    ui->textEdit->changePosition(i,static_cast<int>(j));
                    curs.insertText(ch,chFormat);
                    this->currentCharFormatChanged(chFormat);
                }
             }
        }
    }
    styleSend=true;
    NotRefreshLabels=false;
}

void notepad::fixAlignment(){
    NotRefreshLabels=true;
    styleSend=false;
    auto alignmentStyle=this->doc.getAlignmentStyle();
    auto& symbols= this->doc.getSymbols();
    QTextCursor curs=ui->textEdit->textCursor();
    for(size_t i=0; i<alignmentStyle.size();i++){
        ui->textEdit->changePosition(i,0);
        auto style=alignmentStyle[i];
        Symposium::alignType align=style.first;
        if(align==Symposium::alignType::left){
            this->textAlign(ui->actionAlignTextLeft);
            ui->actionAlignTextLeft->setChecked(true);
        }else if(align==Symposium::alignType::right){
            this->textAlign(ui->actionAlignTextRight);
            ui->actionAlignTextRight->setChecked(true);
        }else if(align==Symposium::alignType::center){
            this->textAlign(ui->actionAlignCenter);
            ui->actionAlignCenter->setChecked(true);
        }else if (align==Symposium::alignType::justify){
            this->textAlign(ui->actionAlignTextJustify);
            ui->actionAlignTextJustify->setChecked(true);
        }
    }
    ui->textEdit->scroll();
    styleSend=true;
    NotRefreshLabels=false;
    ui->textEdit->changePosition(0,0);
}

void notepad::visualizeAllUsers(){
    std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData>> onlineUsers=cl.onlineUser(documentId);
    std::unordered_map<std::string, Symposium::privilege> users=cl.allUser(documentId);
    alluser *alluserWindow = new alluser(this,  priv, documentId, cl.getUser(), pathToFile, onlineUsers, users, fileId, *this);
    goToWindow(*alluserWindow);
}

void notepad::inactiveLink(){
    activenonlink* nonlinkwindow = new activenonlink(this, fileId, pathToFile, sharingPath, *this);
    goToWindow(*nonlinkwindow);
}

void notepad::activeAlwaysLink(){
    activealwayslink* alwayslinkwindow = new activealwayslink(this, fileId, pathToFile, sharingPath, cl.getUser(), *this);
    goToWindow(*alwayslinkwindow);
}

void notepad::timerLink(){
    activetimerlink* timerlinkwindow = new activetimerlink(this, fileId, pathToFile, sharingPath, cl.getUser(), *this);
    goToWindow(*timerlinkwindow);
}

void notepad::counterLink(){
    activecounterlink* counterlinkwindow = new activecounterlink(this, fileId, pathToFile, sharingPath, cl.getUser(), *this);
    goToWindow(*counterlinkwindow);
}

void notepad::closeEvent(QCloseEvent *event){
    if(closedByUser()){
        event->accept();
        if(isLastNotepadOpened())
            showParent();
        closeNotepad();
    }
    else{
        event->accept();
        if(isLastNotepadOpened())
            showParent();
    }
}

void notepad::resizeEvent(QResizeEvent *event){
    int w=event->size().width();
    int h=event->size().height();
    int w1=w-240;
    int w2=w-260;
    int w3=w-30;
    int w4=w-118;
    int w5=w-160;
    if(showUsers)
        w=w-30-240;
    else
        w=w-39;
    h=h-159;
    if(privOpen!=Symposium::privilege::readOnly)
        ui->pushButton->move(w5, 20);
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

void notepad::showLabels(){
    ui->textEdit->setDocumentId(documentId);
    ui->textEdit->setClientDispatcher(&cl);
    ui->textEdit->setThisUserSiteId(cl.getUser().getSiteId());
    const std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData>> onlineUsers=cl.onlineUser(documentId);
    ui->textEdit->constractLabelsCursors(onlineUsers, cl.getUser().getSiteId());
    if(privOpen!=Symposium::privilege::readOnly)
        ui->textEdit->insertCurrentUser(onlineUsers, cl.getUser().getSiteId());
}

bool notepad::isAKeyToIgnore(QKeyEvent* event){
    QString c=event->text();
    auto ch=c[0];
    bool accepted= (ch.isPrint() || ch.isPunct() || ch.isSpace() || event->matches(QKeySequence::Copy)
                    || event->matches(QKeySequence::Paste) || event->matches(QKeySequence::Cut) || event->matches(QKeySequence::SelectAll)
                    || event->matches(QKeySequence::Delete) || event->key()==Qt::Key_Backspace);
    return !accepted || QKeySequence(event->key()+int(event->modifiers())) == QKeySequence("Ctrl+K");
}

void notepad::handleTextEditKeyPress(QKeyEvent* event){

    QTextCursor cursor= ui->textEdit->textCursor();
    QTextCharFormat format = cursor.charFormat();
    format.setForeground(this->colPos);
    QString testo=event->text();
    unsigned row, column;
    NotRefreshLabels=true;
    if(QKeySequence(event->key()+int(event->modifiers())) == QKeySequence("Ctrl+C")
       || isAKeyToIgnore(event) || QKeySequence(event->key()+int(event->modifiers())) == QKeySequence("Ctrl+A")){
        NotRefreshLabels=false;
        return;
    }
    QColor lightColor=this->colPos;
    lightColor.setAlpha(alphaValue);
    ui->textEdit->setTextColor(lightColor);
    if(event->matches(QKeySequence::Paste)){
        this->okPaste=true;
        const QMimeData *md = QApplication::clipboard()->mimeData();
        this->dim=md->text().length();
        return;
    }
    else if(event->key()==Qt::Key_Backspace || event->key()==Qt::Key_Delete || event->matches(QKeySequence::Cut)){
        return handleDeleteKey(event);
    }
    else if(QKeySequence(event->key()+int(event->modifiers())) == QKeySequence("Ctrl+V")
            || isAKeyToIgnore(event))
    {
        NotRefreshLabels=false;
        return;
    }else{
        if(cursor.hasSelection()){
            int pos = cursor.selectionStart();
            handleDeleteKey(event);
            cursor.setPosition(pos);
        }
        row=cursor.blockNumber();
        column=cursor.positionInBlock();
    }
    this->sendSymbolToInsert(row,column,testo,format);
}

bool notepad::eventFilter(QObject *obj, QEvent *event){
    if(event->type() == QEvent::KeyPress){
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if(obj->objectName()=="textEdit"){
            handleTextEditKeyPress(keyEvent);
        }
    }else if(event->type()== QEvent::KeyRelease){
        NotRefreshLabels=true;
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if(isAKeyToIgnore(keyEvent))
            event->ignore();
        else if(QKeySequence(keyEvent->key()+int(keyEvent->modifiers())) == QKeySequence("Ctrl+A"))
            ui->textEdit->moveCursorLabelToEnd();
        else if (QKeySequence(keyEvent->key()+int(keyEvent->modifiers())) != QKeySequence("Ctrl+C")
                && QKeySequence(keyEvent->key()+int(keyEvent->modifiers())) != QKeySequence("Ctrl+A"))
            ui->textEdit->translateCursors(doc.getActiveUsers());
        if(this->pressCanc) {
            QKeyEvent *fake_canc = new QKeyEvent(QKeyEvent::KeyPress, Qt::Key_Delete, Qt::NoModifier, "");
            QApplication::sendEvent(ui->textEdit, fake_canc);
            this->pressCanc=false;
        }
        NotRefreshLabels=false;
    }else if (event->type()==QEvent::MouseButtonPress || event->type()==QEvent::MouseButtonDblClick){
        NotRefreshLabels=false;
    }
    return QObject::eventFilter(obj, event);
}

void notepad::handleDeleteKey(QKeyEvent *event) {
    int row, col;
    bool fakeCanc=this->pressCanc;
    QTextCursor cursor= ui->textEdit->textCursor();
    if(!cursor.hasSelection()){
        if(event->key()==Qt::Key_Backspace){
            row=cursor.blockNumber();
            col=cursor.positionInBlock()-1;
            if((this->indexStyle>0 && this->indexStyle<9) && col==-1)
                this->pressCanc=true;
            if(col<0 && row>0){
                row=cursor.blockNumber()-1;
                cursor.movePosition(QTextCursor::PreviousBlock);
                cursor.movePosition(QTextCursor::EndOfBlock);
                col=cursor.positionInBlock();
                cursor.movePosition(QTextCursor::NextBlock);
                cursor.movePosition(QTextCursor::StartOfBlock);
            }else if(col<0)
                return;
        }else{
            if(cursor.atEnd()){
                return;
            }
            row=cursor.blockNumber();
            col=cursor.positionInBlock();
        }
        if(!fakeCanc)
            cl.localRemove(this->documentId,{row,col});
    }else{
        int start=cursor.selectionStart();
        int end=cursor.selectionEnd();

        cursor.setPosition(start, QTextCursor::KeepAnchor);
        int startBlock= cursor.blockNumber();
        int startCol= cursor.positionInBlock();

        cursor.setPosition(end, QTextCursor::KeepAnchor);
        int endBlock= cursor.blockNumber();
        int endCol= cursor.positionInBlock();

        while(!(startBlock==endBlock && startCol==endCol)) {
            if(endCol==0) {
                cursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::KeepAnchor);
                cursor.movePosition(QTextCursor::EndOfBlock,QTextCursor::KeepAnchor);
            }else
                cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
            endBlock=cursor.blockNumber();
            endCol=cursor.positionInBlock();
            cl.localRemove(this->doc.getId(),{endBlock,endCol});
        }
    }
    this->numChars=this->doc.getNumchar();
    this->labelChars=std::to_string(this->numChars);
    ui->labelChars->setText("Total Chars: "+QString::fromStdString(this->labelChars));
        if(this->numChars==0){
            QTextBlockFormat blockFmt = cursor.blockFormat();
            blockFmt.setHeadingLevel(0);
            blockFmt.setObjectIndex(-1);
            cursor.setBlockFormat(blockFmt);
            ui->styleBox->setCurrentIndex(0);
            QTextCharFormat fmt; QFont fnt;
            fnt.setBold(false); fnt.setItalic(false); fnt.setUnderline(false);
            fnt.setFamily("Times New Roman");
            this->colPos=Qt::GlobalColor::black;
            fmt.setForeground(this->colPos);
            fmt.setFont(fnt);
            currentCharFormatChanged(fmt);
            ui->actionAlignTextLeft->setChecked(true);
            ui->textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
        }
 }

void notepad::sendSymbolToInsert(unsigned row, unsigned column,QString text, QTextCharFormat format){
    std::wstring str=text.toStdWString();
    wchar_t ch=str[0];
    const std::pair<unsigned, unsigned> indexes={row,column};
    QColor col=format.foreground().color();
    QFont font= format.font();
    bool isBold= font.bold();
    bool isUnderlined=font.underline();
    bool isItalic=font.italic();
    unsigned size=font.pointSize();
    std::string fontFamily=font.family().toStdString();
    int blue=col.blue();
    int red=col.red();
    int green=col.green();
    Symposium::Color myCol(red,green,blue);
    Symposium::format charFormat={fontFamily,isBold,isUnderlined,isItalic,size,myCol,this->indexStyle,this->alignment};
    std::vector<int> pos;
    Symposium::symbol sym(ch,cl.getUser().getSiteId(),1,pos,false);
    sym.setCharFormat(charFormat);
    cl.localInsert(this->documentId,sym,indexes);
    this->numChars=this->doc.getNumchar();
    this->labelChars=std::to_string(this->numChars);
    ui->labelChars->setText("Total Chars: " + QString::fromStdString(this->labelChars));
}

void notepad::contV_action(){
    this->okPaste=false;
}

void notepad::addCursor(){
    ui->textEdit->addUser(4, "Tizio");
}

void notepad::removeCursor(){
    ui->textEdit->removeUser(4);
    insertusers();
}

void notepad::changeCursorPos(){
    ui->textEdit->changePosition(4, 1, 1);
}

void notepad::remoteInsert(const Symposium::symbol& sym, Symposium::uint_positive_cnt::type siteId, const std::pair<unsigned,unsigned>& indexes){
    NotRefreshLabels=true;
    int row=indexes.first;
    int column=indexes.second;

    auto styles=this->doc.getAlignmentStyle()[row];

    QTextCursor curs=ui->textEdit->textCursor();
    int actBlock=curs.blockNumber();
    int actColm=curs.positionInBlock();
    Symposium::format f= sym.getCharFormat();
    QTextCharFormat ch_format;QFont ch_font;

    ch_font.setFamily(QString::fromStdString(f.familyType));
    ch_font.setBold(f.isBold);
    ch_font.setUnderline(f.isUnderlined);
    ch_font.setItalic(f.isItalic);
    ch_font.setPointSize(f.size);
    Symposium::Color col=f.col;

    QColor qCol;
    qCol=static_cast<QColor>(col);
    ch_format.setFont(ch_font);
    ch_format.setForeground(qCol);

    if(this->highActivated){
        Symposium::Color colHigh=cl.getColor(this->documentId,sym.getSiteId());
        QColor colUser;
        colUser=static_cast<QColor>(colHigh);
        colUser.setAlpha(alphaValue);
        ch_format.setBackground(colUser);
    }
    curs.movePosition(QTextCursor::Start);
    curs.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor,row);
    curs.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, column);
    ui->textEdit->setTextCursor(curs);
    ui->textEdit->changePosition(siteId,row,column++);

    wchar_t symch=sym.getCh();
    QString ch;
    ch[0]=symch;
    styleSend=false;
    if(this->indexStyle!=styles.second){
        if(row == 0)
            this->textStyle(styles.second);
        else if(this->doc.getAlignmentStyle()[row-1] != this->doc.getAlignmentStyle()[row])
            this->textStyle(styles.second);
    }

    ui->textEdit->changePosition(row,0);
    Symposium::alignType align=styles.first;
    if(align==Symposium::alignType::left){
        this->textAlign(ui->actionAlignTextLeft);
    }else if(align==Symposium::alignType::right){
        this->textAlign(ui->actionAlignTextRight);
    }else if(align==Symposium::alignType::center){
        this->textAlign(ui->actionAlignCenter);
    }else if (align==Symposium::alignType::justify){
        this->textAlign(ui->actionAlignTextJustify);
    }
    styleSend=true;
    curs.insertText(ch,ch_format);
    ui->textEdit->changePosition(siteId,row,column++);
    ui->textEdit->changePosition(actBlock,actColm);
    NotRefreshLabels=false;
    this->numChars=this->doc.getNumchar();
    this->labelChars=std::to_string(this->numChars);
    ui->labelChars->setText("Total Chars: " + QString::fromStdString(this->labelChars));
    ui->textEdit->translateCursors(doc.getActiveUsers());
}

void notepad::verifySymbol(const Symposium::symbol& sym, const std::pair<unsigned, unsigned>& indexes){

    NotRefreshLabels=true;

    int row=indexes.first;
    int column=indexes.second;

    QTextCursor curs=ui->textEdit->textCursor();
    int actBlock=curs.blockNumber();
    int actColm=curs.positionInBlock();

    Symposium::format f= sym.getCharFormat();
    QTextCharFormat ch_format;QFont ch_font;

    ch_font.setFamily(QString::fromStdString(f.familyType));
    ch_font.setBold(f.isBold);
    ch_font.setUnderline(f.isUnderlined);
    ch_font.setItalic(f.isItalic);
    ch_font.setPointSize(f.size);
    Symposium::Color col=f.col;

    QColor qCol;
    qCol=static_cast<QColor>(col);
    QBrush brh(qCol);

    ch_format.setFont(ch_font);
    ch_format.setForeground(brh);

    if(this->highActivated){
        Symposium::Color colHigh=cl.getColor(this->documentId,sym.getSiteId());
        QColor colUser;
        colUser=static_cast<QColor>(colHigh);
        colUser.setAlpha(alphaValue);
        ch_format.setBackground(colUser);
    }

    curs.movePosition(QTextCursor::Start);
    curs.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, row);
    curs.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, column);

    curs.deleteChar();
    wchar_t symch=sym.getCh();
    QString ch;
    ch[0]=symch;
    curs.insertText(ch,ch_format);

    ui->textEdit->changePosition(actBlock,actColm);
    NotRefreshLabels=false;

    QColor lightCol=colPos;
    lightCol.setAlpha(alphaValue);
    ui->textEdit->setTextColor(lightCol);
    this->colorChanged(this->colPos);
}

void notepad::remoteDelete(const std::pair<unsigned, unsigned>& indexes, Symposium::uint_positive_cnt::type siteId){
    NotRefreshLabels=true;

    int block=indexes.first;
    int column=indexes.second;

    column++;
    QTextCursor curs=ui->textEdit->textCursor();
    int actBlock=curs.blockNumber();
    int actColm=curs.positionInBlock();

    curs.movePosition(QTextCursor::Start);
    curs.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor,block);
    curs.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, column);
    ui->textEdit->setTextCursor(curs);
    ui->textEdit->changePosition(2,block,column--);

    curs.deletePreviousChar();

    ui->textEdit->changePosition(siteId,block,column--);
    ui->textEdit->changePosition(actBlock,actColm);

    NotRefreshLabels=false;

    this->numChars=this->doc.getNumchar();
    this->labelChars=std::to_string(this->numChars);
    ui->labelChars->setText("Total Chars: " + QString::fromStdString(this->labelChars));
    ui->textEdit->translateCursors(doc.getActiveUsers());
}

void notepad::on_textEdit_cursorPositionChanged(){
    QTextCursor cc=ui->textEdit->textCursor();
    QTextCharFormat ch=ui->textEdit->currentCharFormat();
     if(NotRefreshLabels == false)
        ui->textEdit->thisUserChangePosition(cl.getUser().getSiteId());

    this->colPos = ch.foreground().color();
    this->currentCharFormatChanged(ch);

    Symposium::Color colHigh=cl.getColor(this->documentId,cl.getUser().getSiteId());
    QColor userCol=static_cast<QColor>(colHigh);
    userCol.setAlpha(alphaValue);

    if(this->highActivated)
        ui->textEdit->setTextBackgroundColor(userCol);

    auto* textEdit=ui->textEdit;
    auto* comboStyle=ui->styleBox;
    if(!ui->textEdit->textCursor().hasSelection())
        alignmentChanged(textEdit->alignment());
    QTextList *list = textEdit->textCursor().currentList();
    if (list) {
        switch (list->format().style()) {
            case QTextListFormat::ListDisc:
                comboStyle->setCurrentIndex(1);
                break;
            case QTextListFormat::ListCircle:
                comboStyle->setCurrentIndex(2);
                break;
            case QTextListFormat::ListSquare:
                comboStyle->setCurrentIndex(3);
                break;
            case QTextListFormat::ListDecimal:
                comboStyle->setCurrentIndex(4);
                break;
            case QTextListFormat::ListLowerAlpha:
                comboStyle->setCurrentIndex(5);
                break;
            case QTextListFormat::ListUpperAlpha:
                comboStyle->setCurrentIndex(6);
                break;
            case QTextListFormat::ListLowerRoman:
                comboStyle->setCurrentIndex(7);
                break;
            case QTextListFormat::ListUpperRoman:
                comboStyle->setCurrentIndex(8);
                break;
            default:
                comboStyle->setCurrentIndex(-1);
                break;
        }
    }else{
        int headingLevel = textEdit->textCursor().blockFormat().headingLevel();
        comboStyle->setCurrentIndex(headingLevel ? headingLevel + 8 : 0);
    }
    this->indexStyle=this->ui->styleBox->currentIndex();
}

void notepad::alignmentChanged(Qt::Alignment a){
    if (a & Qt::AlignLeft) {
        ui->actionAlignTextLeft->setChecked(true);
        this->textAlign(ui->actionAlignTextLeft);
        this->alignment=Symposium::alignType::left;
    }
        else if (a & Qt::AlignHCenter) {
        ui->actionAlignCenter->setChecked(true);
        this->textAlign(ui->actionAlignCenter);
        this->alignment=Symposium::alignType::center;

    }
        else if (a & Qt::AlignRight) {
        ui->actionAlignTextRight->setChecked(true);
        this->textAlign(ui->actionAlignTextRight);
        this->alignment=Symposium::alignType::right;
    }
        else if (a & Qt::AlignJustify) {
        ui->actionAlignTextJustify->setChecked(true);
        this->textAlign(ui->actionAlignTextJustify);
        this->alignment=Symposium::alignType::justify;
    }
}

void notepad::colorText(){
    NotRefreshLabels=true;

    auto& symbols=doc.getSymbols();
    QTextCursor curs=ui->textEdit->textCursor();

    int initRow=curs.blockNumber();
    int initCol=curs.positionInBlock();

    curs.movePosition(QTextCursor::Start);
    for(auto& row:symbols){
        for(auto& sym:row){
            if(sym.getSiteId() != 0){
                Symposium::Color colHigh=cl.getColor(doc.getId(),sym.getSiteId());
                QColor userCol=static_cast<QColor>(colHigh);
                userCol.setAlpha(alphaValue);
                curs.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
                QTextCharFormat format=curs.charFormat();
                format.setBackground(userCol);
                curs.setCharFormat(format);
                curs.movePosition(QTextCursor::PreviousCharacter);
                curs.movePosition(QTextCursor::NextCharacter);
            }
        }
    }
    ui->textEdit->changePosition(initRow,initCol);
    NotRefreshLabels=false;
}

void notepad::uncolorText(){
    NotRefreshLabels=true;

    auto& symbols=doc.getSymbols();
    QTextCursor curs=ui->textEdit->textCursor();
    int initRow=curs.blockNumber();
    int initCol=curs.positionInBlock();
    QColor white(Qt::GlobalColor::white);
    curs.movePosition(QTextCursor::Start);
    for(auto& row:symbols){
        for(auto& sym:row){
            if(sym.getSiteId() != 0){
                curs.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
                QTextCharFormat format=curs.charFormat();
                format.setBackground(white);
                curs.setCharFormat(format);
                curs.movePosition(QTextCursor::PreviousCharacter);
                curs.movePosition(QTextCursor::NextCharacter);
            }
        }
    }
    ui->textEdit->changePosition(initRow,initCol);
    NotRefreshLabels=false;
}

void notepad::on_actionhighlight_triggered(){
    if(highActivated==false){
        this->highActivated=true;
        ui->actionhighlight->setChecked(true);
        cl.mapSiteIdToUser(doc);
    }else{
        this->highActivated=false;
        ui->actionhighlight->setChecked(false);
        this->uncolorText();
    }
}

unsigned int notepad::countCharsInLine(const unsigned line)const {
    return this->doc.countCharsInLine(line);
}

void notepad::insertusers(){
    int count=0;
    ui->tree->clear();
    const std::forward_list<std::pair<const Symposium::user *, Symposium::sessionData>> onlineUsers=cl.onlineUser(documentId);
    ui->tree->setIconSize(QSize(30,30));
    for(const auto& it:onlineUsers){
        QTreeWidgetItem *item=new QTreeWidgetItem();
        item->setIcon(1, QIcon(QString::fromStdString(it.first->getIconPath())));
        if(cl.getUser().getUsername()==it.first->getUsername())
            item->setText(1, "(you)");
        else
            item->setText(1, QString::fromStdString(it.first->getUsername()));
        Symposium::Color colHigh=cl.getColor(documentId,it.first->getSiteId());
        QColor colUser;
        colUser=static_cast<QColor>(colHigh);
        item->setBackgroundColor(0,colUser);
        ui->tree->addTopLevelItem(item);
        count++;
    }
    ui->labelCountUser->setText(QString::number(count));
}

void notepad::on_textEdit_textChanged(){
    if(okPaste){
        QTextCursor c=ui->textEdit->textCursor();
        this->contV_action();
    }
}

void notepad::on_hideUsers_clicked(){
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

void notepad::on_showUsers_clicked(){
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

bool notepad::event(QEvent *e){
    if (e->type() == QEvent::WindowActivate) {
        obtainFocus();
    }
    else if(e->type() == QEvent::WindowDeactivate){
        focusToDirectory();
    }
    return QWidget::event(e);
}

void notepad::on_pushButton_clicked(){
    showParent();
}

void notepad::setreadonly(){
    ui->textEdit->setReadOnly(true);
    ui->formatText->hide();
    ui->fontComboBox->hide();
    ui->sizeBox->hide();
    ui->styleBox->hide();
}

void notepad::modifyWinTitle(Symposium::uint_positive_cnt::type resId, const QString& newName) {
    if(fileId==resId)
        this->setWindowTitle(newName);
}

void notepad::success(){
    colorText();
}

void notepad::editLineStyle(const std::pair<Symposium::alignType, unsigned int> &newLineStyle, unsigned int row) {
    NotRefreshLabels=true;
    QTextCursor curs=ui->textEdit->textCursor();
    int initRow=curs.blockNumber();
    int initCol=curs.positionInBlock();
    curs.movePosition(QTextCursor::Start);
    curs.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor,row);
    ui->textEdit->setTextCursor(curs);

    if (newLineStyle.first == Symposium::alignType::left)
        ui->textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    else if (newLineStyle.first == Symposium::alignType::center)
        ui->textEdit->setAlignment(Qt::AlignHCenter);
    else if (newLineStyle.first == Symposium::alignType::right)
        ui->textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    else if (newLineStyle.first == Symposium::alignType::justify)
        ui->textEdit->setAlignment(Qt::AlignJustify);
    this->alignment=fromQTextAlignment(ui->textEdit->alignment());

    styleSend=false;
    if(this->indexStyle!=newLineStyle.second)
        textStyle(static_cast<int>(newLineStyle.second));
    styleSend=true;
    ui->textEdit->changePosition(initRow,initCol);
    ui->textEdit->scroll();
    NotRefreshLabels=false;
}

void notepad::showTextEditContextMenu(const QPoint &pos){
    m_textEditContextMenu->popup(ui->textEdit->mapToGlobal(pos));
}

void notepad::setSharingPath(const std::string &sharingPath) {
    notepad::sharingPath = sharingPath;
}
