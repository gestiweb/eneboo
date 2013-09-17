/***************************************************************************
AQValidator.cpp
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

#include "AQValidator.h"

AQDoubleValidator::AQDoubleValidator( QObject * parent ) :
    QDoubleValidator( parent )
{}

AQDoubleValidator::AQDoubleValidator( double bottom, double top, int decimals, QObject * parent ) :
    QDoubleValidator( bottom, top, decimals, parent )
{}

QValidator::State AQDoubleValidator::validate( QString & input, int &i ) const
{
  if ( input.isEmpty() )
    return QValidator::Acceptable;
  input.replace( "," , "." );
  QValidator::State state = QDoubleValidator::validate( input, i );
  if ( state == QValidator::Invalid || state == QValidator::Intermediate ) {
    QString s = input.right( input.length() - 1 );

    if ( input.left( 1 ) == "-"
         && ( QDoubleValidator::validate( s, i ) == QValidator::Acceptable
              || s.isEmpty() ) )
      return QValidator::Acceptable;
    return QValidator::Invalid;
  } else
    return QValidator::Acceptable;
}

AQIntValidator::AQIntValidator( QObject * parent ) :
    QIntValidator( parent )
{}

AQIntValidator::AQIntValidator( int minimum, int maximum, QObject * parent ) :
    QIntValidator( minimum, maximum, parent )
{}

QValidator::State AQIntValidator::validate( QString & input, int &i ) const
{
  if ( input.isEmpty() )
    return QValidator::Acceptable;
  QValidator::State state = QIntValidator::validate( input, i );
  if ( state == QValidator::Invalid || state == QValidator::Intermediate ) {
    QString s = input.right( input.length() - 1 );

    if ( input.left( 1 ) == "-"
         && ( QIntValidator::validate( s, i ) == QValidator::Acceptable
              || s.isEmpty() ) )
      return QValidator::Acceptable;
    return QValidator::Invalid;
  } else
    return QValidator::Acceptable;
}

AQUIntValidator::AQUIntValidator( QObject * parent ) :
    QIntValidator( parent )
{}

AQUIntValidator::AQUIntValidator( int minimum, int maximum, QObject * parent ) :
    QIntValidator( minimum, maximum, parent )
{}

QValidator::State AQUIntValidator::validate( QString & input, int &i ) const
{
  if ( input.isEmpty() )
    return QValidator::Acceptable;
  QIntValidator iV( 0, 1000000000, 0 );
  QValidator::State state = iV.validate( input, i );
  if ( state == QValidator::Intermediate )
    state = QValidator::Invalid;
  return state;
}
