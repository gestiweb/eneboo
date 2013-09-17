/***************************************************************************
                   AQTranslations.cpp
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

#include "AQTranslations.h"
#include "metatranslator.h"
#include "proparser.h"

AQTranslations::AQTranslations()
{}

AQTranslations::~AQTranslations()
{}

bool AQTranslations::loadTsFile( MetaTranslator& tor, const QString& tsFileName,
                                 bool /* verbose */ ) const
{
  QString qmFileName = tsFileName;
  qmFileName.replace( QRegExp( QLatin1String( "\\.ts$" ) ), QLatin1String( "" ) );
  qmFileName += QLatin1String( ".qm" );

  bool ok = tor.load( tsFileName );
  //if ( !ok )
  //    fprintf( stderr,
  //             "lrelease warning: For some reason, I cannot load '%s'\n",
  //             tsFileName.toLatin1().data() );
  return ok;
}

void AQTranslations::releaseMetaTranslator( const MetaTranslator& tor,
    const QString& qmFileName, bool verbose,
    bool ignoreUnfinished, bool trimmed ) const
{
  tor.release( qmFileName, verbose, ignoreUnfinished,
               trimmed ? Translator::Stripped
               : Translator::Everything );
  //if ( verbose )
  //    fprintf( stderr, "Updating '%s'...\n", qmFileName.toLatin1().constData() );
  //if ( !tor.release( qmFileName, verbose, ignoreUnfinished,
  //                   trimmed ? Translator::Stripped
  //                   : Translator::Everything ) )
  //    fprintf( stderr,
  //             "lrelease warning: For some reason, I cannot save '%s'\n",
  //             qmFileName.toLatin1().constData() );
}

void AQTranslations::releaseTsFile( const QString& tsFileName, bool verbose,
                                    bool ignoreUnfinished, bool trimmed ) const
{
  MetaTranslator tor;
  if ( loadTsFile( tor, tsFileName, verbose ) ) {
    QString qmFileName = tsFileName;
    qmFileName.replace( QRegExp( QLatin1String( "\\.ts$" ) ), QLatin1String( "" ) );
    qmFileName += QLatin1String( ".qm" );
    releaseMetaTranslator( tor, qmFileName, verbose, ignoreUnfinished,
                           trimmed );
  }
}

void AQTranslations::lrelease( const QString & tsInputFile, const QString & qmOutputFile ) const
{
  bool verbose = true;
  bool ignoreUnfinished = false;
  bool trimmed = false;
  MetaTranslator tor;

  QFile f( tsInputFile );
  if ( !f.open( QIODevice::ReadOnly ) )
    return ;

  QTextStream t( &f );
  QString fullText = t.readAll();
  f.close();

  if ( fullText.contains( QString( QLatin1String( "<!DOCTYPE TS>" ) ) )
       || fullText.contains( QLatin1String( "urn:oasis:names:tc:xliff:document:1.1" ) ) ) {
    if ( qmOutputFile.isEmpty() ) {
      releaseTsFile( tsInputFile, verbose, ignoreUnfinished,
                     trimmed );
    } else {
      loadTsFile( tor, tsInputFile, verbose );
    }
  } else {
    QString oldDir = QDir::currentPath();
    QDir::setCurrent( QFileInfo( tsInputFile ).path() );
    QMap<QByteArray, QStringList> varMap;
    bool ok = evaluateProFile( tsInputFile, verbose, &varMap );
    if ( ok ) {
      QStringList translations = varMap.value( "TRANSLATIONS" );
      if ( translations.isEmpty() ) {
        //fprintf( stderr,
        //         "lrelease warning: Met no 'TRANSLATIONS' entry in"
        //         " project file '%s'\n",
        //         tsInputFile );
      } else {
        for ( QStringList::iterator it = translations.begin(); it != translations.end(); ++it ) {
          releaseTsFile( *it, verbose, ignoreUnfinished, trimmed );
        }
      }

      QDir::setCurrent( oldDir );
    } else {
      //fprintf( stderr, "error: lrelease encountered project file functionality that is currently not supported.\n"
      //         "You might want to consider using .ts files as input instead of a project file.\n"
      //         "Try the following syntax:\n"
      //         "    lrelease [options] ts-files [-qm qm-file]\n" );
    }
  }

  if ( !qmOutputFile.isEmpty() )
    releaseMetaTranslator( tor, qmOutputFile, verbose, ignoreUnfinished,
                           trimmed );
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
