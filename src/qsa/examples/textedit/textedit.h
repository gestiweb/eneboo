/****************************************************************************
** $Id: $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <qmainwindow.h>
#include <qmap.h>
#include <qaccel.h>
#include <qsinterpreter.h>
#include <qsworkbench.h>

class QAction;
class QComboBox;
class QTabWidget;
class QTextEdit;

class TextEdit : public QMainWindow
{
    Q_OBJECT

public:
    TextEdit( QWidget *parent = 0, const char *name = 0 );

    QSProject *qsaProject() const { return project; }

public slots:
    QTextEdit *currentEditor() const;
    void load( const QString &f );

    void fileNew();
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void filePrint();
    void fileClose();
    void fileExit();

    void editUndo();
    void editRedo();
    void editCut();
    void editCopy();
    void editPaste();

    void textBold();
    void textUnderline();
    void textItalic();
    void textFamily( const QString &f );
    void textSize( const QString &p );
    void textStyle( int s );
    void textColor();
    void textAlign( QAction *a );

    void addAccel( int accel, const QString &function );

private:
    void setupFileActions();
    void setupEditActions();
    void setupTextActions();
    void doConnections( QTextEdit *e );

private slots:
    void fontChanged( const QFont &f );
    void colorChanged( const QColor &c );
    void alignmentChanged( int a );
    void editorChanged( QWidget * );
    void runMacro();
    void openIDE();
    void execAccel();

private:
    QAction *actionTextBold,
	*actionTextUnderline,
	*actionTextItalic,
	*actionTextColor,
	*actionAlignLeft,
	*actionAlignCenter,
	*actionAlignRight,
	*actionAlignJustify;
    QComboBox *comboStyle,
	*comboFont,
	*comboSize;
    QTabWidget *tabWidget;
    QMap<QTextEdit*, QString> filenames;
    QMap<QAccel*, QString> accels;
    QSProject *project;
    QSInterpreter *interpreter;
#ifndef QSA_NO_IDE
    QSWorkbench *ide;
#endif
};


#endif
