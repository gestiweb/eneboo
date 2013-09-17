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

#include <AQUi/AQMainWindow.h>

#include "AQXbelReader.h"

AQXbelReader::AQXbelReader( QTreeWidget * treeWidget, AQMainWindow * mw )
    : treeWidget( treeWidget ), mainWindow( mw )
{
  QStyle * style = treeWidget->style();

  folderIcon.addPixmap( style->standardIcon( QStyle::SP_DirClosedIcon ).pixmap( 16, 16 ),
                        QIcon::Normal, QIcon::Off );
  folderIcon.addPixmap( style->standardIcon( QStyle::SP_DirOpenIcon ).pixmap( 16, 16 ),
                        QIcon::Normal, QIcon::On );
  bookmarkIcon.addPixmap( style->standardIcon( QStyle::SP_FileIcon ).pixmap( 16, 16 ) );
}

bool AQXbelReader::read( QIODevice *device )
{
  setDevice( device );

  while ( !atEnd() ) {
    readNext();

    if ( isStartElement() ) {
      if ( name() == "xbel" && attributes().value( "version" ) == "1.0" )
        readXBEL();
      else
        raiseError( QObject::tr( "The file is not an XBEL version 1.0 file." ) );
    }
  }

  return !error();
}

void AQXbelReader::readUnknownElement()
{
  Q_ASSERT( isStartElement() );

  while ( !atEnd() ) {
    readNext();

    if ( isEndElement() )
      break;

    if ( isStartElement() )
      readUnknownElement();
  }
}

void AQXbelReader::readXBEL()
{
  Q_ASSERT( isStartElement() && name() == "xbel" );

  while ( !atEnd() ) {
    readNext();

    if ( isEndElement() )
      break;

    if ( isStartElement() ) {
      if ( name() == "folder" )
        readFolder( 0 );
      else if ( name() == "bookmark" )
        readBookmark( 0 );
      else if ( name() == "separator" )
        readSeparator( 0 );
      else
        readUnknownElement();
    }
  }
}

void AQXbelReader::readTitle( QTreeWidgetItem *item )
{
  Q_ASSERT( isStartElement() && name() == "title" );

  QString title = readElementText();
  item->setText( 0, title );
}

void AQXbelReader::readSeparator( QTreeWidgetItem *item )
{
  QTreeWidgetItem * separator = createChildItem( item );
  separator->setFlags( item->flags() & ~Qt::ItemIsSelectable );
  separator->setText( 0, QString( 30, 0xB7 ) );
  readElementText();
}

void AQXbelReader::readFolder( QTreeWidgetItem *item )
{
  Q_ASSERT( isStartElement() && name() == "folder" );

  QTreeWidgetItem *folder = createChildItem( item );
  folder->setIcon( 0, folderIcon );
  bool folded = ( attributes().value( "folded" ) != "no" );
  treeWidget->setItemExpanded( folder, !folded );

  while ( !atEnd() ) {
    readNext();

    if ( isEndElement() )
      break;

    if ( isStartElement() ) {
      if ( name() == "title" )
        readTitle( folder );
      else if ( name() == "folder" )
        readFolder( folder );
      else if ( name() == "bookmark" )
        readBookmark( folder );
      else if ( name() == "separator" )
        readSeparator( folder );
      else
        readUnknownElement();
    }
  }
}

void AQXbelReader::readBookmark( QTreeWidgetItem *item )
{
  Q_ASSERT( isStartElement() && name() == "bookmark" );

  QTreeWidgetItem *bookmark = createChildItem( item );
  QString href = attributes().value( "href" ).toString();
  bookmark->setIcon( 0, mainWindow->actionIcon( href ) );
  bookmark->setText( 0, QObject::tr( "Unknown title" ) );
  bookmark->setText( 1, href );
  bookmark->setFlags( bookmark->flags() ^ Qt::ItemIsDropEnabled );
  bookmark->setFirstColumnSpanned( true );
  while ( !atEnd() ) {
    readNext();

    if ( isEndElement() )
      break;

    if ( isStartElement() ) {
      if ( name() == "title" )
        readTitle( bookmark );
      else
        readUnknownElement();
    }
  }
}

QTreeWidgetItem *AQXbelReader::createChildItem( QTreeWidgetItem *item )
{
  QTreeWidgetItem * childItem;
  if ( item ) {
    childItem = new QTreeWidgetItem( item );
  } else {
    childItem = new QTreeWidgetItem( treeWidget );
  }
  childItem->setData( 0, Qt::UserRole, name().toString() );
  return childItem;
}
