/***************************************************************************
 AQSObjectFactory_p.h
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

#ifndef AQSOBJECTFACTORY_P_H_
#define AQSOBJECTFACTORY_P_H_

#include <qwidget.h>

#define AQ_REG_CLASS(Class) \
  registerClass(AQ_QUOTEME(Q##Class), AQ_QUOTEME(AQS##Class))
#define AQ_REG_CLASS_AQOBJECT(Class) \
  registerClass(AQ_QUOTEME(AQ##Class), AQ_QUOTEME(AQS##Class))

#define AQ_CRE_OBJECT(Class) \
  if (className == AQ_QUOTEME(Q##Class)) { \
    QObject *qo = AQS##Class::Q##Class##_(arguments); \
    if (!qo && candidateCtors) \
      *candidateCtors = AQS##Class::candidateConstructors(); \
    return qo; \
  }

#define AQ_CRE_AQOBJECT(Class) \
  if (className == AQ_QUOTEME(AQ##Class)) { \
    QObject *aq = AQS##Class::AQ##Class##_(arguments); \
    if (!aq && candidateCtors) \
      *candidateCtors = AQS##Class::candidateConstructors(); \
    return aq; \
  }

#define AQ_CRE_WIDGET(Class) AQ_CRE_OBJECT(Class)

#define AQ_CRE_AQWIDGET(Class) AQ_CRE_AQOBJECT(Class)

#define AQ_CRE_VOIDPTR(Class) \
  if (className == AQ_QUOTEME(Q##Class)) { \
    AQS##Class *aqo = new AQS##Class(AQS##Class::Q##Class##_(arguments)); \
    if ((!aqo || !aqo->isValid()) && candidateCtors) \
      *candidateCtors = AQS##Class::candidateConstructors(); \
    return aqo; \
  }

#define AQ_CRE_VOIDPTR_AQOBJECT(Class) \
  if (className == AQ_QUOTEME(AQ##Class)) { \
    AQS##Class *aqo = new AQS##Class(AQS##Class::AQ##Class##_(arguments)); \
    if ((!aqo || !aqo->isValid()) && candidateCtors) \
      *candidateCtors = AQS##Class::candidateConstructors(); \
    return aqo; \
  }

class AQSObjectFactoryPrivate
{
public:
  AQSObjectFactoryPrivate() {
  }
  ~AQSObjectFactoryPrivate() {
  }

  QMap<int, QStringList> candidateCtors_;
};

#endif /* AQSOBJECTFACTORY_P_H_ */

