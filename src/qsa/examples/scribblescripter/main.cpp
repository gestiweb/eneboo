/****************************************************************************
** $Id: $
**
** Copyright ( C ) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for QSA.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "scribblescripter.h"
#include <qapplication.h>


int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    Scribble *scribble = new Scribble;

    scribble->resize( 700, 500 );
    scribble->setCaption("QSA Example - ScribbleScripter");
    a.setMainWidget( scribble );
    if ( QApplication::desktop()->width() > 750
	 && QApplication::desktop()->height() > 566 )
	scribble->show();
    else
	scribble->showMaximized();
    int ret = a.exec();
    delete scribble;
    return ret;
}
