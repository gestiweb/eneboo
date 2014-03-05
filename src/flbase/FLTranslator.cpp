/***************************************************************************
                         FLTranslator.cpp
                     -------------------
begin                : 07/08/2007
copyright            : (C) 2003-2007 by InfoSiAL S.L.
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

#include "FLTranslator.h"
#include "FLDiskCache.h"
#include "FLApplication.h"
#include "FLTranslations.h"

FLTranslator::FLTranslator(QObject *parent, const char *name,
                           bool multiLang, bool sysTrans) :
  QTranslator(parent ? parent : qApp, name),
  multiLang_(multiLang),
  sysTrans_(sysTrans) {}

bool FLTranslator::loadTsContent(const QString &key)
{
  QString tsFile(AQ_DISKCACHE_FILEPATH(key));
  QString qmFile(AQ_DISKCACHE_DIRPATH + '/' +
                 key + QString::fromLatin1(".qm"));
  FLTranslations trans;

  if (!QFile::exists(qmFile)) {
    if (tsFile.isEmpty())
      return false;
    trans.lrelease(tsFile, qmFile, !multiLang_);
  }

  return QTranslator::load(qmFile);
}

QTranslatorMessage FLTranslator::findMessage(const char *context, const char *sourceText,
                                             const char *comment) const
{
  //  if (multiLang_) {
  //    qWarning("%s %s %s %s", QObject::name(), context, sourceText, comment);
  //    QTranslatorMessage ret = QTranslator::findMessage(context, sourceText, comment);
  //    if (!ret.translation().isNull()) {
  //      qWarning("*** " + ret.translation());
  //      qWarning("### %s ", ret.context());
  //    }
  //    return ret;
  //  }
  if (!comment)
  return QTranslator::findMessage(context, sourceText);
  else
  return QTranslator::findMessage(context, sourceText, comment);
}
