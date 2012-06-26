/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

#include <qfiledialog.h>
#include <qpixmap.h>
#include <qmessagebox.h>
#include "spreadsheet.h"

#include <qsscript.h>
#include <qsinterpreter.h>
#include <qsproject.h>

void AddScriptDialog::init()
{
    // List all global functions of the project
    QSProject *project = ( (SpreadSheet*) parent() )->project;
    comboFunction->insertStringList( project->interpreter()->functions() );
}

void AddScriptDialog::choosePixmap()
{
    QString f = QFileDialog::getOpenFileName();
    if ( f.isEmpty() )
	return;
    QPixmap pix( f );
    labelPixmap->setPixmap( pix );
}

void AddScriptDialog::addScript()
{
    QSInterpreter *script = ( (SpreadSheet*) parent() )->interpreter;
    QString func = comboFunction->currentText();
    if ( script->functions().findIndex( func ) == -1 ) {
	QString msg = tr( "The function <b>%1</b> doesn't exist. "
			  "Do you want to add it?" ).arg( func );
	if ( QMessageBox::information( 0, tr( "Add Function" ), msg,
				       tr( "&Yes" ), tr( "&No" ),
				       "", 0, 1 ) == 0 ) {
 	    QSScript *sc = script->project()->script( "main.qs" );
	    if( !sc )
		sc = script->project()->createScript( "main.qs" );
	    sc->addFunction( func );
	    ( (SpreadSheet*) parent() )->showFunction( sc, func );
	}
    }

    emit newScript( func, editName->text(), *labelPixmap->pixmap() );
    accept();
}
