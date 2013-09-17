/***************************************************************************
       AQTreeWidget.cpp
   -------------------
begin                : 22/09/2007
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

#include <QPaintEvent>
#include <QPainter>

#include <AQCore/AQCore.h>

#include "AQMainWindow.h"
#include "AQTreeWidget.h"

AQTreeWidget::AQTreeWidget( AQMainWindow * mw ) : QTreeWidget( mw ), mainWindow( mw )
{
  logo.load( ":/images/mini_logo_abanq.png" );
}

AQTreeWidget::~AQTreeWidget()
{}

void AQTreeWidget::dropEvent ( QDropEvent * e )
{
  QTreeWidget * src = qobject_cast<QTreeWidget *>( e->source() );
  if ( src ) {
    QList<QAction *> list = mainWindow->actions();
    QAction * a = 0;
    for ( QList<QAction *>::const_iterator it = list.begin(); it != list.end(); ++it ) {
      if ( ( *it ) ->objectName() == src->currentItem() ->text( 1 ) ) {
        a = *it;
        break;
      }
    }
    if ( a ) {
      QTreeWidgetItem * curItem = itemAt( e->pos() );
      if ( curItem && !curItem->flags().testFlag( Qt::ItemIsDropEnabled ) )
        curItem = curItem->parent();
      QTreeWidgetItem * itemAct = new QTreeWidgetItem( curItem );
      itemAct->setText( 0, a->text().remove( "&" ) );
      itemAct->setIcon( 0, a->icon() );
      itemAct->setFont( 0, a->font() );
      itemAct->setData( 0, Qt::UserRole, "bookmark" );
      itemAct->setText( 1, a->objectName() );
      itemAct->setFlags( itemAct->flags() ^ Qt::ItemIsDropEnabled );
      if ( !curItem )
        addTopLevelItem( itemAct );
      setCurrentItem( itemAct );
    }
  }
}

void AQTreeWidget::paintEvent( QPaintEvent * pe )
{
  QPainter p( viewport() );
  p.setClipRegion( pe->region() );
  int dx = viewport() ->width() - logo.width();
  int dy = viewport() ->height() - logo.height();
  QRect rImage( dx, dy, logo.width(), logo.height() );
  if ( pe->rect().intersects( rImage ) )
    p.drawImage ( rImage.topLeft(), logo );
  QRect rLine( dx + 18, 0, 1, dy );
  if ( pe->rect().intersects( rLine ) ) {
    p.setPen( QColor( logo.pixel( 18, 1 ) ) );
    p.setBrush( QColor( logo.pixel( 18, 1 ) ) );
    p.drawRect( rLine );
  }
  p.end();
  QTreeWidget::paintEvent( pe );
}

void AQTreeWidget::scrollContentsBy( int dx, int dy )
{
  int dpx = viewport() ->width() - logo.width();
  int dpy = viewport() ->height() - logo.height();
  QRect rImage( dpx, dpy, logo.width(), logo.height() );
  setDirtyRegion( rImage );
  QTreeWidget::scrollContentsBy( dx, dy );
}
