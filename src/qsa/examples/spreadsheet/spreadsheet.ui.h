/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

#include "addscriptdialog.h"
#include "sheetinterface.h"

#include <qsinputdialogfactory.h>
#include <qsproject.h>
#include <qsscript.h>

#ifndef QSA_NO_IDE
#include <qsworkbench.h>
#endif

#include <qapplication.h>
#include <qfile.h>
#include <qheader.h>
#include <qmessagebox.h>
#include <qmetaobject.h>

void SpreadSheet::init()
{
    currentSheet = sheet1;

    project = new QSProject( this, "spreadsheet_project" );
    interpreter = project->interpreter();
    QSInputDialogFactory *fac = new QSInputDialogFactory;
    interpreter->addObjectFactory( fac );

    project->addObject( new SheetInterface( sheet1, this, "sheet1" ) );
    project->addObject( new SheetInterface( sheet2, this, "sheet2" ) );
    setupSheet( sheet1 );
    setupSheet( sheet2 );

    project->load( "spreadsheet.qsa" );
    connect( project, SIGNAL( projectEvaluated() ),
	     project, SLOT( save() ) );
}

void SpreadSheet::setupSheet( QTable *t )
{
    int num = -1;
    static QString letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int ch = 0;
    for ( int i = 0; i < (int)t->numCols(); ++i ) {
	if ( ch == 26 ) {
	    ch = 0;
	    num++;
	}
	QString s = QString( letters[ ch ] );
	if ( num != -1 )
	    s.prepend( letters[ num ] );
	ch++;
	t->horizontalHeader()->setLabel( i, s );
    }
}

void SpreadSheet::fileExit()
{
    qApp->quit();
}


void SpreadSheet::currentCellChanged( int row, int col )
{
    currentCell->setText( QString( currentSheet->name() )
			  + ":"
			  + currentSheet->horizontalHeader()->label( col )
			  + QString::number( row + 1 ) );
    QMap<QPair<int, int>, QString> &currentMap = ( currentSheet == sheet1 ) ? sheet1Map : sheet2Map;
    QMap<QPair<int, int>, QString>::ConstIterator it = currentMap.find( QPair<int, int>( row, col ) );
    if ( it == currentMap.end() )
	formulaEdit->setText( currentSheet->text( row, col ) );
    else
	formulaEdit->setText( *it );
}

void SpreadSheet::sheetChanged( QWidget *page )
{
    if ( page == tabSheet1 )
	currentSheet = sheet1;
    else
	currentSheet = sheet2;
    currentCellChanged( currentSheet->currentRow(), currentSheet->currentColumn() );
}

void SpreadSheet::currentValueChanged( int row, int col )
{
    formulaEdit->setText( currentSheet->text( row, col ) );
    evaluate();
}

QString SpreadSheet::cellName( int row, int col )
{
    QString s;
    int i = col / 26;
    static QString letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if ( i > 0 ) {
	--i;
	s = QString( letters[i] );
    }
    s += letters[col % 26];
    s += QString::number( row + 1 );

    return s;
}

void SpreadSheet::evaluate()
{
    QMap<QPair<int, int>, QString> &currentMap = ( currentSheet == sheet1 ) ? sheet1Map : sheet2Map;

    currentMap.replace( QPair<int, int>( currentSheet->currentRow(),
					 currentSheet->currentColumn() ),
			formulaEdit->text() );

    QMap<QPair<int, int>, QString>::ConstIterator it;
    QString vars;
    for ( it = currentMap.begin(); it != currentMap.end(); ++it ) {
	QTableItem *item = currentSheet->item( it.key().first, it.key().second );
	if ( !item )
	    continue;
	bool ok = FALSE;
	item->text().toInt( &ok );
	if ( !ok )
	    continue;
	vars += "var " + cellName( item->row(), item->col() ) + "=" + item->text() + ";\n";
    }

    for ( it = currentMap.begin(); it != currentMap.end(); ++it ) {
	QTableItem *item = currentSheet->item( it.key().first, it.key().second );
	if ( !item )
	    continue;
	evaluateCell( item, *it, vars );
    }
}

void SpreadSheet::evaluateCell( QTableItem *item, const QString &f, const QString &vars )
{
    QString formula( f );
    int row = item->row();
    int col = item->col();

    if ( formula[0] != '=' )
	return;
    QString cn = cellName( row, col );
    formula = vars +"\n" + cn + "=" + formula.mid( 1 ) + ";\n" +
	      "Application." + QString( currentSheet->name() ) + ".setText( " + QString::number( row ) +
	      ", " + QString::number( col ) + ", String( " + cn + " ) );";
    interpreter->evaluate( formula );
}


void SpreadSheet::formulaEdited()
{
    currentSheet->setText( currentSheet->currentRow(), currentSheet->currentColumn(),
			   formulaEdit->text() );
    evaluate();
    currentSheet->setFocus();
}

#ifndef QSA_NO_IDE
static QSWorkbench *spreadsheet_ide = 0;
#endif

void SpreadSheet::openIDE()
{
#ifndef QSA_NO_IDE
    // open the QSA Workbench
    if ( !spreadsheet_ide ) spreadsheet_ide = new QSWorkbench( project, this, "qside" );
    spreadsheet_ide->open();
#else
    QMessageBox::information( this, "Disabled feature",
			      "QSA Workbench has been disabled. Reconfigure to enable",
			      QMessageBox::Ok );
#endif
}

void SpreadSheet::showFunction( QSScript *script, const QString &functionName )
{
#ifndef QSA_NO_IDE
    if ( !spreadsheet_ide ) spreadsheet_ide = new QSWorkbench( project, this, "qside" );
    spreadsheet_ide->open();
    spreadsheet_ide->showFunction( script, functionName );
#endif
}

void SpreadSheet::addScript()
{
    // Let the user add a script
    AddScriptDialog dia( this, 0, TRUE );
    connect( &dia, SIGNAL( newScript( const QString &, const QString &,
				      const QPixmap & ) ),
	     this, SLOT( addScript( const QString &, const QString &,
				    const QPixmap & ) ) );
    dia.exec();
}

void SpreadSheet::addScript( const QString &function, const QString &name, const QPixmap &pixmap )
{
    // Add a new action for the script
    QAction *a = new QAction( name, pixmap, name, 0, this, name.latin1() );
    a->addTo( scriptsToolbar );
    a->addTo( scriptsMenu );
    // associate the action with the function name
    scripts.insert( a, function );
    connect( a, SIGNAL( activated() ), this, SLOT( runScript() ) );
}

void SpreadSheet::runScript()
{
    // find the function which has been associated with the activated
    // action (the action is the sender())
    QString s = *scripts.find( (QAction*)sender() );
    // and call that function
    project->commitEditorContents();
    interpreter->call( s, QValueList<QVariant>() );
}
