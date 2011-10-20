/****************************************************************************
** $Id: idewindow.ui.h  1.1.5   edited 2006-06-12T22:23:52$
**
** Copyright (C) 2001-2006 Trolltech AS.  All rights reserved.
**
** This file is part of the Qt Script for Applications framework (QSA).
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding a valid Qt Script for Applications license may use
** this file in accordance with the Qt Script for Applications License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about QSA Commercial License Agreements.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
*****************************************************************************/

#include <stdlib.h>
#include <qtimer.h>
#include <qstyle.h>
#include <qprinter.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qsimplerichtext.h>

static QTextEdit *debugoutput = 0;
static void (*qt_default_message_handler)(QtMsgType, const char *msg);

QString fix_string(const QString &s)
{
    QString ns(s);
    ns.replace("&", "&amp;");
    ns.replace("<", "&lt;");
    ns.replace(">", "&gt;");
    return ns;
}

void debugMessageOutput( QtMsgType type, const char *msg )
{
    // So that we don't override others defaults...
    if (qt_default_message_handler) {
        qt_default_message_handler(type, msg);
    } else {
        fprintf(stderr, "%s\n", msg);
        fflush(stderr);
    }

    if ( type != QtFatalMsg ) {
	if ( debugoutput )
	    debugoutput->append(QString::fromLatin1( fix_string(msg) )
                                 + QString::fromLatin1("\n") );
    } else {
	fprintf( stderr, msg );
	abort();
    }
    qApp->flush();
}

static QIconSet createIconSet( const QString &name, bool disabled = TRUE )
{
    QIconSet ic( QPixmap::fromMimeSource( QString::fromLatin1("") + name ) );
    if ( disabled )
	ic.setPixmap( QPixmap::fromMimeSource( QString::fromLatin1("d_") + name ), QIconSet::Small, QIconSet::Disabled );
    return ic;
}

class ScriptListItem : public QListViewItem
{
    friend class IdeWindow;
public:
    ScriptListItem( QListView *parent, QSScript *script )
	: QListViewItem( parent, script->name() ), qss( script )
    {
	QString fname = ( qss && qss->context() ? QString::fromLatin1("scriptobject.png") : QString::fromLatin1("script.png") );
	setPixmap( 0, QPixmap::fromMimeSource( fname ) );
    }

protected:
private:
    QSScript *qss;
};


void IdeWindow::scriptNew()
{
    bool ok;
    QString qsn =
	QInputDialog::getText( QString::fromLatin1("Input Script Name"),
                               QString::fromLatin1("Script &Name: "),
			       QLineEdit::Normal,
			       QString::fromLatin1( "Script%1.qs" )
                               .arg( project->scripts().count() + 1 ),
			       &ok, this );
    if ( ok && !qsn.isEmpty() ) {
	QSScript *script = project->createScript( qsn );
	if ( !script ) return;
	addPage( script );
	enableEditActions( TRUE );
	enableProjectActions( TRUE );
    }
}


void IdeWindow::scriptImport()
{
    QStringList names = QFileDialog::getOpenFileNames( QString::null, QString::null,
						       this, 0,
                                                       QString::fromLatin1("Import Script") );
    for ( QStringList::Iterator it = names.begin(); it != names.end(); ++it ) {
	QFile file( *it );
	if ( file.open( IO_ReadOnly ) ) {
	    QSScript *script = project->createScript( QFileInfo( file ).fileName(), file.readAll() );
	    if ( !script ) return;
	    addPage( script );
	    enableEditActions( TRUE );
	    enableProjectActions( TRUE );
	} else {
	    QMessageBox::information( this, QString::fromLatin1("Import script failed"),
				      QString::fromLatin1("The file '%1' is not readable "
                                                          "and cannot be imported.").arg(*it),
                                      QMessageBox::Ok );
	    return;
	}
    }
}


void IdeWindow::scriptExport()
{
    QSScript *script = 0;
    QSEditor *activeEditor = static_cast<QSEditor*>(tabWidget->currentPage());
    if (activeEditor) {
        script = activeEditor->script();
    } else {
        ScriptListItem *item = (ScriptListItem *)projectContainer->scriptsListView->currentItem();
        if ( !item )
            return;
        script = item->qss;
    }
    QString name = QFileDialog::getSaveFileName( script->name(), QString::null, this, 0,
                                                 QString::fromLatin1("Export Script") );
    QFile file( name );
    if ( !name.isEmpty() ) {
	if( !file.open( IO_WriteOnly ) ) {
	    QMessageBox::information( this, QString::fromLatin1("Export script failed"),
				      QString::fromLatin1("The file '%1' could not be opened for\n"
                                                          "writing. Script '%2' was not exported.")
                                      .arg(name).arg(script->name()),
                                      QMessageBox::Ok );
	    return;
	}
	QSEditor *editor = project->editor( script );
	if ( editor ) editor->commit();
	QTextStream stream( &file );
	stream << script->code();
    }
}



void IdeWindow::scriptPrint()
{
    QSEditor *activeEditor = static_cast<QSEditor*>(tabWidget->currentPage());
    if (!activeEditor)
        return;

    QString scriptName = activeEditor->script()->name();

    QTextEdit *te = activeEditor->textEdit();

    QString printString = te->text();

    // fix formatting
    printString.replace('&', QString::fromLatin1("&amp;"));
    printString.replace('<', QString::fromLatin1("&lt;"));
    printString.replace('>', QString::fromLatin1("&gt;"));
    printString.replace('\n', QString::fromLatin1("<br>\n"));
    printString.replace('\t', QString::fromLatin1("        "));
    printString.replace(' ', QString::fromLatin1("&nbsp;"));

    printString = QString::fromLatin1("<html><body>") + printString + QString::fromLatin1("</body></html>");

    QPrinter printer( QPrinter::HighResolution );
    printer.setFullPage(TRUE);
    if ( printer.setup( this ) ) {
	QPainter p( &printer );
	// Check that there is a valid device to print to.
	if ( !p.device() ) return;
	QPaintDeviceMetrics metrics( p.device() );
	int dpiy = metrics.logicalDpiY();
	int margin = (int) ( (2/2.54)*dpiy ); // 2 cm margins
	QRect body( margin, margin, metrics.width() - 2*margin, metrics.height() - 2*margin );
	QFont font( te->QWidget::font() );
 	font.setPointSize( 10 ); // we define 10pt to be a nice base size for printing

	QSimpleRichText richText( printString, font,
				  te->context(),
				  te->styleSheet(),
				  te->mimeSourceFactory(),
				  body.height() );
	richText.setWidth( &p, body.width() );
  	QRect view( body );
	int page = 1;
	do {
	    richText.draw( &p, body.left(), body.top(), view, colorGroup() );
	    view.moveBy( 0, body.height() );
	    p.translate( 0 , -body.height() );
	    p.setFont( font );
            QString renderText = scriptName + QString::fromLatin1(", ") + QString::number( page );
	    p.drawText( view.right() - p.fontMetrics().width( renderText ),
			view.bottom() + p.fontMetrics().ascent() + 5,
                        renderText );
	    if ( view.top()  >= richText.height() )
		break;
	    printer.newPage();
	    page++;
	} while (TRUE);
    }
}


void IdeWindow::scriptExit()
{

}

void IdeWindow::scriptRename()
{
    QSEditor *activeEditor = static_cast<QSEditor*>(tabWidget->currentPage());
    if (!activeEditor)
        return;

    QSScript *script = activeEditor->script();
    if (script->context()) {
        QMessageBox::information(this,
                                 QString::fromLatin1("Rename script failed"),
                                 QString::fromLatin1("Unable to rename object scripts"),
                                 QMessageBox::Ok );
        return;
    }

    bool ok = false;
    QString newName = QInputDialog::getText(QString::fromLatin1("Rename script: ") + script->name(),
                                            QString::fromLatin1("New script &Name: "),
                                            QLineEdit::Normal,
                                            QString::fromLatin1("Script%1.qs")
                                            .arg(project->scripts().count()),
                                            &ok, this );

    if ( ok && !newName.isEmpty() ) {
	QSScript *newScript = project->createScript(newName, script->code());
	if ( !newScript ) return;
        delete script;
	addPage( newScript );
	enableEditActions( TRUE );
	enableProjectActions( TRUE );
    }
}

void IdeWindow::editUndo()
{
    QSEditor *editor = (QSEditor *)tabWidget->currentPage();
    if ( !editor ) return;
    editor->undo();
}


void IdeWindow::editRedo()
{
    QSEditor *editor = (QSEditor *)tabWidget->currentPage();
    if ( !editor ) return;
    editor->redo();
}


void IdeWindow::editCut()
{
    QSEditor *editor = (QSEditor *)tabWidget->currentPage();
    if ( !editor ) return;
    editor->cut();
}


void IdeWindow::editCopy()
{
    QSEditor *editor = (QSEditor *)tabWidget->currentPage();
    if ( !editor ) return;
    editor->copy();
}


void IdeWindow::editPaste()
{
    QSEditor *editor = (QSEditor *)tabWidget->currentPage();
    if ( !editor ) return;
    editor->paste();
}

void IdeWindow::editFindNext()
{
    findText->radioForward->setChecked(TRUE);
    editFind();
}

void IdeWindow::editFindPrev()
{
    findText->radioBackward->setChecked(TRUE);
    editFind();
}

void IdeWindow::editFind()
{
    if (findText->comboFind->currentText().isEmpty()) {
        findText->show();
        return;
    }

    QSEditor *editor = (QSEditor *)tabWidget->currentPage();
    if ( !editor ) return;
    QString findString = findText->comboFind->currentText();
    findText->comboFind->insertItem( findString );
    bool caseSensitive = findText->checkCase->isChecked();
    bool wholeWordsOnly = findText->checkWhole->isChecked();
    bool startAtCursor = !findText->checkStart->isChecked();
    bool forward = findText->radioForward->isChecked();
    editor->find( findString, caseSensitive, wholeWordsOnly, forward, startAtCursor );
}


void IdeWindow::editReplace()
{
    editReplace( FALSE );
}


void IdeWindow::editReplaceAll()
{
    editReplace( TRUE );
}


void IdeWindow::editReplace( bool all )
{
    QSEditor *editor = (QSEditor *)tabWidget->currentPage();
    if ( !editor ) return;
    QString findString = replaceText->comboFind->currentText();
    replaceText->comboFind->insertItem( findString );
    QString replaceString = replaceText->comboReplace->currentText();
    replaceText->comboFind->insertItem( replaceString );
    bool caseSensitive = replaceText->checkCase->isChecked();
    bool wholeWordsOnly = replaceText->checkWhole->isChecked();
    bool startAtCursor = !replaceText->checkStart->isChecked();
    bool forward = replaceText->radioForward->isChecked();
    editor->replace( findString, replaceString, caseSensitive,
		     wholeWordsOnly, forward, startAtCursor, all );
}


void IdeWindow::editSelectAll()
{
    QSEditor *editor = (QSEditor *)tabWidget->currentPage();
    editor->selectAll();
}


void IdeWindow::editPreferences()
{
    qsaEditorSyntax->reInit();
    preferencesContainer->show();
}


void IdeWindow::editGotoLine()
{
    QSEditor *editor = (QSEditor *)tabWidget->currentPage();
    gotoLine->hide();
    editor->setFocus();
    int val = gotoLine->spinLine->value();
    int max = editor->textEdit()->lines();
    editor->gotoLine( val > max ? max : val );
}


void IdeWindow::helpIndex()
{

}


void IdeWindow::helpContents()
{

}


void IdeWindow::helpAbout()
{
    QMessageBox box( this );
    box.setText( QString::fromLatin1("<center><img src=\"splash.png\">"
		 "<p>Version " QSA_VERSION_STRING  "</p>"
		 "<p>Copyright (C) 2001-2006 Trolltech ASA. All rights reserved.</p>"
		 "</center><p></p>"
		 "<p>QSA Commercial Edition license holders: This program is"
		 " licensed to you under the terms of the QSA Commercial License"
		 " Agreement. For details, see the file LICENSE that came with"
		 " this software distribution.</p><p></p>"
		 "<p>QSA Free Edition users: This program is licensed to you"
		 " under the terms of the GNU General Public License Version 2."
		 " For details, see the file LICENSE.GPL that came with this"
		 " software distribution.</p><p>The program is provided AS IS"
		 " with NO WARRANTY OF ANY KIND, INCLUDING THE WARRANTY OF"
		 " DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE."
                                     "</p>")
		 );
    box.setCaption( tr( "About QSA Workbench" ) );
    box.setIcon( QMessageBox::NoIcon );
    box.exec();
}


void IdeWindow::addPage( QSScript *s )
{
    Q_ASSERT( s );
    QSEditor *editor = project->createEditor( s, tabWidget, 0 );
    tabWidget->addTab( editor, s->name() );
    int idx = tabWidget->indexOf( editor );
    tabWidget->setCurrentPage( idx );
    enableEditActions( TRUE );
}


void IdeWindow::removePage( QSScript *s )
{
    QSEditor *editor = project->editor( s );
    if ( !editor ) return;
    editor->commit();
    tabWidget->removePage( editor );
    delete editor;
    enableEditActions( tabWidget->count() > 0 );
}


void IdeWindow::removePage()
{
    QSEditor *editor = (QSEditor *)tabWidget->currentPage();
    if ( !editor ) return;
    editor->commit();
    tabWidget->removePage( editor );
    delete editor;
    enableEditActions( tabWidget->count() > 0 );
}


void IdeWindow::showPage( QSScript *s )
{
    QSEditor *editor = project->editor( s );
    int idx = -1;
    if ( editor && ( idx = tabWidget->indexOf( editor ) ) != -1 ) {
	tabWidget->setCurrentPage( idx );
	return;
    }
    addPage( s );
}


void IdeWindow::showFunction( QSScript *s, const QString &f )
{
    showPage( s );
    QSEditor *editor = (QSEditor *)tabWidget->currentPage();
    if ( !editor ) return;
    editor->find( "function " + f, FALSE, TRUE, TRUE, FALSE );
}


void IdeWindow::init()
{

#if (QT_VERSION >= 0x030200)
    QTabBar *tabBar = (QTabBar*)tabWidget->child( 0, "QTabBar", FALSE );
    int m = ( tabBar ? style().pixelMetric( QStyle::PM_TabBarTabVSpace, (QWidget*)tabBar )
	      + style().pixelMetric( QStyle::PM_TabBarBaseHeight, (QWidget*)tabBar ) : 0 );
    int s = tabWidget->height() - m;
    QToolButton *closeTabButton = new QToolButton( tabWidget );
    closeTabButton->setAutoRaise( TRUE );
    closeTabButton->setFixedSize( s, s );
    closeTabButton->setIconSet( QIconSet( style().stylePixmap( QStyle::SP_TitleBarCloseButton ) ) );
    QObject::connect( closeTabButton, SIGNAL( clicked() ), this, SLOT( removePage() ) );
    QToolTip::add( closeTabButton, tr( "Close tab" ) );
    tabWidget->setCornerWidget( closeTabButton, Qt::TopRight );
#endif

    tabWidget->removePage( tabWidget->page( 0 ) );

    QObject::connect( tabWidget, SIGNAL( currentChanged( QWidget * ) ),
		      this, SLOT( currentTabChanged( QWidget * ) ) );

    projectContainerDock = new QDockWindow( QDockWindow::InDock, this );
    projectContainer = new QSProjectContainer( projectContainerDock, 0, FALSE );
    projectContainerDock->setResizeEnabled( TRUE );
    projectContainerDock->setCloseMode( QDockWindow::Always );
    addDockWindow( projectContainerDock, DockLeft );
    projectContainerDock->setWidget( projectContainer );
    projectContainerDock->setCaption( QString::fromLatin1("Project Scripts") );
    projectContainerDock->setFixedExtentWidth( 150 );
    projectContainer->scriptsListView->header()->hide();
    projectContainer->show();

#ifdef QS_VARIABLE_WATCH
    variablesContainerDock = new QDockWindow( QDockWindow::InDock, this );
    variablesContainer = new QSVariablesContainer( variablesContainerDock, 0, FALSE );
    variablesContainerDock->setResizeEnabled( TRUE );
    variablesContainerDock->setCloseMode( QDockWindow::Always );
    addDockWindow( variablesContainerDock, DockRight );
    variablesContainerDock->setWidget( variablesContainer );
    variablesContainerDock->setCaption( "Variables" );
    variablesContainerDock->setFixedExtentWidth( 150 );
    //variablesContainer->variablesListView->header()->hide();
    variablesContainer->show();
#endif

    outputContainerDock = new QDockWindow( QDockWindow::InDock, this );
    outputContainer = new QSOutputContainer( outputContainerDock , 0, FALSE );
    outputContainerDock->setResizeEnabled( TRUE );
    outputContainerDock->setCloseMode( QDockWindow::Always );
    addDockWindow( outputContainerDock, DockBottom );
    outputContainerDock->setWidget( outputContainer );
    outputContainerDock->setCaption( QString::fromLatin1("Output") );
    outputContainerDock->setFixedExtentHeight( 100 );
    outputContainer->show();

    QObject::connect( projectContainer->scriptsListView,
		      SIGNAL( doubleClicked( QListViewItem *) ),
		      this, SLOT( scriptItemDoubleClicked( QListViewItem *) ) );

    findText = new QSFindText( this, 0, FALSE );
    QObject::connect( editFindAction, SIGNAL( activated() ), findText, SLOT( show() ) );
    QObject::connect( findText->pushFind, SIGNAL( clicked() ), this, SLOT( editFind() ) );

    replaceText = new QSReplaceText( this, 0, FALSE );
    QObject::connect( editReplaceAction, SIGNAL( activated() ), replaceText, SLOT( show() ) );
    QObject::connect( replaceText->pushReplace, SIGNAL( clicked() ), this, SLOT( editReplace() ) );
    QObject::connect( replaceText->pushReplaceAll, SIGNAL( clicked() ), this, SLOT( editReplaceAll() ) );
    gotoLine = new QSGotoLine( this, 0, FALSE );
    QObject::connect( editGotoLineAction, SIGNAL( activated() ), gotoLine, SLOT( show() ) );
    QObject::connect( gotoLine->pushGoto, SIGNAL( clicked() ), this, SLOT( editGotoLine() ) );

    preferencesContainer = new QSPreferencesContainer( this, 0 );
    QBoxLayout *preferencesLayout = new QBoxLayout( preferencesContainer->frame, QBoxLayout::Down );
    qsaEditorSyntax = new PreferencesBase( preferencesContainer->frame, "qsaeditor_syntax" );
    preferencesLayout->addWidget( qsaEditorSyntax );
    qsaEditorSyntax->setPath( QString::fromLatin1("/Trolltech/QSAScriptEditor/") );
    QObject::connect( preferencesContainer->pushOk, SIGNAL( clicked() ), this, SLOT( savePreferences() ) );

    windowMenu->insertItem( tr( "&Views" ), createDockWindowMenu( NoToolBars ) );
    windowMenu->insertItem( tr( "&Toolbars" ), createDockWindowMenu( OnlyToolBars ) );

    // restore the old message handler first of all
    if (debugoutput)
        qInstallMsgHandler(0);
    debugoutput = outputContainer->textEdit;
    qt_default_message_handler = qInstallMsgHandler( debugMessageOutput );

    setIcon( QPixmap::fromMimeSource( QString::fromLatin1("qsa.png") ) );
    setupActionIcons();

    enableEditActions( FALSE );
}


void IdeWindow::removeScript( QSScript *s )
{
    delete s;
    enableProjectActions( projectContainer->scriptsListView->childCount() > 0 );
    enableEditActions( tabWidget->count() > 0 );
}


void IdeWindow::removeScript()
{
    ScriptListItem *item = (ScriptListItem *)projectContainer->scriptsListView->currentItem();
    if ( !item )
	return;
    QSScript *script = item->qss;
    delete script;
    enableProjectActions( projectContainer->scriptsListView->childCount() > 0 );
    enableEditActions( tabWidget->count() > 0 );
}

void IdeWindow::setRunningState( bool running )
{
    projectStopAction->setEnabled( running );
    projectRunAction->setEnabled( !running );
    projectCallAction->setEnabled( !running );
    projectEvaluateAction->setEnabled( !running );
}

void IdeWindow::projectRun()
{
    hadError = FALSE;
    project->commitEditorContents();
    QStringList functions = project->interpreter()->functions();
    if ( hadError )
	return;
    bool ok = TRUE;
    if ( runFunction.isNull()
	 || runFunction == QString::fromLatin1("")
	 || functions.find( runFunction ) == functions.end() ) {
	runFunction = QInputDialog::getItem( QString::fromLatin1("Call function"),
                                             QString::fromLatin1("&Function: "), functions,
					     functions.findIndex( runFunction ),
					     FALSE, &ok, this );
    }
    if ( ok && !runFunction.isEmpty() ) {
	setRunningState(TRUE);
	project->interpreter()->call( runFunction );
	setRunningState(FALSE);
    }
}


void IdeWindow::projectCall()
{
    hadError = FALSE;
    project->commitEditorContents();
    QStringList functions = project->interpreter()->functions();
    if ( hadError )
	return;
    bool ok = FALSE;
    runFunction = QInputDialog::getItem( QString::fromLatin1("Call function"), QString::fromLatin1("&Function: "), functions,
					 functions.findIndex( runFunction ),
					 FALSE, &ok, this );
    if ( ok && !runFunction.isEmpty() ) {
	setRunningState(TRUE);
	project->interpreter()->call( runFunction );
	setRunningState(FALSE);
#ifdef QS_VARIABLE_WATCH
	variablesContainer->updateScope(project->interpreter());
#endif
    }
}


void IdeWindow::currentTabChanged( QWidget *w )
{
    QSEditor *editor = (QSEditor*)w;
    ScriptListItem *item = (ScriptListItem *)projectContainer->scriptsListView->findItem( editor->script()->name(), 0 );
    projectContainer->scriptsListView->setCurrentItem( item );
    textChanged();
}


void IdeWindow::projectError( const QString &msg, const QString &file, int line )
{
    hadError = TRUE;

    QStringList error;

    error << QString::fromLatin1("<pre><font color=red><b>Error:</b></font> ") << file
	  << QString::fromLatin1(" : <font color=blue>") << QString::number( line )
          << QString::fromLatin1("</font>\n") << QString::fromLatin1("<i>") << msg
          << QString::fromLatin1("</i>\n");

    QSStackTrace stackTrace = project->interpreter()->stackTrace();
    if( stackTrace.size() ) {
	QString trace = stackTrace.toString();
	error << QString::fromLatin1("Callstack:\n");
	error << QString::fromLatin1("  ") << trace.replace( QString::fromLatin1("\n"),
                                                             QString::fromLatin1("\n  ") );
    }

    error << QString::fromLatin1("</pre>");

    debugoutput->append( error.join( QString::fromLatin1("") ) );

    QSScript *script = project->script( file );
    if ( script ) {
	showPage( script );
	QSEditor *editor = project->editor( script );
	editor->setErrorMark( line );
    }
}


void IdeWindow::scriptItemDoubleClicked( QListViewItem *i )
{
    ScriptListItem *item = (ScriptListItem *)i;
    showPage( item->qss );
}


void IdeWindow::setupActionIcons()
{
    fileNewAction->setIconSet( createIconSet( QString::fromLatin1("scriptnew.png"), FALSE ) );
    fileImportAction->setIconSet( createIconSet( QString::fromLatin1("fileopen.png") ) );
    fileExportAction->setIconSet( createIconSet( QString::fromLatin1("filesave.png") ) );
    editUndoAction->setIconSet( createIconSet( QString::fromLatin1("undo.png") ) );
    editRedoAction->setIconSet( createIconSet( QString::fromLatin1("redo.png") ) );
    editCutAction->setIconSet( createIconSet( QString::fromLatin1("editcut.png") ) );
    editCopyAction->setIconSet( createIconSet( QString::fromLatin1("editcopy.png") ) );
    editPasteAction->setIconSet( createIconSet( QString::fromLatin1("editpaste.png") ) );
    editFindAction->setIconSet( createIconSet( QString::fromLatin1("searchfind.png") ) );
    helpContentsAction->setIconSet( createIconSet( QString::fromLatin1("help.png") ) );
    helpAboutAction->setIconSet( createIconSet( QString::fromLatin1("qsa.png"), FALSE ) );
    fileRemoveAction->setIconSet( createIconSet( QString::fromLatin1("editdelete.png") ) );
    projectRunAction->setIconSet( createIconSet( QString::fromLatin1("playprev.png") ) );
    projectCallAction->setIconSet( createIconSet( QString::fromLatin1("play.png") ) );
    projectStopAction->setIconSet( createIconSet( QString::fromLatin1("stop.png") ) );
}


void IdeWindow::enableEditActions( bool enable )
{
    filePrintAction->setEnabled( enable );
    fileExportAction->setEnabled( enable );
    fileCloseAction->setEnabled( enable );
    editCutAction->setEnabled( enable );
    editCopyAction->setEnabled( enable );
    editPasteAction->setEnabled( enable );
    editFindAction->setEnabled( enable );
    editFindAgainAction->setEnabled( enable );
    editFindAgainBWAction->setEnabled( enable );
    editReplaceAction->setEnabled( enable );
    editGotoLineAction->setEnabled( enable );
    editSelectAllAction->setEnabled( enable );
#if (QT_VERSION >= 0x030200)
    tabWidget->cornerWidget( Qt::TopRight )->setShown( enable );
#endif
}


void IdeWindow::enableProjectActions( bool enable )
{
    fileRemoveAction->setEnabled( enable );
    projectRunAction->setEnabled( enable );
    projectCallAction->setEnabled( enable );
    projectEvaluateAction->setEnabled( enable );
    projectStopAction->setEnabled( FALSE );
}


void IdeWindow::textChanged()
{
    QSEditor *editor = (QSEditor *)tabWidget->currentPage();
    if ( !editor ) return;
    editUndoAction->setEnabled( editor->isUndoAvailable() );
    editRedoAction->setEnabled( editor->isRedoAvailable() );
}


void IdeWindow::setProject( QSProject *p )
{
    Q_ASSERT( p != 0 );
    project = p;
    QObject::connect( project, SIGNAL( editorTextChanged() ),
		      this, SLOT( textChanged() ) );
    projectChanged();
    QPtrList<QSScript> scripts = project->scripts();
    if ( tabWidget->count() == 0 && scripts.count() )
	addPage( scripts.first() );
    connect( project->interpreter(), SIGNAL( error( const QString &, const QString &, int ) ),
	     this, SLOT( projectError( const QString &, const QString &, int ) ) );
    connect( project, SIGNAL( projectChanged() ), this, SLOT( projectChanged() ) );
    errorMode = project->interpreter()->errorMode();
    project->interpreter()->setTimeoutInterval(250);
    connect(project->interpreter(), SIGNAL(timeout(int)), this, SLOT(interpreterTimeout(int)));

#ifdef QS_VARIABLE_WATCH
    connect( project, SIGNAL( projectEvaluated() ),
	     variablesContainer, SLOT( updateScope() ) );
#endif
}


void IdeWindow::savePreferences()
{
    qsaEditorSyntax->save();
    QPtrList<QSEditor> editors = project->editors();
    QSEditor *editor = editors.first();
    while ( editor ) {
	editor->readSettings();
	editor = editors.next();
    }
}

void IdeWindow::projectChanged()
{
    QPtrList<QSScript> scripts = project->scripts();
    projectContainer->scriptsListView->clear();
    for ( QSScript *script = scripts.first(); script; script = scripts.next() ) {
	ScriptListItem *item = new ScriptListItem( projectContainer->scriptsListView, script );
	Q_UNUSED( item );
    }
    enableProjectActions( scripts.count() );
}

void IdeWindow::evaluateProject()
{
    if( project ) {
	project->commitEditorContents();
	QTimer::singleShot( 0, project, SLOT( evaluate() ) );
    }
}

void IdeWindow::hideEvent(QHideEvent *)
{
    if (project)
	project->interpreter()->setErrorMode((QSInterpreter::ErrorMode)errorMode);
}

void IdeWindow::showEvent(QShowEvent *)
{
    if (project)
	project->interpreter()->setErrorMode(QSInterpreter::Nothing);
}

void IdeWindow::projectStop()
{
    if (project)
	project->interpreter()->stopExecution();
}



void IdeWindow::interpreterTimeout(int)
{
    qApp->processEvents();
}
