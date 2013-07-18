/***************************************************************************
                       FLTranslations.cpp
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

#include "FLTranslations.h"
#include "metatranslator.h"
#include "proparser.h"

bool FLTranslations::loadTsFile(MetaTranslator &tor, const QString &tsFileName,
                                bool /* verbose */)
{
  QString qmFileName = tsFileName;
  qmFileName.replace(QRegExp("\\.ts$"), QString::fromLatin1(""));
  qmFileName += QString::fromLatin1(".qm");

  bool ok = tor.load(tsFileName);
  if (!ok)
    fprintf(stderr,
            "lrelease warning: For some reason, I cannot load '%s'\n",
            tsFileName.latin1());
  return ok;
}

void FLTranslations::releaseMetaTranslator(const MetaTranslator &tor,
                                           const QString &qmFileName, bool verbose,
                                           bool stripped)
{
  if (verbose)
    fprintf(stderr, "Updating '%s'...\n", qmFileName.latin1());
  if (!tor.release(qmFileName, verbose,
                   stripped ? QTranslator::Stripped
                   : QTranslator::Everything))
    fprintf(stderr,
            "lrelease warning: For some reason, I cannot save '%s'\n",
            qmFileName.latin1());
}

void FLTranslations::releaseTsFile(const QString &tsFileName, bool verbose,
                                   bool stripped)
{
  MetaTranslator tor;
  if (loadTsFile(tor, tsFileName, verbose)) {
    QString qmFileName = tsFileName;
    qmFileName.replace(QRegExp("\\.ts$"), QString::fromLatin1(""));
    qmFileName += QString::fromLatin1(".qm");
    releaseMetaTranslator(tor, qmFileName, verbose, stripped);
  }
}

void FLTranslations::lrelease(const QString &tsInputFile, const QString &qmOutputFile, bool stripped)
{
  bool verbose = false;
  bool metTranslations = false;
  MetaTranslator tor;

  QFile f(tsInputFile);
  if (!f.open(IO_ReadOnly)) {
    fprintf(stderr,
            "lrelease error: Cannot open file '%s': %s\n", tsInputFile.latin1(),
            strerror(errno));
    return;
  }

  QTextStream t(&f);
  QString fullText = t.read();
  f.close();

  if (fullText.find(QString("<!DOCTYPE TS>")) >= 0) {
    if (qmOutputFile.isEmpty()) {
      releaseTsFile(tsInputFile, verbose, stripped);
    } else {
      loadTsFile(tor, tsInputFile, verbose);
    }
  } else {
    QString oldDir = QDir::currentDirPath();
    QDir::setCurrent(QFileInfo(tsInputFile).dirPath());

    QMap<QString, QString> tagMap = proFileTagMap(fullText);
    QMap<QString, QString>::Iterator it;

    for (it = tagMap.begin(); it != tagMap.end(); ++it) {
      QStringList toks = QStringList::split(' ', it.data());
      QStringList::Iterator t;

      for (t = toks.begin(); t != toks.end(); ++t) {
        if (it.key() == QString("TRANSLATIONS")) {
          metTranslations = TRUE;
          releaseTsFile(*t, verbose, stripped);
        }
      }
    }
    if (!metTranslations)
      fprintf(stderr,
              "lrelease warning: Met no 'TRANSLATIONS' entry in"
              " project file '%s'\n",
              tsInputFile.latin1());
    QDir::setCurrent(oldDir);
  }


  if (!qmOutputFile.isEmpty())
    releaseMetaTranslator(tor, qmOutputFile, verbose, stripped);
}

/****************************************************************************
**
** Copyright (C) 1992-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Linguist of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
