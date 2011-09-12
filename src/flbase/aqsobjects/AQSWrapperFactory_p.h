/***************************************************************************
 AQSWrapperFactory_p.h
 -------------------
 begin                : 29/01/2011
 copyright            : (C) 2003-2011 by InfoSiAL S.L.
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

#ifndef AQSWRAPPERFACTORY_P_H_
#define AQSWRAPPERFACTORY_P_H_

#include <qobject.h>

#define AQ_REG_WRAP(Class) \
  registerWrapper(AQ_QUOTEME(Q##Class), AQ_QUOTEME(AQS##Class))
#define AQ_REG_WRAP_AQOBJECT(Class) \
  registerWrapper(AQ_QUOTEME(AQ##Class), AQ_QUOTEME(AQS##Class))

#define AQ_CRE_WRAP_OBJECT(Class) \
  if (className == AQ_QUOTEME(Q##Class)) { \
    QObject *o = static_cast<QObject *>(ptr); \
    if (qstrcmp(o->className(), AQ_QUOTEME(Q##Class)) != 0) return o; \
    AQS##Class *qo = new AQS##Class(o); \
    qo->setWrap(); \
    return qo; \
  }

#define AQ_CRE_WRAP_AQOBJECT(Class) \
  if (className == AQ_QUOTEME(AQ##Class)) { \
    QObject *o = static_cast<QObject *>(ptr); \
    if (qstrcmp(o->className(), AQ_QUOTEME(AQ##Class)) != 0) return o; \
    AQS##Class *qo = new AQS##Class(o); \
    qo->setWrap(); \
    return qo; \
  }

#define AQ_CRE_WRAP_VOIDPTR(Class) \
  if (className == AQ_QUOTEME(Q##Class)) { \
    AQS##Class *aqo = new AQS##Class(ptr); \
    aqo->setWrap(); \
    return aqo; \
  }

#define AQ_CRE_WRAP_VOIDPTR_AQOBJECT(Class) \
  if (className == AQ_QUOTEME(AQ##Class)) { \
    AQS##Class *aqo = new AQS##Class(ptr); \
    aqo->setWrap(); \
    return aqo; \
  }

class AQSWrapperFactoryPrivate
{
public:
  AQSWrapperFactoryPrivate() {
  }
  ~AQSWrapperFactoryPrivate() {
  }
};

#endif /* AQSWRAPPERFACTORY_P_H_ */
