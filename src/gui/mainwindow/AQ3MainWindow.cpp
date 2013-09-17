/***************************************************************************
                         AQ3MainWindow.cpp
                     -------------------
begin                : 11/09/2007
copyright            : (C) 2003-2007 by InfoSiAL S.L.
email                : mail@infosial.com
***************************************************************************/ 
/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 ***************************************************************************/ 
/***************************************************************************
   Este  programa es software libre. Puede redistribuirlo y/o modificarlo
   bajo  los  términos  de  la  Licencia  Pública General de GNU   en  su
   versión 2, publicada  por  la  Free  Software Foundation.
 ***************************************************************************/

#include <AQCore/AQCore.h>

#include "AQ3MainWindow.h"

AQ3MainWindow::AQ3MainWindow( QWidget * parent, const char * name, Qt::WindowFlags f ) :
    Q3MainWindow( parent, name, f )
{}

AQ3MainWindow::~AQ3MainWindow()
{}

void AQ3MainWindow::openDefaultForm()
{}

void AQ3MainWindow::execDefaultScript()
{
  QString item = sender() ->objectName();
  if ( !actsExecDefault.contains( item ) )
    actsExecDefault << item;
}

void AQ3MainWindow::execMainScript( const QString & n )
{
  QString item = sender() ->objectName() + ":" + n;
  if ( !actsExecMain.contains( item ) )
    actsExecMain << item;
}


