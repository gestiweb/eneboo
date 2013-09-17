/***************************************************************************
AQObject.cpp
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

#ifdef AQ_WUI
# include <AQUi/WQApplication.h>
#endif

#include "AQObject.h"
#include "AQObject_p.h"

AQObject::AQObject( QObject * parent ) :
    QObject( parent ),
    aq_d_ptr( new AQObjectPrivate )
{
  aq_d_ptr->aq_q_ptr = this;
}

AQObject::AQObject( AQObjectPrivate & dd, QObject * parent ) :
    QObject( parent ),
    aq_d_ptr( &dd )
{
  aq_d_ptr->aq_q_ptr = this;
}

AQObject::~AQObject()
{
  AQ_D( AQObject );
  delete d;
  aq_d_ptr = 0;
}

AQObjectPrivate::AQObjectPrivate()
{
  aq_q_ptr = 0;
  parent = 0;
}

AQObjectPrivate::~AQObjectPrivate()
{}

#ifdef AQ_WUI
void AQObject::invokeMethod( const char * member, QGenericArgument val0, QGenericArgument val1 )
{
  WQApplication::invokeMethod( this, member, Qt::AutoConnection, val0, val1 );
}
#endif
