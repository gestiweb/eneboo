/***************************************************************************
 *   Copyright (C) 2005 by Jinsoo Kang                                     *
 *   jsk@gravitybind.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "teddy.h"
#include "teddyIcon.xpm"

#include <math.h>
#include <iostream>
#include <cstdlib>

#include <qprocess.h>


using namespace std;


QIntDict<QPixmap> Teddy::messageIcons = QIntDict<QPixmap>();
QString Teddy::externalEditor;
bool Teddy::externalEditorActive = TRUE;
int Teddy::numRecentDocs = 20;

QStringList Teddy::recentDocList;


Teddy::Teddy() : QMainWindow( 0, "Teddy", WDestructiveClose ) {
	messageIcons.setAutoDelete(TRUE);
	if (messageIcons.isEmpty()) for (int i = (int)QMessageBox::NoIcon; i <= (int)QMessageBox::Question; i++) {
		QImage currImg = QMessageBox::standardIcon( (QMessageBox::Icon)i ).convertToImage();
		if (currImg.isNull()) continue;
		QPixmap* currPix = new QPixmap( currImg.smoothScale(18,18) );
		messageIcons.insert(i, currPix);
	}

	printer = new QPrinter();

	QPixmap appIcon = QPixmap(teddyIcon);
	setIcon(appIcon);

	QSplitter *splitter = new QSplitter(this);
		splitter->setOrientation(QSplitter::Vertical);
		splitter->setChildrenCollapsible(TRUE);
		setCentralWidget( splitter );
	
	xtable = new XmlTable(500, 20, splitter);
	bottomTabs = new QTabWidget(splitter);
		bottomTabs->setTabPosition(QTabWidget::Bottom);
	
	cellEdit = new QTextEdit(bottomTabs);
		cellEdit->setTabStopWidth(4);
		cellEdit->setTextFormat(Qt::PlainText);
		cellEdit->setWordWrap(QTextEdit::NoWrap);
		bottomTabs->addTab(cellEdit, tr("Edit Cell"));
	dtdEdit = new QTextEdit(bottomTabs);
		dtdEdit->setTabStopWidth(4);
		dtdEdit->setTextFormat(Qt::PlainText);
		dtdEdit->setWordWrap(QTextEdit::NoWrap);
		bottomTabs->addTab(dtdEdit, tr("DTD"));
	errorTable = new MessageList();
		bottomTabs->addTab(errorTable, tr("Messages"));
		QObject* errorTableListView = errorTable->child("messageListView", "QListView");
		connect(
			errorTableListView, SIGNAL( selectionChanged(QListViewItem*) ),
			this, SLOT( openMessageTarget(QListViewItem*) )
		);
		connect(
			errorTableListView, SIGNAL( doubleClicked(QListViewItem*, const QPoint&, int) ),
			this, SLOT( openMessageTarget(QListViewItem*) )
		);

	QValueList<int> sizeList;
	sizeList.append(1000);
	sizeList.append(10);
	splitter->setSizes(sizeList);
	
	createFileMenu();
	createEditMenu();
	createFormatMenu();
	createHelpMenu();
	createStatusBar();

	connect( cellEdit, SIGNAL(textChanged()), this, SLOT(updateCellText()) );
	connect( cellEdit, SIGNAL(modificationChanged(bool)), xtable, SLOT(touch(bool)) );
	connect( dtdEdit, SIGNAL(modificationChanged(bool)), xtable, SLOT(touch(bool)) );
	connect( xtable, SIGNAL(dtdLoaded(const QString &)), dtdEdit, SLOT(setText(const QString &)) );
	connect(
		xtable, SIGNAL(message(const QString &, const QString &, bool, bool, QMessageBox::Icon)), 
		this, SLOT(message(const QString &, const QString &, bool, bool, QMessageBox::Icon))
	);

	pref = new PrefDialog( this, tr("Preferences") );
	pref->setCaption( tr("Preferences") );
	connect( pref, SIGNAL(applyButtonPressed()), xtable, SLOT(updateAll()) );
	connect( pref, SIGNAL(applyButtonPressed()), this, SLOT(applyStyleIcons()) );
	connect( pref, SIGNAL(cancelButtonPressed()), xtable, SLOT(updateAll()) );
	
	message( tr("Ready") );
	applySettings();
}


Teddy::~Teddy() {
	delete printer;
	delete errorTable;
}


void Teddy::openMessageTarget(QListViewItem *item) {
	int line = -1, row = -1, col = -1;
	QString target = item->text(0);
	QStringList pairs = QStringList::split(',', target);
	for (QStringList::Iterator it = pairs.begin(); it != pairs.end(); ++it) {
		QString key, val;
		QStringList curr = QStringList::split(':', *it);
		QStringList::Iterator it2 = curr.begin();
		if (it2 != curr.end()) key = *it2;
		++it2;
		if (it2 != curr.end()) val = *it2;
		key = key.stripWhiteSpace();
		val = val.stripWhiteSpace();

		if (!val.isEmpty()) {
			if (key == tr("Line")) line = val.toInt();
			else if (key == tr("Row")) row = val.toInt();
			else if (key == tr("Column")) col = val.toInt();
		}
	}
	if (line >= 0 && Teddy::externalEditorActive) {
		QProcess proc(Teddy::externalEditor);
		proc.addArgument(filename);
		proc.launch("");
	}
	else if (row > 0 && col > 0) {
		xtable->clearSelection();
		row--; col--;
		xtable->setCurrentCell(row, col);
	}
}


void Teddy::message(const QString &msg, const QString &loc, bool printStatusBar, bool printLog, QMessageBox::Icon icn) {
	if (printLog) {
		QObject *list = errorTable->child("messageListView", "QListView");
		if (list == NULL) return;
		QListView *messageListView = (QListView*) list;
		QListViewItem *currItem = new QListViewItem(messageListView, loc, msg);
		QPixmap *currPix = messageIcons[icn];
		if (currPix != NULL) currItem->setPixmap( 1, *currPix );
		messageListView->insertItem(currItem);
		messageListView->scrollBy(0, 100);
		bottomTabs->showPage(errorTable);
	}
	if (printStatusBar) {
		statusBar()->message( msg, 5000 );
	}
}


void Teddy::clearMessages() {
	QObject *list = errorTable->child("messageListView", "QListView");
	if (list == NULL) return;
	QListView *messageListView = (QListView*) list;
	messageListView->clear();
}



void Teddy::applySettings() {
	QSettings settings;
	settings.setPath("gravitybind.com", "teddy", QSettings::User);
	settings.beginGroup("/Teddy");
	
	int xpos = settings.readNumEntry( "/x", 0 );
	int ypos = settings.readNumEntry( "/y", 30 );
	int width = settings.readNumEntry( "/w", 650 );
	int height = settings.readNumEntry( "/h", 600 );
	resize(width, height);
	move(xpos, ypos);

	recentDocList.clear();
	QStringList list = settings.readListEntry("/recentdocs");
	QStringList::Iterator it = list.begin();
	while( it != list.end() ) {
		recentDocList.push_back(*it);
		++it;
	}
	settings.endGroup();
}


void Teddy::saveSettings() {
	QSettings settings;
	settings.setPath("gravitybind.com", "teddy", QSettings::User);
	settings.beginGroup("/Teddy");
	
	settings.writeEntry( "/x", x() );
	settings.writeEntry( "/y", y() );
	settings.writeEntry( "/w", width() );
	settings.writeEntry( "/h", height() );

	settings.writeEntry( "/recentdocs", recentDocList );
	settings.endGroup();
}


void Teddy::updateCellText() {
	int currRow = xtable->currentRow();
	int currCol = xtable->currentColumn();
	if (currRow < 0 || currRow >= xtable->numRows()) return;
	if (currCol < 0 || currCol >= xtable->numCols()) return;
	QString oldstr = xtable->text(currRow, currCol);
	QString newstr = cellEdit->text();
	if (oldstr.isNull()) oldstr = "";
	if (newstr.isNull()) newstr = "";
	if (oldstr != newstr) {
		xtable->addUndoRecord(currRow, currCol, currRow, currCol);
	}
	xtable->ensureCellVisible(currRow, currCol);
	xtable->setText(currRow, currCol, cellEdit->text());
	xtable->updateCell(currRow, currCol);
}


void Teddy::createFileMenu() {
	QPopupMenu * file = new QPopupMenu( this );
	menuBar()->insertItem( tr("&File"), file );
	file->insertItem( tr("&New"), this, SLOT(newDoc()), CTRL+Key_N );
	file->insertItem( tr("&Open..."), this, SLOT(choose()), CTRL+Key_O );
	
	recentMenu = new QPopupMenu(this, "recentDocMenu");
	connect( recentMenu, SIGNAL(aboutToShow()), this, SLOT(updateRecentDocMenu()) );
	connect( recentMenu, SIGNAL(activated(int)), this, SLOT(openRecentFile(int)) );
	file->insertItem( tr("Open Recent"), recentMenu );
	file->insertItem( tr("&Reload"), this, SLOT(reload()), CTRL+Key_L );
	
	file->insertSeparator();
	file->insertItem( tr("&Save"), this, SLOT(save()), CTRL+Key_S );
	file->insertItem( tr("Save &As..."), this, SLOT(saveAs()), CTRL+SHIFT+Key_S );
	file->insertItem( tr("&Export to HTML..."), this, SLOT(exportFile()), CTRL+Key_E );
	file->insertSeparator();
	file->insertItem( tr("&Print..."), this, SLOT(print()), CTRL+Key_P );
    file->insertSeparator();
    file->insertItem( tr("&Close"), this, SLOT(close()), CTRL+Key_W );
    file->insertItem( tr("&Quit"), qApp, SLOT( closeAllWindows() ), CTRL+Key_Q );
}


void Teddy::updateRecentDocList(const QString& newitem) {
	if (!newitem.isEmpty()) {
		recentDocList.remove(newitem);
		recentDocList.push_back(newitem);
		while (recentDocList.size() > (uint)Teddy::numRecentDocs) recentDocList.pop_front();
	}
}


void Teddy::updateRecentDocMenu() {
	if (recentMenu == NULL) return;
	QStringList::Iterator it = recentDocList.begin();
	for ( int i = 0; it != recentDocList.end() && i < Teddy::numRecentDocs; i++ ) {
		if (recentMenu->idAt(i) < 0) {
			recentMenu->insertItem(*it, i, i);
		}
		else {
			recentMenu->changeItem(i, *it);
		}
		recentMenu->setItemParameter(i, i);
		++it;
	}
}


void Teddy::openRecentFile(int id) {
	QString fn = recentDocList[id];
	load( recentDocList[id] );
}


void Teddy::createEditMenu() {
	editMenu = new QPopupMenu( this );
	menuBar()->insertItem( tr("&Edit"), editMenu );
	undoMenuId = editMenu->insertItem( tr("&Undo"), xtable, SLOT(undo()), CTRL+Key_Z );
	redoMenuId = editMenu->insertItem( tr("Re&do"), xtable, SLOT(redo()), CTRL+SHIFT+Key_Z );
	editMenu->insertSeparator();
	editMenu->insertItem( tr("Cu&t"), this, SLOT(cut()), CTRL+Key_X );
	editMenu->insertItem( tr("&Copy"), this, SLOT(copy()), CTRL+Key_C );
	editMenu->insertItem( tr("&Copy As Text"), this, SLOT(copyAsText()) );
	editMenu->insertItem( tr("&Copy As XML"), xtable, SLOT(copyXml()), SHIFT+CTRL+Key_C );
	editMenu->insertSeparator();
	editMenu->insertItem( tr("&Paste"), this, SLOT(paste()), CTRL+Key_V );
	editMenu->insertItem( tr("&Paste As XML"), xtable, SLOT(pasteXml()), SHIFT+CTRL+Key_V );
	editMenu->insertSeparator();
	editMenu->insertItem( tr("&Delete"), this, SLOT(del()), Key_Delete );
	editMenu->insertSeparator();
	editMenu->insertItem( tr("Select &All"), this, SLOT(selectAll()), CTRL+Key_A );
	editMenu->insertSeparator();
	editMenu->insertItem( tr("&Find"), this, SLOT(find()), CTRL+Key_F );
	editMenu->insertItem( tr("Find &Next"), this, SLOT(findNext()), CTRL+Key_G );
	editMenu->insertItem( tr("Find Pre&vious"), this, SLOT(findPrevious()), SHIFT+CTRL+Key_G );
	editMenu->insertItem( tr("&Replace"), this, SLOT(replace()), CTRL+Key_R );
	editMenu->insertSeparator();
	editMenu->insertItem( tr("Document Type"), this, SLOT(docType()));
	editMenu->insertSeparator();
	editMenu->insertItem( tr("Pr&eferences..."), this, SLOT(preferences()) );

	connect(xtable, SIGNAL(undoRecordSizeChanged(int)), this, SLOT(updateUndoMenu(int)));
	connect(xtable, SIGNAL(redoRecordSizeChanged(int)), this, SLOT(updateRedoMenu(int)));
	editMenu->setItemEnabled(undoMenuId, FALSE);
	editMenu->setItemEnabled(redoMenuId, FALSE);
	
	QKeySequence emptyKey;
	findToolBar = new QToolBar( this );
	findToolBar->setOrientation(Horizontal);
	findToolBar->setLabel( tr("Find") );
	findToolBar->setPaletteBackgroundColor( XmlTable::style(XmlTableItem::EMPTY)->bgColor() );
	findToolBar->setPaletteForegroundColor( XmlTable::style(XmlTableItem::EMPTY)->fgColor() );

	QLabel *findLabel = new QLabel( tr("Find:"), findToolBar, "findLabel" );
	findLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	findEdit = new QLineEdit(findToolBar, "findEdit");
	connect( findEdit, SIGNAL(returnPressed()), this, SLOT(findNext()) );

	QAction *findNextAction = new QAction( tr("Find Next"), emptyKey, this );
	findNextAction->addTo(findToolBar);
	connect( findNextAction, SIGNAL(activated()), this, SLOT(findNext()) );
	
	QAction *findPreviousAction = new QAction( tr("Find Previous"), emptyKey, this );
	findPreviousAction->addTo(findToolBar);
	connect( findPreviousAction, SIGNAL(activated()), this, SLOT(findPrevious()) );
	
	findToolBar->addSeparator();
	QAction *matchCaseAction = new QAction( tr("Match case"), emptyKey, this );
	matchCaseAction->setToggleAction(TRUE);
	matchCase = TRUE;
	matchCaseAction->setOn(matchCase);
	matchCaseAction->addTo(findToolBar);
	connect( matchCaseAction, SIGNAL(toggled(bool)), this, SLOT(setMatchCase(bool)) );


	replaceToolBar = new QToolBar( this );
	replaceToolBar->setOrientation(Horizontal);
	replaceToolBar->setLabel( tr("Replace") );
	replaceToolBar->setPaletteBackgroundColor( XmlTable::style(XmlTableItem::EMPTY)->bgColor() );
	replaceToolBar->setPaletteForegroundColor( XmlTable::style(XmlTableItem::EMPTY)->fgColor() );

	QLabel *replaceLabel = new QLabel( tr("Replace:"), replaceToolBar, "replaceLabel" );
	replaceLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	replaceEdit = new QLineEdit(replaceToolBar, "replaceEdit");
	connect( replaceEdit, SIGNAL(returnPressed()), this, SLOT(replaceNext()) );

	QFontMetrics metric(replaceLabel->font());
	int labelWidth = metric.width(tr(" Replace:"));
	findLabel->setMinimumWidth(labelWidth);
	replaceLabel->setMinimumWidth(labelWidth);
	
	QAction *replaceAction = new QAction( tr("Replace"), emptyKey, this );
	replaceAction->addTo(replaceToolBar);
	connect( replaceAction, SIGNAL(activated()), this, SLOT(replaceNext()) );
	QAction *replaceAllAction = new QAction( tr("Replace All"), emptyKey, this );
	replaceAllAction->addTo(replaceToolBar);
	connect( replaceAllAction, SIGNAL(activated()), this, SLOT(replaceAll()) );
	
	
	findToolBar->setCloseMode(QDockWindow::Always);
	replaceToolBar->setCloseMode(QDockWindow::Always);
	moveDockWindow(findToolBar, Qt::DockBottom);
	moveDockWindow(replaceToolBar, Qt::DockBottom);
	findToolBar->hide();
	replaceToolBar->hide();
	connect( findToolBar, SIGNAL(visibilityChanged(bool)), this, SLOT(hideReplaceToolBar(bool)) );
}


void Teddy::updateUndoMenu(int size) {
	editMenu->setItemEnabled(undoMenuId, size > 0);
}


void Teddy::updateRedoMenu(int size) {
	editMenu->setItemEnabled(redoMenuId, size > 0);
}


void Teddy::hideReplaceToolBar(bool vis) {
	if (!vis) replaceToolBar->hide();
}



void Teddy::applyStyleIcons() {
	applyStyleIcons(XmlTableItem::ELEMENT, tr("element"), actionElement);
	applyStyleIcons(XmlTableItem::ATTR_KEY, tr("key"), actionAttrKey);
	applyStyleIcons(XmlTableItem::ATTR_VAL, tr("value"), actionAttrVal);
	applyStyleIcons(XmlTableItem::TEXT, tr("text"), actionText);
	applyStyleIcons(XmlTableItem::CDATA, tr("cdata"), actionCDATA);
	applyStyleIcons(XmlTableItem::COMMENT, tr("comment"), actionComment);
	applyStyleIcons(XmlTableItem::EMPTY, tr("empty"), actionEmpty);
}


void Teddy::applyStyleIcons(int type, const QString &label, QAction *action) {
	QPixmap icon(20,20);
	icon.fill( XmlTable::style(type)->bgColor() );
	QPainter p(&icon);
	QFont f = XmlTable::style(type)->font();
	p.setFont( f );
	p.setPen( QPen( XmlTable::style(type)->fgColor(), 1) );
	p.drawRect(0,0,20,20);
	p.drawText(0, 0, 20, 20, Qt::AlignLeft | Qt::AlignVCenter, label, -1, 0);
	action->setIconSet(icon);
}


void Teddy::createFormatMenu() {
	setUsesTextLabel(TRUE);
	QToolBar *tb = new QToolBar( this );
	tb->setOrientation(Horizontal);
	tb->setLabel( tr("Format") );

	QPopupMenu * menu = new QPopupMenu( this );
	menuBar()->insertItem( tr("&Format"), menu );

	agrp = new QActionGroup(this, NULL, TRUE);
	connect(agrp, SIGNAL( selected(QAction*) ), this, SLOT( cellFormatSelected(QAction*) ) );
	ignoreToggle = FALSE;
	
	actionElement = new QAction ( tr("&Element"), CTRL+Key_1, agrp );
		actionElement->setText( tr("<element") );
		actionElement->setToggleAction(TRUE);
		actionElement->setWhatsThis( tr("Sets the format of selected cell(s) to the ELEMENT type") );
	actionAttrKey = new QAction ( tr("Attribute &Key"), CTRL+Key_2, agrp );
		actionAttrKey->setText( tr("key=") );
		actionAttrKey->setToggleAction(TRUE);
		actionAttrKey->setWhatsThis( tr("Sets the format of selected cell(s) to the ATTRIBUTE-KEY type") );
	actionAttrVal = new QAction ( tr("Attribute &Value"), CTRL+Key_3, agrp );
		actionAttrVal->setText( tr("\"value\">") );
		actionAttrVal->setToggleAction(TRUE);
		actionAttrVal->setWhatsThis( tr("Sets the format of selected cell(s) to the ATTRIBUTE-VALUE type") );
	actionText = new QAction ( tr("&Text"), CTRL+Key_4, agrp );
		actionText->setText( tr("text") );
		actionText->setToggleAction(TRUE);
		actionText->setWhatsThis( tr("Sets the format of selected cell(s) to the TEXT type") );
	actionCDATA = new QAction ( tr("C&DATA"), CTRL+Key_5, agrp );
		actionCDATA->setText( tr("<![CDATA[...]]>") );
		actionCDATA->setToggleAction(TRUE);
		actionCDATA->setWhatsThis( tr("Sets the format of selected cell(s) to the CDATA type") );
	actionComment = new QAction ( tr("C&omment"), CTRL+Key_6, agrp );
		actionComment->setText( tr("<!--comment-->") );
		actionComment->setToggleAction(TRUE);
		actionComment->setWhatsThis( tr("Sets the format of selected cell(s) to the COMMENT type") );
	actionEmpty = new QAction ( tr("E&mpty"), CTRL+Key_0, agrp );
		actionEmpty->setText( tr("______") );
		actionEmpty->setToggleAction(TRUE);
		actionEmpty->setWhatsThis( tr("Sets the format of selected cell(s) to the EMPTY type") );
	agrp->addTo(tb);
	agrp->addTo(menu);
	applyStyleIcons();

	menu->insertSeparator();
	menu->insertItem( tr("&Adjust Cell Sizes"), xtable, SLOT(adjustAll()), CTRL+Key_Return );
	menu->insertSeparator();
	menu->insertItem( tr("Shift &Left"), xtable, SLOT(shiftLeft()), CTRL+Key_Left );
	menu->insertItem( tr("Shift &Right"), xtable, SLOT(shiftRight()), CTRL+Key_Right );
	menu->insertSeparator();
	menu->insertItem( tr("Insert Row"), this, SLOT(insertRow()), CTRL+Key_Equal );
	menu->insertItem( tr("Remove Row"), this, SLOT(removeRow()), CTRL+Key_Minus );
	menu->insertSeparator();
	menu->insertItem( tr("Expand &Document"), xtable, SLOT(expandDoc()), CTRL+Key_D );
	menu->insertItem( tr("&Trim Document"), xtable, SLOT(trimDoc()), CTRL+Key_T );
}


void Teddy::cellFormatSelected(QAction* act) {
	if (ignoreToggle) return;
	XmlTableItem::NodeType newType;
	if (act == actionElement) newType = XmlTableItem::ELEMENT;
	else if (act == actionAttrKey) newType = XmlTableItem::ATTR_KEY;
	else if (act == actionAttrVal) newType = XmlTableItem::ATTR_VAL;
	else if (act == actionText) newType = XmlTableItem::TEXT;
	else if (act == actionCDATA) newType = XmlTableItem::CDATA;
	else if (act == actionComment) newType = XmlTableItem::COMMENT;
	else if (act == actionEmpty) newType = XmlTableItem::EMPTY;
	else return;

	int minX = xtable->numCols(), minY = xtable->numRows(), maxX = 0, maxY = 0;
	int numSel = xtable->numSelections();
	for (int k = 0; k < numSel; k++) {
		QTableSelection sel = xtable->selection(k);
		if (sel.isEmpty()) continue;
		for (int i = sel.topRow(); i <= sel.bottomRow(); i++) {
			for (int j = sel.leftCol(); j <= sel.rightCol(); j++) {
                //minX = minX <? j;
				minX = (minX < j) ? minX : j;
				//minY = minY <? i;
				minY = (minY < i) ? minY : i;
				//maxX = maxX >? j;
				maxX = (maxX > j) ? maxX : j;
				//maxY = maxY >? i;
				maxY = (maxY > i) ? maxY : i;			}
		}
	}
	xtable->addUndoRecord(minY, minX, maxY, maxX);
	XmlTableItem* currItem;
	for (int k = 0; k < numSel; k++) {
		QTableSelection sel = xtable->selection(k);
		if (sel.isEmpty()) continue;
		for (int i = sel.topRow(); i <= sel.bottomRow(); i++) {
			for (int j = sel.leftCol(); j <= sel.rightCol(); j++) {
				currItem = xtable->xmlItem(i, j);
				if (currItem == NULL) {
					currItem = new XmlTableItem(newType, xtable, QTableItem::OnTyping, "");
					xtable->setItem(i, j, currItem);
				}
				else {
					currItem->setItemType(newType);
					xtable->updateCell(i, j);
					if (currItem->isSpanCell()) j = sel.rightCol() + 1;
				}
			}
		}
	}
	xtable->setModified(TRUE);
}



void Teddy::createHelpMenu() {
	menuBar()->insertSeparator();
	QPopupMenu * help = new QPopupMenu( this );
	menuBar()->insertItem( tr("&Help"), help );
	help->insertItem( tr("What's &This"), this, SLOT(whatsThis()), SHIFT+Key_F1 );
	help->insertItem( tr("Online Help..."), this, SLOT(onlineHelp()) );
	help->insertSeparator();
	help->insertItem( tr("&About Teddy"), this, SLOT(about()), Key_F1 );
	help->insertItem( tr("About &Qt"), this, SLOT(aboutQt()) );
}


void Teddy::createStatusBar() {
	rowLabel = new QLabel( tr("Row: 999"), this );
	rowLabel->setAlignment(AlignHCenter);
	rowLabel->setMinimumSize(rowLabel->sizeHint());
	rowLabel->setText( tr("Row: 1") );

	colLabel = new QLabel( tr("Col: 999"), this );
	colLabel->setAlignment(AlignHCenter);
	colLabel->setMinimumSize(colLabel->sizeHint());
	colLabel->setText( tr("Col: 1") );

	typeLabel = new QLabel(" 9999999 ", this);
	typeLabel->setFont( XmlTable::style(XmlTableItem::ELEMENT)->font() );
	typeLabel->setMinimumSize(typeLabel->sizeHint());
	typeLabel->setAlignment(AlignHCenter);
	typeLabel->clear();
	
	statusLabel = new QLabel(this);

	modLabel = new QLabel(tr("MOD"), this);
	modLabel->setAlignment(AlignHCenter);
	modLabel->setMinimumSize(modLabel->sizeHint());
	modLabel->clear();

	statusBar()->addWidget(rowLabel, 0, TRUE);
	statusBar()->addWidget(colLabel, 0, TRUE);
	statusBar()->addWidget(typeLabel, 0, TRUE);
	statusBar()->addWidget(statusLabel, 1, TRUE);
	statusBar()->addWidget(modLabel, 0, TRUE);

	connect( xtable, SIGNAL( currentChanged(int, int) ), this, SLOT( updateCellIndicators(int, int) ) );
	connect( xtable, SIGNAL( modificationChanged(bool) ), this, SLOT( tableModified(bool) ) );
	connect(
		statusBar(), SIGNAL( messageChanged(const QString &) ),
		statusLabel, SLOT( setText(const QString &) )
	);
}


void Teddy::updateCellIndicators(int row, int col) {
	rowLabel->setText( tr("Row: %1").arg(row+1) );
	colLabel->setText( tr("Col: %2").arg(col+1) );
	cellEdit->setText( xtable->text(row, col) );
	XmlTableItem::NodeType currType = xtable->itemType(row, col);
	typeLabel->setText( XmlTable::style(currType)->name() );
	typeLabel->setFont( XmlTable::style(currType)->font() );
	typeLabel->setPaletteForegroundColor( XmlTable::style(currType)->fgColor() );
	typeLabel->setPaletteBackgroundColor( XmlTable::style(currType)->bgColor() );
	
	ignoreToggle = TRUE;
	switch (currType) {
		case XmlTableItem::ELEMENT: actionElement->setOn(TRUE); break;
		case XmlTableItem::ATTR_KEY: actionAttrKey->setOn(TRUE); break;
		case XmlTableItem::ATTR_VAL: actionAttrVal->setOn(TRUE); break;
		case XmlTableItem::TEXT: actionText->setOn(TRUE); break;
		case XmlTableItem::CDATA: actionCDATA->setOn(TRUE); break;
		case XmlTableItem::COMMENT: actionComment->setOn(TRUE); break;
		case XmlTableItem::EMPTY: actionEmpty->setOn(TRUE); break;
		default: break;
	}
	ignoreToggle = FALSE;
	xtable->updateAll();
}


void Teddy::tableModified(bool ismod) {
	if (ismod) modLabel->setText(tr("MOD"));
	else modLabel->clear();
}


void Teddy::setMatchCase(bool mc) {
	matchCase = mc;
}


void Teddy::newDoc() {
	Teddy *ed = new Teddy;
	ed->setCaption("Teddy");
	ed->show();
}


void Teddy::choose() {
	QString fn;
	QSettings settings;
	settings.setPath("gravitybind.com", "teddy", QSettings::User);
	settings.beginGroup("/Teddy");
	QString lastDir = settings.readEntry( "/dir", QString::null );
	settings.endGroup();
	QFileDialog fd( lastDir, QString::null, this, "Open File", TRUE );
	if ( fd.exec() == QDialog::Accepted ) {
		fn = fd.selectedFile();
		const QDir *dir = fd.dir();
		settings.beginGroup("/Teddy");
		settings.writeEntry( "/dir", dir->path() );
		settings.endGroup();
		delete dir;
	}
	
	if ( !fn.isEmpty() ) load( fn );
	else message( tr("Loading aborted") );
}


void Teddy::setActiveWindow() {
	QWidget::setActiveWindow();
}


void Teddy::load( const QString &fn, bool isReload ) {
	if (!isReload) {
		QWidgetList *openAppList = QApplication::topLevelWidgets();
		if (openAppList != NULL) { 
			QWidgetListIt it( *openAppList );  // iterate over the widgets
			QWidget * w;
			while ( (w=it.current()) != 0 ) {   // for each top level widget...
				++it;
				if ( w->name() == fn ) {
					w->raise();
					w->setActiveWindow();
					return;
				}
			}
			delete openAppList;
		}
	}
	
	if (! this->filename.isEmpty()) {
		closeEvent(NULL);
	}
	
	clearMessages();
	QFile f( fn );
	if ( !f.open( IO_ReadOnly ) ) {
		message( tr("Unable to open file: %1").arg(fn), "", TRUE, TRUE, QMessageBox::Critical );
		return;
	}

	setCaption(fn + " - Teddy");
	this->filename = fn;
	setName(fn);
	if ( xtable->setContent(&f) ) {
		message( tr("Loaded document %1").arg(fn) );
	}
	updateRecentDocList(fn);
}


void Teddy::reload() {
	load( this->filename, TRUE );
}



void Teddy::save() {
	clearMessages();
	xtable->setCurrentCell(0, 0);
	if ( filename.isEmpty() ) {
		saveAs();
		return;
	}

	QFile f( filename );
	if ( !f.open(IO_WriteOnly) ) {
		message( tr("Could not write to file: %1").arg(filename), "", TRUE, TRUE, QMessageBox::Critical );
		return;
	}

	QTextStream t( &f );
	/*if (XmlTable::outputEncoding == "UTF-16") {
		t.setEncoding(QTextStream::Unicode);
	}
	else if (XmlTable::outputEncoding == "UTF-8") {
		t.setEncoding(QTextStream::UnicodeUTF8);
	}
	else {
		QTextCodec *codec = QTextCodec::codecForName(XmlTable::outputEncoding);
		if (codec != NULL) {
			t.setEncoding(QTextStream::Locale);
			t.setCodec(codec);
		}
		else {
			XmlTable::outputEncoding = "UTF-16";
			t.setEncoding(QTextStream::Unicode);
		}
	}*/
	xtable->save(t, dtdEdit->text());
	f.close();

	setCaption( filename + " - Teddy");
	message( tr( "File %1 saved" ).arg( filename ) );
}


void Teddy::saveAs() {
	QString fn = QFileDialog::getSaveFileName( QString::null, QString::null, this );
	if ( fn.isEmpty() ) {
		message( tr("Saving aborted") );
		return;
	}
	filename = fn;
	save();
}


void Teddy::exportFile() {
	clearMessages();
	QString fn = QFileDialog::getSaveFileName( QString::null, "HTML (*.html *.htm)", this );
	if ( fn.isEmpty() ) {
		message( tr("Export aborted") );
		return;
	}
	if (!( fn.endsWith(".html", FALSE) || fn.endsWith(".htm", FALSE) )) fn += ".html";
	QFile f( fn );
	if ( !f.open(IO_WriteOnly) ) {
		message( tr("Could not write to file: %1").arg(filename), "", TRUE, TRUE, QMessageBox::Critical );
		return;
	}

	QTextStream t( &f );
	t << xtable->html_css(0, 0, xtable->maxRows(), xtable->maxCols());
	f.close();

	message( tr( "File %1 saved" ).arg( fn ) );
}


void Teddy::print() {
	printRichText(xtable->html2( 0, 0, xtable->maxRows(), xtable->maxCols()));
}


void Teddy::printRichText(const QString &str) {
	if (!printer->setup(this)) return;
	QPainter painter(printer);
	int pageHeight = painter.window().height() - 2 * 48;
	QSimpleRichText richText(str, bodyFont, "", 0, 0, pageHeight);
	richText.setWidth(&painter, painter.window().width());
	int numPages = (int)ceil( (double)richText.height() / pageHeight );
	int index;
	for (int i = 0; i < (int)printer->numCopies(); i++) {
		for (int j = 0; j < numPages; j++) {
			if (i > 0 || j > 0) printer->newPage();
			if (printer->pageOrder() == QPrinter::LastPageFirst) {
				index = numPages - j - 1;
			}
			else index = j;
			printPage(&painter, richText, pageHeight, index);
		}
	}
}


void Teddy::printPage(QPainter *painter, const QSimpleRichText &richText, int pageHeight, int index) {
	QRect rect(0, index * pageHeight + 48, richText.width(), pageHeight);
	painter->saveWorldMatrix();
	painter->translate(0, -rect.y());
	richText.draw(painter, 0, 48, rect, colorGroup());
	painter->restoreWorldMatrix();
	painter->setFont(footerFont);
	painter->drawText( painter->window(), AlignHCenter | AlignBottom, QString::number(index+1) );
}


void Teddy::closeEvent( QCloseEvent* ce ) {
    if ( !xtable->isModified() ) {
		saveSettings();
		if (ce != NULL) ce->accept();
	}
	else {
		int ask = QMessageBox::information( this, "Teddy",
			tr("Do you want to save the changes to the document?"),
			tr("Yes"), tr("No"), tr("Cancel"), 0, 1
		);
		switch(ask) {
		    case 0:
				save();
				if (ce != NULL) ce->accept();
				break;
			case 1:
				if (ce != NULL) ce->accept();
				break;
			case 2:
			default:
				if (ce != NULL) ce->ignore();
				break;
		}
	}
}


void Teddy::cut() {
	copy();
	del();
}


void Teddy::copy() {
	if (xtable->hasFocus()) xtable->copy(FALSE);
	else if (cellEdit->hasFocus()) cellEdit->copy();
}


void Teddy::copyAsText() {
	if (xtable->hasFocus()) xtable->copy(TRUE);
	else if (cellEdit->hasFocus()) cellEdit->copy();
}


void Teddy::paste() {
	if (xtable->hasFocus()) xtable->paste();
	else if (cellEdit->hasFocus()) cellEdit->paste();
}


void Teddy::del() {
	if (xtable->hasFocus()) xtable->del();
	else if (cellEdit->hasFocus()) cellEdit->del();
}


void Teddy::selectAll() {
	if (xtable->hasFocus()) xtable->selectCells( 0, 0, xtable->maxRows() /*xtable->numRows()*/, xtable->maxCols() /*xtable->numCols()*/ );
	else if (cellEdit->hasFocus()) cellEdit->selectAll(TRUE);
}


void Teddy::find() {
	findToolBar->show();
	findEdit->setFocus();
	findEdit->selectAll();
}


void Teddy::docType() {
	bottomTabs->showPage(dtdEdit);
	dtdEdit->setFocus();
}
	
void Teddy::setTabEditCell() {
	bottomTabs->showPage(cellEdit);
}


void Teddy::findNext() {
	QString val = findEdit->text();
	if (val.isEmpty()) return;
	xtable->clearSelection(TRUE);
	int r = xtable->currentRow(), c = xtable->currentColumn();
	if ( (r >= xtable->maxRows()) && (c >= xtable->maxCols()) ) {
		r = 0; c = 0;
	}
	QTableItem* currItem;
	for (int i = 0; r <= xtable->maxRows(); r++, i++) {
		if (i > 0) c = 0;
		for (; c <= xtable->maxCols(); c++) {
			currItem = xtable->item(r, c);
			if (currItem == NULL) continue;
			if (currItem->row() != r) continue;
			if (currItem->col() != c) continue;
			if (xtable->text(r, c).find(val, 0, matchCase) < 0) continue;
			xtable->setCurrentCell(r, c);
			if (! cellEdit->find(val, matchCase, FALSE, TRUE) ) continue;
			cellEdit->setFocus();
			return;
		}
	}
	xtable->setCurrentCell(0, -1);
	xtable->clearSelection(TRUE);
	message( tr("End of document reached") );
}


void Teddy::findPrevious() {
	QString val = findEdit->text();
	if (val.isEmpty()) return;
	xtable->clearSelection(TRUE);
	int r = xtable->currentRow(), c = xtable->currentColumn();
	if ( (r <= 0) && (c <= 0) ) {
		r = xtable->maxRows();
		c = xtable->maxCols();
	}
	QTableItem* currItem;
	for (int i = 0; r >= 0; r--, i++) {
		if (i > 0) c = xtable->maxCols();
		for (; c >= 0; c--) {
			currItem = xtable->item(r, c);
			if (currItem == NULL) continue;
			if (currItem->row() != r) continue;
			if (currItem->col() != c) continue;
			if (xtable->text(r, c).find(val, 0, matchCase) < 0) continue;
			xtable->setCurrentCell(r, c);
			if (! cellEdit->find(val, matchCase, FALSE, FALSE) ) continue;
			cellEdit->setFocus();
			return;
		}
	}
	xtable->setCurrentCell(0, -1);
	xtable->clearSelection(TRUE);
	message( tr("End of document reached") );
}


void Teddy::replace() {
	find();
	replaceToolBar->show();
}


void Teddy::replaceNext() {
	int r = xtable->currentRow(), c = xtable->currentColumn();
	QString currVal = xtable->text(r, c);
	if (cellEdit->hasSelectedText()) {
		cellEdit->insert(replaceEdit->text());
		xtable->updateCell(r, c);
	}
	findNext();
	xtable->setModified(TRUE);
}


void Teddy::replaceAll() {
	QTableItem *currItem;
	xtable->clearSelection(TRUE);
	QString val = findEdit->text();
	int numCellsReplaced = 0;
	xtable->addUndoRecord(0, 0, xtable->maxRows(), xtable->maxCols());
	for (int r = 0; r <= xtable->maxRows(); r++) {
		for (int c = 0; c <= xtable->maxCols(); c++) {
			currItem = xtable->item(r, c);
			if (currItem == NULL) continue;
			if (currItem->row() != r) continue;
			if (currItem->col() != c) continue;
			QString currVal = xtable->text(r, c);
			if (currVal.find(val, 0, matchCase) < 0) continue;
			currVal.replace( findEdit->text(), replaceEdit->text(), matchCase );
			xtable->setText(r, c, currVal);
			xtable->updateCell(r, c);
			numCellsReplaced++;
		}
	}
	if (numCellsReplaced == 0) return;
	xtable->setModified(TRUE);
	QMessageBox::information( this, tr("Replace All"),
		tr("Text in %1 cell(s) replaced").arg(numCellsReplaced) 
	);
}


void Teddy::preferences() {
	pref->show();
	pref->raise();
}


void Teddy::insertRow() {
	int r = xtable->currentRow(), c = xtable->currentColumn();
	xtable->insertRows( r, 1 );
	xtable->setCurrentCell(r, c);
	xtable->resetUndo();
	xtable->resetRedo();
}


void Teddy::removeRow() {
	int r = xtable->currentRow(), c = xtable->currentColumn();
	xtable->removeRow( xtable->currentRow() );
	xtable->setCurrentCell(r, c);
	xtable->resetUndo();
	xtable->resetRedo();
}


void Teddy::onlineHelp() {
#if defined(Q_WS_WIN)
	QString launch("C:\\Program Files\\Internet Explorer\\iexplore.exe");
#endif
#if defined(Q_WS_MAC)
	QString launch("/usr/bin/open"); 
#endif
#if defined(Q_WS_X11)
	QString launch(getenv("DESKTOP_LAUNCH")); 
#endif
#if defined(Q_WS_QWS)
	QString launch(getenv("DESKTOP_LAUNCH")); 
#endif
	QProcess proc(launch);
	proc.addArgument("http://www.gravitybind.com/help.html");
	proc.launch("");
}


void Teddy::about() {
	QMessageBox::about( this, tr("About")+" Teddy", tr(
		"<b>Teddy</b>   <small>version 1.0</small>"
		"<br><nobr>A <b>t</b>abular <b>ed</b>itor and <b>d</b>ispla<b>y</b> for XML.</nobr>"
		"<br>"
		"<br>See <tt>http://www.gravitybind.com</tt> for more information."
		"<br>"
		"<br><nobr>Written by: Jinsoo Kang (<tt>jsk@gravitybind.com</tt>)</nobr>"
		"<br>"
		"<br>"
		"<br><small>Copyright (C) 2005 by Jinsoo Kang</small>"
	));
}


void Teddy::aboutQt() {
	QMessageBox::aboutQt( this, tr("About Qt") );
}

