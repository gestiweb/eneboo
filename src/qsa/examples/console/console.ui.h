/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

#include <qevent.h>
#include <qapplication.h>
#include <qmetaobject.h>
#include <stdio.h>
#include <stdlib.h>
#include <qmessagebox.h>
#include <qsinputdialogfactory.h>
#include <qsutilfactory.h>

static QTextEdit *debugoutput = 0;

void debugMessageOutput( QtMsgType type, const char *msg )
{
    QString s( msg );

    // filter out C++ asserts
    if ( s.startsWith( "ASSERT" ) ) {
	fprintf( stderr, msg );
	return;
    }

    if ( type != QtFatalMsg ) {
	if ( debugoutput )
	    debugoutput->append( s + "\n" );
    } else {
	fprintf( stderr, msg );
	abort();
    }

    qApp->flush();
}

void Console::init()
{
    interpreter = new QSInterpreter( this );
    interpreter->setErrorMode( QSInterpreter::Nothing );

    editInput->setInterpreter( interpreter, interpreter );

    // grant access to low level Qt API for demonstration purposes
    interpreter->addObjectFactory(new QSInputDialogFactory());
    interpreter->addObjectFactory(new QSUtilFactory);

    edit = editInput->textEdit();
    edit->installEventFilter( this );
    edit->viewport()->installEventFilter( this );
    edit->setFocus();
    index = 0;
    debugoutput = output;
    qInstallMsgHandler( debugMessageOutput );
    connect( interpreter, SIGNAL( error( const QString &, const QString &, int ) ),
	     this, SLOT( error( const QString &, const QString &, int ) ) );
}

bool Console::eventFilter( QObject *o, QEvent *e )
{
    if ( o != edit && o != edit->viewport() )
	return FALSE;
    if ( e->type() == QEvent::KeyPress ) {
	QKeyEvent *ke = (QKeyEvent*)e;
	switch ( ke->key() ) {
	case Key_Return:
	case Key_Enter:
	    if ( ke->state() & ControlButton ) {
		edit->doKeyboardAction( QTextEdit::ActionReturn );
	    } else {
		QString code = edit->text();
		if ( code[ 0 ] == '?' )
		    code = "debug(" + code.mid( 1 ) + ");";
		if ( !interpreter->checkSyntax( code ) ) {
		    edit->doKeyboardAction( QTextEdit::ActionReturn );
		    return TRUE;
		}
		QSArgument value = interpreter->evaluate( code );
		QString tn;
		switch( value.type() ) {
		case QSArgument::Variant:
		    {
			QVariant var = value.variant();
			if ( !var.toString().isEmpty() )
			    tn = var.toString() + " ["
				 + ( var.type() == QVariant::Double ? "Number" :var.typeName() )
				 + "]";
			break;
		    }
		case QSArgument::QObjectPtr:
		    tn = QString( "%1 [%2]" ).arg( value.qobject()->name() )
			 .arg( value.qobject()->metaObject()->className() );
		    break;
		case QSArgument::VoidPointer:
		    tn = QString( "0x%1 [void*]" ).arg( (ulong) value.ptr(), 16 );
		    break;
		case QSArgument::Invalid:
		    break;
		}
		if( !tn.isEmpty() )
		    debugoutput->append( tn );
		history.append( edit->text() );
		index = history.count();
		historyView->append( edit->text() );
		edit->clear();
	    }
	    return TRUE;
	case Key_Up:
	    if ( ke->state() & ControlButton ) {
		if ( index > 0 ) {
		    index--;
		    edit->setText( *history.at( index ) );
		}
		return TRUE;
	    }
	    break;
	case Key_Down:
	    if ( ke->state() & ControlButton ) {
		if ( index < (int)history.count() - 1 ) {
		    index++;
		    edit->setText( *history.at( index ) );
		} else {
		    edit->clear();
		}
		return TRUE;
	    }
	    break;
	case Key_Escape:
	    ke->ignore();
	    return TRUE;
	}
    }
    return FALSE;
}

void Console::error( const QString &msg, const QString &, int line )
{
    debugoutput->append( "<b>Error in line " + QString::number( line ) + ":</b><pre><font color=red>" + msg + "</font></pre>" );
}
