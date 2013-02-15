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

#include "prefdialog.h"
#include "teddy.h"


PrefStyleEdit::PrefStyleEdit( int type, XmlTableStyle* style, QWidget * parent )
	: QWidget() {
	this->parent = parent;
	this->type = type;
	this->style = style;
	labelVal = new QLabel( style->name(), parent );
	bgButtonVal = new QPushButton( parent );
	fgButtonVal = new QPushButton( parent );
	fontButtonVal = new QPushButton( parent );
	
	hAlignCombo = new QComboBox( parent );
	hAlignCombo->insertItem( tr("left") );
	hAlignCombo->insertItem( tr("center") );
	hAlignCombo->insertItem( tr("right") );
	
	vAlignCombo = new QComboBox( parent );
	vAlignCombo->insertItem( tr("top") );
	vAlignCombo->insertItem( tr("middle") );
	vAlignCombo->insertItem( tr("bottom") );
	
	wrapCombo = new QComboBox( parent );
	wrapCombo->insertItem( tr("single line") );
	wrapCombo->insertItem( tr("break anywhere") );
	wrapCombo->insertItem( tr("break on words") );

	fgPixmap = QPixmap(19,19);
	bgPixmap = QPixmap(19,19);
	
	resetStyle();
}


void PrefStyleEdit::resetStyle() {
	labelVal->setAlignment( style->alignment() );
	labelVal->setFont( style->font() );
	labelVal->setPaletteForegroundColor( style->fgColor() );
	labelVal->setPaletteBackgroundColor( style->bgColor() );

	fontButtonVal->setText(style->fontName());
	hAlignCombo->setCurrentText( style->alignText() );
	vAlignCombo->setCurrentText( style->valignText() );
	wrapCombo->setCurrentText( style->wordWrapText() );
	
	fgPixmap.fill(style->fgColor());
	fgButtonVal->setPixmap(fgPixmap);
	bgPixmap.fill(style->bgColor());
	bgButtonVal->setPixmap(bgPixmap);
}


QLabel* PrefStyleEdit::label() {
	return labelVal;
}


QButton* PrefStyleEdit::fontButton() {
	return fontButtonVal;
}


QButton* PrefStyleEdit::fgButton() {
	return fgButtonVal;
}


QButton* PrefStyleEdit::bgButton() {
	return bgButtonVal;
}


QComboBox* PrefStyleEdit::hCombo() {
	return hAlignCombo;
}


QComboBox* PrefStyleEdit::vCombo() {
	return vAlignCombo;
}


QComboBox* PrefStyleEdit::wordWrapCombo() {
	return wrapCombo;
}


void PrefStyleEdit::fontDialog() {
	QFont currFont = style->font();
	bool ok;
	currFont = QFontDialog::getFont( &ok, currFont, parent);
	if (ok) {
		style->setFont(currFont);
		labelVal->setFont( currFont );
		fontButtonVal->setText(style->fontName());
	}
}


void PrefStyleEdit::fgDialog() {
	QColor currColor = style->fgColor();
	currColor = QColorDialog::getColor( currColor, parent );
	if (currColor.isValid()) {
		style->setFgColor(currColor.name());
		
		labelVal->setPaletteForegroundColor( currColor );
		fgPixmap.fill(currColor);
		fgButtonVal->setPixmap(fgPixmap);
	}
}


void PrefStyleEdit::bgDialog() {
	QColor currColor = style->bgColor();
	currColor = QColorDialog::getColor( currColor, parent );
	if (currColor.isValid()) {
		style->setBgColor(currColor.name());
		
		labelVal->setPaletteBackgroundColor( currColor );
		bgPixmap.fill(currColor);
		bgButtonVal->setPixmap(bgPixmap);
	}
}


void PrefStyleEdit::alignChanged() {
	int currAlign = hAlign() | vAlign();
	style->setAlignment(currAlign);
	labelVal->setAlignment(currAlign);
}


void PrefStyleEdit::wrapChanged() {
	style->setWordWrap(wordWrap());
}


int PrefStyleEdit::hAlign() {
	switch (hAlignCombo->currentItem()) {
		case 0: return Qt::AlignLeft;
		case 1: return Qt::AlignHCenter;
		case 2: return Qt::AlignRight;
	}
	return 0;
}


int PrefStyleEdit::vAlign() {
	switch (vAlignCombo->currentItem()) {
		case 0: return Qt::AlignTop;
		case 1: return Qt::AlignVCenter;
		case 2: return Qt::AlignBottom;
	}
	return 0;
}


int PrefStyleEdit::wordWrap() {
	switch (wrapCombo->currentItem()) {
		case 0: return Qt::SingleLine;
		case 1: return Qt::BreakAnywhere;
		case 2: return Qt::WordBreak;
	}
	return 1;
}


PrefDialog::PrefDialog( QWidget *parent, const char *name )
	: QTabDialog( parent, name ) {
	setupTab_general();
	setupTab_style();
	setupTab_editor();
	setDefaultButton();
	setOkButton();
	setCancelButton();
	connect( this, SIGNAL(applyButtonPressed()), this, SLOT(applySettings()) );
	connect( this, SIGNAL(defaultButtonPressed()), this, SLOT(defaultSettings()) );
	connect( this, SIGNAL(cancelButtonPressed()), this, SLOT(cancelSettings()) );

	QSettings settings;
	settings.setPath("gravitybind.com", "teddy", QSettings::User);
	settings.beginGroup("/Teddy/externalEditor");
	Teddy::externalEditor = settings.readEntry("/editor", editorPref->editor());
	Teddy::externalEditorActive = settings.readBoolEntry("/active", editorPref->editorActive());
	XmlTable::outputEncoding = settings.readEntry("/encoding", editorPref->outputEncoding());
	settings.endGroup();
	editorPref->setEditor(Teddy::externalEditor);
	editorPref->setEditorActive(Teddy::externalEditorActive);
	editorPref->setOutputEncoding(XmlTable::outputEncoding);
	
	settings.beginGroup("/Teddy/general");
	Teddy::numRecentDocs = settings.readNumEntry("/numRecentDocs", genPref->numRecentDocs());
	XmlTable::borderColor.setNamedColor(
		settings.readEntry("/borderColor", genPref->filledColor().name())
	);
	XmlTable::emptyBorderColor.setNamedColor(
		settings.readEntry("/emptyBorderColor", genPref->emptyColor().name())
	);
	XmlTable::focusBorderColor.setNamedColor(
		settings.readEntry("/focusBorderColor", genPref->focusColor().name())
	);
	XmlTable::indentWidth = settings.readNumEntry("/indentWidth", genPref->indentWidth());
	XmlTable::minColumnWidth = settings.readNumEntry("/minColumnWidth", genPref->minColumnWidth());
	XmlTable::sortAttr = settings.readBoolEntry("/sortAttr", genPref->sortAttr());
	XmlTable::displayBranches = settings.readBoolEntry("/displayBranches", genPref->drawBranches());
	settings.endGroup();
	genPref->setNumRecentDocs(Teddy::numRecentDocs);
	genPref->setFilledColor(XmlTable::borderColor);
	genPref->setEmptyColor(XmlTable::emptyBorderColor);
	genPref->setFocusColor(XmlTable::focusBorderColor);
	genPref->setIndentWidth(XmlTable::indentWidth);
	genPref->setMinColumnWidth(XmlTable::minColumnWidth);
	genPref->setSortAttr(XmlTable::sortAttr);
	genPref->setDrawBranches(XmlTable::displayBranches);
}


PrefDialog::~PrefDialog() {
	delete editorPref;
	delete genPref;
}


void PrefDialog::applySettings() {
	XmlTable::saveSettings();

	QSettings settings;
	settings.setPath("gravitybind.com", "teddy", QSettings::User);
	settings.beginGroup("/Teddy/externalEditor");
	settings.writeEntry("/editor", Teddy::externalEditor = editorPref->editor());
	settings.writeEntry("/active", Teddy::externalEditorActive = editorPref->editorActive());
	settings.writeEntry("/encoding", XmlTable::outputEncoding = editorPref->outputEncoding());
	settings.endGroup();

	settings.beginGroup("/Teddy/general");
	settings.writeEntry("/numRecentDocs", Teddy::numRecentDocs = genPref->numRecentDocs());
	XmlTable::borderColor = genPref->filledColor();
		settings.writeEntry("/borderColor", XmlTable::borderColor.name());
	XmlTable::emptyBorderColor = genPref->emptyColor();
		settings.writeEntry("/emptyBorderColor", XmlTable::emptyBorderColor.name());
	XmlTable::focusBorderColor = genPref->focusColor();
		settings.writeEntry("/focusBorderColor", XmlTable::focusBorderColor.name());
	settings.writeEntry("/indentWidth", XmlTable::indentWidth = genPref->indentWidth());
	settings.writeEntry("/minColumnWidth", XmlTable::minColumnWidth = genPref->minColumnWidth());
	settings.writeEntry("/sortAttr", XmlTable::sortAttr = genPref->sortAttr());
	settings.writeEntry("/displayBranches", XmlTable::displayBranches = genPref->drawBranches());
	settings.endGroup();

}


void PrefDialog::cancelSettings() {
	XmlTable::applySettings();
	for (int type = XmlTableItem::EMPTY; type <= XmlTableItem::COMMENT; type++) {
		styleEdits[type]->resetStyle();
	}

	editorPref->setEditor(Teddy::externalEditor);	
	editorPref->setEditorActive(Teddy::externalEditorActive);
	editorPref->setOutputEncoding(XmlTable::outputEncoding);

	genPref->setNumRecentDocs(Teddy::numRecentDocs);
	genPref->setFilledColor(XmlTable::borderColor);
	genPref->setEmptyColor(XmlTable::emptyBorderColor);
	genPref->setFocusColor(XmlTable::focusBorderColor);
	genPref->setIndentWidth(XmlTable::indentWidth);
	genPref->setMinColumnWidth(XmlTable::minColumnWidth);
	genPref->setSortAttr(XmlTable::sortAttr);
	genPref->setDrawBranches(XmlTable::displayBranches);
}


void PrefDialog::defaultSettings() {
	QWidget *currTab = currentPage();
	if (currTab == genPref) {
		genPref->setDefaults();
	}
	else if (currTab == editorPref) {
		editorPref->setDefaultEditor();
	}
	else {
		XmlTable::defaultSettings();
		for (int type = XmlTableItem::EMPTY; type <= XmlTableItem::COMMENT; type++) {
			styleEdits[type]->resetStyle();
		}
	}
}


void PrefDialog::setupTab_editor() {
	editorPref = new ExternalEditorPref();
	addTab( editorPref, tr("I/O") );
}


void PrefDialog::setupTab_general() {
	genPref = new GeneralPref();
	addTab( genPref, tr("General") );
}


void PrefDialog::setupTab_style() {
	QGrid *tab1 = new QGrid( 7, this );
	tab1->setMargin( 20 );
	tab1->setSpacing( 10 );

	QLabel *headerLabel = new QLabel( tr("     Format     "), tab1 );
	headerLabel->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
	headerLabel = new QLabel( tr("Cell\nColor"), tab1 );
	headerLabel->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
	headerLabel = new QLabel( tr("Text\nColor"), tab1 );
	headerLabel->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
	headerLabel = new QLabel( tr("Font"), tab1 );
	headerLabel->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
	headerLabel = new QLabel( tr("Horizontal\nAlignment"), tab1 );
	headerLabel->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
	headerLabel = new QLabel( tr("Vertical\nAlignment"), tab1 );
	headerLabel->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
	headerLabel = new QLabel( tr("Word Wrap"), tab1 );
	headerLabel->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
	
	styleEdits.setAutoDelete(TRUE);
	for (int type = XmlTableItem::EMPTY; type <= XmlTableItem::COMMENT; type++) {
		styleEdits.insert(type, new PrefStyleEdit(
			type, XmlTable::style(type), tab1
		));
		connect( styleEdits[type]->fontButton(), SIGNAL(clicked()), styleEdits[type], SLOT(fontDialog()) );
		connect( styleEdits[type]->fgButton(), SIGNAL(clicked()), styleEdits[type], SLOT(fgDialog()) );
		connect( styleEdits[type]->bgButton(), SIGNAL(clicked()), styleEdits[type], SLOT(bgDialog()) );
		connect( styleEdits[type]->hCombo(), SIGNAL(activated(int)), styleEdits[type], SLOT(alignChanged()) );
		connect( styleEdits[type]->vCombo(), SIGNAL(activated(int)), styleEdits[type], SLOT(alignChanged()) );
		connect( styleEdits[type]->wordWrapCombo(), SIGNAL(activated(int)), styleEdits[type], SLOT(wrapChanged()) );
	}

	addTab( tab1, tr("Style") );
}


