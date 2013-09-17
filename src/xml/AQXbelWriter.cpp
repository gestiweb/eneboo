/****************************************************************************
**
** Copyright (C) 2005-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
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

#include <QtGui>

#include "AQXbelWriter.h"

AQXbelWriter::AQXbelWriter( QTreeWidget *treeWidget )
    : treeWidget( treeWidget )
{
  setAutoFormatting( true );
}

bool AQXbelWriter::writeFile( QIODevice *device )
{
  setDevice( device );

  writeStartDocument();
  writeDTD( "<!DOCTYPE xbel>" );
  writeStartElement( "xbel" );
  writeAttribute( "version", "1.0" );
  for ( int i = 0; i < treeWidget->topLevelItemCount(); ++i )
    writeItem( treeWidget->topLevelItem( i ) );

  writeEndDocument();
  return true;
}

void AQXbelWriter::writeItem( QTreeWidgetItem *item )
{
  QString tagName = item->data( 0, Qt::UserRole ).toString();
  if ( tagName == "folder" ) {
    bool folded = !treeWidget->isItemExpanded( item );
    writeStartElement( tagName );
    writeAttribute( "folded", folded ? "yes" : "no" );
    writeTextElement( "title", item->text( 0 ) );
    for ( int i = 0; i < item->childCount(); ++i )
      writeItem( item->child( i ) );
    writeEndElement();
  } else if ( tagName == "bookmark" ) {
    writeStartElement( tagName );
    if ( !item->text( 1 ).isEmpty() )
      writeAttribute( "href", item->text( 1 ) );
    writeTextElement( "title", item->text( 0 ) );
    writeEndElement();
  } else if ( tagName == "separator" ) {
    writeEmptyElement( tagName );
  }
}
