/***************************************************************************
                        AQTranslations.h
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

#ifndef AQTRANSLATIONS_H_
#define AQTRANSLATIONS_H_

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include <QTextStream>

#include <errno.h>

class MetaTranslatorMessage;
class MetaTranslator;

class AQTranslations
{

public:

  typedef QList<MetaTranslatorMessage> TML;

  /**
  constructor
  */
  AQTranslations();

  /**
  destructor
  */
  ~AQTranslations();

  bool loadTsFile( MetaTranslator& tor, const QString& tsFileName, bool /* verbose */ ) const;

  void releaseMetaTranslator( const MetaTranslator& tor, const QString& qmFileName,
                              bool verbose, bool ignoreUnfinished, bool trimmed ) const;

  void releaseTsFile( const QString& tsFileName, bool verbose,
                      bool ignoreUnfinished, bool trimmed ) const;

  void lrelease( const QString & tsInputFile, const QString & qmOutputFile ) const;
};

#endif /*AQTRANSLATIONS_H_*/

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
