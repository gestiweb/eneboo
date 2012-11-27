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

#ifndef XMLTABLE_H
#define XMLTABLE_H


#include <qtable.h>
#include <qdom.h>
#include <qobject.h>
#include <qpainter.h>
#include <qfont.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qvaluevector.h>
#include <qsettings.h>
#include <qcolor.h>
#include <qapplication.h>
#include <qclipboard.h>
#include <qfontdialog.h>
#include <qmessagebox.h>
#include <qstring.h>
#include <qprinter.h>
#include <qevent.h>
#include <qpoint.h>
#include <qfile.h>
#include <qdir.h>


#include "xmltablestyle.h"


class XmlTableItem : public QTableItem {

public:
	enum NodeType {EMPTY=0, ELEMENT, ATTR_KEY, ATTR_VAL, TEXT, CDATA, COMMENT};

	XmlTableItem(NodeType type, QTable *t, EditType et, const QString &txt );
	int alignment() const;

	QSize sizeHint() const;
	void setSizeHint(bool isInit = FALSE);


	QWidget *createEditor() const;
	void setContentFromEditor( QWidget *w );


	bool isSpanCell() const;
	int spanWidth() const;
	NodeType itemType() const;
	void setItemType(NodeType type);

	void setDepth(int d);
	int depth();
	void decreaseDepth();
	void increaseDepth();

	void setExpanded(bool isExpanded);
	bool expanded();

	void setCommentDepth(int d) { commentDepth_ = d; }
	int commentDepth();

private:
	int nextDepth();
	int previousDepth();
	int countWrapedLines(const QString & str, int maxWidth, const QFontMetrics & metric, int wordWrap) const;

	NodeType type;

	QSize size;

	int hierDepth;
	int commentDepth_;
	bool isExpanded;
};



class XmlTable : public QTable {
	Q_OBJECT

public:
	XmlTable(int r, int c, QWidget * parent = 0);
	~XmlTable();

	void setItem( int r, int c, QTableItem *i );//save undo
	void setText( int r, int c, const QString & text );
	bool setContent(QIODevice *dev);

	XmlTableItem::NodeType itemType(int r, int c);
	XmlTableItem* xmlItem(int r, int c);

	int maxRows();
	int maxCols();
	virtual void setNumRows( int r );
	void insertRows( int r, int n );

	int depth(int row);
	int nextDepth(int row, int* loc = NULL);
	int previousDepth(int row, int* loc = NULL);
	bool isParent(int row);
	bool isFirstSibling(int row);
	bool isLastSibling(int row);
	int hasSibling(int row, int selDepth = -1);
	int previousRowWithDepth(int startRow, int depth);

	bool isModified() const;
	void save(QTextStream & str, const QString &docType);
	QString html(int topRow, int leftCol, int bottomRow, int rightCol);
	QString html2(int topRow, int leftCol, int bottomRow, int rightCol);
	QString html_css(int topRow, int leftCol, int bottomRow, int rightCol);


	static XmlTableStyle* style(int type);
	static void applySettings();
	static void defaultSettings();
	static void saveSettings();

	void paintFocus( QPainter * p, const QRect & cr );
	void paintCell(QPainter * p, int row, int col, const QRect & cr, bool selected, const QColorGroup & cg);
	bool inExpandBox(int row, int col, const QPoint &p);

protected:
	int indexOf( int row, int col ) const;
	void setCellContentFromEditor( int row, int col );


public slots:
	void setModified(bool val);
	void touch(bool val);

	void adjustColumn( int col );
	void adjustAll();
	void copy(bool isTextOnly);
	void copyXml();
	void paste();
	void pasteXml();
	void del();
	void shiftLeft();
	void shiftRight();
	void expandDoc();
	void trimDoc();
	void updateAll();
	void updateColumn(int col);
	void updateRow(int row);
	void relayMousePress( int row, int col, int button, const QPoint & mousePos );
	void undo();
	void redo();

	void resetUndo();
	void resetRedo();
	void addUndoRecord(int topRow, int leftCol, int bottomRow, int rightCol, bool fromRedo = FALSE);
	void addRedoRecord(int topRow, int leftCol, int bottomRow, int rightCol);


signals:
	void modificationChanged(bool val);
	void dtdLoaded( const QString & txt );
	void message(
		const QString &msg, const QString &loc,
		bool printStatusBar, bool printLog,
		QMessageBox::Icon icn
	);
	void undoRecordSizeChanged(int size);
	void redoRecordSizeChanged(int size);


private:
	void layoutNode(int &row, int level, QDomNode *node, QStringList **lastAttrList, int *lastLevel);
	void layoutData(int &row, int level, QDomElement *elem, QStringList **lastAttrList, int *lastLevel);
	void layoutText(int &row, int level, QDomNode *node, int eLevel = 0);
	bool isNewNodeType(QStringList *newAttrs, QStringList *oldAttrs);


	QString getKey(int r, int c);
	int getElementRange(int row, int col, int lenRow);
	QString encodeToXml(const QString &str);
	QString encodeToHtml(const QString &str, int cellWidth = 0, XmlTableItem::NodeType type = XmlTableItem::EMPTY);
	bool printDocument(QTextStream & out, int startRow, int startCol, int lenRow, int lenCol);
	void printElement(QTextStream & out, int startRow, int startCol, int lenRow, int lenCol);
	void printAttr(QTextStream & out, int startRow, int startCol, int lenCol);
	void printIndent(QTextStream &out, int len);

	XmlTableItem::NodeType itemTypeForString(const QString & str);


	QString fixXmlNameString(const QString &val, bool* wasChanged);


	bool noDiff(QFile *f1, QFile *f2);

	QString xmlVersion;
	QString xmlEncoding;
	QString indent;

	int max_rows;
	int max_cols;
	bool isMod;


	//XML text
	bool isLetter(const QChar &c);
	bool isDigit(const QChar &c);
	bool isCombiningChar(const QChar &c);
	bool isExtender(const QChar &c);
	bool isBaseChar(const QChar &c);
	bool isIdeographic(const QChar &c);
	bool inRange(const QChar &c, int lower, int upper = -1);


	int tmpFileIndex;
	int undoIndex;
	int redoIndex;

	int editingRow, editingColumn;

private slots:
	void endEditAtLastLocation();


private:
	static QIntDict<XmlTableStyle> styleVector;
	static QString tempFilePath(QString type, int i, int index);
	static QString newTempFilePath(QString type, int i, int &index);

public:
	void setEditingLocation(int r = -1, int c = -1);

	static int minColumnWidth;
	static int indentWidth;
	static QColor borderColor;
	static QColor emptyBorderColor;
	static QColor focusBorderColor;
	static bool displayBranches;
	static bool sortAttr;
	static QString outputEncoding;
};




#endif
