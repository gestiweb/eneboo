/***************************************************************************
                         FLTranslations.h
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

#ifndef FLTRANSLATIONS_H_
#define FLTRANSLATIONS_H_

#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qregexp.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qtextstream.h>

#include <errno.h>

class MetaTranslatorMessage;
class MetaTranslator;

class FL_EXPORT FLTranslations
{

public:

  typedef QPtrList<MetaTranslatorMessage> TML;

  bool loadTsFile(MetaTranslator &tor, const QString &tsFileName, bool /* verbose */);

  void releaseMetaTranslator(const MetaTranslator &tor,
                             const QString &qmFileName, bool verbose,
                             bool stripped);

  void releaseTsFile(const QString &tsFileName, bool verbose,
                     bool stripped);

  void lrelease(const QString &tsInputFile, const QString &qmOutputFile, bool stripped = true);
};

#endif /*FLTRANSLATIONS_H_*/

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
