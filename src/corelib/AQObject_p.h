/***************************************************************************
AQObject_p.h
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

#ifndef AQOBJECT_P_H_
#define AQOBJECT_P_H_

#include <AQCore/AQObject.h>

inline AQObjectData::~AQObjectData()
{}

class AQObjectPrivate : public AQObjectData
{
  AQ_DECLARE_PUBLIC( AQObject )

public:

  AQObjectPrivate();
  virtual ~AQObjectPrivate();

  static AQObjectPrivate * get( AQObject * o )
  {
    return o->d_func();
  }
};

#endif /*AQOBJECT_P_H_*/
