/***************************************************************************
AQAbout.h
-------------------
begin                : 29/08/2007
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

#ifndef AQABOUT_H
#define AQABOUT_H

#include <uic/AQ3About.h>

/**
Implementación del cuadro de diálogo Acerca de..

@author InfoSiAL S.L.
*/
class AQAbout : public AQ3About
{
  Q_OBJECT

public:

  /**
  constructor
  */
  Q_INVOKABLE AQAbout( QWidget * parent = 0, const char * name = 0 );

  /**
  constructor.

  @param v Versión de la aplicación.
  */
  AQAbout( const QString & v, QWidget * parent = 0, const char * name = 0 );
};

#endif
