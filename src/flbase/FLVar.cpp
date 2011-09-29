/***************************************************************************
 FLVar.cpp
 -------------------
 begin                : Thu Jul 29 2004
 copyright            : (C) 2004-2005 by InfoSiAL S.L.,
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

#include "FLUtil.h"
#include "FLApplication.h"
#include "FLSqlQuery.h"
#include "FLVar.h"

bool FLVar::set(const QString &n, const QVariant &v)
{
  QString idSesion(aqApp->timeUser().toString(Qt::ISODate));
  QString where(QString::fromLatin1("idvar='") + n +
                QString::fromLatin1("' AND idsesion='") + idSesion +
                QString::fromLatin1("'"));

  FLSqlQuery q;
  q.setTablesList("flvar");
  q.setSelect("id");
  q.setFrom("flvar");
  q.setWhere(where);
  q.setForwardOnly(true);

  if (!q.exec())
    return false;
  if (q.next())
    return FLUtil::sqlUpdate("flvar", "valor", v.toString(),
                             QString::fromLatin1("id='") + q.value(0).toString() +
                             QString::fromLatin1("'"));

  QString values(n + QString::fromLatin1(",") + idSesion +
                 QString::fromLatin1(",") + v.toString());
  return FLUtil::sqlInsert("flvar", "idvar,idsesion,valor", values);
}

QVariant FLVar::get(const QString &n)
{
  QString idSesion(aqApp->timeUser().toString(Qt::ISODate));
  QString where(QString::fromLatin1("idvar='") + n +
                QString::fromLatin1("' AND idsesion='") + idSesion +
                QString::fromLatin1("'"));
  return FLUtil::sqlSelect("flvar", "valor", where, "flvar");
}

bool FLVar::del(const QString &n)
{
  QString idSesion(aqApp->timeUser().toString(Qt::ISODate));
  QString where(QString::fromLatin1("idvar='") + n +
                QString::fromLatin1("' AND idsesion = '") + idSesion +
                QString::fromLatin1("'"));
  return FLUtil::sqlDelete("flvar", where);
}

bool FLVar::clean()
{
  QString idSesion(aqApp->timeUser().toString(Qt::ISODate));
  QString where(QString::fromLatin1("idsesion='") + idSesion + QString::fromLatin1("'"));
  return FLUtil::sqlDelete("flvar", where);
}
