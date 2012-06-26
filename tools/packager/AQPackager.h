/***************************************************************************
 AQPackager.h
 -------------------
 begin                : 06/03/2011
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

#ifndef AQPACKAGER_H_
#define AQPACKAGER_H_

#include <AQUnpacker.h>

class AQPackager
{
public:
  AQPackager(const QString & = QString::null) {}
  bool pack(const QString &, bool = true, bool = false, bool = false, bool = true, bool = false) { return false; }
  bool unpack(const QString &) { return false; }
  QString output() const { return QString::null; }
  QString outputPackage() const { return QString::null; }
  void setEncodeUtf8(bool = true) {}
  QStringList errorMessages() const { return QStringList(); }
  QStringList logMessages() const { return QStringList(); }
  void setFilter(const QStringList &) {}
  QStringList filter() const { return QStringList(); }
};

#endif /* AQPACKAGER_H_ */
