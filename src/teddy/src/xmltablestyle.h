/***************************************************************************
 *   Copyright (C) 2005 by Jinsoo Kang                                     *
 *   jsk@earthlink.net                                                     *
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


class XmlTableStyle {

public:
	XmlTableStyle(){}
	
	QString fontName() {
		QString retval = QString("%1 %2")
			.arg(fontVal.family())
			.arg(fontVal.pointSize());
		return retval;
	}
	
	const QString & name() const {
		return nameVal;
	}
	const QFont & font() const {
		return fontVal;
	}
	const QColor & bgColor() const {
		return bgColorVal;
	}
	const QColor & fgColor() const {
		return fgColorVal;
	}
	int alignment() const {
		return align;
	}
	int wordWrap() const {
		return wrap;
	}
	bool isWordWrap() {
		return wrap == Qt::WordBreak || wrap == Qt::BreakAnywhere;
	}

	QString alignText() {
		if (align & Qt::AlignLeft) return "left";
		else if (align & Qt::AlignRight) return "right";
		else if (align & Qt::AlignHCenter) return "center";
		else return "left";
	}
	QString valignText() {
		if (align & Qt::AlignTop) return "top";
		else if (align & Qt::AlignBottom) return "bottom";
		else if (align & Qt::AlignVCenter) return "middle";
		else return "middle";
	}
	QString wordWrapText() {
		if (wrap & Qt::SingleLine) return "single line";
		else if (wrap & Qt::BreakAnywhere) return "break anywhere";
		else if (wrap & Qt::WordBreak) return "break on words";
		else return "break anywhere";
	}

	void setName(const QString & nameVal) {
		this->nameVal = nameVal;
	}
	void setFont(const QString & fontVal) {
		this->fontVal.fromString(fontVal);
	}
	void setFont(const QFont & fontVal) {
		this->fontVal = fontVal;
	}
	
	void setBgColor(const QString & bg) {
		bgColorVal.setNamedColor(bg);
	}
	void setFgColor(const QString & fg) {
		fgColorVal.setNamedColor(fg);
	}
	void setAlignment(int align) {
		this->align = align;
	}
	void setWordWrap(int wrap) {
		this->wrap = wrap;
	}
	
	int textFlags() const {
		return alignment() | wordWrap();
	}
	
private:
	QString nameVal;
	QFont fontVal;
	QColor bgColorVal, fgColorVal;
	int align;
	int wrap;
};


