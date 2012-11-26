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

#ifndef PREFDIALOG_H
#define PREFDIALOG_H

#include <qtabdialog.h>
#include <qstring.h>
#include <qgrid.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qdatetime.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qlistbox.h>
#include <qapplication.h>
#include <qsettings.h>
#include <qpushbutton.h>

#include <qptrvector.h>
#include <qcolordialog.h>
#include <qcombobox.h>

#include "xmltable.h"
#include "externaleditor.h"
#include "general.h"


class PrefStyleEdit : public QWidget {
	Q_OBJECT

public:
	PrefStyleEdit(int type, XmlTableStyle* style, QWidget * parent );
	QLabel* label();
	QButton* fontButton();
	QButton* fgButton();
	QButton* bgButton();
	QComboBox* hCombo();
	QComboBox* vCombo();
	QComboBox* wordWrapCombo();

	int hAlign();
	int vAlign();
	int wordWrap();
	void resetStyle();
	

public slots:
	void fontDialog();
	void fgDialog();
	void bgDialog();
	void alignChanged();
	void wrapChanged();
	
private:
	QWidget* parent;
	int type;
	QLabel *labelVal;
	QButton *fontButtonVal, *fgButtonVal, *bgButtonVal;
	QComboBox *hAlignCombo, *vAlignCombo, *wrapCombo;
	XmlTableStyle* style;
	QPixmap fgPixmap, bgPixmap;
};


class PrefDialog : public QTabDialog {
	Q_OBJECT

public:
	PrefDialog( QWidget *parent, const char *name);
	~PrefDialog();

public slots:
	void applySettings();
	void cancelSettings();
	void defaultSettings();

	
private:
	void setupTab_style();
	void setupTab_editor();
	void setupTab_general();

	QIntDict<PrefStyleEdit> styleEdits;
	ExternalEditorPref *editorPref;
	GeneralPref *genPref;

	
};

#endif
