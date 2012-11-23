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

#include "xmltable.h"

#include <iostream>
using namespace std;


QIntDict<XmlTableStyle> XmlTable::styleVector = QIntDict<XmlTableStyle>();
int XmlTable::minColumnWidth = 50;
int XmlTable::indentWidth = 30;
QColor XmlTable::borderColor(0,0,0);
QColor XmlTable::emptyBorderColor("gray");
QColor XmlTable::focusBorderColor(0,0,0);
bool XmlTable::displayBranches = true;
bool XmlTable::sortAttr = true;
QString XmlTable::outputEncoding;


XmlTable::XmlTable( int r, int c, QWidget * parent) : QTable( r, c, parent) {
	max_cols = 0;
	max_rows = 0;
	applySettings();
	indent = "\t";
	xmlVersion = "1.0";
	setModified(FALSE);
	setCurrentCell(0, 0);
	setFocusStyle(QTable::FollowStyle);
	setPaletteBackgroundColor( style(XmlTableItem::EMPTY)->bgColor() );
	setPaletteForegroundColor( style(XmlTableItem::EMPTY)->fgColor() );

	setLeftMargin( fontMetrics().width("W") * QString::number(r).length() );

	connect(
		this, SIGNAL(pressed( int, int, int, const QPoint &)),
		this, SLOT(relayMousePress( int, int, int, const QPoint &))
	);
	connect(
		this, SIGNAL(currentChanged( int, int )),
		this, SLOT(endEditAtLastLocation())
	);

	tmpFileIndex = -1;
	resetUndo();
	resetRedo();
	QString undoFile;
	do {
		tmpFileIndex++;
		undoFile = tempFilePath("undo", tmpFileIndex, 0);
	} while (QFile::exists(undoFile));
	QFile firstFile(undoFile);
	if (!firstFile.open(IO_WriteOnly)) return;
	QTextStream stream( &firstFile );
	stream << endl;
	firstFile.flush();
	firstFile.close();
}


XmlTable::~XmlTable() {
	QDir tdir = QDir::home();
	tdir.cd(".teddy");
	QStringList ls = tdir.entryList( QString("??do.%1.*").arg(tmpFileIndex), QDir::Files );
	for ( QStringList::Iterator it = ls.begin(); it != ls.end(); ++it ) {
		QFile::remove(tdir.absPath() + tdir.separator() + (*it));
	}
}


void XmlTable::endEditAtLastLocation() {
	if (editingRow < 0 || editingColumn < 0) return;
	endEdit(editingRow, editingColumn, TRUE, FALSE);
	setEditingLocation();
}


void XmlTable::resetUndo() {
	undoIndex = 0;
	emit undoRecordSizeChanged(undoIndex);
}


void XmlTable::resetRedo() {
	redoIndex = 0;
	emit redoRecordSizeChanged(redoIndex);
}


QString XmlTable::tempFilePath(QString type, int i, int index) {
	QDir homeDir = QDir::home();
	homeDir.mkdir(".teddy");
	QString tmpPath = QString(
		homeDir.absPath() + homeDir.separator()
		+ ".teddy" + homeDir.separator()
		+ "%1.%2.%3"
	).arg(type).arg(i).arg(index);
	return tmpPath;
}


QString XmlTable::newTempFilePath(QString type, int i, int &index) {
	QDir homeDir = QDir::home();
	homeDir.mkdir(".teddy");
	QString tmpPath;
	do {
		index++;
		tmpPath = QString(
			homeDir.absPath() + homeDir.separator()
			+ ".teddy" + homeDir.separator()
			+ "%1.%2.%3"
		).arg(type).arg(i).arg(index);
	} while ( QFile::exists(tmpPath) );
	return tmpPath;
}


void XmlTable::addUndoRecord(int topRow, int leftCol, int bottomRow, int rightCol, bool fromRedo) {
	QFile undoFile(tempFilePath("undo", tmpFileIndex, ++undoIndex));
	if (!undoFile.open(IO_WriteOnly)) return;
	if (!fromRedo) resetRedo();
	QTextStream stream( &undoFile );
	stream << endl
		<< QString("<table rA='%1' cA='%2' rZ='%3' cZ='%4'>").arg(topRow).arg(leftCol).arg(bottomRow).arg(rightCol)
		<< endl;
	for (int r=topRow; r <= bottomRow; r++) {
		stream << "<tr>" << endl;
		for (int c = leftCol; c <= rightCol; c++) {
			XmlTableItem *currItem = xmlItem(r, c);
			if (currItem != NULL) {
				stream << QString("<td class='%1' depth='%2'>")
					.arg( style(currItem->itemType())->name() )
					.arg( currItem->depth()-1 );
			}
			else stream << "<td>";
			stream << encodeToXml(text(r, c));
			stream << "</td>" << endl;
			if (currItem != NULL && currItem->isSpanCell()) {
				c++;
				while (c <= rightCol) {
					stream << "<td class='empty' depth='0'/>" << endl;
					c++;
				}
			}
		}
		stream << "</tr>" << endl;
	}
	stream << QString("</table>") << endl;
	undoFile.flush();
	undoFile.close();

	if (undoIndex > 1) {
		QFile newUndoFile(tempFilePath("undo", tmpFileIndex, undoIndex));
		QFile oldUndoFile(tempFilePath("undo", tmpFileIndex, undoIndex-1));
		if (noDiff( &newUndoFile, &oldUndoFile )) undoIndex--;
		newUndoFile.close();
		oldUndoFile.close();
	}
	emit undoRecordSizeChanged(undoIndex);
}


void XmlTable::addRedoRecord(int topRow, int leftCol, int bottomRow, int rightCol) {
	QFile redoFile(tempFilePath("redo", tmpFileIndex, ++redoIndex));
	if (!redoFile.open(IO_WriteOnly)) return;
	QTextStream stream( &redoFile );
	stream << endl
		<< QString("<table rA='%1' cA='%2' rZ='%3' cZ='%4'>").arg(topRow).arg(leftCol).arg(bottomRow).arg(rightCol)
		<< endl;
	for (int r=topRow; r <= bottomRow; r++) {
		stream << "<tr>" << endl;
		for (int c = leftCol; c <= rightCol; c++) {
			XmlTableItem *currItem = xmlItem(r, c);
			if (currItem != NULL) {
				stream << QString("<td class='%1' depth='%2'>")
					.arg( style(currItem->itemType())->name() )
					.arg( currItem->depth()-1 );
			}
			else stream << "<td>";
			stream << encodeToXml(text(r, c));
			stream << "</td>" << endl;
			if (currItem != NULL && currItem->isSpanCell()) {
				c++;
				while (c <= rightCol) {
					stream << "<td class='empty' depth='0'/>" << endl;
					c++;
				}
			}
		}
		stream << "</tr>" << endl;
	}
	stream << QString("</table>") << endl;
	redoFile.flush();
	redoFile.close();

	if (redoIndex > 1) {
		QFile newRedoFile(tempFilePath("redo", tmpFileIndex, redoIndex));
		QFile oldRedoFile(tempFilePath("redo", tmpFileIndex, redoIndex-1));
		if (noDiff( &newRedoFile, &oldRedoFile )) redoIndex--;
		newRedoFile.close();
		oldRedoFile.close();
	}
	emit redoRecordSizeChanged(redoIndex);
}


bool XmlTable::noDiff(QFile *f1, QFile *f2) {
	QTextStream t1(f1), t2(f2);
	QString curr1, curr2;
	if (!f1->open(IO_ReadOnly)) return FALSE;
	if (!f2->open(IO_ReadOnly)) return FALSE;
	do {
		curr1 = t1.readLine();
		curr2 = t2.readLine();
		if (curr1 != curr2) {
			return FALSE;
		}
	}
	while (!curr1.isNull() && !curr2.isNull());
	if (!curr1.isNull() || !curr2.isNull()) {
		return FALSE;
	}
	return TRUE;
}


void XmlTable::undo() {
	if (undoIndex <= 0) return;
	QFile undoFile(tempFilePath("undo", tmpFileIndex, undoIndex));
	undoIndex = undoIndex - 1;
	QDomDocument xmldoc;
	QTextStream t( &undoFile );
	bool isXml = xmldoc.setContent(t.read());
	if (!isXml) return;
	QDomElement root = xmldoc.documentElement();
	int rpos = root.attribute("rA", "0").toInt();
	int cpos = root.attribute("cA", "0").toInt();
	int rposZ = root.attribute("rZ", "0").toInt();
	int cposZ = root.attribute("cZ", "0").toInt();
	addRedoRecord(rpos, cpos, rposZ, cposZ);
	ensureCellVisible(rpos, cpos);
	QDomNodeList trList = root.childNodes();
	for (uint i = 0, r = 0; i < trList.length(); i++) {
		QDomNode trNode = trList.item(i);
		if (!(trNode.isElement() && trNode.nodeName() == "tr")) continue;
		QDomNodeList tdList = trNode.childNodes();
		for (uint j = 0, c = 0; j < tdList.length(); j++) {
			QDomNode tdNode = tdList.item(j);
			if (!(tdNode.isElement() && tdNode.nodeName() == "td")) continue;
			int row = rpos + r;
			int col = cpos + c;
			if (row < numRows() && col < numCols()) {
				QDomElement currElem = tdNode.toElement();
				setText(row, col, currElem.text());
				XmlTableItem* currXmlItem = xmlItem(row, col);
				if (currXmlItem != NULL) {
					currXmlItem->setSpan( 1,1 );
					currXmlItem->setItemType( itemTypeForString( currElem.attribute("class", "") ) );
					if (col == 0) currXmlItem->setDepth( currElem.attribute("depth", "").toInt() );
				}
				updateCell(row, col);
			}
			c++;
		}
		r++;
	}
	setModified(TRUE);
	emit undoRecordSizeChanged(undoIndex);
}


void XmlTable::redo() {
	if (redoIndex <= 0) return;
	QFile redoFile(tempFilePath("redo", tmpFileIndex, redoIndex));
	redoIndex = redoIndex - 1;
	QDomDocument xmldoc;
	QTextStream t(&redoFile);
	bool isXml = xmldoc.setContent(t.read());
	if (!isXml) return;
	QDomElement root = xmldoc.documentElement();
	int rpos = root.attribute("rA", "0").toInt();
	int cpos = root.attribute("cA", "0").toInt();
	int rposZ = root.attribute("rZ", "0").toInt();
	int cposZ = root.attribute("cZ", "0").toInt();
	addUndoRecord(rpos, cpos, rposZ, cposZ, TRUE);
	ensureCellVisible(rpos, cpos);
	QDomNodeList trList = root.childNodes();
	for (uint i = 0, r = 0; i < trList.length(); i++) {
		QDomNode trNode = trList.item(i);
		if (!(trNode.isElement() && trNode.nodeName() == "tr")) continue;
		QDomNodeList tdList = trNode.childNodes();
		for (uint j = 0, c = 0; j < tdList.length(); j++) {
			QDomNode tdNode = tdList.item(j);
			if (!(tdNode.isElement() && tdNode.nodeName() == "td")) continue;
			int row = rpos + r;
			int col = cpos + c;
			if (row < numRows() && col < numCols()) {
				QDomElement currElem = tdNode.toElement();
				setText(row, col, currElem.text());
				XmlTableItem* currXmlItem = xmlItem(row, col);
				if (currXmlItem != NULL) {
					currXmlItem->setSpan( 1,1 );
					currXmlItem->setItemType( itemTypeForString( currElem.attribute("class", "") ) );
					if (col == 0) currXmlItem->setDepth( currElem.attribute("depth", "").toInt() );
				}
				updateCell(row, col);
			}
			c++;
		}
		r++;
	}
	setModified(TRUE);
	emit redoRecordSizeChanged(redoIndex);
}


void XmlTable::relayMousePress( int row, int col, int button, const QPoint & mousePos ) {
	if (itemType(row, col) != XmlTableItem::ELEMENT
	|| button != Qt::LeftButton
	|| !inExpandBox(row, col, mousePos)) {
		return;
	}
	XmlTableItem *curritem = xmlItem(row, col);
	bool ex = !curritem->expanded();
	curritem->setExpanded(ex);
	updateCell(row, col);

	int currDepth = depth(row);
	for (int r = row+1; r <= max_rows /*< numRows()*/; r++) {
		if (depth(r) <= currDepth) {
			if (itemType(r, col) == XmlTableItem::ELEMENT) break;
			else if (depth(r+1) <= currDepth
			&& itemType(r+1, col) == XmlTableItem::ELEMENT
			&& itemType(r, col+1) == XmlTableItem::ATTR_KEY) break;
		}
		curritem = xmlItem(r, col);
		if (curritem != NULL) {
			curritem->setExpanded(ex);
			updateCell(r, col);
		}
		if (ex) showRow(r);
		else hideRow(r);
	}
}

void XmlTable::setNumRows( int r ) {
	QTable::setNumRows(r);
	setLeftMargin( fontMetrics().width("E") * QString::number(r).length() );
}


void XmlTable::adjustColumn( int col ) {
	QTable::adjustColumn( col );
	if (columnWidth(col) < minColumnWidth) {
		setColumnWidth( col, minColumnWidth );
	}
}


void XmlTable::setText( int r, int c, const QString & text ) {
	XmlTableItem *ti = xmlItem(r, c);
	if (ti != NULL) {
		if (text != ti->text()) setModified(TRUE);
		ti->setText(text);
		if (c == 0) {
			ti->setItemType(XmlTableItem::ELEMENT);
		}
		return;
	}

	int guessedDepth = 0;
	XmlTableItem::NodeType guessedType = XmlTableItem::EMPTY,
		aboveType = itemType(r-1, c),
		leftType = XmlTableItem::EMPTY;
	for (int i = c-1; i >= 0 && leftType == XmlTableItem::EMPTY; i--) {
		leftType = itemType(r, i);
	}

	if (text.isEmpty()) {
		return;
	}
	else if (c == 0) {
		guessedType = XmlTableItem::ELEMENT;
		guessedDepth = previousDepth(r);
		if (guessedDepth > 1) guessedDepth--;
	}
	else if (leftType == XmlTableItem::ATTR_KEY || leftType == XmlTableItem::ATTR_VAL) {
		guessedType = leftType;
	}
	else if (leftType == XmlTableItem::ELEMENT) {
		guessedType = XmlTableItem::ATTR_VAL;
	}
	else if (leftType == XmlTableItem::EMPTY && c == 1) {
		guessedType = XmlTableItem::ATTR_KEY;
	}
	else if (aboveType == XmlTableItem::ATTR_KEY || leftType == XmlTableItem::ATTR_VAL) {
		guessedType = XmlTableItem::ATTR_VAL;
	}

	XmlTableItem* xitem = new XmlTableItem(guessedType, this, QTableItem::OnTyping, text);
	if (c == 0) xitem->setDepth(guessedDepth);
	setItem(r, c, xitem);
	if (!text.isEmpty()) setModified(TRUE);
	if (guessedType == XmlTableItem::ELEMENT) {
		updateColumn(0);
	}
}


void XmlTable::setItem( int r, int c, QTableItem *i ) {
	//addUndoRecord(r, c, r, c);
	if (r >= numRows()) setNumRows(r+500);
	if (c >= numCols()) setNumCols(c+20);
	QTable::setItem( r, c, i );
	if (r > max_rows) max_rows = r;
	if (c > max_cols) max_cols = c;
 	setModified(TRUE);
 }


void XmlTable::expandDoc() {
	setNumRows(numRows()+500);
	setNumCols(numCols()+20);
}


void XmlTable::trimDoc() {
	setNumRows(max_rows+1);
	setNumCols(max_cols+1);
}



void XmlTable::touch(bool val) {
	if (val == TRUE) setModified(TRUE);
}

void XmlTable::setModified( bool val ) {
	isMod = val;
	emit modificationChanged(val);
}


bool XmlTable::isModified() const {
	return isMod;
}


XmlTableItem::NodeType XmlTable::itemType(int r, int c) {
	if (c < 0 || r < 0) return XmlTableItem::EMPTY;
	XmlTableItem* currXmlItem = xmlItem(r, c);
	if (currXmlItem == NULL) {
		return XmlTableItem::EMPTY;
	}
	return currXmlItem->itemType();
}


QString XmlTable::getKey(int r, int c) {
	int currRow;
	XmlTableItem* currXmlItem;
	for (currRow = r-1; currRow >= 0; currRow--) {
		currXmlItem = xmlItem(currRow, c);
		if (currXmlItem == NULL) continue;
		switch (currXmlItem->itemType()) {
			case XmlTableItem::ELEMENT:
				currRow = -1;
				break;
			case XmlTableItem::ATTR_KEY: {
				QString retval = currXmlItem->text();
				bool wasChanged;
				retval = fixXmlNameString(retval, &wasChanged);
				if (wasChanged) {
					currXmlItem->setText(retval);
					emit message(
						tr("Attribute name had illegal characters that were replaced with \"_\" in the save."),
						tr("Row: %1, Column: %2").arg(currRow+1).arg(c+1),
						FALSE, TRUE, QMessageBox::Warning
					);
				}
				return retval;
				}
				break;
			default:
				break;
		}
	}
	return QString("C%1").arg(c);
}


void XmlTable::save(QTextStream & out, const QString &docType) {
	//out << QString("<?xml version='%1' encoding='%2'?>").arg(xmlVersion).arg(XmlTable::outputEncoding);
	if (!docType.isEmpty()) out << docType;

	bool firstElemEncountered = FALSE;
	for (int r = 0; r <= maxRows(); r++) {
		XmlTableItem* item = xmlItem(r, 0);
		if (item != NULL && item->itemType() == XmlTableItem::ELEMENT) {
			if (!firstElemEncountered) {
				item->setDepth(0);
				firstElemEncountered = TRUE;
			}
			else {
				if (item->depth() <= 1) item->setDepth(1);
			}
		}
	}

	printDocument(out, 0,0, max_rows+1 /*numRows()*/, max_cols+1 /*numCols()*/);
	setModified(FALSE);
}


//returns length of range between current element and next
int XmlTable::getElementRange(int row, int currDepth, int lenRow) {
	int currRow = row+1;
	for (int i = 1; i < lenRow; currRow++, i++) {
		XmlTableItem* item = xmlItem(currRow, 0);
		if (item == NULL) {
			XmlTableItem* item2 = xmlItem(currRow, 1);
			if (item2 != NULL) {
				if (item2->itemType() == XmlTableItem::COMMENT && item2->commentDepth() < currDepth) {
					currRow--;
				}
			}
			continue;
		}
		if (item->itemType() == XmlTableItem::ELEMENT) {
			if (item->depth() <= currDepth) return currRow - row;
		}
	}
	return currRow - row;
}


QString XmlTable::encodeToXml(const QString &str) {
	QString xml;
	QChar currChar;
	for (uint i = 0; i < str.length(); i++) {
		currChar = str.at(i);
		switch (currChar) {
			case '&': xml += "&amp;"; break;
			case '<': xml += "&lt;"; break;
			case '>': xml += "&gt;"; break;
			case '\'': xml += "&apos;"; break;
			case '\"': xml += "&quot;"; break;
			default: xml += currChar; break;
		}
	}
	return xml;
}


QString XmlTable::encodeToHtml(const QString &str, int cellWidth, XmlTableItem::NodeType type) {
	QFontMetrics metric(XmlTable::style(type)->font());
	QString currLine;
	QString xml;
	QChar currChar;
	for (uint i = 0; i < str.length(); i++) {
		currChar = str.at(i);
		currLine += currChar;
		if ( cellWidth > 0
//		&& XmlTable::style(type)->wordWrap() == Qt::BreakAnywhere
		&& metric.width(currLine, currLine.length()) > cellWidth) {
			currLine = currChar;
			xml += "<br>";
		}
		switch (currChar) {
			case '&': xml += "&amp;"; break;
			case '<': xml += "&lt;"; break;
			case '>': xml += "&gt;"; break;
			case '\'': xml += "&apos;"; break;
			case '\"': xml += "&quot;"; break;
			case '\n':
				xml += "<br>";
				currLine = "";
				break;
			default: xml += currChar; break;
		}
	}
	return xml;
}


bool XmlTable::printDocument(QTextStream & out, int startRow, int startCol, int lenRow, int lenCol) {
	bool childPrinted = FALSE;
	for (int currRow = startRow, i = 0; i < lenRow; currRow++, i++) {
		if (itemType(currRow, startCol) == XmlTableItem::ELEMENT) {
			printIndent(out, depth(currRow));
			int range = getElementRange(currRow, depth(currRow), lenRow-i);
			printElement(out, currRow, startCol, range, lenCol);
			childPrinted = childPrinted || TRUE;
			i += range - 1;
			currRow += range - 1;
			continue;
		}
		int currCol = startCol + 1;
		switch ( itemType(currRow, currCol) ) {
			case XmlTableItem::COMMENT:
				printIndent(out, depth(currRow)+1);
				out << "<!--" << text(currRow, currCol) << "-->";
				childPrinted = childPrinted || TRUE;
				break;
			case XmlTableItem::TEXT:
				printIndent(out, depth(currRow)+1);
				out << encodeToXml(text(currRow, currCol));
				childPrinted = childPrinted || FALSE;
				break;
			case XmlTableItem::CDATA: {
				printIndent(out, depth(currRow)+1);
				//TODO:error msg if CDATA contains "]]>"
				QString cdataText = text(currRow, currCol);
				int numContained = cdataText.contains("]]>");
				if (numContained > 0) {
					emit message(
						tr("CDATA section contains %1 premature case(s) of the termination sequence: ]]>").arg(numContained),
						tr("Row: %1, Column: %2").arg(currRow+1).arg(currCol+1),
						FALSE, TRUE,
						QMessageBox::Critical
					);
				}
				out << "<![CDATA[" << cdataText << "]]>";
  				childPrinted = childPrinted || FALSE;
				break;
			}
			default:
				break;
		}
	}
	return childPrinted;
}


void XmlTable::printIndent(QTextStream &out, int len) {
	out << endl;
	for (int i = 1; i < len; i++) out << indent;
}


QString XmlTable::fixXmlNameString(const QString &val, bool* wasChanged) {
	QString name = val.stripWhiteSpace();
	*wasChanged = FALSE;
	if (name.length() == 0) {
		*wasChanged = TRUE;
		return "_";
	}
	QString retval;
	for (uint i = 0; i < name.length(); i++) {
		QChar currChar = name[i];
		if (i == 0) {
			if (isLetter(currChar) || currChar == '_' || currChar == ':') {
				retval += currChar;
			}
			else {
				*wasChanged = TRUE;
				retval += '_';
			}
		}
		else {
			if (isLetter(currChar) || isDigit(currChar)
			|| currChar == '.' || currChar == '-' || currChar == '_' || currChar == ':'
			|| isCombiningChar(currChar) || isExtender(currChar)) {
				retval += currChar;
			}
			else {
				*wasChanged = TRUE;
				retval += '_';
			}
		}
	}
	return retval;
}



bool XmlTable::isLetter(const QChar &c) {
	if (isBaseChar(c) || isIdeographic(c)) return TRUE;
	return FALSE;
}


bool XmlTable::isDigit(const QChar &c) {
	if (
	   inRange(c, 0x0030,0x0039) || inRange(c, 0x0660,0x0669) || inRange(c, 0x06F0,0x06F9)
	|| inRange(c, 0x0966,0x096F) || inRange(c, 0x09E6,0x09EF) || inRange(c, 0x0A66,0x0A6F)
	|| inRange(c, 0x0AE6,0x0AEF) || inRange(c, 0x0B66,0x0B6F) || inRange(c, 0x0BE7,0x0BEF)
	|| inRange(c, 0x0C66,0x0C6F) || inRange(c, 0x0CE6,0x0CEF) || inRange(c, 0x0D66,0x0D6F)
	|| inRange(c, 0x0E50,0x0E59) || inRange(c, 0x0ED0,0x0ED9) || inRange(c, 0x0F20,0x0F29)
	) return TRUE;
	return FALSE;
}


bool XmlTable::isCombiningChar(const QChar &c) {
	if (
	   inRange(c, 0x0300,0x0345) || inRange(c, 0x0360,0x0361) || inRange(c, 0x0483,0x0486)
	|| inRange(c, 0x0591,0x05A1) || inRange(c, 0x05A3,0x05B9) || inRange(c, 0x05BB,0x05BD)
	|| inRange(c, 0x05BF)        || inRange(c, 0x05C1,0x05C2) || inRange(c, 0x05C4)
	|| inRange(c, 0x064B,0x0652) || inRange(c, 0x0670)        || inRange(c, 0x06D6,0x06DC)
	|| inRange(c, 0x06DD,0x06DF) || inRange(c, 0x06E0,0x06E4) || inRange(c, 0x06E7,0x06E8)
	|| inRange(c, 0x06EA,0x06ED) || inRange(c, 0x0901,0x0903) || inRange(c, 0x093C)
	|| inRange(c, 0x093E,0x094C) || inRange(c, 0x094D)        || inRange(c, 0x0951,0x0954)
	|| inRange(c, 0x0962,0x0963) || inRange(c, 0x0981,0x0983) || inRange(c, 0x09BC)
	|| inRange(c, 0x09BE)        || inRange(c, 0x09BF)        || inRange(c, 0x09C0,0x09C4)
	|| inRange(c, 0x09C7,0x09C8) || inRange(c, 0x09CB,0x09CD) || inRange(c, 0x09D7)
	|| inRange(c, 0x09E2,0x09E3) || inRange(c, 0x0A02)        || inRange(c, 0x0A3C)
	|| inRange(c, 0x0A3E)        || inRange(c, 0x0A3F)        || inRange(c, 0x0A40,0x0A42)
	|| inRange(c, 0x0A47,0x0A48) || inRange(c, 0x0A4B,0x0A4D) || inRange(c, 0x0A70,0x0A71)
	|| inRange(c, 0x0A81,0x0A83) || inRange(c, 0x0ABC)        || inRange(c, 0x0ABE,0x0AC5)
	|| inRange(c, 0x0AC7,0x0AC9) || inRange(c, 0x0ACB,0x0ACD) || inRange(c, 0x0B01,0x0B03)
	|| inRange(c, 0x0B3C)        || inRange(c, 0x0B3E,0x0B43) || inRange(c, 0x0B47,0x0B48)
	|| inRange(c, 0x0B4B,0x0B4D) || inRange(c, 0x0B56,0x0B57) || inRange(c, 0x0B82,0x0B83)
	|| inRange(c, 0x0BBE,0x0BC2) || inRange(c, 0x0BC6,0x0BC8) || inRange(c, 0x0BCA,0x0BCD)
	|| inRange(c, 0x0BD7)        || inRange(c, 0x0C01,0x0C03) || inRange(c, 0x0C3E,0x0C44)
	|| inRange(c, 0x0C46,0x0C48) || inRange(c, 0x0C4A,0x0C4D) || inRange(c, 0x0C55,0x0C56)
	|| inRange(c, 0x0C82,0x0C83) || inRange(c, 0x0CBE,0x0CC4) || inRange(c, 0x0CC6,0x0CC8)
	|| inRange(c, 0x0CCA,0x0CCD) || inRange(c, 0x0CD5,0x0CD6) || inRange(c, 0x0D02,0x0D03)
	|| inRange(c, 0x0D3E,0x0D43) || inRange(c, 0x0D46,0x0D48) || inRange(c, 0x0D4A,0x0D4D)
	|| inRange(c, 0x0D57)        || inRange(c, 0x0E31)        || inRange(c, 0x0E34,0x0E3A)
	|| inRange(c, 0x0E47,0x0E4E) || inRange(c, 0x0EB1)        || inRange(c, 0x0EB4,0x0EB9)
	|| inRange(c, 0x0EBB,0x0EBC) || inRange(c, 0x0EC8,0x0ECD) || inRange(c, 0x0F18,0x0F19)
	|| inRange(c, 0x0F35)        || inRange(c, 0x0F37)        || inRange(c, 0x0F39)
	|| inRange(c, 0x0F3E)        || inRange(c, 0x0F3F)        || inRange(c, 0x0F71,0x0F84)
	|| inRange(c, 0x0F86,0x0F8B) || inRange(c, 0x0F90,0x0F95) || inRange(c, 0x0F97)
	|| inRange(c, 0x0F99,0x0FAD) || inRange(c, 0x0FB1,0x0FB7) || inRange(c, 0x0FB9)
	|| inRange(c, 0x20D0,0x20DC) || inRange(c, 0x20E1)        || inRange(c, 0x302A,0x302F)
	|| inRange(c, 0x3099)        || inRange(c, 0x309A)
	) return TRUE;
	return FALSE;
}


bool XmlTable::isExtender(const QChar &c) {
	if (
	   inRange(c, 0x00B7)        || inRange(c, 0x02D0)        || inRange(c, 0x02D1)
	|| inRange(c, 0x0387)        || inRange(c, 0x0640)        || inRange(c, 0x0E46)
	|| inRange(c, 0x0EC6)        || inRange(c, 0x3005)        || inRange(c, 0x3031,0x3035)
	|| inRange(c, 0x309D,0x309E) || inRange(c, 0x30FC,0x30FE)
	) return TRUE;
	return FALSE;
}


bool XmlTable::isBaseChar(const QChar &c) {
	if (
	   inRange(c, 0x0041,0x005A) || inRange(c, 0x0061,0x007A) || inRange(c, 0x00C0,0x00D6)
	|| inRange(c, 0x00D8,0x00F6) || inRange(c, 0x00F8,0x00FF) || inRange(c, 0x0100,0x0131)
	|| inRange(c, 0x0134,0x013E) || inRange(c, 0x0141,0x0148) || inRange(c, 0x014A,0x017E)
	|| inRange(c, 0x0180,0x01C3) || inRange(c, 0x01CD,0x01F0) || inRange(c, 0x01F4,0x01F5)
	|| inRange(c, 0x01FA,0x0217) || inRange(c, 0x0250,0x02A8) || inRange(c, 0x02BB,0x02C1)
	|| inRange(c, 0x0386)        || inRange(c, 0x0388,0x038A) || inRange(c, 0x038C)
	|| inRange(c, 0x038E,0x03A1) || inRange(c, 0x03A3,0x03CE) || inRange(c, 0x03D0,0x03D6)
	|| inRange(c, 0x03DA)        || inRange(c, 0x03DC)        || inRange(c, 0x03DE)
	|| inRange(c, 0x03E0)        || inRange(c, 0x03E2,0x03F3) || inRange(c, 0x0401,0x040C)
	|| inRange(c, 0x040E,0x044F) || inRange(c, 0x0451,0x045C) || inRange(c, 0x045E,0x0481)
	|| inRange(c, 0x0490,0x04C4) || inRange(c, 0x04C7,0x04C8) || inRange(c, 0x04CB,0x04CC)
	|| inRange(c, 0x04D0,0x04EB) || inRange(c, 0x04EE,0x04F5) || inRange(c, 0x04F8,0x04F9)
	|| inRange(c, 0x0531,0x0556) || inRange(c, 0x0559)        || inRange(c, 0x0561,0x0586)
	|| inRange(c, 0x05D0,0x05EA) || inRange(c, 0x05F0,0x05F2) || inRange(c, 0x0621,0x063A)
	|| inRange(c, 0x0641,0x064A) || inRange(c, 0x0671,0x06B7) || inRange(c, 0x06BA,0x06BE)
	|| inRange(c, 0x06C0,0x06CE) || inRange(c, 0x06D0,0x06D3) || inRange(c, 0x06D5)
	|| inRange(c, 0x06E5,0x06E6) || inRange(c, 0x0905,0x0939) || inRange(c, 0x093D)
	|| inRange(c, 0x0958,0x0961) || inRange(c, 0x0985,0x098C) || inRange(c, 0x098F,0x0990)
	|| inRange(c, 0x0993,0x09A8) || inRange(c, 0x09AA,0x09B0) || inRange(c, 0x09B2)
	|| inRange(c, 0x09B6,0x09B9) || inRange(c, 0x09DC,0x09DD) || inRange(c, 0x09DF,0x09E1)
	|| inRange(c, 0x09F0,0x09F1) || inRange(c, 0x0A05,0x0A0A) || inRange(c, 0x0A0F,0x0A10)
	|| inRange(c, 0x0A13,0x0A28) || inRange(c, 0x0A2A,0x0A30) || inRange(c, 0x0A32,0x0A33)
	|| inRange(c, 0x0A35,0x0A36) || inRange(c, 0x0A38,0x0A39) || inRange(c, 0x0A59,0x0A5C)
	|| inRange(c, 0x0A5E)        || inRange(c, 0x0A72,0x0A74) || inRange(c, 0x0A85,0x0A8B)
	|| inRange(c, 0x0A8D)        || inRange(c, 0x0A8F,0x0A91) || inRange(c, 0x0A93,0x0AA8)
	|| inRange(c, 0x0AAA,0x0AB0) || inRange(c, 0x0AB2,0x0AB3) || inRange(c, 0x0AB5,0x0AB9)
	|| inRange(c, 0x0ABD)        || inRange(c, 0x0AE0)        || inRange(c, 0x0B05,0x0B0C)
	|| inRange(c, 0x0B0F,0x0B10) || inRange(c, 0x0B13,0x0B28) || inRange(c, 0x0B2A,0x0B30)
	|| inRange(c, 0x0B32,0x0B33) || inRange(c, 0x0B36,0x0B39) || inRange(c, 0x0B3D)
	|| inRange(c, 0x0B5C,0x0B5D) || inRange(c, 0x0B5F,0x0B61) || inRange(c, 0x0B85,0x0B8A)
	|| inRange(c, 0x0B8E,0x0B90) || inRange(c, 0x0B92,0x0B95) || inRange(c, 0x0B99,0x0B9A)
	|| inRange(c, 0x0B9C)        || inRange(c, 0x0B9E,0x0B9F) || inRange(c, 0x0BA3,0x0BA4)
	|| inRange(c, 0x0BA8,0x0BAA) || inRange(c, 0x0BAE,0x0BB5) || inRange(c, 0x0BB7,0x0BB9)
	|| inRange(c, 0x0C05,0x0C0C) || inRange(c, 0x0C0E,0x0C10) || inRange(c, 0x0C12,0x0C28)
	|| inRange(c, 0x0C2A,0x0C33) || inRange(c, 0x0C35,0x0C39) || inRange(c, 0x0C60,0x0C61)
	|| inRange(c, 0x0C85,0x0C8C) || inRange(c, 0x0C8E,0x0C90) || inRange(c, 0x0C92,0x0CA8)
	|| inRange(c, 0x0CAA,0x0CB3) || inRange(c, 0x0CB5,0x0CB9) || inRange(c, 0x0CDE)
	|| inRange(c, 0x0CE0,0x0CE1) || inRange(c, 0x0D05,0x0D0C) || inRange(c, 0x0D0E,0x0D10)
	|| inRange(c, 0x0D12,0x0D28) || inRange(c, 0x0D2A,0x0D39) || inRange(c, 0x0D60,0x0D61)
	|| inRange(c, 0x0E01,0x0E2E) || inRange(c, 0x0E30)        || inRange(c, 0x0E32,0x0E33)
	|| inRange(c, 0x0E40,0x0E45) || inRange(c, 0x0E81,0x0E82) || inRange(c, 0x0E84)
	|| inRange(c, 0x0E87,0x0E88) || inRange(c, 0x0E8A)        || inRange(c, 0x0E8D)
	|| inRange(c, 0x0E94,0x0E97) || inRange(c, 0x0E99,0x0E9F) || inRange(c, 0x0EA1,0x0EA3)
	|| inRange(c, 0x0EA5)        || inRange(c, 0x0EA7)        || inRange(c, 0x0EAA,0x0EAB)
	|| inRange(c, 0x0EAD,0x0EAE) || inRange(c, 0x0EB0)        || inRange(c, 0x0EB2,0x0EB3)
	|| inRange(c, 0x0EBD)        || inRange(c, 0x0EC0,0x0EC4) || inRange(c, 0x0F40,0x0F47)
	|| inRange(c, 0x0F49,0x0F69) || inRange(c, 0x10A0,0x10C5) || inRange(c, 0x10D0,0x10F6)
	|| inRange(c, 0x1100)        || inRange(c, 0x1102,0x1103) || inRange(c, 0x1105,0x1107)
	|| inRange(c, 0x1109)        || inRange(c, 0x110B,0x110C) || inRange(c, 0x110E,0x1112)
	|| inRange(c, 0x113C)        || inRange(c, 0x113E)        || inRange(c, 0x1140)
	|| inRange(c, 0x114C)        || inRange(c, 0x114E)        || inRange(c, 0x1150)
	|| inRange(c, 0x1154,0x1155) || inRange(c, 0x1159)        || inRange(c, 0x115F,0x1161)
	|| inRange(c, 0x1163)        || inRange(c, 0x1165)        || inRange(c, 0x1167)
	|| inRange(c, 0x1169)        || inRange(c, 0x116D,0x116E) || inRange(c, 0x1172,0x1173)
	|| inRange(c, 0x1175)        || inRange(c, 0x119E)        || inRange(c, 0x11A8)
	|| inRange(c, 0x11AB)        || inRange(c, 0x11AE,0x11AF) || inRange(c, 0x11B7,0x11B8)
	|| inRange(c, 0x11BA)        || inRange(c, 0x11BC,0x11C2) || inRange(c, 0x11EB)
	|| inRange(c, 0x11F0)        || inRange(c, 0x11F9)        || inRange(c, 0x1E00,0x1E9B)
	|| inRange(c, 0x1EA0,0x1EF9) || inRange(c, 0x1F00,0x1F15) || inRange(c, 0x1F18,0x1F1D)
	|| inRange(c, 0x1F20,0x1F45) || inRange(c, 0x1F48,0x1F4D) || inRange(c, 0x1F50,0x1F57)
	|| inRange(c, 0x1F59)        || inRange(c, 0x1F5B)        || inRange(c, 0x1F5D)
	|| inRange(c, 0x1F5F,0x1F7D) || inRange(c, 0x1F80,0x1FB4) || inRange(c, 0x1FB6,0x1FBC)
	|| inRange(c, 0x1FBE)        || inRange(c, 0x1FC2,0x1FC4) || inRange(c, 0x1FC6,0x1FCC)
	|| inRange(c, 0x1FD0,0x1FD3) || inRange(c, 0x1FD6,0x1FDB) || inRange(c, 0x1FE0,0x1FEC)
	|| inRange(c, 0x1FF2,0x1FF4) || inRange(c, 0x1FF6,0x1FFC) || inRange(c, 0x2126)
	|| inRange(c, 0x212A,0x212B) || inRange(c, 0x212E)        || inRange(c, 0x2180,0x2182)
	|| inRange(c, 0x3041,0x3094) || inRange(c, 0x30A1,0x30FA) || inRange(c, 0x3105,0x312C)
	|| inRange(c, 0xAC00,0xD7A3)
	) return TRUE;
	return FALSE;
}


bool XmlTable::isIdeographic(const QChar &c) {
	if (
		inRange(c, 0x4E00,0x9FA5) || inRange(c, 0x3007) || inRange(c, 0x3021,0x3029)
	) return TRUE;
	return FALSE;
}


bool XmlTable::inRange(const QChar &c, int lower, int upper) {
	if (upper < 0) return c == QChar(lower);
	return c >= QChar(lower) && c <= QChar(upper);
}



void XmlTable::printElement(QTextStream & out, int startRow, int startCol, int lenRow, int lenCol) {
	QString elemName = text(startRow, startCol);
	bool wasChanged;
	elemName = fixXmlNameString(elemName, &wasChanged);
	if (wasChanged) {
		setText(startRow, startCol, elemName);
		emit message(
			tr("Element had illegal characters that were replaced with \"_\" in the save."),
			tr("Row: %1, Column: %2").arg(startRow+1).arg(startCol+1),
			FALSE, TRUE, QMessageBox::Warning
		);
	}
	//TODO:error msg if element name uses illegal characters
	out << "<" << elemName;
	printAttr(out, startRow, startCol+1, lenCol-1);
	bool childPrinted = FALSE;
	if (lenRow > 1 && lenCol > 1) {
		childPrinted = printDocument(out, startRow+1, startCol, lenRow-1, lenCol);
		if (childPrinted) printIndent(out, depth(startRow));
	}
	out << "</" << elemName << ">";
}


void XmlTable::printAttr(QTextStream & out, int startRow, int startCol, int lenCol) {
	QString currKey;
	//TODO:error msg if attr name uses illegal characters
	for (int currCol = startCol, i = 0; i < lenCol; currCol++, i++) {
		switch ( itemType(startRow, currCol) ) {
			case XmlTableItem::ATTR_VAL:
				currKey = getKey(startRow, currCol);
				out << " " << currKey << "='" << encodeToXml(text(startRow, currCol)) << "'";
				break;
			case XmlTableItem::TEXT:
				out << ">" << encodeToXml(text(startRow, currCol));
				return;
			case XmlTableItem::CDATA:
				out << "><![CDATA[" << text(startRow, currCol) << "]]>";
				return;
			default:
				break;
		}
	}
	out << ">";
}


XmlTableItem* XmlTable::xmlItem(int r, int c) {
	QTableItem *currItem = item(r, c);
	if (currItem == NULL) return NULL;
	return (XmlTableItem*)currItem;
}




void XmlTable::updateAll() {
	for (int c = 0; c <= max_cols /*< numCols()*/; c++) {
		updateColumn(c);
	}
}


void XmlTable::updateColumn(int col) {
	for (int r = 0; r <= max_rows /*< numRows()*/; r++) {
		updateCell(r, col);
	}
}


void XmlTable::updateRow(int row) {
	for (int c = 0; c <= max_cols /*< numCols()*/; c++) {
		updateCell(row, c);
	}
}


void XmlTable::adjustAll() {
	//adjust column sizes, span text cells to take remainder of row
	XmlTableItem *currXmlItem;
	int c, r;
	for (c = 0; c <= max_cols /*< numCols()*/; c++) {
		for (r = 0; r <= max_rows /*< numRows()*/; r++) {
			currXmlItem = xmlItem(r, c);
			if (currXmlItem == NULL) continue;
			if (c != currXmlItem->col()) continue;
			if (currXmlItem->isSpanCell() ) {
				currXmlItem->setSpan( 1, max_cols+1 /*numCols()*/ - currXmlItem->col() );
			}
			else {
				currXmlItem->setSpan(1, 1);
			}
		}
	}
	for (c = 0; c <= max_cols /*< numCols()*/; c++) {
		for (r = 0; r <= max_rows /*< numRows()*/; r++) {
			currXmlItem = xmlItem(r, c);
			if (currXmlItem == NULL) continue;
			if ( !XmlTable::style(currXmlItem->itemType())->isWordWrap() ) {
				currXmlItem->setSizeHint();
			}
		}
	}


	for (c = 0; c <= max_cols /*< numCols()*/; c++) {
		for (r = 0; r <= max_rows /*< numRows()*/; r++) {
			currXmlItem = xmlItem(r, c);
			if (currXmlItem == NULL) continue;
			if ( XmlTable::style(currXmlItem->itemType())->isWordWrap() ) {
				currXmlItem->setSizeHint();
			}
		}
	}
	for (c = 0; c <= max_cols /*< numCols()*/; c++) adjustColumn(c);
	for (c = 0; c <= max_cols /*< numCols()*/; c++) {
		for (r = 0; r <= max_rows /*< numRows()*/; r++) {
			currXmlItem = xmlItem(r, c);
			if (currXmlItem == NULL) continue;
			if ( XmlTable::style(currXmlItem->itemType())->isWordWrap() ) {
//			if ( XmlTable::style(currXmlItem->itemType())->wordWrap() == Qt::BreakAnywhere ) {
				currXmlItem->setSizeHint();
			}
		}
	}
	for (r = 0; r <= max_rows /*< numRows()*/; r++) adjustRow(r);
}


bool XmlTable::setContent(QIODevice *dev) {
	//clear all existing data
	for (int r = 0; r <= max_rows /*< numRows()*/; r++) {
		for (int c = 0; c <= max_cols /*< numCols()*/; c++) {
			clearCell(r, c);
		}
	}
	resetUndo();
	resetRedo();
	QDomDocument xmldoc;
	QTextStream t( dev );
	QString errorMsg;
	int errorLine, errorColumn;
	bool retval = xmldoc.setContent(t.read(), &errorMsg, &errorLine, &errorColumn);
	if (!retval) {
		emit message(
			tr("XML read error: %1").arg(errorMsg),
			tr("Line: %1, Column: %2").arg(errorLine).arg(errorColumn),
			FALSE, TRUE, QMessageBox::Critical
		);
		updateAll();
		return retval;
	}

	QDomDocumentType dtd = xmldoc.doctype();
	QString docType;
	QTextStream dtds( &docType, IO_WriteOnly );
	dtd.save(dtds, 0);
	docType = docType.stripWhiteSpace();
	emit dtdLoaded(docType);

	QDomElement root = xmldoc.documentElement();
	max_rows = 0; max_cols = 0;
	int lastLevel = 0;

	QStringList *lastAttrList = NULL;
	layoutNode(max_rows, 0, &root, &lastAttrList, &lastLevel);
	if (lastAttrList != NULL) delete lastAttrList;

	if (max_rows > 1) max_rows--;
	adjustAll();
	setModified(FALSE);
	setCurrentCell(0, 0);
	return retval;
}


void XmlTable::insertRows( int r, int n ) {
	if (r <= max_rows) max_rows++;
	if (max_rows >= numRows()) setNumRows(numRows()+500);
	QTable::insertRows(r, n);
}


void XmlTable::layoutNode(int &row, int level, QDomNode *node, QStringList **lastAttrList, int *lastLevel) {
	if (node->isElement()) {
		QDomElement elem = node->toElement();
		layoutData(row, level, &elem, lastAttrList, lastLevel);
		if (!node->hasChildNodes()) return;

		QDomNodeList list = node->childNodes();
		QDomNode currNode;
		//skip 1st TEXT or CDATA section
		bool skippedText = FALSE;
		for (uint i = 0; i < list.length(); i++) {
			currNode = list.item(i);
			if (!skippedText) switch (currNode.nodeType()) {
				case QDomNode::TextNode:
				case QDomNode::CDATASectionNode:
					skippedText = TRUE;
					continue;
				default:
					skippedText = TRUE;
			}
			layoutNode(row, level+1, &currNode, lastAttrList, lastLevel);
		}
	}
	else {
		layoutText(row, 1, node, level);
	}
}


void XmlTableItem::decreaseDepth() {
	hierDepth--;
	if (hierDepth < 1) hierDepth = 1;
}


void XmlTableItem::increaseDepth() {
	hierDepth++;
}


int XmlTableItem::depth() {
	//if (itemType() == EMPTY) return nextDepth() <? previousDepth();
	if (itemType() == EMPTY) return ( ( nextDepth() < previousDepth() ) ? nextDepth() : previousDepth() );
	return hierDepth;
}

int XmlTableItem::commentDepth() {
	if (itemType() == COMMENT)
		return commentDepth_;
	return hierDepth;
}

bool XmlTable::isNewNodeType(QStringList *newAttrs, QStringList *oldAttrs) {
	if (oldAttrs == NULL || newAttrs == NULL) return TRUE;
	if (newAttrs->empty()) return FALSE;
	return (*newAttrs) != (*oldAttrs);
}


void XmlTable::layoutData(int &row, int level, QDomElement *elem, QStringList **lastAttrList, int *lastLevel) {
	int curr_col = 0;
	uint i;
	QDomNamedNodeMap map = elem->attributes();

	QStringList *attrKeyList = new QStringList();
	for (uint i = 0; i < map.length(); i++) {
		attrKeyList->append(map.item(i).nodeName());
	}
	if (XmlTable::sortAttr) attrKeyList->sort();

	if (isNewNodeType(attrKeyList, *lastAttrList)) {
		clearCell(row, curr_col++);
		for (QStringList::iterator it = attrKeyList->begin(); it != attrKeyList->end(); ++it) {
			setItem(row, curr_col++,
				new XmlTableItem(XmlTableItem::ATTR_KEY, this, QTableItem::OnTyping, *it)
			);
		}
		row++;
		curr_col = 0;
	}
	XmlTableItem* elemItem = new XmlTableItem(XmlTableItem::ELEMENT, this, QTableItem::OnTyping, elem->nodeName());
	if (curr_col == 0) elemItem->setDepth(level);
	setItem(row, curr_col++, elemItem);

	for (QStringList::iterator it = attrKeyList->begin(); it != attrKeyList->end(); ++it) {
		setItem(row, curr_col++,
			new XmlTableItem(XmlTableItem::ATTR_VAL, this, QTableItem::OnTyping, elem->attribute(*it, ""))
		);
	}

	QDomNodeList list = elem->childNodes();
	QDomNode currNode;
	//print 1st TEXT or CDATA section only
	bool found = FALSE, wasAdjusted = FALSE;
	for (i = 0; i < list.length() && !found; i++) {
		currNode = list.item(i);
		switch (currNode.nodeType()) {
			case QDomNode::TextNode:
			case QDomNode::CDATASectionNode:
				found = TRUE;
				layoutText(row, curr_col++, &currNode);
				wasAdjusted = TRUE;
				continue;
			default:
				found = TRUE;
		}
	}

	if (!wasAdjusted) adjustRow(row++);

	if (lastAttrList != NULL && *lastAttrList != NULL) delete (*lastAttrList);
	*lastAttrList = attrKeyList;

	*lastLevel = level;
	if (curr_col > max_cols) max_cols = curr_col;
}


void XmlTable::layoutText(int &row, int level, QDomNode *node, int eLevel) {
	XmlTableItem::NodeType ntype;
	switch (node->nodeType()) {
		case QDomNode::TextNode:
			ntype = XmlTableItem::TEXT;
			break;
		case QDomNode::CDATASectionNode:
			ntype = XmlTableItem::CDATA;
			break;
		case QDomNode::CommentNode:
			ntype = XmlTableItem::COMMENT;
			break;
		default:
			ntype = XmlTableItem::TEXT;
			break;
	}

	QString val = node->nodeValue().stripWhiteSpace();
	XmlTableItem *item = new XmlTableItem(ntype, this, QTableItem::OnTyping, val);
	if ( ntype == XmlTableItem::COMMENT )
		item->setCommentDepth(eLevel);
	setItem(row, level, item);
	adjustRow(row++);

	if (level > max_cols) max_cols = level;
}


QString XmlTable::html(int topRow, int leftCol, int bottomRow, int rightCol) {
	QString str = "<html><body>";
	XmlTableItem* currItem;
	str += "<table border='1' cellpadding='0' cellspacing='0'>";
	for (int r = topRow; r <= bottomRow; r++) {
		str += "\n<tr>";
		for (int c = leftCol; c <= rightCol; c++) {
			currItem = xmlItem(r, c);
			if (currItem == NULL) {
				str += "<td/>";
				continue;
			}
			XmlTableItem::NodeType currType = currItem->itemType();
			bool isPre = (currType == XmlTableItem::TEXT || currType == XmlTableItem::CDATA);
			XmlTableStyle* currStyle = style(currType);
			QFont currFont = currStyle->font();
			str += QString("<td colspan='%1' bgcolor='%2' align='%3' valign='%4'>")
				.arg( currItem->colSpan() )
				.arg( currStyle->bgColor().name() )
				.arg( currStyle->alignText() )
				.arg( currStyle->valignText() );
			if (isPre) str += "<pre>";
			str += QString("<font color='%1' face='%2' size='%3'>")
				.arg( currStyle->fgColor().name() )
				.arg( currFont.family() )
				.arg( /*currFont.pointSize()*/0 );
			if (currFont.bold()) str += "<b>";
			if (currFont.italic()) str += "<i>";
			str += encodeToHtml(text(r, c));
			if (currFont.italic()) str += "</i>";
			if (currFont.bold()) str += "</b>";
			if (currItem != NULL && currItem->isSpanCell()) c = rightCol+1;
			str += "</font>";
			if (isPre) str += "</pre>";
			str += "</td>";
		}
		str += "</tr>";
	}
	str += "\n</table></body></html>";
	return str;
}


QString XmlTable::html2(int topRow, int leftCol, int bottomRow, int rightCol) {
	int maxDepth = 1;
	for (int r = topRow; r <= bottomRow; r++) {
		//maxDepth = maxDepth >? depth(r);
		maxDepth = (maxDepth > depth(r)) ? maxDepth : depth(r);
	}
	QString str = "<html><body>";
	XmlTableItem* currItem;
	str += "<table border='1' frame='void' cellpadding='0' cellspacing='0'>";
	for (int r = topRow; r <= bottomRow; r++) {
		str += "\n<tr>";
		for (int c = leftCol; c <= rightCol; c++) {
			currItem = xmlItem(r, c);
			if (currItem == NULL) {
				if (c == 0) {
					for (int i = 1; i <= depth(r); i++) {
						str += QString("<td width='%1'>&nbsp;</td>").arg(indentWidth);
					}
					str += QString("<td colspan='%1'>&nbsp;</td>").arg(maxDepth - depth(r) );
				}
				else str += "<td>&nbsp;</td>";
				continue;
			}
			XmlTableItem::NodeType currType = currItem->itemType();
			bool isPre = (currType == XmlTableItem::TEXT || currType == XmlTableItem::CDATA);
			XmlTableStyle* currStyle = style(currType);
			QFont currFont = currStyle->font();
			if (currType == XmlTableItem::ELEMENT) {
				for (int i = 1; i < currItem->depth(); i++) {
					str += QString("<td width='%1'>&nbsp;</td>").arg(indentWidth);
				}
//				str += QString("<td class='teddy_%1' colspan='%2'>")
//					.arg( currStyle->name() )
//					.arg( maxDepth - currItem->depth() +1);
				str += QString("<td colspan='%1' bgcolor='%2' align='%3' valign='%4'>")
					.arg( maxDepth - currItem->depth() +1 )
					.arg( currStyle->bgColor().name() )
					.arg( currStyle->alignText() )
					.arg( currStyle->valignText() );
			}
			else if (!currItem->isSpanCell()) {
				str += QString("<td bgcolor='%1' align='%2' valign='%3'>")
					.arg( currStyle->bgColor().name() )
					.arg( currStyle->alignText() )
					.arg( currStyle->valignText() );
			}
			else {
				str += QString("<td colspan='%1' bgcolor='%2' align='%3' valign='%4'>")
					.arg( currItem->colSpan() )
					.arg( currStyle->bgColor().name() )
					.arg( currStyle->alignText() )
					.arg( currStyle->valignText() );
			}

			if (isPre) str += "<pre>";

			str += QString("<font color='%1' face='%2' size='%3'>")
				.arg( currStyle->fgColor().name() )
				.arg( currFont.family() )
				.arg( /*currFont.pointSize()*/0 );
			if (currFont.bold()) str += "<b>";
			if (currFont.italic()) str += "<i>";

			str += encodeToHtml(text(r, c), currItem->spanWidth(), currType);

			if (currFont.italic()) str += "</i>";
			if (currFont.bold()) str += "</b>";
			if (currItem != NULL && currItem->isSpanCell()) c = rightCol+1;
			str += "</font>";

			if (isPre) str += "</pre>";
			str += "</td>";
		}
		str += "</tr>";
	}
	str += "\n</table></body></html>\n";
	return str;
}


QString XmlTable::html_css(int topRow, int leftCol, int bottomRow, int rightCol) {
	int maxDepth = 1;
	for (int r = topRow; r <= bottomRow; r++) {
		//maxDepth = maxDepth >? depth(r);
		maxDepth = (maxDepth > depth(r)) ? maxDepth : depth(r);
	}

	QString str = "<html><head><style type='text/css'>\n"
		"body { margin: 0 0 0 0; } \n";

	for (int type = XmlTableItem::EMPTY; type <= XmlTableItem::COMMENT; type++) {
		XmlTableStyle* currStyle = styleVector[type];
		QString name = currStyle->name();
		QFont font = currStyle->font();
		QString style = font.italic() ? "italic":"normal";
		str += QString(".teddy_%1 { \n"
			"color:%2; background:%3; \n"
			"text-align:%4; vertical-align:%5; \n"
			"font-family:%6; font-size:%7pt; font-weight:%8; font-style:%9; \n")
			.arg(currStyle->name())
			.arg(currStyle->fgColor().name())
			.arg(currStyle->bgColor().name())
			.arg(currStyle->alignText())
			.arg(currStyle->valignText())
			.arg(font.family())
			.arg(font.pointSize())
			.arg(font.weight()*8)
			.arg(style);
		switch (type) {
			case XmlTableItem::EMPTY:
				str += "border-top:thin dashed gray; border-right:thin dashed gray; \n";
				break;
			case XmlTableItem::ELEMENT:
				str += "border-top:thin solid black; border-right:thin solid black; border-left:thin solid black; \n";
				break;
			default:
				str += "border-top:thin solid black; border-right:thin solid black; \n";
		}
		str += "}\n";
	}
	str += QString(".teddy_emptyA { \n"
		"border-top:thin dashed gray; border-left:thin dashed gray; \n"
		"}\n"
	);
	str += QString(".teddy_emptyZ { "
		"border-top:thin dashed gray; border-left:thin dashed gray; border-right:thin solid black; "
		"}\n"
	);

	str += "</style></head><body>";
	XmlTableItem* currItem;
	str += "<table cellpadding='0' cellspacing='0'>";
	for (int r = topRow; r <= bottomRow; r++) {
		str += "\n<tr>";
		for (int c = leftCol; c <= rightCol; c++) {
			currItem = xmlItem(r, c);
			if (currItem == NULL) {
				if (c == 0) {
					for (int i = 1; i <= depth(r); i++) {
						str += QString("<td class='teddy_emptyA' width='%1'>&nbsp;</td>").arg(indentWidth);
					}
					str += QString("<td class='teddy_emptyZ' colspan='%1'>&nbsp;</td>").arg(maxDepth - depth(r) );
				}
				else str += "<td class='teddy_empty'>&nbsp;</td>";
				continue;
			}
			XmlTableItem::NodeType currType = currItem->itemType();
			bool isPre = (currType == XmlTableItem::TEXT || currType == XmlTableItem::CDATA);
			XmlTableStyle* currStyle = style(currType);
			QFont currFont = currStyle->font();
			if (currType == XmlTableItem::ELEMENT) {
				for (int i = 1; i < currItem->depth(); i++) {
					str += QString("<td class='teddy_emptyA' width='%1'>&nbsp;</td>").arg(indentWidth);
				}
				str += QString("<td class='teddy_%1' colspan='%2'>")
					.arg( currStyle->name() )
					.arg( maxDepth - currItem->depth() +1);
			}
			else if (!currItem->isSpanCell()) {
				str += QString("<td class='teddy_%1'>").arg( currStyle->name() );
			}
			else {
				str += QString("<td class='teddy_%1' colspan='%2'>")
					.arg( currStyle->name() )
					.arg( currItem->colSpan() );
			}

			if (isPre) str += "<pre>";
			str += encodeToHtml(text(r, c), currItem->spanWidth(), currType);
			if (currItem != NULL && currItem->isSpanCell()) c = rightCol+1;
			if (isPre) str += "</pre>";
			str += "</td>";
		}
		str += "</tr>";
	}
	str += "\n</table></body></html>\n";
	return str;
}


/*
<table r='x' c='y'>
<tr><td class='type' depth='d'>text</td>
	</tr>
</undo>

*/


void XmlTable::copy(bool isTextOnly) {
if (isTextOnly) {
	QString str;
	int numSel = numSelections();
	if (numSel <= 0) {
		selectCells(currentRow(), currentColumn(), currentRow(), currentColumn());
		numSel = numSelections();
	}
	XmlTableItem* currItem;
	for (int k = 0; k < numSel; k++) {
		QTableSelection sel = selection(k);
		if (sel.isEmpty()) continue;
		if (k > 0) str += "\n";
		for (int r=sel.topRow(); r <= sel.bottomRow(); r++) {
			if (r > sel.topRow()) str += "\n";
			for (int c = sel.leftCol(); c <= sel.rightCol(); c++) {
				if (c > sel.leftCol()) str += "\t";
				str += text(r, c);
				currItem = xmlItem(r, c);
				if (currItem != NULL && currItem->isSpanCell()) c = sel.rightCol()+1;
			}
		}
	}
	QApplication::clipboard()->setText(str);
}//IF
else {
	QString str = "<table>";
	int numSel = numSelections();
	if (numSel <= 0) {
		selectCells(currentRow(), currentColumn(), currentRow(), currentColumn());
		numSel = numSelections();
	}
	XmlTableItem* currItem;
	for (int k = 0; k < numSel; k++) {
		QTableSelection sel = selection(k);
		if (sel.isEmpty()) continue;
		for (int r=sel.topRow(); r <= sel.bottomRow(); r++) {
			str += "\n<tr>";
			for (int c = sel.leftCol(); c <= sel.rightCol(); c++) {
				currItem = xmlItem(r, c);
				if (currItem != NULL) {
					str += QString("<td class='%1' depth='%2'>")
						.arg( style(currItem->itemType())->name() )
						.arg( currItem->depth()-1 );
				}
				else str += "<td>";
				str += encodeToXml(text(r, c));
				if (currItem != NULL && currItem->isSpanCell()) c = sel.rightCol()+1;
				str += "</td>";
			}
			str += "</tr>";
		}
	}
	str += "\n</table>";
	QApplication::clipboard()->setText(str);
}//ELSE
}


void XmlTable::copyXml() {
	QString str;
	int numSel = numSelections();
	if (numSel <= 0) {
		selectCells(currentRow(), currentColumn(), currentRow(), currentColumn());
		numSel = numSelections();
	}
	QTextStream ts( &str, IO_WriteOnly );
	for (int k = 0; k < numSel; k++) {
		QTableSelection sel = selection(k);
		if (sel.isEmpty()) continue;
		if (k > 0) ts << endl;
		printDocument(ts, sel.topRow(), 0, sel.bottomRow()-sel.topRow()+1, max_cols+1);
	}
	QApplication::clipboard()->setText(str);
}


void XmlTable::shiftLeft() {
	int numSel = numSelections();
	if (numSel <= 0) selectCells(currentRow(), currentColumn(), currentRow(), currentColumn());
	QTableSelection sel = selection(0);
	if (sel.isEmpty()) return;
	//if (sel.leftCol() == 0) return;
	setModified(TRUE);
	if (sel.leftCol() == 0) {
		addUndoRecord(sel.topRow(), 0, sel.bottomRow(), 0);
		for (int r=sel.topRow(); r <= sel.bottomRow(); r++) {
			XmlTableItem *currItem = xmlItem(r, 0);
			if (currItem != NULL) currItem->decreaseDepth();
			//updateCell(r, 0);
		}
		updateColumn(0);
		return;
	}
	else if (sel.leftCol() == 1) {
		return;
	}
	int top = sel.topRow(), bottom = sel.bottomRow(), left = sel.leftCol(), right = sel.rightCol();
	addUndoRecord(top, 1, bottom, numCols()-1);
	int r, c;
	for (r=sel.topRow(); r <= sel.bottomRow(); r++) {
		for (c = sel.leftCol(); c < numCols(); c++) {
			QTableItem* currItem = item(r, c);
			if (currItem != NULL) currItem->setSpan(1, 1);
		}
		if (sel.leftCol() > 0) clearCell(r, sel.leftCol()-1);
		for (c = sel.leftCol(); c < numCols(); c++) {
			if (c > 0) swapCells(r, c, r, c-1);
		}
		updateRow(r);
	}

	if (left > 0) {left--; right--;}
	clearSelection(TRUE);
	setCurrentCell(top, left);
	selectCells( top, left, bottom, right );
//	adjustAll();
}


void XmlTable::shiftRight() {
	int numSel = numSelections();
	if (numSel <= 0) selectCells(currentRow(), currentColumn(), currentRow(), currentColumn());
	QTableSelection sel = selection(0);
	if (sel.isEmpty()) return;
	setModified(TRUE);
	if (sel.leftCol() == 0) {
		addUndoRecord(sel.topRow(), 0, sel.bottomRow(), 0);
		for (int r=sel.topRow(); r <= sel.bottomRow(); r++) {
			XmlTableItem *currItem = xmlItem(r, 0);
			if (currItem != NULL) currItem->increaseDepth();
			//updateCell(r, 0);
		}
		updateColumn(0);
		return;
	}
	else if (sel.rightCol() == numCols()-1) {
		return;
	}
	int top = sel.topRow(), bottom = sel.bottomRow(), left = sel.leftCol(), right = sel.rightCol();
	addUndoRecord(top, 1, bottom, numCols()-1);
	int r, c;
	for (r=sel.topRow(); r <= sel.bottomRow(); r++) {
		for (c = sel.leftCol(); c < numCols(); c++) {
			QTableItem* currItem = item(r, c);
			if (currItem != NULL) currItem->setSpan(1, 1);
		}
		if ( !text(r, numCols()-1).isEmpty() ) {
			setNumCols( numCols()+1 );
		}
		for (c = numCols()-1; c > sel.leftCol(); c--) {
			swapCells(r, c, r, c-1);
		}
		updateRow(r);

	}

	if (right < numCols()-1) {left++; right++;}
	clearSelection(TRUE);
	setCurrentCell(top, left);
	selectCells( top, left, bottom, right );
	//adjustAll();
}


XmlTableItem::NodeType XmlTable::itemTypeForString(const QString & str) {
	for (int i = XmlTableItem::EMPTY; i <= XmlTableItem::COMMENT; i++) {
		if (str == style((XmlTableItem::NodeType)i)->name()) return (XmlTableItem::NodeType)i;
	}
	return XmlTableItem::EMPTY;
}


void XmlTable::paste() {
	int rpos, cpos;
	if (numSelections() == 0) {
		selectCells(currentRow(), currentColumn(), currentRow(), currentColumn());
	}
	QTableSelection sel = selection(0);
	rpos = sel.topRow(); cpos = sel.leftCol();

	QString str = QApplication::clipboard()->text();
	QDomDocument xmldoc;
	bool isXml = xmldoc.setContent(str);

	int minX = numCols(), minY = numRows(), maxX = 0, maxY = 0;
	if (isXml) {
		QDomElement root = xmldoc.documentElement();
		QDomNodeList trList = root.childNodes();
		for (uint i = 0, r = 0; i < trList.length(); i++) {
			QDomNode trNode = trList.item(i);
			if (!(trNode.isElement() && trNode.nodeName() == "tr")) continue;
			QDomNodeList tdList = trNode.childNodes();
			for (uint j = 0, c = 0; j < tdList.length(); j++) {
				QDomNode tdNode = tdList.item(j);
				if (!(tdNode.isElement() && tdNode.nodeName() == "td")) continue;
				int row = rpos + r;
				int col = cpos + c;
				if (row < numRows() && col < numCols()) {
					//minX = minX <? col;
					minX = (minX < col) ? minX : col;
					//minY = minY <? row;
					minY = (minY < row) ? minY : row;
					//maxX = maxX >? col;
					maxX = (maxX > col) ? maxX : col;
					//maxY = maxY >? row;
					maxY = (maxY > row) ? maxY : row;
				}
				c++;
			}
			r++;
		}
		addUndoRecord(minY, minX, maxY, maxX);

		for (uint i = 0, r = 0; i < trList.length(); i++) {
			QDomNode trNode = trList.item(i);
			if (!(trNode.isElement() && trNode.nodeName() == "tr")) continue;
			QDomNodeList tdList = trNode.childNodes();
			for (uint j = 0, c = 0; j < tdList.length(); j++) {
				QDomNode tdNode = tdList.item(j);
				if (!(tdNode.isElement() && tdNode.nodeName() == "td")) continue;
				int row = rpos + r;
				int col = cpos + c;
				if (row < numRows() && col < numCols()) {
					QDomElement currElem = tdNode.toElement();
					setText(row, col, currElem.text());
					XmlTableItem* currXmlItem = xmlItem(row, col);
					if (currXmlItem != NULL) {
						currXmlItem->setSpan( 1,1 );
						currXmlItem->setItemType( itemTypeForString( currElem.attribute("class", "") ) );
						if (col == 0) currXmlItem->setDepth( currElem.attribute("depth", "").toInt() );
					}
					updateCell(row, col);
				}
				c++;
			}
			r++;
		}
	}//IF
	else {
		QStringList rows = QStringList::split("\n", str, true);
		for (int i = 0, n = rows.size(); i < n; i++) {
			QStringList cols = QStringList::split("\t", rows[i], true);
			for (int j = 0, m = cols.size(); j < m; j++) {
				int row = rpos + i;
				int col = cpos + j;
				if (row < numRows() && col < numCols()) {
					//minX = minX <? col;
					minX = (minX < col) ? minX : col;
					//minY = minY <? row;
					minY = (minY < row) ? minY : row;
					//maxX = maxX >? col;
					maxX = (maxX > col) ? maxX : col;
					//maxY = maxY >? row;
					maxY = (maxY > row) ? maxY : row;
				}
			}
		}
		addUndoRecord(minY, minX, maxY, maxX);

		for (int i = 0, n = rows.size(); i < n; i++) {
			QStringList cols = QStringList::split("\t", rows[i], true);
			for (int j = 0, m = cols.size(); j < m; j++) {
				int row = rpos + i;
				int col = cpos + j;
				if (row < numRows() && col < numCols()) {
					setText(row, col, cols[j]);
					XmlTableItem* currXmlItem = xmlItem(row, col);
					if (currXmlItem != NULL) {
						currXmlItem->setSpan( 1,1 );
						currXmlItem->setItemType(XmlTableItem::EMPTY);
					}
					updateCell(row, col);
				}
			}
		}
	}//ELSE
	setModified(TRUE);
}


void XmlTable::pasteXml() {
	int rpos, cpos;
	if (numSelections() == 0) {
		selectCells(currentRow(), currentColumn(), currentRow(), currentColumn());
	}
	QTableSelection sel = selection(0);
	rpos = sel.topRow(); cpos = sel.leftCol();

	QString str = "<paste>"; str.append(
	QApplication::clipboard()->text() ); str.append("</paste>");
	QDomDocument xmldoc; bool isXml = xmldoc.setContent(str); if (isXml)
	{
		int currDepth = (rpos > 0) ? depth(rpos-1) : depth(rpos);
		int lastLevel = currDepth;
		QStringList *lastAttrList = NULL;

		QDomElement root = xmldoc.documentElement();
		QDomNodeList trList = root.childNodes();
		for (uint i = 0; i < trList.length(); i++) {
			QDomNode trNode = trList.item(i);
			layoutNode(rpos, currDepth, &trNode, &lastAttrList, &lastLevel);
		}
		if (lastAttrList != NULL) delete lastAttrList;
		resetUndo(); resetRedo();
	}//IF
	else {
		addUndoRecord(rpos, cpos, rpos, cpos);
		setText(rpos, cpos, QApplication::clipboard()->text());
	}
	setModified(TRUE);
}



void XmlTable::del() {
	int numSel = numSelections();
	if (numSel <= 0) {
		selectCells(currentRow(), currentColumn(), currentRow(), currentColumn());
		numSel = numSelections();
	}

	int minX = numCols(), minY = numRows(), maxX = 0, maxY = 0;
	for (int k = 0; k < numSel; k++) {
		QTableSelection sel = selection(k);
		if (sel.isEmpty()) continue;
		for (int r=sel.topRow(); r <= sel.bottomRow(); r++) {
			for (int c = sel.leftCol(); c <= sel.rightCol(); c++) {
				//minX = minX <? c;
				minX = (minX < c) ? minX : c;
				//minY = minY <? r;
				minY = (minY < r) ? minY : r;
				//maxX = maxX >? c;
				maxX = (maxX > c) ? maxX : c;
				//maxY = maxY >? r;
				maxY = (maxY > r) ? maxY : r;
			}
		}
	}
	addUndoRecord(minY, minX, maxY, maxX);
	for (int k = 0; k < numSel; k++) {
		QTableSelection sel = selection(k);
		if (sel.isEmpty()) continue;
		for (int r=sel.topRow(); r <= sel.bottomRow(); r++) {
			for (int c = sel.leftCol(); c <= sel.rightCol(); c++) {
				clearCell(r, c);
			}
		}
	}
	setModified(TRUE);
}


XmlTableItem::XmlTableItem(NodeType type, QTable *t, EditType et, const QString &txt )
	: QTableItem( t, et ) {
	setItemType(type);
	QString rtxt = txt;
	//rtxt.replace(QChar('\t'), QChar(' '));
	setText(rtxt);
	setSizeHint(TRUE);
	setDepth(0);
	setExpanded(true);
	commentDepth_ = 1;
}


void XmlTableItem::setItemType(NodeType type) {
	this->type = type;
	setWordWrap(XmlTable::style(type)->isWordWrap());
}


int XmlTableItem::alignment() const {
	return XmlTable::style(type)->alignment();
}


int XmlTableItem::spanWidth() const {
	int retval = 0;
	for (int currCol = col(), i = 0; i < colSpan(); i++, currCol++) {
		retval += table()->columnWidth(currCol);
	}
	return retval;
}



int XmlTableItem::countWrapedLines(const QString & str, int maxWidth, const QFontMetrics & metric, int wordWrap) const {
	int h = 1;
	int currWidth = 0;
	int lineIndex = 0;
	int wordWidth = 0;
	int charWidth = 0;
	QChar currChar;

	for (uint i = 0, limit = 0; i < str.length() && limit < 1000; i++, limit++) {
		currChar = str.at(i);
		if (currChar == '\n') {//new line
			h++;
			currWidth = 0;
			lineIndex = i;
			continue;
		}
		charWidth = metric.charWidth(str, i);
		currWidth += charWidth;
		if (currChar.isSpace()) wordWidth = 0;
		else wordWidth += charWidth;

		if (currWidth < maxWidth) continue;

		if (currChar.isSpace()) {//space at end of line -> absorb remaining spaces
			do {
				i++;
				if (i < str.length()) currChar = str.at(i);
			} while (i < str.length() && currChar.isSpace());
			h++;
			currWidth = 0;
			lineIndex = i;
		}
		else {//character at end of line
			int j = i;
			if (wordWrap == Qt::BreakAnywhere && wordWidth >= maxWidth) {//break word
				do {
					j--;
					currChar = str.at(j);
					charWidth = metric.charWidth(str, j);
					wordWidth -= charWidth;
				} while (!currChar.isSpace() && wordWidth >= maxWidth);
			}
			else {//go back to last space
				do {
					j--;
					if (j > lineIndex) currChar = str.at(j);
				} while (!currChar.isSpace() && j > lineIndex);
			}
			if (j > lineIndex) {
				i = j;
				h++;
				currWidth = 0;
				wordWidth = 0;
				lineIndex = i;
			}
		}
	}
	return h;
}


QSize XmlTableItem::sizeHint() const {
	return size;
}


void XmlTableItem::setSizeHint(bool isInit) {
	QFontMetrics metric(XmlTable::style(type)->font());
	if (isInit) {
		size.setWidth( 100 );
		size.setHeight( metric.height()+1 );
		return;
	}
	QString val = text();

	int w = XmlTable::minColumnWidth, h = 0;
	int oneCharWidth = metric.width('9');

	if (XmlTable::style(type)->isWordWrap()) {
		//int maxWidth = spanWidth() >? oneCharWidth*5;
		int maxWidth = (( spanWidth() > oneCharWidth*5 )? spanWidth() :oneCharWidth*5 );
		h = countWrapedLines(
			val, maxWidth-2, metric, XmlTable::style(type)->wordWrap()
		);
	}
	else {
		QStringList slist = QStringList::split('\n', val, TRUE);
		h = slist.size();
		QStringList::iterator it;
		int currWidth;
		for ( it = slist.begin(); it != slist.end(); ++it ) {
			currWidth = metric.width(*it, (*it).length()) + oneCharWidth;
			if (currWidth > w) w = currWidth;
		}
	}
	int dh = metric.lineSpacing();

	h = h * dh;
	if (type == EMPTY) {
		if (w < XmlTable::minColumnWidth) w = XmlTable::minColumnWidth;
	}
	if (type == ELEMENT) {
		int dh = XmlTable::indentWidth;//( metric.lineSpacing()/2 )*2;
		size.setWidth(w + dh*(hierDepth-1));
	}
	else size.setWidth(w);
	size.setHeight(h);
}


void XmlTableItem::setDepth(int d) {
	hierDepth = d+1;
}


int XmlTable::depth(int row) {
	XmlTableItem* curr = xmlItem(row, 0);
	if (curr == NULL || curr->itemType() == XmlTableItem::EMPTY) {
		int nextLoc = -1, prevLoc = -1;
		int nextD = nextDepth(row, &nextLoc);
		int prevD = previousDepth(row, &prevLoc);

		int retval;
		//if (nextD != prevD) retval = nextD <? prevD;
		if (nextD != prevD) {retval = ((nextD < prevD) ? nextD : prevD);}
		else retval = nextD - 1;
		return retval;
	}
	return curr->depth();
}


void XmlTable::paintFocus( QPainter * p, const QRect & cr ) {
	QPen pen(focusBorderColor, 3, Qt::DashLine, Qt::FlatCap, Qt::RoundJoin);
	p->setPen( pen );
	p->drawRect( -1,-1, cr.width()+1,cr.height()+1 );
}


void XmlTableItem::setExpanded(bool isExpanded) {
	this->isExpanded = isExpanded;
}


bool XmlTableItem::expanded() {
	return this->isExpanded;
}


bool XmlTable::inExpandBox(int row, int col, const QPoint &p) {
	QFontMetrics metric(XmlTable::style(XmlTableItem::ELEMENT)->font());
	int dy = metric.lineSpacing()/2;
	int y = rowPos(row) + dy;
	int x = columnPos(col) + indentWidth * (depth(row)-1) - indentWidth/2;
	return p.x() >= x-dy && p.x() <= x+dy && p.y() >= y-dy && p.y() <= y+dy;
}


void XmlTable::paintCell(QPainter * p, int row, int col, const QRect & cr, bool selected, const QColorGroup & /*cg*/) {
	p->save();
	XmlTableItem::NodeType type = itemType(row, col);
	QColor bgColor, fgColor;
	if ( selected && !(row == currentRow() && col == currentColumn()) ) {
		fgColor = XmlTable::style(type)->bgColor(),
		bgColor = XmlTable::style(type)->fgColor();
	}
	else {
		bgColor = XmlTable::style(type)->bgColor(),
		fgColor = XmlTable::style(type)->fgColor();
	}
	p->setFont(XmlTable::style(type)->font());
	p->setTabStops(4);

	if (type == XmlTableItem::ELEMENT || type == XmlTableItem::EMPTY && col == 0) {
		p->setPen( QPen(borderColor, 2) );
		p->setBrush( NoBrush );
		p->setBackgroundColor(XmlTable::style(XmlTableItem::EMPTY)->bgColor());

		QFontMetrics metric(XmlTable::style(XmlTableItem::ELEMENT)->font());
		int dh = metric.lineSpacing();
		int dw = indentWidth;
		int w = cr.width(), h = cr.height();

		p->eraseRect(0,0, w, h);
		p->setPen( QPen(borderColor, 1) );

		if (type == XmlTableItem::EMPTY) {
			for (int i = 0; i < depth(row); i++) {
				p->setPen( QPen(emptyBorderColor, 1, DotLine) );
				p->drawLine(1,h-1, dw, h-1);//bottom
				p->drawLine(dw,1, dw, h);//right

				if (displayBranches) {
					p->setPen( QPen(borderColor, 1) );
					int nextD = nextDepth(row);
					int prevD = previousDepth(row);
					if (nextD < prevD && prevD == i+2);
					else if (nextD > prevD && nextD == i+2) p->drawLine(dw/2,0, dw/2,h);
					else if (hasSibling(previousRowWithDepth(row, i+2)) == -1);
					else p->drawLine(dw/2,0, dw/2,h);
				}
				p->translate(dw, 0);
			}
			p->fillRect(0, 0, w - dw*depth(row), h, QBrush(bgColor) );
		}
		else if (type == XmlTableItem::ELEMENT) {
			for (int i = 1; i < depth(row) && col == 0; i++) {
				p->setPen( QPen(emptyBorderColor, 1, DotLine) );
				p->drawLine(1,h-1, dw, h-1);//bottom
				p->drawLine(dw,1, dw, h);//right

				if (displayBranches) {
				p->setPen( QPen(borderColor, 1) );
				if (i+1 == depth(row)) {
					int nextD = nextDepth(row);
					int thisD = depth(row);
					if (hasSibling(row) < 0) p->drawLine(dw/2,0, dw/2,dh/2);//top only
					else if (i > nextDepth(row)) p->drawLine(dw/2,0, dw/2,dh/2);//top only
					else if (nextD < thisD && thisD == i+1) p->drawLine(dw/2,0, dw/2,dh/2);//top only
					else p->drawLine(dw/2,0, dw/2,h);//top to bottom
					p->drawLine(dw/2,dh/2, dw,dh/2);//right

					if (nextDepth(row) <= depth(row)) {
						if (xmlItem(row, col)->expanded()) {
							p->fillRect(dw/2-1, dh/2-1, 3, 3, QBrush(borderColor) );
						}
						else {
							p->fillRect(dw/2-4, dh/2-4, 9, 9, QBrush(QColor("pink")) );
							p->drawRect(dw/2-4, dh/2-4, 9, 9);
							p->drawLine(dw/2-4, dh/2, dw/2+4, dh/2);
							p->drawLine(dw/2, dh/2-4, dw/2, dh/2+4);
						}
					}
					else {
						if (xmlItem(row, col)->expanded()) {
							p->fillRect(dw/2-4, dh/2-4, 9, 9, QBrush(QColor("lightgreen")) );
							p->drawRect(dw/2-4, dh/2-4, 9, 9);
							p->drawLine(dw/2-4, dh/2, dw/2+4, dh/2);
						}
						else {
							p->fillRect(dw/2-4, dh/2-4, 9, 9, QBrush(QColor("pink")) );
							p->drawRect(dw/2-4, dh/2-4, 9, 9);
							p->drawLine(dw/2-4, dh/2, dw/2+4, dh/2);
							p->drawLine(dw/2, dh/2-4, dw/2, dh/2+4);
						}
					}
				}
				else {
					int nextD = nextDepth(row);
					int prevD = previousDepth(row);
					if (nextD < prevD && prevD == i+1);
					else if (nextD > prevD && nextD == i+1) p->drawLine(dw/2,0, dw/2,h);
					else if (hasSibling(previousRowWithDepth(row, i+1)) == -1);
					else {
						if (i > nextDepth(row)) {
							p->drawLine(dw/2,0, dw/2,dh/2);//top
							p->fillRect(dw/2-1, dh/2-1, 3, 3, QBrush(borderColor) );
						}
						else p->drawLine(dw/2,0, dw/2,h);//to to bottom
					}
				}
				}

				p->translate(dw, 0);
			}
			if (col == 0) w = w - dw*(depth(row)-1);

			p->fillRect(0, 0, w, h, QBrush(bgColor) );
			p->setPen( QPen(fgColor, 1) );
			p->drawText(0, 0, w, h, XmlTable::style(type)->textFlags(), text(row, col), -1, 0);

			p->setPen( QPen(borderColor, 1) );
			p->drawRect(-1,-1, w+1, h+1);
		}
	}
	else {
		XmlTableItem* xitem = xmlItem(row, col);
		int w = cr.width(), h = cr.height();
		if (xitem != NULL) w = xitem->spanWidth();
		p->fillRect(0, 0, w, h, QBrush(bgColor) );
		p->setPen( QPen(fgColor, 1) );
		p->drawText(0, 0, w, h, XmlTable::style(type)->textFlags(), text(row, col), -1, 0);
		if (type != XmlTableItem::EMPTY) {
			p->setPen( QPen(borderColor, 1) );
			p->drawRect(-1,-1, w+1, h+1);
		}
	}
	p->restore();
	if (type == XmlTableItem::EMPTY) {
		int w = cr.width()-1, h = cr.height()-1;
		p->setPen( QPen(emptyBorderColor, 1, DotLine) );
		if (itemType(row+1, col) == XmlTableItem::EMPTY) p->drawLine(1,h, w, h);//bottom
		if (itemType(row, col+1) == XmlTableItem::EMPTY) p->drawLine(w,1, w, h);//right
	}
}



int XmlTable::maxRows() {
	return max_rows;
}
int XmlTable::maxCols() {
	return max_cols;
}


int XmlTableItem::nextDepth() {
	XmlTable* xtable = (XmlTable*) table();
	for (int r = row()+1; r <= xtable->maxRows() /*< xtable->numRows()*/; r++) {
		XmlTableItem* itemBelow = xtable->xmlItem(r, col());
		if (itemBelow != NULL && itemBelow->itemType() == ELEMENT) {
			return itemBelow->depth();
		}
	}
	return 0;
}


int XmlTableItem::previousDepth() {
	XmlTable* xtable = (XmlTable*) table();
	for (int r = row()-1; r >= 0; r--) {
		XmlTableItem* itemAbove = xtable->xmlItem(r, col());
		if (itemAbove != NULL && itemAbove->itemType() == ELEMENT) {
			return itemAbove->depth();
		}
	}
	return 0;
}





bool XmlTable::isParent(int row) {
	int currDepth = depth(row);
	for (int r = row+1; r <= max_rows /*< numRows()*/; r++) {
		XmlTableItem* itemBelow = xmlItem(r, 0);
		if (itemBelow != NULL && itemBelow->itemType() == XmlTableItem::ELEMENT) {
			if (itemBelow->depth() > currDepth) return TRUE;
			if (itemBelow->depth() <= currDepth) return FALSE;
		}
	}
	return FALSE;
}


bool XmlTable::isLastSibling(int row) {
	int currDepth = depth(row);
	for (int r = row+1; r <= max_rows /*< numRows()*/; r++) {
		XmlTableItem* itemBelow = xmlItem(r, 0);
		if (itemBelow != NULL && itemBelow->itemType() == XmlTableItem::ELEMENT) {
			if (itemBelow->depth() >= currDepth) return FALSE;
			if (itemBelow->depth() < currDepth) return TRUE;
		}
	}
	return TRUE;
}


int XmlTable::hasSibling(int row, int selDepth) {
	int currDepth = selDepth;
	if (currDepth < 0) currDepth = depth(row);
	for (int r = row+1; r <= max_rows /*< numRows()*/; r++) {
		XmlTableItem* itemBelow = xmlItem(r, 0);
		if (itemBelow != NULL && itemBelow->itemType() == XmlTableItem::ELEMENT) {
			if (itemBelow->depth() == currDepth) return r;
			if (itemBelow->depth() < currDepth) return -1;
		}
	}
	return -1;
}


bool XmlTable::isFirstSibling(int row) {
	int currDepth = depth(row);
	for (int r = row-1; r >= 0; r--) {
		XmlTableItem* itemAbove = xmlItem(r, 0);
		if (itemAbove != NULL && itemAbove->itemType() == XmlTableItem::ELEMENT) {
			if (itemAbove->depth() > currDepth) return FALSE;
			if (itemAbove->depth() <= currDepth) return TRUE;
		}
	}
	return TRUE;
}


int XmlTable::nextDepth(int row, int* loc) {
	for (int r = row+1; r <= max_rows /*< numRows()*/; r++) {
		XmlTableItem* itemBelow = xmlItem(r, 0);
		if (itemBelow != NULL && itemBelow->itemType() == XmlTableItem::ELEMENT) {
			if (loc != NULL) *loc = r;
			return itemBelow->depth();
		}
	}
	if (loc != NULL) *loc = -1;
	return 0;
}


int XmlTable::previousDepth(int row, int* loc) {
	for (int r = row-1; r >= 0; r--) {
		XmlTableItem* itemAbove = xmlItem(r, 0);
		if (itemAbove != NULL && itemAbove->itemType() == XmlTableItem::ELEMENT) {
			if (loc != NULL) *loc = r;
			return itemAbove->depth();
		}
	}
	if (loc != NULL) *loc = 0;
	return 0;
}


int XmlTable::previousRowWithDepth(int startRow, int depth) {
	for (int r = startRow-1; r >= 0; r--) {
		XmlTableItem* itemAbove = xmlItem(r, 0);
		if (itemAbove != NULL && itemAbove->itemType() == XmlTableItem::ELEMENT) {
			if (itemAbove->depth() == depth) return r;
		}
	}
	return 0;
}



bool XmlTableItem::isSpanCell() const {
	switch (type) {
		case TEXT:
		case CDATA:
		case COMMENT:
			return TRUE;
		default:
			return FALSE;
	}
}


QWidget* XmlTableItem::createEditor() const {
	XmlTable *xtable = (XmlTable*)table();
	xtable->setEditingLocation(row(), col());
	switch (type) {
		case EMPTY:
		case ATTR_KEY:
		case ATTR_VAL:
		case ELEMENT: {
			QLineEdit* w = (QLineEdit*) QTableItem::createEditor();
			w->setPaletteBackgroundColor(XmlTable::style(type)->bgColor());
			w->setAlignment(alignment());
			w->setFont(XmlTable::style(type)->font());
			return w;
		}
		case TEXT:
		case CDATA:
		case COMMENT: {
			QTextEdit *textEdit = new QTextEdit(table()->viewport());
			textEdit->setText(text());
			textEdit->setWordWrap(
				XmlTable::style(type)->isWordWrap() ?
				QTextEdit::WidgetWidth : QTextEdit::NoWrap
			);
			textEdit->setPaletteBackgroundColor(XmlTable::style(type)->bgColor());
			textEdit->setAlignment(alignment());
			textEdit->setFont(XmlTable::style(type)->font());
			textEdit->setTabStopWidth(4);
			textEdit->setTextFormat(Qt::PlainText);
			textEdit->setHScrollBarMode(QScrollView::AlwaysOff);
			textEdit->setLineWidth(0);
			textEdit->setMidLineWidth(0);
			return textEdit;
		}
		default:
			return QTableItem::createEditor();
	}
}


void XmlTable::setEditingLocation(int r, int c) {
	editingRow = r;
	editingColumn = c;
}


void XmlTable::setCellContentFromEditor( int row, int col ) {
	QTableItem *curritem = item(row, col);
	if (curritem == NULL) {
		addUndoRecord(row, col, row, col);
	}
	QTable::setCellContentFromEditor(row, col);
}


void XmlTableItem::setContentFromEditor( QWidget *w ) {
	bool modified;
	XmlTable* xtable = (XmlTable*)table();
	if (w->inherits("QTextEdit")) {
		QTextEdit* ed = (QTextEdit*) w;
		QString oldstr = text();
		QString newstr = ed->text();
		if (oldstr.isNull()) oldstr = "";
		if (newstr.isNull()) newstr = "";
		if (oldstr != newstr) {
			xtable->addUndoRecord(row(), col(), row(), col());
		}
		setText(ed->text());
		modified = ed->isModified();
	}
	else {
		QLineEdit* ed = (QLineEdit*) w;
		QString oldstr = text();
		QString newstr = ed->text();
		if (oldstr.isNull()) oldstr = "";
		if (newstr.isNull()) newstr = "";
		if (oldstr != newstr) {
			xtable->addUndoRecord(row(), col(), row(), col());
		}
		modified = ed->isModified();
		QTableItem::setContentFromEditor(w);
	}
	if (modified) {
		xtable->setModified(TRUE);
		xtable->adjustRow(row());
		xtable->adjustColumn(col());
	}
}


XmlTableItem::NodeType XmlTableItem::itemType() const {
	return type;
}



void XmlTable::defaultSettings() {
	styleVector.setAutoDelete(TRUE);
	if (styleVector.isEmpty()) {
		for (int type = XmlTableItem::EMPTY; type <= XmlTableItem::COMMENT; type++) {
			styleVector.insert(type, new XmlTableStyle());
		}
	}
	QFont currFont;
	int align = Qt::AlignLeft | Qt::AlignTop;
	int wrap = Qt::BreakAnywhere;
	QString fg = "#000000", bg, name;
	for (int type = XmlTableItem::EMPTY; type <= XmlTableItem::COMMENT; type++) {
		XmlTableStyle* currStyle = styleVector[type];
		switch (type) {
			case XmlTableItem::ELEMENT:
				name = "element";
				currFont = QFont("Helvetica", 10, QFont::Bold, FALSE);
				align = Qt::AlignRight | Qt::AlignTop;
				wrap = Qt::SingleLine;
				bg = "#BBBBFF";
				break;
			case XmlTableItem::ATTR_KEY:
				name = "key";
				currFont = QFont("Arial", 9, QFont::Bold, FALSE);
				align = Qt::AlignHCenter | Qt::AlignBottom;
				wrap = Qt::SingleLine;
				bg = "#BBBBBB";
				break;
			case XmlTableItem::ATTR_VAL:
				name = "value";
				currFont = QFont("Arial", 10, QFont::Normal, FALSE);
				align = Qt::AlignHCenter | Qt::AlignTop;
				wrap = Qt::BreakAnywhere;
				bg = "#DDDDDD";
				break;
			case XmlTableItem::TEXT:
				name = "text";
				currFont = QFont("Andale Mono", 10, QFont::Normal, FALSE);
				align = Qt::AlignLeft | Qt::AlignTop;
				wrap = Qt::WordBreak;
				bg = "#FFBBBB";
				break;
			case XmlTableItem::CDATA:
				name = "CDATA";
				currFont = QFont("Andale Mono", 10, QFont::Normal, FALSE);
				align = Qt::AlignLeft | Qt::AlignTop;
				wrap = Qt::BreakAnywhere;
				bg = "#BBFFBB";
				break;
			case XmlTableItem::COMMENT:
				name = "comment";
				currFont = QFont("Times New Roman", 10, QFont::Normal, TRUE);
				align = Qt::AlignLeft | Qt::AlignTop;
				wrap = Qt::WordBreak;
				bg = "#FFFFFF";
				break;
			case XmlTableItem::EMPTY:
				name = "empty";
				currFont = QFont("Helvetica", 10, QFont::Normal, FALSE);
				align = Qt::AlignLeft | Qt::AlignTop;
				wrap = Qt::WordBreak;
				bg = "#FFFFFF";
				break;
		}
		currStyle->setName(name);
		currStyle->setFont(currFont.toString());
		currStyle->setAlignment(align);
		currStyle->setWordWrap(wrap);
		currStyle->setBgColor(bg);
		currStyle->setFgColor(fg);
	}
	minColumnWidth = 50;
}


void XmlTable::applySettings() {
	XmlTable::defaultSettings();
	QSettings settings;
	settings.setPath("gravitybind.com", "teddy", QSettings::User);
	settings.beginGroup("/Teddy/XmlTable");
	for (int type = XmlTableItem::EMPTY; type <= XmlTableItem::COMMENT; type++) {
		XmlTableStyle* currStyle = styleVector[type];
		QString name = currStyle->name();
		currStyle->setFont(
			settings.readEntry( QString("/%1/font").arg(name), currStyle->font().toString() )
		);
		currStyle->setAlignment(
			settings.readNumEntry( QString("/%1/align").arg(name), currStyle->alignment() )
		);
		currStyle->setWordWrap(
			settings.readNumEntry( QString("/%1/wrap").arg(name), currStyle->wordWrap() )
		);
		currStyle->setBgColor(
			settings.readEntry( QString("/%1/bgColor").arg(name), currStyle->bgColor().name() )
		);
		currStyle->setFgColor(
			settings.readEntry( QString("/%1/fgColor").arg(name), currStyle->fgColor().name() )
		);
	}
	settings.endGroup();
}


void XmlTable::saveSettings() {
	QSettings settings;
	settings.setPath("gravitybind.com", "teddy", QSettings::User);
	settings.beginGroup("/Teddy/XmlTable");
	for (int type = XmlTableItem::EMPTY; type <= XmlTableItem::COMMENT; type++) {
		XmlTableStyle* currStyle = styleVector[type];
		QString name = currStyle->name();
		settings.writeEntry( QString("/%1/font").arg(name), currStyle->font().toString() );
		settings.writeEntry( QString("/%1/align").arg(name), currStyle->alignment() );
		settings.writeEntry( QString("/%1/wrap").arg(name), currStyle->wordWrap() );
		settings.writeEntry( QString("/%1/bgColor").arg(name), currStyle->bgColor().name() );
		settings.writeEntry( QString("/%1/fgColor").arg(name), currStyle->fgColor().name() );
	}
	settings.endGroup();
}


XmlTableStyle* XmlTable::style(int type) {
	return styleVector[type];
}


