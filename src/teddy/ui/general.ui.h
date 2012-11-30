/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/

#include <qcolordialog.h>


void GeneralPref::init() {
	setDefaults();
}


void GeneralPref::setDefaults() {
	emptyBorderColor = QColor("gray");
	filledBorderColor = QColor(0,0,0);
	focusBorderColor = QColor(0,0,0);
	setColorIcon();
	
	setNumRecentDocs(20);
	setIndentWidth(30);
	setSortAttr(true);
}

void GeneralPref::setColorIcon() {
	QPixmap ePixmap(19,19), fPixmap(19,19), focPixmap(19,19);
	ePixmap.fill(emptyBorderColor);
	fPixmap.fill(filledBorderColor);
	focPixmap.fill(focusBorderColor);
	button_emptyBorder->setPixmap(ePixmap);
	button_filledBorder->setPixmap(fPixmap);
	button_focusBorder->setPixmap(focPixmap);
}


QColor GeneralPref::emptyColor() {
	return emptyBorderColor;
}


void GeneralPref::setEmptyColor(QColor c) {
	emptyBorderColor = c;
	setColorIcon();
}


QColor GeneralPref::filledColor() {
	return filledBorderColor;
}


void GeneralPref::setFilledColor(QColor c) {
	filledBorderColor = c;
	setColorIcon();
}


QColor GeneralPref::focusColor() {
	return focusBorderColor;
}


void GeneralPref::setFocusColor(QColor c) {
	focusBorderColor = c;
	setColorIcon();
}


void GeneralPref::emptyBorderDialog() {
	QColor currColor = emptyBorderColor;
	currColor = QColorDialog::getColor( currColor, this );
	if (currColor.isValid()) {
		emptyBorderColor = currColor;
		setColorIcon();
	}
}


void GeneralPref::filledBorderDialog() {
	QColor currColor = filledBorderColor;
	currColor = QColorDialog::getColor( currColor, this );
	if (currColor.isValid()) {
		filledBorderColor = currColor;
		setColorIcon();
	}
}


void GeneralPref::focusBorderDialog() {
	QColor currColor = focusBorderColor;
	currColor = QColorDialog::getColor( currColor, this );
	if (currColor.isValid()) {
		focusBorderColor = currColor;
		setColorIcon();
	}
}


int GeneralPref::numRecentDocs() {
	return spinBox_recentDocs->value();
}


void GeneralPref::setNumRecentDocs(int val) {
	spinBox_recentDocs->setValue(val);
}


int GeneralPref::indentWidth() {
	return spinBox_indent->value();
}


void GeneralPref::setIndentWidth(int width) {
	spinBox_indent->setValue(width);
}


int GeneralPref::minColumnWidth() {
	return spinBox_minColumnWidth->value();
}


void GeneralPref::setMinColumnWidth(int width) {
	spinBox_minColumnWidth->setValue(width);
}


bool GeneralPref::sortAttr() {
	return checkBox_sortAttr->isChecked();
}


void GeneralPref::setSortAttr(bool val) {
	checkBox_sortAttr->setChecked(val);
}


bool GeneralPref::drawBranches() {
	return checkBox_drawBranches->isChecked();
}


void GeneralPref::setDrawBranches(bool val) {
	checkBox_drawBranches->setChecked(val);
}
