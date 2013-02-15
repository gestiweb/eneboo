#include <qapplication.h>
#include <qtextcodec.h>
#include <qsettings.h>
#include <qpaintdevicemetrics.h> 
#include "mainform.h"

int main( int argc, char ** argv ) {
	QApplication a( argc, argv );

	QSettings settings;
	settings.setPath( "InfoSiAL", "FacturaLUX", QSettings::User );
	QFont appFont;
	QString keybase( "/facturalux/lite/" );

#if defined (Q_OS_LINUX)
	QPaintDeviceMetrics pdm( QApplication::desktop() );
	float relDpi;
	if ( pdm.logicalDpiX() < pdm.logicalDpiY() )
		relDpi = 78. / pdm.logicalDpiY();
	else
		relDpi = 78. / pdm.logicalDpiX();
	int pointSize = 10 * relDpi;
#else
	int pointSize = 10;
#endif

#if defined(Q_OS_WIN32)
	appFont.setFamily( settings.readEntry( keybase + "fuente/familia", "Tahoma" ) );
	pointSize = 8;
#else

#if defined(Q_OS_MACX)
	appFont.setFamily( settings.readEntry( keybase + "fuente/familia", "Lucida Grande" ) );
	pointSize = 10;
#else
	appFont.setFamily( settings.readEntry( keybase + "fuente/familia", "Sans" ) );
#endif

#endif

	appFont.setPointSize( settings.readNumEntry( keybase + "fuente/puntos", pointSize ) );
	appFont.setBold( settings.readBoolEntry( keybase + "fuente/negrita", false ) );
	appFont.setItalic( settings.readBoolEntry( keybase + "fuente/cursiva", false ) );
	appFont.setUnderline( settings.readBoolEntry( keybase + "fuente/subrayado", false ) );
	appFont.setStrikeOut( settings.readBoolEntry( keybase + "fuente/tachado", false ) );

	a.setFont( appFont );

#if defined(Q_OS_WIN32)
	a.setStyle( settings.readEntry( keybase + "estilo", "Bluecurve" ) );
#else
	a.setStyle( settings.readEntry( keybase + "estilo", "Plastik" ) );
#endif

	fmMain *w = new fmMain;
	w->show();
	if ( argc == 2 )
		w->fileOpen( argv[ 1 ] );
	a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
	return a.exec();
}
