/***************************************************************************
 FLSettings.cpp
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

#include <qsettings.h>

#include "FLSettings.h"
#include "AQConfig.h"

#define AQ_NEW_SETTING \
  QSettings s; \
  s.setPath("Eneboo", "Eneboo", QSettings::User)

#define AQ_OLD_SETTING \
  s.setPath("InfoSiAL", "FacturaLUX", QSettings::User)

QStringList FLSettings::readListEntry(const QString &key, bool *retOk)
{
  AQ_NEW_SETTING;
  bool ok = true;
  QStringList ret(s.readListEntry(AQ_KEYBASE + key, &ok));
  if (retOk)
    *retOk = ok;
  if (ok)
    return ret;
  AQ_OLD_SETTING;
  return s.readListEntry(QString::fromLatin1("/facturalux/lite/") + key, retOk);
}

QString FLSettings::readEntry(const QString &key, const QString &def, bool *retOk)
{
  AQ_NEW_SETTING;
  bool ok = true;
  QString ret(s.readEntry(AQ_KEYBASE + key, def, &ok));
  if (retOk)
    *retOk = ok;
  if (ok)
    return ret;
  AQ_OLD_SETTING;
  return s.readEntry(QString::fromLatin1("/facturalux/lite/") + key, def, retOk);
}

int FLSettings::readNumEntry(const QString &key, int def, bool *retOk)
{
  AQ_NEW_SETTING;
  bool ok = true;
  int ret(s.readNumEntry(AQ_KEYBASE + key, def, &ok));
  if (retOk)
    *retOk = ok;
  if (ok)
    return ret;
  AQ_OLD_SETTING;
  return s.readNumEntry(QString::fromLatin1("/facturalux/lite/") + key, def, retOk);
}

double FLSettings::readDoubleEntry(const QString &key, double def, bool *retOk)
{
  AQ_NEW_SETTING;
  bool ok = true;
  double ret(s.readDoubleEntry(AQ_KEYBASE + key, def, &ok));
  if (retOk)
    *retOk = ok;
  if (ok)
    return ret;
  AQ_OLD_SETTING;
  return s.readDoubleEntry(QString::fromLatin1("/facturalux/lite/") + key, def, retOk);
}

bool FLSettings::readBoolEntry(const QString &key, bool def, bool *retOk)
{
  AQ_NEW_SETTING;
  bool ok = true;
  bool ret(s.readBoolEntry(AQ_KEYBASE + key, def, &ok));
  if (retOk)
    *retOk = ok;
  if (ok)
    return ret;
  AQ_OLD_SETTING;
  return s.readBoolEntry(QString::fromLatin1("/facturalux/lite/") + key, def, retOk);
}

bool FLSettings::writeEntry(const QString &key, bool value)
{
  AQ_NEW_SETTING;
  return s.writeEntry(AQ_KEYBASE + key, value);
}

bool FLSettings::writeEntry(const QString &key, double value)
{
  AQ_NEW_SETTING;
  return s.writeEntry(AQ_KEYBASE + key, value);
}

bool FLSettings::writeEntry(const QString &key, int value)
{
  AQ_NEW_SETTING;
  return s.writeEntry(AQ_KEYBASE + key, value);
}

bool FLSettings::writeEntry(const QString &key, const QString &value)
{
  AQ_NEW_SETTING;
  return s.writeEntry(AQ_KEYBASE + key, value);
}

bool FLSettings::writeEntry(const QString &key, const QStringList &value)
{
  AQ_NEW_SETTING;
  return s.writeEntry(AQ_KEYBASE + key, value);
}

bool FLSettings::writeEntryList(const QString &key, const QStringList &value)
{
  AQ_NEW_SETTING;
  return s.writeEntry(AQ_KEYBASE + key, value);
}

