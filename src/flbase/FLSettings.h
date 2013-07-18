/***************************************************************************
 FLSettings.h
 -------------------
 begin                : 28/03/2011
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

#ifndef FLSETTINGS_H_
#define FLSETTINGS_H_

#include "AQGlobal.h"

class AQ_EXPORT FLSettings
{
public:
  static QStringList readListEntry(const QString &key, bool *retOk = 0);
  static QString readEntry(const QString &key,
                           const QString &def = QString::null, bool *retOk = 0);
  static int readNumEntry(const QString &key, int def = 0, bool *retOk = 0);
  static double readDoubleEntry(const QString &key, double def = 0, bool *retOk = 0);
  static bool readBoolEntry(const QString &key, bool def = false, bool *retOk = 0);

  static bool writeEntry(const QString &key, bool value);
  static bool writeEntry(const QString &key, double value);
  static bool writeEntry(const QString &key, int value);
  static bool writeEntry(const QString &key, const QString &value);
  static bool writeEntry(const QString &key, const QStringList &value);
  static bool writeEntryList(const QString &key, const QStringList &value);
};

#endif /* FLSETTINGS_H_ */
