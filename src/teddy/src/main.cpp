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
#include <qstring.h>
#include <AQConfig.h>

#include <qapplication.h>
#include <qtextcodec.h>

#include "teddy.h"


int main( int argc, char ** argv ) {
	QApplication a( argc, argv );

	QSettings settings;
	settings.setPath( "InfoSiAL", "FacturaLUX", QSettings::User );
	QFont appFont;
	QString keybase( "/facturalux/lite/" );
#if defined(Q_OS_MACX)
	a.setLibraryPaths( a.applicationDirPath() + QString::fromLatin1("/../../../..") + "/plugins" );
#else
        a.setLibraryPaths( a.applicationDirPath() + QString::fromLatin1("/..") + "/plugins" );
#endif

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

	if ( argc <= 1 ) {
		Teddy *mw = new Teddy();
		mw->setCaption( "Teddy" );
		mw->show();
	}
	else {
		for (int i = 1; i < argc; i++) {
			Teddy *mw = new Teddy();
			mw->load( argv[i] );
			mw->show();
		}
	}

	a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
	return a.exec();
}


/*
TODO:
threading
direct save to .ted files

*/
