/***************************************************************************
AQValidator.h
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

#ifndef AQVALIDATOR_H_
#define AQVALIDATOR_H_

#include <QValidator>

class AQDoubleValidator: public QDoubleValidator
{
public:

  AQDoubleValidator( QObject * parent );
  AQDoubleValidator( double bottom, double top, int decimals, QObject * parent );
  QValidator::State validate( QString & input, int & ) const;
};

class AQIntValidator: public QIntValidator
{
public:

  AQIntValidator( QObject * parent );
  AQIntValidator( int minimum, int maximum, QObject * parent );
  QValidator::State validate( QString & input, int & ) const;
};

class AQUIntValidator: public QIntValidator
{
public:

  AQUIntValidator( QObject * parent );
  AQUIntValidator( int minimum, int maximum, QObject * parent );
  QValidator::State validate( QString & input, int & ) const;
};

#endif /*AQVALIDATOR_H_*/
