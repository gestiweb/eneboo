/****************************************************************************
 ** ui.h extension file, included from the uic-generated form implementation.
 **
 ** If you wish to add, delete or rename functions or slots use
 ** Qt Designer which will update this file, preserving your code. Create an
 ** init() function in place of a constructor, and a destroy() function in
 ** place of a destructor.
 *****************************************************************************/

#include <qmessagebox.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qsinterpreter.h>

QString loadFile( const QString &name )
{
    QFile file( name );
    if ( !file.open( IO_ReadOnly ) ) {
	qDebug( "Can't open file " + name );
	return QString();
    }
    QTextStream str( &file );
    return str.read();
}

void GameWindow::init()
{
    connect( &timer, SIGNAL( timeout() ), this, SLOT( timeout() ) );
    connect( playField, SIGNAL( gameCompleted() ), this, SLOT( stopTimer() ) );
    connect( &qsa, SIGNAL( error( const QString &, const QString &, int ) ),
	     this, SLOT( stopTimer() ) );
    ok = TRUE;
}

void GameWindow::timeout()
{
    qsa.call( "next", QValueList<QVariant>(), playField );
    playField->update();
}

void GameWindow::startTimer()
{
    ok = TRUE;
    QString code = loadFile( combo->currentText() );
    if( code.isNull() ) {
	QMessageBox::warning( this, "Failed to load file",
			      "The file containing the script code could not be loaded" );
	return;
    }
    playField->clear();
    playField->setFocus();
    qsa.clear();
    qsa.evaluate( code, playField );
    if ( !ok )
	return;
    qsa.call( "init", QValueList<QVariant>(), playField );
    startButton->setEnabled( false );
    stopButton->setEnabled( true );
    timer.start( 25 );
}

void GameWindow::stopTimer()
{
    ok = FALSE;
    timer.stop();
    startButton->setEnabled( true );
    stopButton->setEnabled( false );
}

