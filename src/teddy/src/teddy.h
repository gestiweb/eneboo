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

#ifndef TEDDY_H
#define TEDDY_H

#include <qmainwindow.h>
#include <qtable.h>
#include <qlabel.h>
#include <qaction.h>
#include <qbutton.h>
#include <qbuttongroup.h>
#include <qapplication.h>
#include <qclipboard.h>
#include <qcheckbox.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qdockwindow.h>
#include <qsimplerichtext.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qpen.h>
#include <qtabwidget.h>
#include <qlistview.h>
#include <qwidgetfactory.h>
#include <qmessagebox.h>

#include <qimage.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>

#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qtextedit.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qprinter.h>
#include <qapplication.h>
#include <qaccel.h>
#include <qtextstream.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qwhatsthis.h>
#include <qtable.h>
#include <qsplitter.h>
#include <qapplication.h>
#include <qwidgetlist.h>
#include <qtextcodec.h>

#include "xmltable.h"
#include "prefdialog.h"
#include "messagelist.h"


class Teddy: public QMainWindow {
	Q_OBJECT

public:
	Teddy();
	~Teddy();

	void setActiveWindow();

protected:
	void closeEvent( QCloseEvent* );


public slots:
	
	void updateCellIndicators(int row, int col);
	void tableModified(bool ismod);
	void load( const QString &fileName, bool isReload = FALSE );
	void message(
		const QString &msg, const QString &loc = "", 
		bool printStatusBar = TRUE, bool printLog = FALSE,
		QMessageBox::Icon icn = QMessageBox::Information
	);
	void clearMessages();

	
private slots:
	void updateCellText();
	void newDoc();
	void choose();
	void save();
	void saveAs();
	void reload();
	void print();

	void cellFormatSelected(QAction* act);

	void exportFile();
	void cut();
	void copy();
	void copyAsText();

	void paste();
	void del();
	void selectAll();
	void find();
	void findNext();
	void findPrevious();
	void replace();
	void replaceNext();
	void replaceAll();
	void docType();
	void setTabEditCell();
	void setMatchCase(bool mc);
	void hideReplaceToolBar(bool vis);

	void preferences();

	void insertRow();
	void removeRow();
	
	void onlineHelp();
	void about();
	void aboutQt();
	
	void applyStyleIcons();
	void applyStyleIcons(int type, const QString &label, QAction *action);
	void openMessageTarget(QListViewItem *item);

	void openRecentFile(int id);

	void updateRecentDocMenu();
	void updateRecentDocList(const QString& newitem = QString::null );
	
	void updateUndoMenu(int size);
	void updateRedoMenu(int size);

private:
	void applySettings();
	void saveSettings();

	void createFileMenu();
	void createEditMenu();
	void createFormatMenu();
	void createHelpMenu();

	
	void createStatusBar();
	void printRichText(const QString &str);
	void printPage(QPainter *painter, const QSimpleRichText &richText, int pageHeight, int index);


	QActionGroup *agrp;
	QAction *actionElement,
		*actionAttrKey,
		*actionAttrVal,
		*actionText,
		*actionCDATA,
		*actionComment,
		*actionEmpty;

	
	QPrinter *printer;
	QString filename;
	
	XmlTable *xtable;
	QTextEdit *cellEdit;
	QTextEdit *dtdEdit;
	QWidget *errorTable;

	PrefDialog *pref;

	//find / replace
	QToolBar *findToolBar, *replaceToolBar;
	QLineEdit *findEdit, *replaceEdit;
	bool matchCase;
	
	//status bar
	QLabel *rowLabel;
	QLabel *colLabel;
	QLabel *typeLabel;
	QLabel *statusLabel;
	QLabel *modLabel;

	QTabWidget *bottomTabs;
	
	bool ignoreToggle;

	QFont bodyFont, footerFont;
	
	
	static QIntDict<QPixmap> messageIcons;

	QPopupMenu *recentMenu;
	static QStringList recentDocList;

	QPopupMenu *editMenu;
	int undoMenuId, redoMenuId;
	
public:
	static QString externalEditor;
	static bool externalEditorActive;
	static int numRecentDocs;
	
};


#endif
