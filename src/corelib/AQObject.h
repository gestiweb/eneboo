/***************************************************************************
AQObject.h
-------------------
begin                : 11/08/2008
copyright            : (C) 2003-2008 by InfoSiAL S.L.
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

#ifndef AQOBJECT_H_
#define AQOBJECT_H_

#include <AQCore/AQCore.h>

#ifdef AQ_WUI
# include <QGenericArgument>
#endif

class AQObject;
class AQObjectPrivate;

class AQObjectData
{
public:

  virtual ~AQObjectData() = 0;

  AQObject * aq_q_ptr;
  QObject * parent;
};

class AQObject : public QObject
{
  AQ_DECLARE_PRIVATE( AQObject )

public:

  explicit AQObject( QObject * parent = 0 );
  virtual ~AQObject();

protected:

  AQObject( AQObjectPrivate & dd, QObject * parent = 0 );

  AQObjectData * aq_d_ptr;

#ifdef AQ_WUI
  void invokeMethod( const char * member,
                     QGenericArgument val0 = QGenericArgument( 0 ),
                     QGenericArgument val1 = QGenericArgument() );
#endif
};

#endif /*AQOBJECT_H_*/
