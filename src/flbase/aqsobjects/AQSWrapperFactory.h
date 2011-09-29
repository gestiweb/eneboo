/***************************************************************************
 AQSWrapperFactory.h
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

#ifndef AQSWRAPPERFACTORY_H_
#define AQSWRAPPERFACTORY_H_

#include <qswrapperfactory.h>

class AQSWrapperFactoryPrivate;

class AQSWrapperFactory : public QSWrapperFactory
{
public:
  AQSWrapperFactory();
  ~AQSWrapperFactory();

  QObject *create(const QString &className, void *ptr);
  static QObject *staticCreate(const QString &className, void *ptr);

private:
  AQSWrapperFactoryPrivate *d;
};

#endif /* AQSWRAPPERFACTORY_H_ */
